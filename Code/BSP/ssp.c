/**
 **********************************************************************************************************************
 * @file         ssp.c
 * @author       Diamond Sparrow
 * @version      1.0.0.0
 * @date         2016-08-30
 * @brief        SSP C source file.
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
#include <string.h>

#include "chip.h"

#include "ssp.h"

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
/* SPI Transfer Setup */
static Chip_SSP_DATA_SETUP_T ssp_0_xfer;
static SSP_ConfigFormat ssp_0_format;

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of local functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported functions
 *********************************************************************************************************************/
void ssp_0_init(void)
{
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 9, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN)); // MOSI, P0.9
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 8, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN)); // MISO, P0.8
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 6, (IOCON_FUNC2 | IOCON_MODE_INACT | IOCON_DIGMODE_EN)); // SCK,  P0.6

    Chip_SSP_Init(LPC_SSP0);

    ssp_0_format.frameFormat = SSP_FRAMEFORMAT_SPI;
    ssp_0_format.bits = SSP_BITS_8;
    ssp_0_format.clockMode = SSP_CLOCK_MODE0;
    Chip_SSP_SetFormat(LPC_SSP0, ssp_0_format.bits, ssp_0_format.frameFormat, ssp_0_format.clockMode);
    Chip_SSP_SetMaster(LPC_SSP0, 1);
    Chip_SSP_SetBitRate(LPC_SSP0, 12000000);
    Chip_SSP_Enable(LPC_SSP0);

    NVIC_DisableIRQ(SSP0_IRQn);

    return;
}

void ssp_0_read_buffer(uint8_t *buffer, uint16_t size)
{
    Chip_SSP_ReadFrames_Blocking(LPC_SSP0, buffer, size);

    return;
}

void ssp_0_write_buffer(uint8_t *buffer, uint16_t size)
{
    //Chip_SSP_WriteFrames_Blocking(LPC_SSP0, buffer, size);
    while(size--)
    {
        LPC_SSP0->DR = *buffer;
        while(LPC_SSP0->SR & SSP_STAT_BSY);
        LPC_SSP0->DR;
        buffer++;
    }

    return;
}

void ssp_0_write_read(uint8_t *tx, uint16_t tx_size, uint8_t *rx, uint16_t rx_size)
{
    ssp_0_xfer.tx_data = tx;   /* Transmit Buffer */
    ssp_0_xfer.rx_data = rx;   /* Receive Buffer */
    ssp_0_xfer.length = tx_size + rx_size;  /* Total frame length */
    ssp_0_xfer.tx_cnt = 0;
    ssp_0_xfer.rx_cnt = 0;

    Chip_SSP_RWFrames_Blocking(LPC_SSP0, &ssp_0_xfer);

    return;
}

/**
 * ********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/
