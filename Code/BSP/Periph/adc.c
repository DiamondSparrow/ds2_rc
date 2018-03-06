/**
 **********************************************************************************************************************
 * @file         adc.c
 * @author       Diamond Sparrow
 * @version      1.0.0.0
 * @date         2017-04-11
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

#include "adc.h"

#include "chip.h"

/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/
/**
 * @brief   ADC channel
 */
typedef struct
{
    uint8_t port;       //!< ADC channel port.
    uint8_t pin;        //!< ADC channel pin.
    uint8_t channel;    //!< ADC channel.
    uint32_t modefunc;  //!< Pin mux settings.
} adc_ch_t;

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/
/** ADC channel list. See @ref adc_itwm_t. */
const adc_ch_t adc_ch_list[ADC_ID_LAST] =
{
    // ADC_ID_JOYSTICK_LEFT_X
    {.port = 0, .pin = 16, .channel = 2, .modefunc = (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_ADMODE_EN)},
    // ADC_ID_JOYSTICK_LEFT_Y
    {.port = 0, .pin = 12, .channel = 8, .modefunc = (IOCON_FUNC2 | IOCON_MODE_INACT | IOCON_ADMODE_EN)},
    // ADC_ID_JOYSTICK_RIGHT_X
    {.port = UINT8_MAX, .pin = UINT8_MAX, .channel = UINT8_MAX, .modefunc = (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_ADMODE_EN)},
    // ADC_ID_JOYSTICK_RIGHT_Y
    {.port = UINT8_MAX, .pin = UINT8_MAX, .channel = UINT8_MAX, .modefunc = (IOCON_FUNC2 | IOCON_MODE_INACT | IOCON_ADMODE_EN)},
};

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/
void adc_init(void)
{
    uint8_t i = 0;
    uint32_t seq_opt = (ADC_SEQ_CTRL_CHANSEL(0) | ADC_SEQ_CTRL_LOWPRIO | ADC_SEQ_CTRL_MODE_EOS);

    /* Setup ADC for 12-bit mode and normal power */
    Chip_ADC_Init(LPC_ADC, 0);

    /* Use higher voltage trim */
    //Chip_ADC_SetTrim(LPC_ADC, ADC_TRIM_VRANGE_LOWV);

    /* Need to do a calibration after initialization and trim. */
    //Chip_ADC_SetClockRate(LPC_ADC, 500000);
    Chip_ADC_StartCalibration(LPC_ADC);
    while(!(Chip_ADC_IsCalibrationDone(LPC_ADC)));

    /* Setup ADC clock rate */
    Chip_ADC_SetClockRate(LPC_ADC, ADC_CLK);

    /* Setup sequencer A and pins mux. */
    for(i = 0; i < ADC_ID_LAST; i++)
    {
        if(adc_ch_list[i].channel != UINT8_MAX)
        {
            seq_opt |= ADC_SEQ_CTRL_CHANSEL(adc_ch_list[i].channel);
        }
    }
    Chip_ADC_SetupSequencer(LPC_ADC, ADC_SEQA_IDX, seq_opt);
    
    /* Setup pin mux. */
    for(i = 0; i < ADC_ID_LAST; i++)
    {
        if(adc_ch_list[i].port != UINT8_MAX && adc_ch_list[i].pin != UINT8_MAX)
        {
            Chip_IOCON_PinMuxSet(LPC_IOCON, adc_ch_list[i].port, adc_ch_list[i].pin, adc_ch_list[i].modefunc);
        }
    }

    /* Power up/down the internal temperature sensor - this also selects the temperature sensor as the input for the ADC0 input */
    Chip_SYSCTL_PowerUp(SYSCTL_POWERDOWN_TS_PD);
    
    /* Use higher voltage trim */
    Chip_ADC_SetTrim(LPC_ADC, ADC_TRIM_VRANGE_HIGHV);

    /* Setup threshold 0 low and high values to about 25% and 75% of max */
    Chip_ADC_SetThrLowValue(LPC_ADC, 0, ((1 * 0xFFF) / 4));
    Chip_ADC_SetThrHighValue(LPC_ADC, 0, ((3 * 0xFFF) / 4));
    
    /* Clear all pending interrupts */
    Chip_ADC_ClearFlags(LPC_ADC, Chip_ADC_GetFlags(LPC_ADC));
    /* Enable ADC overrun and sequence A completion interrupts */
    //Chip_ADC_EnableInt(LPC_ADC, (ADC_INTEN_SEQA_ENABLE));
    /* Enable ADC NVIC interrupt */
    //NVIC_EnableIRQ(ADC_A_IRQn);
    NVIC_DisableIRQ(ADC_A_IRQn);
    NVIC_DisableIRQ(ADC_B_IRQn);

    /* Enable sequencer */
    Chip_ADC_EnableSequencer(LPC_ADC, ADC_SEQA_IDX);
    /* Start Burst sequencer. */
    Chip_ADC_StartBurstSequencer(LPC_ADC, ADC_SEQA_IDX);

    return;
}

uint32_t adc_read_raw(adc_id_t id)
{
    uint32_t raw = 0;
    uint32_t value = 0;
    uint32_t i = ADC_AVG_COUNT;
    uint32_t j = 0;

    if(adc_ch_list[id].channel == UINT8_MAX)
    {
        return UINT32_MAX;
    }

    do
    {
        //Chip_ADC_StartSequencer(LPC_ADC, ADC_SEQA_IDX);
        while((raw = Chip_ADC_GetDataReg(LPC_ADC, adc_ch_list[id].channel)) & ADC_SEQ_GDAT_DATAVALID);
        value += ADC_DR_RESULT(raw);
        j++;
    }
    while(i-- > 1);
    
    if(j > 0)
    {
        value = value / j;
    }
    else
    {
        value = UINT32_MAX;
    }

    return value;
}

uint32_t adc_read_volt(adc_id_t id)
{
    uint32_t raw = UINT32_MAX;
    uint32_t volts = UINT32_MAX;
    
    raw = adc_read_raw(id);
    
    if(raw > ADC_RESOLUTION)
    {
        return UINT32_MAX;
    }
    
    volts = raw * ADC_VREF;
    volts /= ADC_RESOLUTION;
    
    return volts;
}

/**********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/
