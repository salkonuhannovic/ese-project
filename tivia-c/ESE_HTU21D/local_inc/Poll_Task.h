/*! \file Poll_Task.h
    \brief Poll task
*/

#ifndef POLL_TASK_H_
#define POLL_TASK_H_

#include <stdbool.h>
#include <stdint.h>
#include <xdc/std.h>

#include <ti/drivers/I2C.h>

/* \fn I2CInit
 * \brief Initialising I2C
 *
 * \param I2CParams - I2CParams
 * \param I2CHandle - I2CHandle
 *
 * \return zero if successful, else one.
 */
int I2CInit(I2C_Params *I2CParams, I2C_Handle *I2CHandle);


/* \fn PollingFunction
 * \brief
 *
 * \param I2CHandle
 * \param I2CTransaction
 * \param temperature
 */
void PollingFunction(I2C_Handle *I2CHandle, I2C_Transaction *I2CTransaction, float *temperature, float *rhval);


/*! \fn PollingFxn
 *  \brief
 *
 *   \param arg0
 *   \param arg1
 */
void PollingFxn(UArg arg0, UArg arg1);


/*! \fn setup_Poll_Task
 *  \brief Setup Poll task
 *
 *  Setup Poll task
 *  Task has highest priority and receives 1kB of stack
 *
 *   \param
 *   \param
 *
 *  \return always zero. In case of error the system halts.
 */
int setup_Poll_Task();

#endif
