/**
 **********************************************************************************************************************
 * @file         radio.c
 * @author       Diamond Sparrow
 * @version      1.0.0.0
 * @date         2018-02-06
 * @brief        Radio C source file.
 **********************************************************************************************************************
 * @warning     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR \n
 *              IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND\n
 *              FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR\n
 *              CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL\n
 *              DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,\n
 *              DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN\n
 *              CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF\n
 *              THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **********************************************************************************************************************
 */

/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "radio/radio.h"
#include "radio/nrf24l01.h"

#include "cmsis_os2.h"
#include "debug.h"

#include "app.h"
#include "sensors/sensors.h"

/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/
#define RADIO_CHANNEL               1       //!< Radio channel: from 0 to 125;
#define RADIO_PAYLAOD_SIZE          32      //!< Payload size in bytes.
#define RADIO_TRANSMIT_TMO_MS       50      //!< Radio data transmit timeout in milliseconds.
#define RADIO_RECEIVE_TMO_MS        50      //!< Radio data receive timeout in milliseconds.
#define RADIO_COMM_PERIOD_MS        1000    //!< Radio communication period in milliseconds.
#define RADIO_CONNECT_COUNT         4       //!< Connect/Disconnect count limit.

#define RADIO_MODE_1_WAY            0                   //!< One way (without response) communication.
#define RADIO_MODE_2_WAY            1                   //!< Two way (with response) communication.
#define RADIO_MODE                  RADIO_MODE_2_WAY    //!< Radio mode. See @ref RADIO_MODE_1_WAY and @ref RADIO_MODE_2_WAY.

#if RADIO_MODE == RADIO_MODE_2_WAY
#define RADIO_PACKET_PREAMBLE   0x7E
#else
#if RADIO_MODE == RADIO_MODE_1_WAY
#define RADIO_PACKET_PREAMBLE   0x7F
#else
#error "Invalid radio mode or preamle not specified."
#endif

#if RADIO_PAYLAOD_SIZE > NRF24L01_MAX_PAYLOAD
#error "Invalid radio payload."
#endif
#endif

/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/
typedef struct __attribute__((packed))
{
    uint8_t preamble;
    uint8_t sequence;
    uint8_t mode;
    int16_t magnitude;
    int16_t direction;
    uint8_t reserved[25];
} radio_packet_control_t;


#if RADIO_MODE == RADIO_MODE_2_WAY
typedef struct __attribute((packed))
{
    uint8_t preamble;
    uint8_t sequence;
    uint8_t mode;
    uint16_t curruent_motor_1;
    uint16_t current_motor_2;
    uint8_t reserved[25];
} radio_packet_feedback_t;
#endif

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/
/** Sensors thread attributes. */
const osThreadAttr_t radio_thread_attr =
{
    .name = "RADIO",
    .stack_size = 1024,
    .priority = osPriorityAboveNormal,
};
/** Radio configuration see @ref nrf24l01_cfg_t. */
const nrf24l01_cfg_t radio_config =
{
    .channel = RADIO_CHANNEL,
    .payload_size = RADIO_PAYLAOD_SIZE,
    .data_rate = NRF24L01_DATA_RATE_1M,
    .tx_power = NRF24L01_TX_POWER_0DBM,
};
/** My address. */
const uint8_t radio_my_address[NRF24L01_ADDRESS_SIZE] = {0xDE, 0xAD, 0xBE, 0xAF, 0x01};
/** Peer address. */
const uint8_t radio_peer_address[NRF24L01_ADDRESS_SIZE] = {0xDE, 0xAD, 0xBE, 0xAF, 0x07};

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
/** Radio thread ID. */
osThreadId_t radio_thread_id;
/** Radio data structure. */
volatile radio_data_t radio_data = {0};
/** Radio data buffer. */
static uint8_t radio_data_buffer[RADIO_PAYLAOD_SIZE] = {0};
/** Radio sequence. */
static uint8_t radio_sequence = 0;
/** Connect state. */
static bool radio_connect_state = false;

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/
/**
 * @brief   Radio transmit handler. Performs all transmits.
 *
 * @return  State of transmitter.
 * @retval  0   transmit failed.
 * @retval  1   transmit was successful.
 */
