/**
 **********************************************************************************************************************
 * @file        uart.h
 * @author      Diamond Sparrow
 * @version     1.0.0.0
 * @date        2016-04-10
 * @brief       This is C header file template.
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

#ifndef UART_H_
#define UART_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include <stdint.h>

/**********************************************************************************************************************
 * Exported definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported types
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of exported constants
 *********************************************************************************************************************/
    
/**********************************************************************************************************************
 * Prototypes of exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of exported functions
 *********************************************************************************************************************/
/**
 * @brief   Initialize UART 0.
 */
void uart_0_init(void);

/**
 * @brief   Send data through UART 0 in blocking way.
 *
 * @param   data    Pointer to data to send.
 * @param   size    Size of data to send in bytes
 */
void uart_0_send(uint8_t *data, uint32_t size);

/**
 * @brief   Send data through UART 0 using ring buffer (via IRQ).
 *
 * @param   data    Pointer to data to send.
 * @param   size    Size of data to send in bytes
 */
void uart_0_send_rb(uint8_t *data, uint32_t size);

/**
 * @brief   Read data from UART 0 using ring buffer (visa IRQ).
 *
 * @param   data    Pointer where to store received data.
 * @param   size    Size of data in bytes to receive.
 *
 * @return  Actual received data size.
 */
uint32_t uart_0_read_rb(uint8_t *data, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif /* UART_H_ */
