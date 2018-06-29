/*
 *  ======== Poll_Task.c ========
 *  Author: Michael Kramer / Matthias Wenzl
 *  Eva Gergely
 */
#include <stdbool.h>
#include <stdint.h>
#include <inc/hw_memmap.h>

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Memory.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

/* Driverlib headers */
#include <driverlib/gpio.h>

/* Board Header files */
#include <Board.h>
#include <EK_TM4C1294XL.h>

/* Application headers */
#include <local_inc/Poll_Task.h>
#include <local_inc/HTUTask.h>

int I2CInit(I2C_Params *I2CParams, I2C_Handle *I2CHandle)
{
    I2C_Params_init(I2CParams);
    I2CParams->transferMode = I2C_MODE_BLOCKING;
    I2CParams->bitRate = I2C_400kHz;

    *I2CHandle = I2C_open(EK_TM4C1294XL_I2C7, I2CParams);

    if (*I2CHandle == NULL) {
        System_printf("I2C_open() failed.\n"); //System_abort
        return 1;
    } else {
        System_printf("I2C_open() successful.\n");
        return 0; //I2C init correct
    }

}


void PollingFunction(I2C_Handle *I2CHandle, I2C_Transaction *I2CTransaction, float *temperature, float *rhval)
{
    *temperature = GetTemperature(I2CHandle, I2CTransaction);

    *rhval = GetRelativeHumidity(I2CHandle, I2CTransaction);

    /*TODO: rh und taupunkt*/

}


void PollingFxn(UArg arg0, UArg arg1)
{
    float temp;
    float rh;

    I2C_Handle I2CHandle = NULL;
    I2C_Params I2CParams;
    I2C_Transaction I2CTransaction;


    if((I2CInit(&I2CParams, &I2CHandle)) != 0) {
            System_abort("I2C could not be initialized in Fxn.\n");
        }

    HTU21DSoftreset(&I2CHandle, &I2CTransaction);

        /* Manual p. 13:
         * The content of user register is described in the table below.
         * Reserved bits must not be changed and default values of respective reserved
         * bits may change over time without prior notice. Therefore, for any writing
         * to user register, default values of reserved bits must be read first.*/
    HTU21D_REG_read(&I2CHandle, &I2CTransaction);
    HTU21D_REG_write(&I2CHandle, &I2CTransaction);
    HTU21D_REG_read(&I2CHandle, &I2CTransaction);

    while(1) {

        PollingFunction(&I2CHandle, &I2CTransaction, &temp, &rh);
        /*TODO: Mailbox_send*/
        Task_sleep(5000);

    }
}


/*
 *  Setup task function
 */
int setup_Poll_Task()
{
    Task_Params taskParams;
    Task_Handle taskHandle;
    Error_Block eb;

    Error_init(&eb);
    Task_Params_init(&taskParams);

    taskParams.stackSize = 1024; /* stack in bytes - sollte hier ein größerer wert wie zb 2048 gewählt werden? */
    taskParams.priority = 15; /* 0-15 (15 is highest priority on default -> see RTOS Task configuration) */

    taskHandle = Task_create((Task_FuncPtr)PollingFxn, &taskParams, &eb);
    
    if (taskHandle == NULL) {
        System_abort("Task create failed (Poll Task)");
    }

    return (0);
}
