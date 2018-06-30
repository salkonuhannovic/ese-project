/*
 *  ======== Poll_Task.h ========
 *  Author: Hannes Aurednik, Eva Gergely, Stephan Nöhrer, Salko Nuhanovic
 */

/*! \file Poll_Task.h
    \brief Poll task
*/

#ifndef POLL_TASK_H_
#define POLL_TASK_H_

//#include <stdbool.h>
//#include <stdint.h>
//#include <xdc/std.h>

#include <ti/drivers/I2C.h>

/* \fn I2CInit
 * \brief I2CInit
 *
 * Initialising I2C interface to the slave peripheral.
 *
 * \param I2CParams - I2CParams
 * \param I2CHandle - I2C config struct
 *
 * \return zero if successful, else one.
 */
int I2CInit(I2C_Params *I2CParams, I2C_Handle *I2CHandle);


/* \fn PollingFunction
 * \brief PollingFunction
 *
 * Function attends to polling from the slave peripheral
 *
 * \param I2CHandle - I2C config struct
 * \param I2CTransaction - defines the nature of the I2C transaction. Specifies the buffer and the buffer's size
 *                         that is to be written to or read from the I2C slave peripheral
 * \param temperature - temperature value
 * \param rhval - relative humidity value
 *
 * \return void
 */
void PollingFunction(I2C_Handle *I2CHandle, I2C_Transaction *I2CTransaction, float *temperature, float *rhval);


/* \fn PollingFxn
 * \brief PollingFxn
 *
 * Includes while(1) loop. Calls PollingFunction and posts to Mailbox
 *
 * \param arg0 no value passed
 * \param arg1 no value passed
 *
 * \return void
 */
void PollingFxn(UArg arg0, UArg arg1);


/* \fn setup_Poll_Task
 * \brief setup_Poll_Task
 *
 * Creates PollingFxn
 *
 * \return always zero. In case of error the system halts.
 */
int setup_Poll_Task();

#endif
