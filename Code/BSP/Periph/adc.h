/**
 **********************************************************************************************************************
 * @file        adc.h
 * @author      Diamond Sparrow
 * @version     1.0.0.0
 * @date        2017-04-11
 * @brief       ADC C header file.
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

#ifndef ADC_H_
#define ADC_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include <stdint.h>

/**********************************************************************************************************************
 * Exported definitions and macros
 *********************************************************************************************************************/
#define ADC_CLK                 4400000     //!< 1000000, set to 4.4Mhz
#define ADC_VREF                3300.0F     //!< Reference voltage in mV.
#define ADC_RESOLUTION          4096.0F     //!< ADC resolution 12 bit.
#define ADC_LLS_SLOPE           (-2.36)     //!< Temperature sensor LLS slope value in volts.
#define ADC_LLS_INTERCEPT       (606.0)     //!< Temperature sensor LLS intercept value.
#define ADC_AVG_COUNT           100         //!< Average count.

/**********************************************************************************************************************
 * Exported types
 *********************************************************************************************************************/
/**
 * @brief   ADC channels enumerator.
 */
typedef enum
{
    ADC_ID_JOYSTICK_LEFT_X  = 0,    //!< Joystick left X axis channel.
    ADC_ID_JOYSTICK_LEFT_Y  = 1,    //!< Joystick left Y axis channel.
    ADC_ID_JOYSTICK_RIGHT_X = 2,    //!< Joystick right X axis channel.
    ADC_ID_JOYSTICK_RIGHT_Y = 3,    //!< Joystick right Y axis channel.
    ADC_ID_LAST,                    //!< Last should stay last!
} adc_id_t;

/**********************************************************************************************************************
 * Prototypes of exported constants
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of exported functions
 *********************************************************************************************************************/
/**
 * @brief   Initialize ADC.
 */
void adc_init(void);

/**
 * @brief   Read ADC value in raw.
 *
 * @param   id  ADC channel ID. See @ref adc_id_t.
 *
 * @return  ADC value in raw.
 */
uint32_t adc_read_raw(adc_id_t id);

/**
 * @brief   Read ADC value.
 *
 * @param   id  ADC channel ID. See @ref adc_id_t.

 * @return  ADC value in millivolts (0 - 3000). If failure - 0xFFFFFFFF.
 */
uint32_t adc_read_volt(adc_id_t id);

#ifdef __cplusplus
}
#endif

#endif /* ADC_H_ */