static bool radio_transmit_handler(void);

/**
 * @brief   Build transmit packet.
 *
 * @param   packet  Pointer to packet were to build transmit packet.
 * @param   size    Size of packet in bytes.
 *
 * @return  State of build.
 * @retval  0   failed to build.
 * @retval  1   build was successful.
 */
static bool radio_transmit_packet_builder(uint8_t *packet, uint8_t size);

#if RADIO_MODE == RADIO_MODE_2_WAY
/**
 * @brief   Radio receive handler. Performs data receive.
 *
 * @return  State of receive.
 * @retval  0   failed to receive.
 * @retval  1   receive was successful/
 */
static bool radio_receive_handler(void);

/**
 * @brief   Received packet parser.
 *
 * @param   packet  Pointer to packet where received data is hold to parse.
 * @param   size    Size of packet in bytes.
 *
 * @return  State of parser.
 * @retval  0   failed to parse.
 * @retval  1   successfully parsed.
 */
static bool radio_receive_packet_parser(uint8_t *packet, uint8_t size);
#endif // RADIO_MODE is RADIO_MODE_2_WAY

/**
 * @brief   Performs quality check.
 */
static void radio_quality_check(void);

/**
 * @brief   Control connect. Will update @ref radio_connect_state whether is connected or not.
 *
 * @param   packet_state    Packet state after receive or transmit. 0 - failed, 1 - success.
 */
static void radio_connect_control(bool packet_state);

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/
bool radio_init(void)
{
    if(RADIO_PAYLAOD_SIZE != sizeof(radio_packet_control_t))
    {
        return false;
    }
#if RADIO_MODE == RADIO_MODE_2_WAY
    if(RADIO_PAYLAOD_SIZE != sizeof(radio_packet_feedback_t))
    {
        return false;
    }
#endif // RADIO_MODE is RADIO_MODE_2_WAY

    // Create sensors thread.
    if((radio_thread_id = osThreadNew(&radio_thread, NULL, &radio_thread_attr)) == NULL)
    {
        // Failed to create a thread.
        return false;
    }

    return true;
}

