/*
 *  ======== Blink_Task.c ========
 *  Author: Michael Kramer / Matthias Wenzl
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
#include "Blink_Task.h"

/*CRC*/
#include <driverlib/crc.h>

/* WERTE AUS DEM HTU21D BEISPIELCODE:
 * HTU21D_ADDR                 (0x40)
 * HTU21D_CLEAR_BIT            (0x40)
 * HTU21D_WORD_BIT             (0x20)
 * HTU21D_BLOCK_BIT            (0x10)
 * TRIGGER_TEMP_MEASURE_HOLD    0xE3
 * TRIGGER_HUMD_MEASURE_HOLD    0xE5
 * TRIGGER_TEMP_MEASURE_NOHOLD  0xF3
 * TRIGGER_HUMD_MEASURE_NOHOLD  0xF5
 * WRITE_USER_REG               0xE6
 * READ_USER_REG                0xE7
 * SOFT_RESET                   0xFE
 */

#define HTU21D_ADDR                 0x40
#define READ_USER_REG               0xE7
#define WRITE_USER_REG              0xE6

/* HTU21D.pdf p. 11
 * two different op. modes to communicate with HTU - hold master and no hold master mode. in this case hold master
 * means that SCK line is blocked (controlled by HTU21D) during measurement process*/
#define TRIGGER_TEMP_MEASURE_HOLD   0xE3
#define RIGGER_HUMD_MEASURE_HOLD    0xE5
#define SOFT_RESET                  0xFE

char Output[40];

void DataGetI2CFxn(UArg arg0, UArg arg1)
{
    /*von Daniel und Raphael*/
    float temp;
    float rh;
    float taupunkt;
    /*......................*/
    I2C_Handle I2CHandle = NULL;
    I2C_Params I2CParams;
    I2C_Transaction I2CTransaction;

    HTU21DSoftreset(&I2CHandle, &I2CTransaction);

    if((I2CInit(&I2CParams, &I2CHandle)) != 0) {
        System_abort("I2C could not be initialized in Fxn.");
    }

    while(1) {



    }
}


/*
 *  Setup task function
 */
int setup_I2C_Data_Get_Task(led_descriptor_t *led_desc, uint32_t wait_ticks)
{
    Task_Params taskParams;
    Task_Handle taskHandle;
    Error_Block eb;


    /* Create task with highest priority*/
    Error_init(&eb);
    Task_Params_init(&taskParams);
    taskParams.stackSize = 2048; /* stack in bytes */
    taskParams.priority = 15; /* 0-15 (15 is highest priority on default -> see RTOS Task configuration) */
    taskHandle = Task_create((Task_FuncPtr)DataGetI2CFxn, &taskParams, &eb);
    
    if (taskHandle == NULL) {
        System_abort("TaskLed create failed");
    }

    return (0);
}

int I2CInit(I2C_Params *I2CParams, I2C_Handle *I2CHandle)
{
    I2C_Params_init(I2CParams);
    I2CParams->transferMode = I2C_MODE_BLOCKING;
    I2CParams->bitRate = I2C_400kHz;

    I2CHandle = I2C_open(EK_TM4C1294XL_I2C7, I2CParams);

    if (*I2CHandle == NULL) {
        System_abort("I2C_open() failed!");
        return 1;
    } else {
        return 0; //I2C init correct
    }

}


/* http://www.ti.com/lit/ug/spruhd4m/spruhd4m.pdf
 * p. 60 - write / read transaction (blocking mode)
 * soft reset is recommended at the start of the device (p. 10ff - manual HTU21D)
 * The command is used for rebooting the HTU21D sensor switching the power off and on again.
 * Upon reception of this command, the HTU21D sensor system reinitializes and starts operation according to the
 * default settings (with the exception of the heater bit in the userreg)*/

void HTU21DSoftreset(I2C_Handle *I2CHandle, I2C_Transaction *I2CTransaction)
{
    UChar readBuffer[1];
    UChar writeBuffer[1] = {SOFT_RESET}; //0xFE

    if(!(I2CTransferConfig(writeBuffer, 1, readBuffer, 0, I2CHandle, I2CTransaction))) {
        sprintf(Output, "Soft reset terminated");
        System_printf("%s\n", Output);
        System_flush();
    } else {
        System_error("Soft reset failed");
    }


    Task_sleep(50);

}

void HTU21D_REG_read(I2C_Handle *I2CHandle, I2C_Transaction *I2CTransaction)
{
    UChar readBuffer[1];
    UChar writeBuffer[1] = {READ_USER_REG}; //0xFE writeBuffer[0] = READ_USER_REG

    if(!(I2CTransferConfig(writeBuffer, 1, readBuffer, 1, I2CHandle, I2CTransaction))) {
        sprintf(Output, "User read Buf %d", readBuffer[0]);
        System_printf("%s\n", Output);
        System_flush();
    }else {
        //error
    }

    Task_sleep(20);

}


void HTU21D_REG_write(I2C_Handle *I2CHandle, I2C_Transaction *I2CTransaction)
{
    UChar readBuffer[1];
    UChar writeBuffer[2] = {WRITE_USER_REG, 0x01}; //0xFE


    if(!(I2CTransferConfig(writeBuffer, 2, readBuffer, 0, I2CHandle, I2CTransaction))) {
        sprintf(Output, "User read Buf %d", readBuffer[0]);
        System_printf("%s\n", Output);
        System_flush();
    }else {
        //error
    }

    Task_sleep(20);

}

float GetTemperature(I2C_Handle *I2CHandle, I2C_Transaction *I2CTransaction)
{
    UChar readBuffer[3];
    UChar writeBuffer[1];
    uint16_t value = 0;

    float temperature;

    /*void *memset(void *str, int c, size_t n)*/
    memset(readBuffer, 0, sizeof(readBuffer)); //im Buffer alles mit 0 füllen
    memset(writeBuffer, 0, sizeof(writeBuffer));
    writeBuffer[0] = TRIGGER_TEMP_MEASURE_HOLD;

    /*  uint16_t value = 0;
     value = (unsigned)r_buffer[0] << 8;
     value = value | r_buffer[1];
     return value & 0xFFFC;*/


    /*TODO: CONTINUEEEEE*/

}

int I2CTransferConfig(UChar *writeBuffer, size_t writeBufLen, UChar *readBuffer, size_t readBufLen, I2C_Handle *I2CHandle, I2C_Transaction *I2CTransaction)
{
    Bool transferOK;

    I2CTransaction->slaveAddress = HTU21D_ADDR; /*7-bit peripheral slave address*/
    I2CTransaction->writeBuf = writeBuffer; /*Buffer to be written*/
    I2CTransaction->writeCount = writeBufLen; /*Number of bytes to be written*/
    I2CTransaction->readBuf = readBuffer; /*Buffer to be read*/
    I2CTransaction->readCount = readBufLen; /*Number of bytes to be read*/

    transferOK = I2C_transfer(&I2CHandle, &I2CTransaction); /*Perform I2C transfer*/
    Task_sleep(20);

    if(!(transferOK)) {
        return 1; //Fehler
    } else {
        return 0;
    }

}
