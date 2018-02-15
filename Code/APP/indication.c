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

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
osTimerId_t indication_timer_id;
/** Indication timer attributes. */
const osTimerAttr_t indication_timer_attr =
{
    .name = "INDICATION",
};

indication_t indication_flag = INDICATION_OFF;

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/
/**
 * @brief   Indication 'INDICATION_OFF' control.
 */
static void indication_cntrl_off(void);

/**
 * @brief   Indication 'INDICATION_ON' control.
 */
static void indication_cntrl_on(void);

/**
 * @brief   Indication 'INDICATION_BOOT' control.
 */
static void indication_cntrl_boot(void);

/**
 * @brief   Indication 'INDICATION_INIT' control.
 */
static void indication_cntrl_init(void);

/**
 * @brief   Indication 'INDICATION_STANDBY' control.
 */
static void indication_cntrl_standby(void);

/**
 * @brief   Indication 'INDICATION_IDLE' control.
 */
static void indication_cntrl_idle(void);

/**
 * @brief   Indication 'INDICATION_FAULT' control.
 */
static void indication_cntrl_fault(void);

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

void indication_set(indication_t indication)
{
    __disable_irq();
    indication_flag = indication;
    __enable_irq();

    return;
}

void indication_set_blocking(indication_t indication)
{
    __disable_irq();
    indication_flag = indication;
    indication_handle(NULL);
    __enable_irq();

    return;
}

void indication_handle(void *arguments)
{
    indication_t indication = indication_flag;

    switch(indication)
    {
        default:
        case INDICATION_OFF:
            indication_cntrl_off();
            break;
        case INDICATION_ON:
            indication_cntrl_on();
            break;
        case INDICATION_BOOT:
            indication_cntrl_boot();
            break;
        case INDICATION_INIT:
            indication_cntrl_init();
            break;
        case INDICATION_STANDBY:
            indication_cntrl_standby();
            break;;
        case INDICATION_IDLE:
            indication_cntrl_idle();
            break;
        case INDICATION_FAULT:
            indication_cntrl_fault();
            break;
    }

    return;
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/
static void indication_cntrl_off(void)
{
    gpio_output_low(GPIO_ID_LED_STATUS);

    return;
}

static void indication_cntrl_on(void)
{
    gpio_output_high(GPIO_ID_LED_STATUS);

    return;
}

static void indication_cntrl_boot(void)
{
    gpio_output_high(GPIO_ID_LED_STATUS);

    return;
}

static void indication_cntrl_init(void)
{
    gpio_output_high(GPIO_ID_LED_STATUS);

    return;
}

static void indication_cntrl_standby(void)
{
    gpio_output_high(GPIO_ID_LED_STATUS);
    osDelay(50);
    gpio_output_low(GPIO_ID_LED_STATUS);

    return;
}

static void indication_cntrl_idle(void)
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

static void indication_cntrl_fault(void)
{
    gpio_output_high(GPIO_ID_LED_STATUS);
    osDelay(500);
    gpio_output_low(GPIO_ID_LED_STATUS);

    return;
}
