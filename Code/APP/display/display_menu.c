/**
 **********************************************************************************************************************
 * @file        display_menu.c
 * @author      Diamond Sparrow
 * @version     1.0.0.0
 * @date        2018-02-12
 * @brief       Display menu C source file.
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
#include <math.h>

#include "display/display.h"
#include "display/display_menu.h"
#include "display/ssd1306.h"

#include "app.h"
#include "radio/radio.h"
#include "sensors/sensors.h"
#include "sensors/joystick.h"

#include "bsp.h"

/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/
#define DISPLAY_MENU_HEADER_LINE_X  0
#define DISPLAY_MENU_HEADER_LINE_Y  13
#define DISPLAY_MENU_LINE_X         1
#define DISPLAY_MENU_LINE_Y_0       1
#define DISPLAY_MENU_LINE_Y_1       18
#define DISPLAY_MENU_LINE_Y_2       30
#define DISPLAY_MENU_LINE_Y_3       42
#define DISPLAY_MENU_LINE_Y_4       54
#define DISPLAY_MENU_LINE_LENGTH    19

/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
volatile display_menu_t display_menu_list[DISPLAY_MENU_ID_LAST] = {0};

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/
extern radio_data_t radio_data;

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/
/**
 * @brief   Draw menu header.
 *
 * @param   id  Menu id. See @ref display_menu_id_t.
 * @param   str Pointer to header string.
 */
static void display_menu_header(display_menu_id_t id, uint8_t *str);

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/
static void display_menu_header(display_menu_id_t id, uint8_t *str)
{
    uint8_t tmp[DISPLAY_MENU_LINE_LENGTH] = {0};
    uint8_t x_offset = 0;

    if(display_menu_list[id].init == false)
    {
        ssd1306_draw_line(DISPLAY_MENU_HEADER_LINE_X, DISPLAY_MENU_HEADER_LINE_Y,
                          SSD1306_WIDTH, DISPLAY_MENU_HEADER_LINE_Y,
                          SSD1306_COLOR_WHITE);

        x_offset = (SSD1306_WIDTH / (DISPLAY_MENU_ID_LAST - 1)) * (id - 1);

        ssd1306_draw_filled_rectangle(x_offset, DISPLAY_MENU_HEADER_LINE_Y - 1,
                                      (SSD1306_WIDTH / (DISPLAY_MENU_ID_LAST - 1)) + 2, 2,
                                      SSD1306_COLOR_WHITE);

        snprintf((char *)tmp, DISPLAY_MENU_LINE_LENGTH, "# %-16.16s", str);
        ssd1306_goto_xy(DISPLAY_MENU_LINE_X, DISPLAY_MENU_LINE_Y_0);
        ssd1306_puts((uint8_t *)tmp, &fonts_7x10, SSD1306_COLOR_WHITE);

        //ssd1306_update_screen();
    }

    return;
}

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/
void display_menu_init(display_menu_id_t id, uint32_t period, display_menu_cb_t cb)
{
    display_menu_list[id].period = period;
    display_menu_list[id].cb = cb;
    display_menu_list[id].enable = false;
    display_menu_list[id].init = false;

    return;
}

void display_menu_cb_welcome(display_menu_id_t id)
{
    ssd1306_goto_xy(40, 20);
    ssd1306_puts((uint8_t *)"DS-2", &fonts_11x18, SSD1306_COLOR_WHITE);
    ssd1306_goto_xy(15, 40);
    ssd1306_puts((uint8_t *)"Remote Control", &fonts_7x10, SSD1306_COLOR_WHITE);

    ssd1306_update_screen();

    return;
}

