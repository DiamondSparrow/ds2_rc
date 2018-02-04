/**
 **********************************************************************************************************************
 * @file         app.c
 * @author       Diamond Sparrow
 * @version      1.0.0.0
 * @date         2016-11-21
 * @brief        Application C source file.
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
#include <stdio.h>
#include <string.h>

#include "cmsis_os.h"

#include "app.h"
#include "bsp.h"
#include "debug.h"
#include "indication.h"

#include "cli/cli_app.h"
#include "display/display.h"
#include "radio/nrf24l01.h"

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/
/** Define application thread */
osThreadDef(app_thread, osPriorityNormal, 1, 512);

/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
/** Application thread ID. */
osThreadId app_thread_id;

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/
/**
 * @brief   Main function.
 *
 * @return  None.
 */
int main(void)
{
    bsp_init();
    indication_set_blocking(INDICATION_BOOT);
    DEBUG_BOOT("");
    DEBUG_BOOT("     .-'/ ,_  \'-.   ");
    DEBUG_BOOT("    /  (  ( >  )  \\ ");
    DEBUG_BOOT("    \\   '-' '-'   / ");
    DEBUG_BOOT("    '-..__ __..-'    ");
    DEBUG_BOOT("          /_\\       ");
    DEBUG_BOOT(" # DS-2 Remote Controller #");
    DEBUG_BOOT(" * Booting.");
    DEBUG_BOOT("Build ....... %s %s", __DATE__, __TIME__);
    DEBUG_BOOT("Core Clock .. %ld Hz.", SystemCoreClock);

    // Setup and initialize peripherals.
    DEBUG_BOOT("BSP ......... ok.");

    // initialize RTOS kernel.
    if(osKernelInitialize() != osOK)
    {
        // Invoke application error function.
        app_error();
    }
    DEBUG_BOOT("Kernel ...... ok.");

    // Create application thread.
    if((app_thread_id = osThreadCreate(osThread(app_thread), NULL)) == NULL)
    {
        // Failed to create a thread
        app_error();
    }
    DEBUG_BOOT("APP ......... ok.");

    // Start kernel.
    if(osKernelStart() != osOK)
    {
        // app_error application error function.
        app_error();
    }
}

void app_thread(void const *arg)
{
    uint32_t i = 0;
    uint8_t ret = 0;
    uint8_t count = 0;
    uint8_t data[32] = {0};

    debug_init();
    DEBUG_INIT(" * Initializing.");
    indication_set_blocking(INDICATION_INIT);
    
    ret = indication_init();
    DEBUG_INIT("Indication .. %s.", ret ? "ok" : "err");
    ret = cli_app_init();
    DEBUG_INIT("CLI APP ..... %s.", ret ? "ok" : "err");
    //ret = display_init();
    //DEBUG_INIT("Display ..... %s.", ret == false ? "err" : "ok");

    nrf24l01_init(1, 32);
    nrf24l01_set_my_address((uint8_t []){0xD7,0xD7,0xD7,0xD7,0xD7});
    nrf24l01_set_tx_address((uint8_t []){0xE7,0xE7,0xE7,0xE7,0xE7});

    DEBUG(" * Running.");
    indication_set(INDICATION_IDLE);
    DEBUG("State: idle.");

    while(1)
    {
        for (i = 1; i < 32; i++)
        {
            data[i] = i;
        }
        nrf24l01_transmit(data);
        while(1)
        {
            osDelay(1);
            ret = nrf24l01_get_tx_status();
            if(ret != NRF24L01_TX_STATUS_SENDING)
            {
                break;
            }
        }
        switch(ret)
        {
            case NRF24L01_TX_STATUS_OK:
                count = nrf24l01_get_retransmissions_count();
                DEBUG("Transmit: OK (0x%02X, %d).", ret, count);
                nrf24l01_power_up_rx();
                osDelay(10);
                count = 10;
                while(1)
                {
                    osDelay(1);
                    if(nrf24l01_data_ready())
                    {
                        nrf24l01_get_data(data);
                        DEBUG("Received data:");
                        debug_send_hex_os(data, 32);
                        data[0]++;
                        break;
                    }
                    count--;
                    if(!count)
                    {
                        DEBUG("No response.");
                        break;
                    }
                }
                break;
            case NRF24L01_TX_STATUS_LOST:
                count = nrf24l01_get_retransmissions_count();
                DEBUG_INIT("Transmit: LOST (0x%02X, %d).", ret, count);
                break;
            case NRF24L01_TX_STATUS_SENDING:
                DEBUG_INIT("Transmit: SENDING (0x%02X, %d).", ret, count);
                break;
            default:
                DEBUG_INIT("Transmit: ERROR (0x%02X).", ret);
                break;
        }
        osDelay(1000);

/*
        c++;
        if(c == 5)
        {
            display_turn_off();
        }
        if(c == 10)
        {
            display_turn_on();
            c = 0;
        }
*/
    }
}

void app_error(void)
{
    indication_set_blocking(INDICATION_FAULT);
    debug_send_blocking((uint8_t *)"ERROR!\r", 7);

    while(1)
    {
        __nop();
    }
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/
