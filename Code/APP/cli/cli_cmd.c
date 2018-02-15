/**
 **********************************************************************************************************************
 * @file        cli_cmd.c
 * @author      Diamond Sparrow
 * @version     1.0.0.0
 * @date        2016-04-10
 * @brief       Command line ineterface (CLI) commands C source file.
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
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "cli_cmd.h"
#include "cli.h"

#include "debug.h"
#include "common.h"
#include "bsp.h"

#include "cmsis_os2.h"

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
/** The definition of the list of commands. Commands that are registered are added to this list. */
const cli_cmd_t cli_cmd_list[CLI_CMD_COUNT] =
{
    {
        (const uint8_t *)"help",
        (const uint8_t *)"help      Lists all the registered commands.",
        cli_cmd_cb_help,
        0,
    },
    {
        (const uint8_t *)"info",
        (const uint8_t *)"info      Shows device information.",
        cli_cmd_cb_info,
        0,
    },
    {
        (const uint8_t *)"os_info",
        (const uint8_t *)"os_info   Get OS  information (thread stack size, etcs).",
        cli_cmd_cb_os_info,
        0,
    }
};

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/
extern osThreadId_t app_thread_id;
extern osThreadId_t cli_app_thread_id;
extern osThreadId_t display_thread_id;
extern osThreadId_t radio_thread_id;

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/
static void cli_cmd_os_info_print(osThreadId_t id);

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/
bool cli_cmd_cb_help(uint8_t *data, uint32_t size, const uint8_t *cmd)
{
    uint8_t i = 0;

    UNUSED_VARIABLE(cmd);

    /* Return the next command help string, before moving the pointer on to
     the next command in the list. */
    for(i = 0; i < CLI_CMD_COUNT; i++)
    {
        if(strlen((char *)cli_cmd_list[i].help) > 2)
        {
            DEBUG("%s", cli_cmd_list[i].help);
        }
    }

    return false;
}

bool cli_cmd_cb_info(uint8_t *data, uint32_t size, const uint8_t *cmd)
{
    UNUSED_VARIABLE(cmd);

    DEBUG("Device ...... DS-2 Controller");
    DEBUG("Build ....... %s %s", __DATE__, __TIME__);
    DEBUG("Core Clock .. %ld MHz.", bsp_get_system_core_clock());

    return false;
}

bool cli_cmd_cb_os_info(uint8_t *data, uint32_t size, const uint8_t *cmd)
{
    DEBUG("# OS info:");
    cli_cmd_os_info_print(app_thread_id);
    cli_cmd_os_info_print(cli_app_thread_id);
    cli_cmd_os_info_print(display_thread_id);
    cli_cmd_os_info_print(radio_thread_id);

    return false;
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/
static void cli_cmd_os_info_print(osThreadId_t id)
{
    DEBUG("- %s: prio = %d, stat = %d, sz = %d/%d B.;",
          osThreadGetName(id),
          osThreadGetState(id),
          osThreadGetPriority(id),
          osThreadGetStackSize(id),
          osThreadGetStackSpace(id));

    return;
}