void display_menu_cb_main(display_menu_id_t id)
{
    uint8_t tmp[DISPLAY_MENU_LINE_LENGTH] = {0};

    display_menu_header(id, (uint8_t *)"Main");

    switch(app_rc_mode_get())
    {
        case APP_RC_MODE_STANDBY:
            snprintf((char *)tmp, DISPLAY_MENU_LINE_LENGTH, "STA: standby");
            break;
        case APP_RC_MODE_IDLE:
            snprintf((char *)tmp, DISPLAY_MENU_LINE_LENGTH, "STA: idle");
            break;
    }
    ssd1306_goto_xy(DISPLAY_MENU_LINE_X, DISPLAY_MENU_LINE_Y_1);
    ssd1306_puts(tmp, &fonts_7x10, SSD1306_COLOR_WHITE);

    snprintf((char *)tmp, DISPLAY_MENU_LINE_LENGTH, "VEC: %d %d     ",
            sensors_data.joystick_1.magnitude,
            sensors_data.joystick_1.direction);
    ssd1306_goto_xy(DISPLAY_MENU_LINE_X, DISPLAY_MENU_LINE_Y_2);
    ssd1306_puts(tmp, &fonts_7x10, SSD1306_COLOR_WHITE);


    snprintf((char *)tmp, DISPLAY_MENU_LINE_LENGTH, "COM: %d %%      ",
            radio_data.quality);
    ssd1306_goto_xy(DISPLAY_MENU_LINE_X, DISPLAY_MENU_LINE_Y_3);
    ssd1306_puts(tmp, &fonts_7x10, SSD1306_COLOR_WHITE);

    ssd1306_update_screen();
}

void display_menu_cb_radio(display_menu_id_t id)
{
    uint8_t tmp[DISPLAY_MENU_LINE_LENGTH] = {0};

    display_menu_header(id, (uint8_t *)"Radio");

    snprintf((char *)tmp, DISPLAY_MENU_LINE_LENGTH, "TxD: %d / %d   ", radio_data.tx_counter, radio_data.tx_lost_counter);
    ssd1306_goto_xy(DISPLAY_MENU_LINE_X, DISPLAY_MENU_LINE_Y_1);
    ssd1306_puts(tmp, &fonts_7x10, SSD1306_COLOR_WHITE);

    snprintf((char *)tmp, DISPLAY_MENU_LINE_LENGTH, "RxD: %d    ", radio_data.rx_counter);
    ssd1306_goto_xy(DISPLAY_MENU_LINE_X, DISPLAY_MENU_LINE_Y_2);
    ssd1306_puts(tmp, &fonts_7x10, SSD1306_COLOR_WHITE);

    snprintf((char *)tmp, DISPLAY_MENU_LINE_LENGTH, "RTR: %d / %d   ", radio_data.rtr_current, radio_data.rtr);
    ssd1306_goto_xy(DISPLAY_MENU_LINE_X, DISPLAY_MENU_LINE_Y_3);
    ssd1306_puts(tmp, &fonts_7x10, SSD1306_COLOR_WHITE);

    snprintf((char *)tmp, DISPLAY_MENU_LINE_LENGTH, "QLT: %d %%  ", radio_data.quality);
    ssd1306_goto_xy(DISPLAY_MENU_LINE_X, DISPLAY_MENU_LINE_Y_4);
    ssd1306_puts(tmp, &fonts_7x10, SSD1306_COLOR_WHITE);

    ssd1306_update_screen();

    return;
}

void display_menu_cb_info(display_menu_id_t id)
{
    uint8_t tmp[DISPLAY_MENU_LINE_LENGTH] = {0};

    display_menu_header(id, (uint8_t *)"Info");

    snprintf((char *)tmp, DISPLAY_MENU_LINE_LENGTH, "Ver: 1.1-a1");
    ssd1306_goto_xy(DISPLAY_MENU_LINE_X, DISPLAY_MENU_LINE_Y_1);
    ssd1306_puts(tmp, &fonts_7x10, SSD1306_COLOR_WHITE);

    snprintf((char *)tmp, DISPLAY_MENU_LINE_LENGTH, "Clk: %d MHz.", (uint32_t)(bsp_get_system_core_clock() / 1000000));
    ssd1306_goto_xy(DISPLAY_MENU_LINE_X, DISPLAY_MENU_LINE_Y_2);
    ssd1306_puts(tmp, &fonts_7x10, SSD1306_COLOR_WHITE);

    ssd1306_update_screen();

    return;
}
