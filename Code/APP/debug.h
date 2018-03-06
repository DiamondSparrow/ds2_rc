/**
 **********************************************************************************************************************
 * @file        debug.h
 * @author      Diamond Sparrow
 * @version     1.0.0.0
 * @date        2016-04-10
 * @brief       DEbug C header file.
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

#ifndef DEBUG_H_
#define DEBUG_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/**********************************************************************************************************************
 * Exported definitions and macros
 *********************************************************************************************************************/
/** Debug macros: */
#define DEBUG_BOOT(F, ...)      debug_send(F "\r", ##__VA_ARGS__)
#define DEBUG_INIT(F, ...)      debug_send_os(F "\r", ##__VA_ARGS__)
#define DEBUG(F, ...)           debug_send_os(F "\r", ##__VA_ARGS__)
#define DEBUG_RADIO(F, ...)     debug_send_os("[RADIO] " F "\r", ##__VA_ARGS__)
#define DEBUG_DISPLAY(F, ...)   debug_send_os("[DISPLAY] " F "\r", ##__VA_ARGS__)
#define DEBUG_SENSORS(F, ...)   debug_send_os("[SENSORS] " F "\r", ##__VA_ARGS__)

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
 * @brief   Initialize debug.
 *
 * @return  State of initialization.
 * @retval  0   failed.
 * @retval  1   success.
 */
bool debug_init(void);

/**
 * @brief   Send debug using std args (printf format).
 *
 * @note    Should be used when no OS running.
 *
 * @param   fmt     Pointer to debug message format.
 */
void debug_send(const char *fmt, ...);

/**
 * @brief   Send debug using std args (printf format).
 *
 * @note    Should be used when OS running.
 *
 * @param   fmt     Pointer to debug message format.
 */
void debug_send_os(const char *fmt, ...);

/**
 * @brief   Send data buffer as hex.
 *
 * @param   buffer  Pointer to data buffer.
 * @param   size    Size of data buffer in byets.
 */
void debug_send_hex_os(uint8_t *buffer, uint16_t size);

/**
 * @brief   Send debug in simple blocking way.
 *
 * @note    Should be used in critical conditions like IRQ (hardfault etc.).
 *
 * @param   data    Pointer to data which needs to be sent as debug message.
 * @param   size    Size of data to send in bytes.
 */
void debug_send_blocking(uint8_t *data, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_H_ */
