/**
 **********************************************************************************************************************
 * @file         gpio.c
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
#include <stdbool.h>

#include "chip.h"

#include "gpio.h"

/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/
typedef struct
{
    uint8_t port;       /**< GPIO port number. */
    uint8_t pin;        /**< GPIO Pin number. */
    bool dir;           /**< Pin directions: 0 - input, 1 - output. */
    bool state;         /**< Pin state: 0 - low, 1 - high. */
    uint32_t modefunc;  /**< GPIO pin mode and/or function. */
} gpio_item_t;

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/
const gpio_item_t gpio_list[GPIO_ID_LAST] =
{
    {.port = 2, .pin = 19, .dir = true,   .state = false,  .modefunc = IOCON_FUNC0 | IOCON_MODE_INACT,},  // GPIO_ID_LED_STATUS
    {.port = 0, .pin = 7,  .dir = true,   .state = true,   .modefunc = IOCON_FUNC0 | IOCON_MODE_INACT,},  // GPIO_ID_DISPLAY_SELECT
    {.port = 1, .pin = 28, .dir = true,   .state = true,   .modefunc = IOCON_FUNC0 | IOCON_MODE_INACT,},  // GPIO_ID_DISPLAY_DC
    {.port = 1, .pin = 24, .dir = true,   .state = true,   .modefunc = IOCON_FUNC0 | IOCON_MODE_INACT,},  // GPIO_ID_DISPLAY_RESTART
    {.port = 2, .pin = 6,  .dir = true,   .state = false,  .modefunc = IOCON_FUNC0 | IOCON_MODE_INACT,},  // GPIO_ID_NRF24L01_CE
    {.port = 1, .pin = 23, .dir = true,   .state = true,   .modefunc = IOCON_FUNC0 | IOCON_MODE_INACT,},  // GPIO_ID_NRF24L01_CSN
    {.port = 1, .pin = 9,  .dir = false,  .state = false, .modefunc = IOCON_FUNC0 | IOCON_MODE_PULLUP,}, // GPIO_ID_JOYSTICK_LEFT_SW
    {.port = UINT8_MAX, .pin = UINT8_MAX, .dir = false, .state = false, .modefunc = IOCON_FUNC0 | IOCON_MODE_PULLUP,}, // GPIO_ID_JOYSTICK_RIGHT_SW
    {.port = 0, .pin = 13, .dir = false,  .state = false, .modefunc = IOCON_FUNC0 | IOCON_MODE_PULLUP,}, // GPIO_ID_BUTTON_LEFT
    {.port = 0, .pin = 14, .dir = false,  .state = false, .modefunc = IOCON_FUNC0 | IOCON_MODE_PULLUP,}, // GPIO_ID_BUTTON_RIGHT
};

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/
/**
 * @brief   Check if GPIO is enabled.
 *
 * @param   id      GPIO id. See @ref gpio_id_t.
 *
 * @return  GPIO enable/disable state.
 * @retval  0   disabled.
 * @retval  1   enabled.
 */
static bool gpio_is_enabled(gpio_id_t id);

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/
void gpio_init(void)
{
    uint8_t i = 0;

    Chip_GPIO_Init(LPC_GPIO);

    for(i = 0; i < GPIO_ID_LAST; i++)
    {
        if(!gpio_is_enabled((gpio_id_t)i))
        {
            continue;
        }

        Chip_GPIO_SetPinDIR(LPC_GPIO, gpio_list[i].port, gpio_list[i].pin, gpio_list[i].dir);
        if(gpio_list[i].dir == true)
        {
            Chip_GPIO_SetPinState(LPC_GPIO, gpio_list[i].port, gpio_list[i].pin, gpio_list[i].state);
        }
    }

    return;
}

void gpio_output(gpio_id_t id)
{
    if(!gpio_is_enabled(id))
    {
        return;
    }

    Chip_GPIO_SetPinDIROutput(LPC_GPIO, gpio_list[id].port, gpio_list[id].pin);

    return;
}

void gpio_output_set(gpio_id_t id, bool state)
{
    if(!gpio_is_enabled(id))
    {
        return;
    }

    Chip_GPIO_SetPinState(LPC_GPIO, gpio_list[id].port, gpio_list[id].pin, state);

    return;
}

void gpio_output_low(gpio_id_t id)
{
    if(!gpio_is_enabled(id))
    {
        return;
    }

    Chip_GPIO_SetPinState(LPC_GPIO, gpio_list[id].port, gpio_list[id].pin, false);

    return;
}

void gpio_output_high(gpio_id_t id)
{
    if(!gpio_is_enabled(id))
    {
        return;
    }

    Chip_GPIO_SetPinState(LPC_GPIO, gpio_list[id].port, gpio_list[id].pin, true);

    return;
}

void gpio_output_toggle(gpio_id_t id)
{
    if(!gpio_is_enabled(id))
    {
        return;
    }

    Chip_GPIO_SetPinToggle(LPC_GPIO, gpio_list[id].port, gpio_list[id].pin);

    return;
}

void gpio_input(gpio_id_t id)
{
    if(!gpio_is_enabled(id))
    {
        return;
    }

    Chip_GPIO_SetPinDIRInput(LPC_GPIO, gpio_list[id].port, gpio_list[id].pin);

    return;
}

bool gpio_input_get(gpio_id_t id)
{
    if(!gpio_is_enabled(id))
    {
        return false;
    }

    return Chip_GPIO_ReadPortBit(LPC_GPIO, gpio_list[id].port, gpio_list[id].pin);
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/
static bool gpio_is_enabled(gpio_id_t id)
{
    if(gpio_list[id].port == UINT8_MAX || gpio_list[id].pin == UINT8_MAX)
    {
        return false;
    }

    return true;
}
