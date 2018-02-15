/**
 **********************************************************************************************************************
 * @file        app.c
 * @author      Diamond Sparrow
 * @version     1.0.0.0
 * @date        2016-11-21
 * @brief       Application C source file.
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
#include <stdio.h>
#include <string.h>

#include "cmsis_os2.h"

#include "app.h"
#include "bsp.h"
#include "debug.h"
#include "indication.h"

#include "cli/cli_app.h"
#include "display/display.h"
#include "radio/radio.h"

/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/
/** Application thread attributes. */
const osThreadAttr_t app_thread_attr =
{
    .name = "APP",
    .stack_size = 1024,
    .priority = osPriorityNormal,
};

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
/** Application thread ID. */
osThreadId_t app_thread_id;

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/
/**
 * @brief   Main function.
 *
 * @return  None.
 */
int main(void)
{
    bsp_init();
    indication_set_blocking(INDICATION_BOOT);
    DEBUG_BOOT("");
    DEBUG_BOOT("     .-'/ ,_  \'-.   ");
    DEBUG_BOOT("    /  (  ( >  )  \\ ");
    DEBUG_BOOT("    \\   '-' '-'   / ");
    DEBUG_BOOT("    '-..__ __..-'    ");
    DEBUG_BOOT("          /_\\       ");
    DEBUG_BOOT(" # DS-2 Remote Controller #");
    DEBUG_BOOT(" * Booting.");
    DEBUG_BOOT("%-15.15s %s %s", "Build:", __DATE__, __TIME__);
    DEBUG_BOOT("%-15.15s %ld Hz.", "Core Clock:", bsp_get_system_core_clock());

    // Setup and initialize peripherals.
    DEBUG_BOOT("%-15.15s ok.", "BSP:");

    // initialize RTOS kernel.
    if(osKernelInitialize() != osOK)
    {
        DEBUG_BOOT("%-15.15s err.", "RTOS Kernel:");
        // Invoke application error function.
        app_error();
    }
    DEBUG_BOOT("%-15.15s ok.", "RTOS Kernel:");

    // Create application thread.
    if((app_thread_id = osThreadNew(&app_thread, NULL, &app_thread_attr)) == NULL)
    {
        DEBUG_BOOT("%-15.15s err.", "APP:");
        // Invoke error function.
        app_error();
    }
    DEBUG_BOOT("%-15.15s ok.", "APP:");

    // Start kernel.
    if(osKernelStart() != osOK)
    {
        // app_error application error function.
        app_error();
    }
}

void app_thread(void *arguments)
{
    uint8_t ret = 0;

    debug_init();
    DEBUG_INIT(" * Initializing.");
    indication_set_blocking(INDICATION_INIT);
    
    ret = indication_init();
    DEBUG_INIT("Indication .. %s.", ret ? "ok" : "err");
    ret = cli_app_init();
    DEBUG_INIT("CLI APP ..... %s.", ret ? "ok" : "err");
    ret = display_init();
    DEBUG_INIT("Display ..... %s.", ret == false ? "err" : "ok");
    ret = radio_init();
    DEBUG_INIT("Radio ..... %s.", ret == false ? "err" : "ok");

    DEBUG(" * Running.");
    indication_set(INDICATION_IDLE);
    DEBUG("State: idle.");

    while(1)
    {
        osDelay(100);
    }
}

void app_error(void)
{
    indication_set_blocking(INDICATION_FAULT);
    debug_send_blocking((uint8_t *)"FATAL ERROR!\r", 13);

    while(1)
    {
        __nop();
    }
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/
