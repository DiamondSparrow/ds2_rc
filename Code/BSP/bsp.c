/**
 **********************************************************************************************************************
 * @file         bsp.c
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

#include "bsp.h"

#include "periph/adc.h"
#include "periph/gpio.h"
#include "periph/ssp.h"
#include "periph/uart.h"
#include "periph/wdt.h"

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
/** Saved reset status. */
static uint32_t bsp_rst_status = 0;
/** Unique ID of MCU. */
volatile uint32_t bsp_mcu_uid[4] = {0};

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/
/**
 * @brief   Read unique MCU id. Will be stored in @ref bsp_uid.
 */
static void bsp_read_mcu_uid(void);

/**
 * @brief   Read MCU reset status. Will be stored in @ref bsp_rst_status.
 */
static void bsp_read_rst_status(void);

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/
void bsp_init(void)
{
    SystemCoreClockUpdate();

    /* Enable SWM and IOCON clocks */
    Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_IOCON);

    //wdt_init();
    gpio_init();
    adc_init();
    ssp_0_init();
    ssp_1_init();
    uart_0_init();

    bsp_read_mcu_uid();
    bsp_read_rst_status();

    return;
}

uint32_t bsp_get_reset_cause(void)
{
    return bsp_rst_status;
}

uint32_t bsp_get_system_core_clock(void)
{
    return SystemCoreClock;
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/
static void bsp_read_mcu_uid(void)
{
    uint32_t command[5] = {0};

    command[0] = IAP_READ_UID_CMD;
    __disable_irq();
    iap_entry(command, (uint32_t *)bsp_mcu_uid);
    __enable_irq();

    return;
}

static void bsp_read_rst_status(void)
{
    bsp_rst_status = Chip_SYSCTL_GetSystemRSTStatus();
    Chip_SYSCTL_ClearSystemRSTStatus(bsp_rst_status);

    return;
}
