/**
 **********************************************************************************************************************
 * @file        wdt.c
 * @author      Deimantas Zvirblis
 * @version     1.0.0.0
 * @date        2017-04-11
 * @brief       Watchdog and Brown-Out Detect (BOD) C source file.
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

#include "periph/wdt.h"

#include "chip.h"

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
/** Watchdog feed flag. If true watchdog feed ok, else - no feed. */
__IO bool wdt_feed_flag = false;
/** Watchdog initialization flag. */
static bool wdt_init_flag = false;

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/
void wdt_init(void)
{
    uint32_t freq = 0;

    // WDT oscillator freq = 0.6MHz divided by 64 = 9.375khz
    Chip_Clock_SetWDTOSC(WDTLFO_OSC_0_60, 64);

    // Enable the power to the WDT
    Chip_SYSCTL_PowerUp(SYSCTL_SLPWAKE_WDTOSC_PD);

    // The WDT divides the input frequency into it by 4
    freq = Chip_Clock_GetWDTOSCRate() / 4;

    // Initialize WWDT (also enables WWDT clock)
    Chip_WWDT_Init(LPC_WWDT);

    // Use WDTOSC as the WDT clock
    Chip_WWDT_SelClockSource(LPC_WWDT, WWDT_CLKSRC_WATCHDOG_WDOSC);

    /*
     * Set watchdog feed time constant to approximately 2s
     * Set watchdog warning time to 512 ticks after feed time constant
     * Set watchdog window time to 3s
     */
    Chip_WWDT_SetTimeOut(LPC_WWDT, freq * WDT_TIMEOUT_S);
    Chip_WWDT_SetWarning(LPC_WWDT, WDT_WARNING_TICKS > 1023 ? 1023 : WDT_WARNING_TICKS);

    // Configure WWDT to reset on timeout
    Chip_WWDT_SetOption(LPC_WWDT, WWDT_WDMOD_WDRESET);

    // Clear watchdog warning and timeout interrupts
    Chip_WWDT_ClearStatusFlag(LPC_WWDT, WWDT_WDMOD_WDTOF | WWDT_WDMOD_WDINT);

    // Enable RTC as a peripheral wake up event
    Chip_SYSCTL_EnablePeriphWakeup(SYSCTL_WAKEUP_BOD_WDT_INT);

    // Clear and enable watchdog interrupt
    NVIC_ClearPendingIRQ(BOD_WDT_IRQn);
    NVIC_EnableIRQ(BOD_WDT_IRQn);

    /* Start watchdog */
    Chip_WWDT_Start(LPC_WWDT);
    wdt_init_flag = true;

    return;
}

void wdt_bod_init(void)
{
    /* Set brown-out interrupt at 2.8v and reset at 1.46v  */
    Chip_SYSCTL_SetBODLevels(SYSCTL_BODRSTLVL_LEVEL3, SYSCTL_BODINTVAL_2_LEVEL2);
    Chip_SYSCTL_EnableBODReset();

    return;
}

void wdt_feed(void)
{
    __disable_irq();
    Chip_WWDT_Feed(LPC_WWDT);
    __enable_irq();

    return;
}

bool wdt_feed_soft(void)
{
    __disable_irq();
    wdt_feed_flag = true;
    __enable_irq();

    return wdt_feed_flag;
}

bool wdt_get_init_flag(void)
{
    return wdt_init_flag;
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/
/**
 * @brief   Brown Out Detection and Watchdog IRQ handler.
 */
void BOD_WDT_IRQHandler(void)
{
    uint32_t status = Chip_WWDT_GetStatus(LPC_WWDT);

    // Handle warning interrupt
    if(status & WWDT_WDMOD_WDINT)
    {
        if(wdt_feed_flag == true)
        {
            Chip_WWDT_Feed(LPC_WWDT);
            wdt_feed_flag = false;
        }
        /* A watchdog feed didn't occur prior to warning timeout */
        Chip_WWDT_ClearStatusFlag(LPC_WWDT, WWDT_WDMOD_WDINT);
    }
    /*
     * The chip will reset before this happens, but if the WDT doesn't
     * have WWDT_WDMOD_WDRESET enabled, this will hit once
     */
    if(status & WWDT_WDMOD_WDTOF)
    {
        // A watchdog feed didn't occur prior to window timeout
        Chip_WWDT_UnsetOption(LPC_WWDT, WWDT_WDMOD_WDEN);       // Stop WDT
        Chip_WWDT_ClearStatusFlag(LPC_WWDT, WWDT_WDMOD_WDTOF);  // Clear flags
        Chip_WWDT_Start(LPC_WWDT);                              // Needs restart
    }

    __DSB();

    return;
}
