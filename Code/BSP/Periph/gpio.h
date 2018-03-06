/**
 **********************************************************************************************************************
 * @file        gpio.h
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

#ifndef GPIO_H_
#define GPIO_H_

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

/**********************************************************************************************************************
 * Exported types
 *********************************************************************************************************************/
/**
 * @brief   List of GPIO's.
 */
typedef enum
{
    GPIO_ID_LED_STATUS,       //!< Status LED GPIO.
    GPIO_ID_DISPLAY_SELECT,   //!< Display select GPIO.
    GPIO_ID_DISPLAY_DC,       //!< Display DC pin GPIO>
    GPIO_ID_DISPLAY_RESTART,  //!< Display restart GPIO.
    GPIO_ID_NRF24L01_CE,      //!< NRF24L01 CE pin GPIO.
    GPIO_ID_NRF24L01_CSN,     //!< NRF24L01 CSN pin GPIO.
    GPIO_ID_JOYSTICK_LEFT_SW, //!< Left joystick switch GPIO.
    GPIO_ID_JOYSTICK_RIGHT_SW,//!< Right joystick switch GPIO.
    GPIO_ID_BUTTON_LEFT,      //!< Left button GPIO.
    GPIO_ID_BUTTON_RIGHT,     //!< Right button GPIO.
    GPIO_ID_LAST,             //!< Last should stay last.
} gpio_id_t;

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
 * @brief   Initialize GPIO's.
 */
void gpio_init(void);

/**
 * @brief   Set GPIO as output.
 *
 * @param   id      GPIO id. See @ref gpio_id_t.
 */
void gpio_output(gpio_id_t id);

/**
 * @brief   Set output state.
 *
 * @param   id      GPIO id. See @ref gpio_id_t.
 * @param   state   New GPIO output state. 0 - low, 1 - high.
 */
void gpio_output_set(gpio_id_t id, bool state);

/**
 * @brief   Set GPIO output to low.
 *
 * @param   id      GPIO id. See @ref gpio_id_t.
 */
void gpio_output_low(gpio_id_t id);

/**
 * @brief   Set GPIO output to high.
 *
 * @param   id      GPIO id. See @ref gpio_id_t.
 */
void gpio_output_high(gpio_id_t id);

/**
 * @brief   Toggle GPIO output.
 *
 * @param   id      GPIO id. See @ref gpio_id_t.
 */
void gpio_output_toggle(gpio_id_t id);

/**
 * @brief   Set GPIO as input.
 *
 * @param   id      GPIO id. See @ref gpio_id_t.
 */
void gpio_input(gpio_id_t id);

/**
 * @brief  Get input GPIO state.
 *
 * @param   id      GPIO id. See @ref gpio_id_t.
 *
 * @return  State of input.
 * @retval  0   low
 * @retval  1   high
 */
bool gpio_input_get(gpio_id_t id);

#ifdef __cplusplus
}
#endif

#endif /* GPIO_H_ */