void radio_thread(void *arguments)
{
    bool ret = false;

    nrf24l01_init(&radio_config);
    nrf24l01_set_my_address((uint8_t *)radio_my_address);
    nrf24l01_set_tx_address((uint8_t *)radio_peer_address);

    while(1)
    {
#if RADIO_MODE == RADIO_MODE_2_WAY
        if((ret = radio_transmit_handler()) == true)
        {
            ret = radio_receive_handler();
        }
#else
        ret = radio_transmit_handler();
#endif
        radio_connect_control(ret);
        osDelay(RADIO_COMM_PERIOD_MS);
    }
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/
static bool radio_transmit_handler(void)
{
    uint8_t c = RADIO_TRANSMIT_TMO_MS;
    nrf24l01_tx_status_t status = NRF24L01_TX_STATUS_LOST;

    if(radio_transmit_packet_builder(radio_data_buffer, RADIO_PAYLAOD_SIZE) == false)
    {
        return false;
    }

    nrf24l01_transmit(radio_data_buffer);
    while((c--))
    {
        osDelay(1);
        status = nrf24l01_get_tx_status();
        if(status != NRF24L01_TX_STATUS_SENDING)
        {
            break;
        }
    }

    radio_data.tx_counter++;
    radio_quality_check();

    switch(status)
    {
        case NRF24L01_TX_STATUS_OK:
            DEBUG_RADIO("Transmit: OK (%d ms., %d/%d, %d %%).", (RADIO_TRANSMIT_TMO_MS - c),
                        radio_data.rtr_current, radio_data.rtr, radio_data.quality);
            break;
        case NRF24L01_TX_STATUS_LOST:
            DEBUG_RADIO("Transmit: LOST (%d/%d, %d %%).",
                        radio_data.rtr_current, radio_data.rtr, radio_data.quality);
            radio_data.tx_lost_counter++;
            break;
        case NRF24L01_TX_STATUS_SENDING:
            DEBUG_RADIO("Transmit: SENDING (%d/%d, %d %%).",
                        radio_data.rtr_current, radio_data.rtr, radio_data.quality);
            radio_data.tx_lost_counter++;
            break;
        default:
            DEBUG_RADIO("Transmit: ERROR 0x%02X (%d/%d, %d %%).",
                        status,
                        radio_data.rtr_current, radio_data.rtr, radio_data.quality);
            radio_data.tx_lost_counter++;
            break;
    }

    return status == NRF24L01_TX_STATUS_OK ? true : false;
}

static bool radio_transmit_packet_builder(uint8_t *packet, uint8_t size)
{
    radio_packet_control_t *cntrl = (radio_packet_control_t *)packet;

    if(cntrl == NULL || packet == NULL || size ==0)
    {
        return false;
    }

    __disable_irq();
    radio_sequence++;
    cntrl->preamble = RADIO_PACKET_PREAMBLE;
    cntrl->sequence = radio_sequence;
    cntrl->mode = app_rc_mode_get();
    cntrl->magnitude = (int16_t)sensors_data.joystick_1.magnitude;
    cntrl->direction = (int16_t)sensors_data.joystick_1.direction;
    __enable_irq();

    return true;
}

#if RADIO_MODE == RADIO_MODE_2_WAY
static bool radio_receive_handler(void)
{
    uint8_t c = RADIO_RECEIVE_TMO_MS;

    nrf24l01_power_up_rx();
    while((c--))
    {
        osDelay(1);
        if(nrf24l01_data_ready())
        {
            nrf24l01_get_data(radio_data_buffer);
            DEBUG_RADIO("Received data (%d ms.):", (RADIO_RECEIVE_TMO_MS - c));
            debug_send_hex_os(radio_data_buffer, RADIO_PAYLAOD_SIZE);
            if(radio_receive_packet_parser(radio_data_buffer, RADIO_PAYLAOD_SIZE) == true)
            {
                radio_data.rx_counter++;
                return true;
            }
        }
    }
    DEBUG_RADIO("No response.");

    return false;
}

static bool radio_receive_packet_parser(uint8_t *packet, uint8_t size)
{
    radio_packet_feedback_t *feedback = (radio_packet_feedback_t *)packet;

    if(feedback == NULL || packet == NULL || size ==0)
    {
        return false;
    }

    if(feedback->preamble != RADIO_PACKET_PREAMBLE)
    {
        return false;
    }

    if(feedback->sequence != radio_sequence)
    {
        return false;
    }

    return true;
}
#endif  // RADIO_MODE is RADIO_MODE_2_WAY

static void radio_quality_check(void)
{
    uint8_t rtr = 0;

    rtr = nrf24l01_get_retransmissions_count();
    
    if(radio_data.tx_counter)
    {
        if(rtr <= NRF24L01_MAX_RTR)
        {
            radio_data.rtr_acumulator += rtr;
        }
    }
    else
    {
        radio_data.rtr_acumulator = rtr;
        radio_data.tx_counter = 1;
    }

    radio_data.rtr_current = rtr;
    radio_data.rtr = (uint32_t)((float)radio_data.rtr_acumulator / (float)radio_data.tx_counter);
    radio_data.quality = (uint32_t)(100.0 - ((float)radio_data.rtr * (100.0 / (float)NRF24L01_MAX_RTR)));

    return;
}

static void radio_connect_control(bool packet_state)
{
    static uint8_t counter_success = 0;
    static uint8_t counter_failed = 0;

    if(packet_state == true)
    {
        counter_failed = 0;
        if(radio_connect_state == false)
        {
            counter_success++;
            if(counter_success > RADIO_CONNECT_COUNT)
            {
                counter_success = 0;
                radio_connect_state = true;
            }
        }
    }
    else
    {
        counter_success = 0;
        counter_failed++;
        if(counter_failed > RADIO_CONNECT_COUNT)
        {
            counter_failed = 0;
            radio_connect_state = false;
        }
    }

    return;
}

