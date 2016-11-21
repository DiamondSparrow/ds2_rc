/**
 **********************************************************************************************************************
 * @file        cli.h
 * @author      Diamond Sparrow
 * @version     1.0.0.0
 * @date        Apr 6, 2016
 * @brief       Command Line Interface C header file.
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

#ifndef CLI_H_
#define CLI_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/**********************************************************************************************************************
 * Exported constants
 *********************************************************************************************************************/
#define CLI_CMD_MAX_SIZE        16  //!< Maximum size of commands in CLI.

/**********************************************************************************************************************
 * Exported definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported types
 *********************************************************************************************************************/
/**
 * @brief   The prototype to which callback functions used to process command line commands must comply.
 *
 * @param   data    Buffer into which the output from executing the command can be written.
 * @param   size    Length, in bytes of the data buffer.
 * @param   cmd     The entire string as input by the user (from which parameters can be extracted).
 */
typedef bool (*cli_cmd_callback)(uint8_t *data, size_t size, const uint8_t *cmd);

/**
 * @brief   The structure that defines command line commands.
 *
 * @note    A command line command should be defined by declaring a 'const' structure of this type.
 */
typedef struct
{
    const uint8_t * const cmd;          /**< The command that causes pxCommandInterpreter to be executed.
                                             For example "help".  Must be all lower case. */
    const uint8_t * const help;         /**< String that describes how to use the command. Should start with
                                             the command itself, and end with "\r\n".
                                             For example "help: Returns a list of all the commands\r\n". */
    const cli_cmd_callback callback;    /**< A pointer to the callback function that will return the output generated
                                             by the command. */
    int8_t prm_count;                   /**< Commands expect a fixed number of parameters, which may be zero. */
} cli_cmd_t;

/**********************************************************************************************************************
 * Prototypes of exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of exported functions
 *********************************************************************************************************************/
/**
 * @brief   Initialize Command Line Interface.
 */
void cli_init(void);

/**
 * @brief   Register the command.
 *
 * @note    Registering a command adds the command to the list of commands that are handled by the command interpreter.
 *          Once a command has been registered it can be executed from the command line.
 *
 * @param   cmd Command data to register. See @ref cli_cmd_t.
 *
 * @return  State of register.
 * @retval  false   failed to register.
 * @retval  true    registered successfully.
 */
bool cli_register_cmd(const cli_cmd_t * const cmd);

/**
 * @brief   Runs the command interpreter for the input command string. Any output generated by running the command
 *          will be placed into buffer.
 *
 * @note    Should be called repeatedly until it returns false.
 *
 * @warning It must not be called from more than one task - or at least - by more than one task at a time.
 *
 * @param   input   Pointer to input command string.
 * @param   buffer  Pointer to output buffer.
 * @param   size    Size of buffer in bytes.
 *
 * @return  State of command processing.
 * @retval  0   done.
 * @retval  1   processing, repeat calling it.
 */
bool cli_process_cmd(const uint8_t * const input, uint8_t *buffer, size_t size);

/**
 * @brief   Get a buffer into which command outputs can be written is declared in the main command interpreter,
 *          rather than in the command console implementation, to allow application that provide access to the command
 *          console via multiple interfaces to share a buffer, and therefore save RAM
 *
 * @note    However, that the command interpreter itself is not reentrant, so only one command console interface can
 *          be used at any one time.  For that reason, no attempt is made to provide any mutual exclusion mechanism
 *          on the output buffer.
 *
 * @return  The address of the output buffer.
 */
uint8_t *cli_get_output_buffer(void);


/**
 * @brief   Return a pointer to the parameter in command string.
 * @param   cmd         Pointer to command string.
 * @param   prm_index   Parameter index to return.
 * @param   prm_size    Returned parameter string size in bytes.
 * @return
 */
const uint8_t *cli_get_parameter(const uint8_t *cmd, uint8_t prm_index, uint8_t *size);

/**
 * @brief   The callback function that is executed when "help" is entered.  This is the only default command
 *          that is always present.
 *
 * @param   data    Buffer into which the output from executing the command can be written.
 * @param   size    Length, in bytes of the data buffer.
 * @param   cmd     The entire string as input by the user (from which parameters can be extracted).
 *
 * @return  None.
 */
bool cli_help_cmd_callback(uint8_t *data, size_t size, const uint8_t *cmd);

#ifdef __cplusplus
}
#endif

#endif /* CLI_H_ */