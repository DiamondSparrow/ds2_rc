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
 * Private definitions and macros
 *********************************************************************************************************************/
#define SSP_0_BIT_RATE  12000000
#define SSP_1_BIT_RATE  8000000

/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
/* SPI Transfer Setup */
static Chip_SSP_DATA_SETUP_T ssp_0_xfer;

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

    Chip_SSP_SetFormat(LPC_SSP0, SSP_FRAMEFORMAT_SPI, SSP_BITS_8, SSP_CLOCK_MODE0);
    Chip_SSP_SetMaster(LPC_SSP0, 1);
    Chip_SSP_SetBitRate(LPC_SSP0, SSP_0_BIT_RATE);
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

void ssp_1_init(void)
{
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 21, (IOCON_FUNC2 | IOCON_MODE_INACT | IOCON_DIGMODE_EN)); // MOSI, P0.21
    Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 21, (IOCON_FUNC2 | IOCON_MODE_INACT | IOCON_DIGMODE_EN)); // MISO, P1.21
    Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 27, (IOCON_FUNC4 | IOCON_MODE_INACT | IOCON_DIGMODE_EN)); // SCK,  P1.27
    //Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 23, (IOCON_FUNC2 | IOCON_MODE_INACT | IOCON_DIGMODE_EN)); // SSEL, P1.23

    Chip_SSP_Init(LPC_SSP1);

    Chip_SSP_SetFormat(LPC_SSP1, SSP_FRAMEFORMAT_SPI, SSP_BITS_8, SSP_CLOCK_MODE0);
    Chip_SSP_SetMaster(LPC_SSP1, 1);
    Chip_SSP_SetBitRate(LPC_SSP1, SSP_1_BIT_RATE);
    Chip_SSP_Enable(LPC_SSP1);

    NVIC_DisableIRQ(SSP1_IRQn);

    return;
}

void ssp_1_send_byte(uint8_t data)
{
    /* Put the data on the FIFO */
    LPC_SSP1->DR = data;

    /* Wait for sending to complete */
    while((LPC_SSP1->SR & 0x10));

    data = LPC_SSP1->DR;

    return;
}

void ssp_1_send_buffer(uint8_t *buffer, uint32_t size)
{
    while(size--)
    {
        /* Put the data on the FIFO */
        LPC_SSP1->DR = *buffer;
        buffer++;

        /* Wait for sending to complete */
        while((LPC_SSP1->SR & 0x10));

        LPC_SSP1->DR;
    }

    return;
}

uint8_t ssp_1_recv_byte(void)
{
    /* Put the data on the FIFO */
    LPC_SSP1->DR = 0xFF;

    /* Wait for sending to complete */
    while((LPC_SSP1->SR & 0x10));

    /* Return the received value */
    return (LPC_SSP1->DR);
}

void ssp_1_recv_buffer(uint8_t *buffer, uint32_t size)
{
    while(size--)
    {
        /* Put the data on the FIFO */
        LPC_SSP1->DR = 0xFF;

        /* Wait for sending to complete */
        while((LPC_SSP1->SR & 0x10));

        *buffer = LPC_SSP1->DR;
        buffer++;
    }

    return;
}

/**
 * ********************************************************************************************************************
 * Private functions
 *********************************************************************************************************************/
