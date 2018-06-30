/*
 *  Author: Michael Kramer / Matthias Wenzl
 *  Adjusted by: Hannes Aurednik, Eva Gergely, Stephan Nöhrer, Salko Nuhanovic
 */
#include <stdio.h>
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
#include <ti/drivers/I2C.h>

/* Driverlib headers */
#include <driverlib/gpio.h>

/* Board Header files */
#include <Board.h>
#include <EK_TM4C1294XL.h>

/* Application headers */
#include <driverlib/crc.h>
#include <local_inc/Poll_Task.h>
#include <local_inc/HTUTask.h>


/* VALUES TO BE FOUND IN HTU21D EXAMPLE CODE AND DOCUMENTATION:
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
#define TRIGGER_TEMP_MEASURE_HOLD   0xE3
#define TRIGGER_HUMD_MEASURE_HOLD   0xE5
#define SOFT_RESET                  0xFE

char Output[40];

void HTU21DSoftreset(I2C_Handle *I2CHandle, I2C_Transaction *I2CTransaction)
{
    UChar readBuffer[1];
    UChar writeBuffer[1] = {SOFT_RESET}; //0xFE

    if(!(I2CTransferConfig(writeBuffer, 1, readBuffer, 0, I2CHandle, I2CTransaction))) {
        sprintf(Output, "Soft reset terminated");
        System_printf("%s\n", Output);
        System_flush();
    } else {
        System_printf("Soft reset failed");
    }


    Task_sleep(20);

}

void HTU21D_REG_read(I2C_Handle *I2CHandle, I2C_Transaction *I2CTransaction)
{
    UChar readBuffer[1];
    UChar writeBuffer[1] = {READ_USER_REG}; //0xE7

    if(!(I2CTransferConfig(writeBuffer, 1, readBuffer, 1, I2CHandle, I2CTransaction))) {
        sprintf(Output, "User read Buf %d", readBuffer[0]);
        System_printf("%s\n", Output);
        System_flush();
    }else {
        System_printf("Error in function HTU21D_REG_read");
    }

    Task_sleep(20);

}


void HTU21D_REG_write(I2C_Handle *I2CHandle, I2C_Transaction *I2CTransaction)
{
    UChar readBuffer[1];
    UChar writeBuffer[2] = {WRITE_USER_REG, 0x80}; //0xE6

    if(!(I2CTransferConfig(writeBuffer, 2, readBuffer, 1, I2CHandle, I2CTransaction))) {
        sprintf(Output, "User read Buf %d", readBuffer[0]);
        System_printf("%s\n", Output);
        System_flush();
    }else {
        System_printf("Error in function HTU21D_REG_write");
    }

    Task_sleep(20);

}


unsigned CalcValues(UChar *readBuffer)
{
    uint16_t temp = 0;
    temp = (unsigned)readBuffer[0] << 8;
    temp = temp | readBuffer[1];
    return temp & 0xFFFC;

}


float GetTemperature(I2C_Handle *I2CHandle, I2C_Transaction *I2CTransaction)
{
    UChar readBuffer[3];
    UChar writeBuffer[1];
    float temperature;
    unsigned tempres = 0;

    memset(readBuffer, 0, sizeof(readBuffer));
    memset(writeBuffer, 0, sizeof(writeBuffer));
    writeBuffer[0] = TRIGGER_TEMP_MEASURE_HOLD; //0xE3

    if(I2CTransferConfig(writeBuffer, 1, readBuffer, 3, I2CHandle, I2CTransaction)){
        System_printf("Error in function GetTemperature - TransferConfig");
    } else {

        tempres = CalcValues(readBuffer);

        temperature = -46.85 + 175.72 * tempres / 65536.0;


        #ifdef DEBUG

        sprintf(Output, "Temperature before calc: %d", tempres);
        System_printf("%s\n", Output);
        System_flush();

        sprintf(Output, "Temperature: %.2f °C", temperature);
        System_printf("%s\n", Output);
        System_flush();

        #endif

    }


    return temperature;

}


float GetRelativeHumidity(I2C_Handle *I2CHandle, I2C_Transaction *I2CTransaction)
{
    UChar readBuffer[3];
    UChar writeBuffer[1];
    float rhval;
    uint16_t tempres = 0;

    memset(readBuffer, 0, sizeof(readBuffer));
    memset(writeBuffer, 0, sizeof(writeBuffer));
    writeBuffer[0] = TRIGGER_HUMD_MEASURE_HOLD; //0xE5

    if(I2CTransferConfig(writeBuffer, 1, readBuffer, 3, I2CHandle, I2CTransaction)){
        System_printf("Error in function GetTemperature - TransferConfig");
    } else {

        tempres = CalcValues(readBuffer);

        rhval = ((tempres / 65536.0) * 125 - 6);


        #ifdef DEBUG

        sprintf(Output, "Relative Humidity before calc: %d", tempres);
        System_printf("%s\n", Output);
        System_flush();

        sprintf(Output, "Relative Humidity : %.2f %%", rhval);
        System_printf("%s\n", Output);
        System_flush();

        #endif

    }

    return rhval;


}



int I2CTransferConfig(UChar *writeBuffer, size_t writeBufLen, UChar *readBuffer, size_t readBufLen, I2C_Handle *I2CHandle, I2C_Transaction *I2CTransaction)
{
    Bool transferOK;

    I2CTransaction->slaveAddress = HTU21D_ADDR; /*7-bit peripheral slave address*/
    I2CTransaction->writeBuf = writeBuffer; /*Buffer to be written*/
    I2CTransaction->writeCount = writeBufLen; /*Number of bytes to be written*/
    I2CTransaction->readBuf = readBuffer; /*Buffer to be read*/
    I2CTransaction->readCount = readBufLen; /*Number of bytes to be read*/

    transferOK = I2C_transfer(*I2CHandle, I2CTransaction); /*Perform I2C transfer*/
    Task_sleep(60);

    if(!(transferOK)) {
        return 1; /*Error*/
    } else {
        return 0;
    }

}
