/**
 **********************************************************************************************************************
 * @file        indication.c
 * @author      Diamond Sparrow
 * @version     1.0.0.0
 * @date        Apr 6, 2016
 * @brief       Indication control C source file.
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
#include "bsp.h"
#include "cmsis_os2.h"

#include "indication.h"

/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/
/**
 * @brief   Buttons callback function prototype
 *
 * @note    This callback function will be called when button state changes.
 */
typedef void (*indication_callback_t)(void);

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/
/** Indication timer attributes. */
const osTimerAttr_t indication_timer_attr =
{
    .name = "INDICATION",
};

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
/** Indication timer id. */
osTimerId_t indication_timer_id;
/** Indication flag. See @ref indication_id_t. */
indication_id_t indication_flag = INDICATION_ID_OFF;
/** Indications callback functions list. */
const indication_callback_t indications_cb_list[INDICATSION_ID_LAST] =
{
    indication_cb_off,
    indication_cb_on,
    indication_cb_boot,
    indication_cb_init,
    indication_cb_standby,
    indication_cb_idle,
    indication_cb_fault,
};

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/
bool indication_init(void)
{
    if((indication_timer_id = osTimerNew(&indication_handle, osTimerPeriodic, NULL, &indication_timer_attr)) == NULL)
    {
        return false;
    }

    if((osTimerStart(indication_timer_id, INDICATION_PERIOD)) != osOK)
    {
        return false;
    }

    return true;
}

void indication_set(indication_id_t indication)
{
    __disable_irq();
    indication_flag = indication;
    __enable_irq();

    return;
}

void indication_set_blocking(indication_id_t indication)
{
    __disable_irq();
    indication_flag = indication;
    indication_handle(NULL);
    __enable_irq();

    return;
}

void indication_handle(void *arguments)
{
    indication_id_t indication = indication_flag;

    indications_cb_list[indication]();

    return;
}


void indication_cb_off(void)
{
    gpio_output_low(GPIO_ID_LED_STATUS);

    return;
}

void indication_cb_on(void)
{
    gpio_output_high(GPIO_ID_LED_STATUS);

    return;
}

void indication_cb_boot(void)
{
    gpio_output_high(GPIO_ID_LED_STATUS);

    return;
}

void indication_cb_init(void)
{
    gpio_output_high(GPIO_ID_LED_STATUS);

    return;
}

void indication_cb_standby(void)
{
    gpio_output_high(GPIO_ID_LED_STATUS);
    osDelay(50);
    gpio_output_low(GPIO_ID_LED_STATUS);

    return;
}

void indication_cb_idle(void)
{
    gpio_output_high(GPIO_ID_LED_STATUS);
    osDelay(50);
    gpio_output_low(GPIO_ID_LED_STATUS);
    osDelay(50);
    gpio_output_high(GPIO_ID_LED_STATUS);
    osDelay(50);
    gpio_output_low(GPIO_ID_LED_STATUS);

    return;
}

void indication_cb_fault(void)
{
    gpio_output_high(GPIO_ID_LED_STATUS);
    osDelay(500);
    gpio_output_low(GPIO_ID_LED_STATUS);

    return;
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/
