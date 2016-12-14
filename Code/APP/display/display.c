/**
 **********************************************************************************************************************
 * @file         display.c
 * @author       Diamond Sparrow
 * @version      1.0.0.0
 * @date         2016-09-01
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
#include <stdio.h>
#include <string.h>

#include "display/display.h"
#include "display/display_menu.h"
#include "display/ssd1306.h"

#include "cmsis_os.h"
#include "bsp.h"

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/
/** Define application thread */
osThreadDef(display_thread, osPriorityNormal, 1, 1024);

/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
/** Display thread ID. */
osThreadId display_thread_id;

volatile bool display_power_cntrl = true;
volatile display_menu_id_t display_curr_menu_id = DISPLAY_MENU_ID_WELCOME;

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/
extern display_menu_t display_menu_list[DISPLAY_MENU_ID_LAST];

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/

void display_power_on(void);
void display_power_off(void);
static void display_delay(display_menu_id_t id);

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/
bool display_init(void)
{
    if(ssd1306_init() == false)
    {
        return false;
    }

    display_menu_init(DISPLAY_MENU_ID_WELCOME, 0, display_menu_cb_welcome);
    display_menu_init(DISPLAY_MENU_ID_CLOCK, 1000, display_menu_cb_clock);
    display_menu_init(DISPLAY_MENU_ID_JOYSTICK, 50, display_menu_cb_joystick);
    display_menu_init(DISPLAY_MENU_ID_INFO, 1000, display_menu_cb_info);

    display_set_menu(DISPLAY_MENU_ID_WELCOME);

    // Create display thread.
    if((display_thread_id = osThreadCreate(osThread(display_thread), NULL)) == NULL)
    {
        // Failed to create a thread.
        return false;
    }

    return true;
}

void display_turn_off(void)
{
    __disable_irq();
    display_power_cntrl = false;
    __enable_irq();

    return;
}

void display_turn_on(void)
{
    __disable_irq();
    display_power_cntrl = true;
    __enable_irq();

    return;
}

void display_thread(void const *arg)
{
    display_menu_id_t id = DISPLAY_MENU_ID_WELCOME;

    osDelay(500);
    ssd1306_update_screen();
    osDelay(10);

    while(1)
    {
        while(!display_power_cntrl)
        {
            osDelay(100);
        }
        display_power_on();
        while(display_power_cntrl)
        {
            id = display_curr_menu_id;
            if(display_menu_list[id].enable == true)
            {
                if(display_menu_list[id].init == false)
                {
                    ssd1306_fill(SSD1306_COLOR_BLACK);
                }
                if(display_menu_list[id].cb != NULL)
                {
                    display_menu_list[id].cb(id);
                }
                if(display_menu_list[id].period)
                {
                    display_delay(id);
                }
                else
                {
                    display_menu_list[id].enable = false;
                }
                display_menu_list[id].init = true;
            }
            else
            {
                display_delay(id);
            }
        }
        display_power_off();
    }
}

void display_set_menu(display_menu_id_t id)
{
    __disable_irq();
    display_curr_menu_id = id;
    display_menu_list[id].enable = true;
    display_menu_list[id].init = false;
    __enable_irq();

    return;
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/
static void display_power_on(void)
{
    //ssp_1_init();
    ssd1306_display_on();

    return;
}

static void display_power_off(void)
{
    ssd1306_display_off();
    //ssp_1_deinit();

    return;
}

static void display_delay(display_menu_id_t id)
{
    static uint32_t c = 0;
    uint32_t delay = display_menu_list[id].period == 0 ? 10 : display_menu_list[id].period;

    while(delay--)
    {
        osDelay(1);
        c++;
        if(c > 100)
        {
            c = 0;
        }
        if(id != display_curr_menu_id)
        {
            break;
        }
    }

    return;
}

