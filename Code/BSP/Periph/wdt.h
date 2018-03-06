/**
 **********************************************************************************************************************
 * @file        wdt.h
 * @author      Deimantas Zvirblis
 * @version     1.0.0.0
 * @date        2017-04-11
 * @brief       Watchdog and Brown-Out Detect (BOD) C header file.
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

#ifndef WDT_H_
#define WDT_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include <stdbool.h>

/**********************************************************************************************************************
 * Exported definitions and macros
 *********************************************************************************************************************/
#define WDT_TIMEOUT_S           4       //!< Watchdog timeout in seconds.
#define WDT_WARNING_TICKS       1023    //!< Maximum 10 bits (1023).

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
 * @brief   Initialize wathcdog.
 */
void wdt_init(void);

/**
 * @brief   Initialize Brawn Out Detect.
 *
 * @note    BOD is a part of watchdog, therefore watchdog should be initialized before.
 *
 * @warning When BOD is enabled, current consumption of device will increase.
 */
void wdt_bod_init(void);

/**
 * @brief   Feed watchdog.
 */
void wdt_feed(void);

/**
 * @brief   Watchdog software feed.
 *
 * @return  Feed flag.
 * @retval  0   no feed.
 * @retval  1   feed.
 */
bool wdt_feed_soft(void);

/**
 * @brief   Get watchdog initialization flag.
 *
 * @return  Flag.
 * @retval  0 - watchdog not running.
 * @retval  1 - watchdog running.
 */
bool wdt_get_init_flag(void);

#ifdef __cplusplus
}
#endif

#endif /* WDT_H_ */
