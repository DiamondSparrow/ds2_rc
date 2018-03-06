/**
 **********************************************************************************************************************
 * @file        ssp
 * @author      Diamond Sparrow
 * @version     1.0.0.0
 * @date        2016-08-30
 * @brief       SSP C header file.
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

#ifndef SSP_H_
#define SSP_H_

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
 * @brief   Initialize SSP 0.
 */
void ssp_0_init(void);

/**
 * @brief   Read data from SSP 0.
 *
 * @param   buffer  Pointer where read data should be stored.
 * @param   size    Size of data to read in bytes.
 */
void ssp_0_read_buffer(uint8_t *buffer, uint16_t size);
void ssp_0_write_buffer(uint8_t *buffer, uint16_t size);
void ssp_0_write_read(uint8_t *tx, uint16_t tx_size, uint8_t *rx, uint16_t rx_size);

/**
 * @brief   Initialize SSP 1.
 */
void ssp_1_init(void);
void ssp_1_send_byte(uint8_t data);
void ssp_1_send_buffer(uint8_t *buffer, uint32_t size);
uint8_t ssp_1_recv_byte(void);
void ssp_1_recv_buffer(uint8_t *buffer, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif /* SSP_H_ */
