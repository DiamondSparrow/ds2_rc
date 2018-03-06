/**
 **********************************************************************************************************************
 * @file         sensors.c
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

#include "sensors/sensors.h"
#include "sensors/joystick.h"

#include "display/display.h"

#include "debug.h"
#include "cmsis_os2.h"

/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/
#define SENSORS_READ_PERIOD     10  //!< Sensors read period in milliseconds.

/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/
/** Sensors thread attributes. */
const osThreadAttr_t sensors_thread_attr =
{
    .name = "SENSORS",
    .stack_size = 512,
    .priority = osPriorityNormal,
};

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
/** Sensors thread ID. */
osThreadId_t sensors_thread_id;
/** Sensors data structure. */
volatile sensors_data_t sensors_data;

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/
static void sensors_joystick_handler(void);

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/
bool sensors_init(void)
{
    // Create sensors thread.
    if((sensors_thread_id = osThreadNew(&sensors_thread, NULL, &sensors_thread_attr)) == NULL)
    {
        // Failed to create a thread.
        return false;
    }

    return true;
}

void sensors_thread(void *arguments)
{
    while(1)
    {
        osDelay(SENSORS_READ_PERIOD);
        sensors_joystick_handler();
    }
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/
static void sensors_joystick_handler(void)
{
    bool sw = false;
    int32_t magnitude = 0;
    int32_t direction = 0;

    if(sensors_data.joystick_1.state == false)
    {
        joystick_init();
        sensors_data.joystick_1.state  = true;
        osDelay(10); // Give some time to settle.
    }

    sw = joystick_get_sw(JOYSTICK_ID_LEFT);
    joystick_get_vector(JOYSTICK_ID_LEFT, &magnitude, &direction);

    if(magnitude > 0 && direction > 0)
    {
        display_keep_on();
    }

    __disable_irq();
    sensors_data.joystick_1.sw  = sw;
    sensors_data.joystick_1.magnitude = magnitude;
    sensors_data.joystick_1.direction = direction;
    __enable_irq();

    return;
}
