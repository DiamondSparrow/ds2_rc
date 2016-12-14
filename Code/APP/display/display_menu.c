/**
 **********************************************************************************************************************
 * @file         display_menu.c
 * @author       Diamond Sparrow
 * @version      1.0.0.0
 * @date         2016-12-14
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

#include "display.h"
#include "display_menu.h"
#include "ssd1306.h"

#include "chip.h"

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/
#define DISPLAY_LINE_X      3
#define DISPLAY_MENU_LINE_Y_1    18
#define DISPLAY_MENU_LINE_Y_2    29
#define DISPLAY_MENU_LINE_Y_3    40
#define DISPLAY_MENU_LINE_Y_4    51

/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
volatile display_menu_t display_menu_list[DISPLAY_MENU_ID_LAST] = {0};

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/
static void display_menu_header(display_menu_id_t id, uint8_t *str);

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
    //ssd1306_draw_rectangle(0, 0, SSD1306_WIDTH - 1, SSD1306_HEIGHT - 1, SSD1306_COLOR_WHITE);

    ssd1306_goto_xy(40, 13);
    ssd1306_puts((uint8_t *)"Remote", &fonts_7x10, SSD1306_COLOR_WHITE);
    ssd1306_goto_xy(40, 25);
    ssd1306_puts((uint8_t *)"DS-2", &fonts_11x18, SSD1306_COLOR_WHITE);
    ssd1306_goto_xy(27, 45);
    ssd1306_puts((uint8_t *)"Controller", &fonts_7x10, SSD1306_COLOR_WHITE);

    ssd1306_update_screen();

    return;
}

void display_menu_cb_clock(display_menu_id_t id)
{
    //uint8_t tmp[16] = {0};
    //uint32_t timestamp = rtc_get();
    //struct tm clock = {0};

    //ConvertRtcTime(timestamp, &clock);

    display_menu_header(id, "Clock");
/*
    ssd1306_goto_xy(20, 23);
    snprintf((char *)tmp, sizeof(tmp), "%02d:%02d:%02d", clock.tm_hour, clock.tm_min, clock.tm_sec);
    ssd1306_puts((uint8_t *)tmp, &fonts_11x18, SSD1306_COLOR_WHITE);
    ssd1306_goto_xy(29, 44);
    snprintf((char *)tmp, sizeof(tmp), "%04d-%02d-%02d", clock.tm_year + 1970, clock.tm_mon + 1, clock.tm_mday);
    ssd1306_puts((uint8_t *)tmp, &fonts_7x10, SSD1306_COLOR_WHITE);
*/
    ssd1306_update_screen();

    return;
}

void display_menu_cb_joystick(display_menu_id_t id)
{
    uint8_t tmp[18] = {0};
    int32_t magn = 0;
    int32_t dir = 0;

    display_menu_header(id, "Joystick");

    snprintf((char *)tmp, 18, "X:  %d       ", 0); // joystick_get_x(JOYSTICK_ID_1));
    ssd1306_goto_xy(DISPLAY_LINE_X, DISPLAY_MENU_LINE_Y_1);
    ssd1306_puts(tmp, &fonts_7x10, SSD1306_COLOR_WHITE);

    snprintf((char *)tmp, 18, "Y:  %d       ", 0); // jjoystick_get_y(JOYSTICK_ID_1));
    ssd1306_goto_xy(DISPLAY_LINE_X, DISPLAY_MENU_LINE_Y_2);
    ssd1306_puts(tmp, &fonts_7x10, SSD1306_COLOR_WHITE);

    //joystick_get_vector(JOYSTICK_ID_1, &magn, &dir);
    snprintf((char *)tmp, 18, "V:  %d %d      ", magn, dir);
    ssd1306_goto_xy(DISPLAY_LINE_X, DISPLAY_MENU_LINE_Y_3);
    ssd1306_puts(tmp, &fonts_7x10, SSD1306_COLOR_WHITE);

    snprintf((char *)tmp, 18, "SW: %01d   ", 0); // jjoystick_get_sw(JOYSTICK_ID_1));
    ssd1306_goto_xy(DISPLAY_LINE_X, DISPLAY_MENU_LINE_Y_4);
    ssd1306_puts(tmp, &fonts_7x10, SSD1306_COLOR_WHITE);

    ssd1306_update_screen();

    return;
}

void display_menu_cb_info(display_menu_id_t id)
{
    uint8_t tmp[18] = {0};

    display_menu_header(id, "Info");

    snprintf((char *)tmp, 18, "Ver: 1.1-a1");
    ssd1306_goto_xy(DISPLAY_LINE_X, DISPLAY_MENU_LINE_Y_1);
    ssd1306_puts(tmp, &fonts_7x10, SSD1306_COLOR_WHITE);

    snprintf((char *)tmp, 18, "Clk: %ld MHz.", SystemCoreClock / 1000000);
    ssd1306_goto_xy(DISPLAY_LINE_X, DISPLAY_MENU_LINE_Y_2);
    ssd1306_puts(tmp, &fonts_7x10, SSD1306_COLOR_WHITE);

//    snprintf((char *)tmp, 18, "Tmp: %.02f degC.", adc_get_temperature());
//    ssd1306_goto_xy(DISPLAY_LINE_X, DISPLAY_MENU_LINE_Y_3);
//    ssd1306_puts(tmp, &fonts_7x10, SSD1306_COLOR_WHITE);

    ssd1306_update_screen();

    return;
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/
static void display_menu_header(display_menu_id_t id, uint8_t *str)
{
    uint8_t tmp[24] = {0};

    if(display_menu_list[id].init == false)
    {
        //ssd1306_draw_rectangle(0, 0, SSD1306_WIDTH - 1, SSD1306_HEIGHT - 1, SSD1306_COLOR_WHITE);
        //ssd1306_draw_rectangle(0, 0, SSD1306_WIDTH, 13, SSD1306_COLOR_WHITE);
        //ssd1306_draw_filled_rectangle(0, 0, SSD1306_WIDTH, 13, SSD1306_COLOR_WHITE);

        ssd1306_draw_line(0, 13, SSD1306_WIDTH, 13, SSD1306_COLOR_WHITE);

        snprintf((char *)tmp, 24, "< %-13.13s >", str);
        ssd1306_goto_xy(4, 1);
        ssd1306_puts((uint8_t *)tmp, &fonts_7x10, SSD1306_COLOR_WHITE);

        ssd1306_update_screen();
    }

    return;
}
