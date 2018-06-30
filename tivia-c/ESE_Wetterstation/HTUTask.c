/*
 *  ======== Blink_Task.c ========
 *  Author: Michael Kramer / Matthias Wenzl
 *  Erweitert durch ESE Gruppe: Hannes Aurednik, Salko Nuhanovic, Stephan Nöhrer, Eva Gergely
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
#define TRIGGER_HUMD_MEASURE_HOLD   0xE5
#define SOFT_RESET                  0xFE

char Output[40];




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

    /* int I2CTransferConfig(UChar *writeBuffer, size_t writeBufLen, UChar *readBuffer,
     * size_t readBufLen, I2C_Handle *I2CHandle, I2C_Transaction *I2CTransaction)*/
    if(!(I2CTransferConfig(writeBuffer, 1, readBuffer, 0, I2CHandle, I2CTransaction))) {
        sprintf(Output, "Soft reset terminated");
        System_printf("%s\n", Output);
        System_flush();
    } else {
        System_printf("Soft reset failed"); //System_error
    }


    Task_sleep(20);

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
        System_printf("Error in function HTU21D_REG_read"); //error
    }

    Task_sleep(20);

}


void HTU21D_REG_write(I2C_Handle *I2CHandle, I2C_Transaction *I2CTransaction)
{
    UChar readBuffer[1];
    UChar writeBuffer[2] = {WRITE_USER_REG, 0x80}; //0x01 ? 0xE6 {WRITE_USER_REG, 0xC1};


    /*if(!(I2CTransferConfig(writeBuffer, 2, readBuffer, 0, I2CHandle, I2CTransaction)))*/
    if(!(I2CTransferConfig(writeBuffer, 2, readBuffer, 1, I2CHandle, I2CTransaction))) {
        sprintf(Output, "User read Buf %d", readBuffer[0]);
        System_printf("%s\n", Output);
        System_flush();
    }else {
        System_printf("Error in function HTU21D_REG_write"); //System_error
    }

    Task_sleep(20);

}

/* see also: HTUAccess example code:
 * unsigned HTU21D_ReadValue(char regSelect){
 * unsigned value = 0;
 * char reg_data[3];
 *
 * reg_data[0] = regSelect;
 * I2C1_Start();
 * I2C1_Write(HTU21D_ADDR, reg_data, 1, END_MODE_RESTART);
 * I2C1_Read (HTU21D_ADDR, reg_data, 3, END_MODE_STOP);
 * value = ((unsigned)reg_data[0] << 8) | reg_data[1] ;
 * return value & 0xFFFC;            // Clear status bits
 * }*/

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
    unsigned tempres = 0; //uint16_t?

    /*void *memset(void *str, int c, size_t n)*/
    memset(readBuffer, 0, sizeof(readBuffer)); //im Buffer alles mit 0 füllen
    memset(writeBuffer, 0, sizeof(writeBuffer));
    writeBuffer[0] = TRIGGER_TEMP_MEASURE_HOLD; //0xE3

    /* int I2CTransferConfig(UChar *writeBuffer, size_t writeBufLen, UChar *readBuffer,
     * size_t readBufLen, I2C_Handle *I2CHandle, I2C_Transaction *I2CTransaction)*/
    if(I2CTransferConfig(writeBuffer, 1, readBuffer, 3, I2CHandle, I2CTransaction)){
        System_printf("Error in function GetTemperature - TransferConfig");
    } else {

        tempres = CalcValues(readBuffer);


        /* Function to be found at p. 15 of the HTU12D manual
         * Temperature conversion
         * Temp = -46.85 + 175.72 x S_Temp / 2^16*/
        temperature = -46.85 + 175.72 * tempres / 65536.0; //2^16 = 65536.0

        sprintf(Output, "Temperature before calc: %d", tempres);
        System_printf("%s\n", Output);
        System_flush();

       /*.........
        * ........
        * ........*/
        sprintf(Output, "Temperature: %.2f °C", temperature);

        System_printf("%s\n", Output);
        System_flush();


    }


    return temperature;

}


float GetRelativeHumidity(I2C_Handle *I2CHandle, I2C_Transaction *I2CTransaction)
{
    UChar readBuffer[3];
    UChar writeBuffer[1];
    float rhval;
    uint16_t tempres = 0;

    /*void *memset(void *str, int c, size_t n)*/
    memset(readBuffer, 0, sizeof(readBuffer)); //im Buffer alles mit 0 füllen
    memset(writeBuffer, 0, sizeof(writeBuffer));
    writeBuffer[0] = TRIGGER_HUMD_MEASURE_HOLD; //0xE5


    /*if(I2CTransferConfig(writeBuffer, 1, readBuffer, 3, I2CHandle, I2CTransaction))*/
    if(I2CTransferConfig(writeBuffer, 1, readBuffer, 3, I2CHandle, I2CTransaction)){
        System_printf("Error in function GetTemperature - TransferConfig");
    } else {

        tempres = CalcValues(readBuffer);

        //realtemp function???

        /* Function to be found at p. 15 of the HTU12D manual
         * Relative Humidity conversion
         * RH = -6+125 x (S_RH / 2^16)*/
        rhval = ((tempres / 65536.0) * 125 - 6); //2^16 = 65536.0

        sprintf(Output, "Relative Humidity before calc: %d", tempres);
        System_printf("%s\n", Output);
        System_flush();

       /*.........
        * ........
        * ........*/

        sprintf(Output, "Relative Humidity : %.2f %%", rhval);

        System_printf("%s\n", Output);
        System_flush();



    }

    return rhval;


}


float GetDewPoint(float temperature, float rhval)
{
    /*ist diese funktion überhaupt notwendig?!*/
    return 0;

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
        return 1; //Fehler
    } else {
        return 0;
    }

}
