/**
 **********************************************************************************************************************
 * @file        app.h
 * @author      Diamond Sparrow
 * @version     1.0.0.0
 * @date        2016-11-21
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

#ifndef APP_H_
#define APP_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/

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
 * @brief   Application main thread.
 *
 * @param   arguments   Pointer to thread arguments.
 */
void app_thread(void *arguments);

/**
 * @brief   Application error handler.
 */
void app_error(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_H_ */
