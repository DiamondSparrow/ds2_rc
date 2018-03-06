/**
 **********************************************************************************************************************
 * @file         uart.c
 * @author       Diamond Sparrow
 * @version      1.0.0.0
 * @date         2016-04-10
 * @brief        This is C source file template.
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

#include "chip.h"

#include "uart.h"

/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/
#define UART_0_BAUDRATE         115200  //!< UART 0 baudrate.
#define UART_0_RX_BUFFER_SIZE   128     //!< Receive buffer size in bytes.
#define UART_0_TX_BUFFER_SIZE   512     //!< Transmit buffer size in bytes.

/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
/** Transmit ring buffers */
__IO RINGBUFF_T uart0_tx_rb;
/** Receive ring buffers */
__IO RINGBUFF_T uart0_rx_rb;
/** Receive buffer. See @ref UART_0_RX_BUFFER_SIZE. */
uint8_t uart_0_rx_buffer[UART_0_RX_BUFFER_SIZE];
/** Transmit buffer. See @ref UART_0_TX_BUFFER_SIZE. */
uint8_t uart_0_tx_buffer[UART_0_TX_BUFFER_SIZE];

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/
void uart_0_init(void)
{
    /* Disables pullups/pulldowns and enable digitial mode */
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 18, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 19, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));

    /* Setup UART */
    Chip_UART0_Init(LPC_USART0);
    Chip_UART0_SetBaud(LPC_USART0, UART_0_BAUDRATE);
    Chip_UART0_ConfigData(LPC_USART0, (UART0_LCR_WLEN8 | UART0_LCR_SBS_1BIT));
    Chip_UART0_TXEnable(LPC_USART0);

    /* Before using the ring buffers, initialize them using the ring buffer init function */
    RingBuffer_Init((RINGBUFF_T *)&uart0_rx_rb, uart_0_rx_buffer, 1, UART_0_RX_BUFFER_SIZE);
    RingBuffer_Init((RINGBUFF_T *)&uart0_tx_rb, uart_0_tx_buffer, 1, UART_0_TX_BUFFER_SIZE);

    /* Enable receive data and line status interrupt */
    Chip_UART0_IntEnable(LPC_USART0, (UART0_IER_RBRINT | UART0_IER_RLSINT));

    /* Enable UART interrupt */
    NVIC_EnableIRQ(USART0_IRQn);

    return;
}

void uart_0_send(uint8_t *data, uint32_t size)
{
    Chip_UART0_SendBlocking(LPC_USART0, data, size);

    return;
}

void uart_0_send_rb(uint8_t *data, uint32_t size)
{
    Chip_UART0_SendRB(LPC_USART0, (RINGBUFF_T *)&uart0_tx_rb, data, size);

    return;
}

uint32_t uart_0_read_rb(uint8_t *data, uint32_t size)
{
    return Chip_UART0_ReadRB(LPC_USART0, (RINGBUFF_T *)&uart0_rx_rb, data, size);
}

void USART0_IRQHandler(void)
{
    Chip_UART0_IRQRBHandler(LPC_USART0, (RINGBUFF_T *)&uart0_rx_rb, (RINGBUFF_T *)&uart0_tx_rb);

    return;
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/
