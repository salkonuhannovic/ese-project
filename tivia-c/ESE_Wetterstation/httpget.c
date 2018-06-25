/*
 * Copyright (c) 2015-2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== httpget.c ========
 *  HTTP Client GET example application
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
#include <ti/drivers/I2C.h>

/* Currently unused RTOS headers that are needed
 * for advanced features like IPC. */
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/hal/Timer.h>

/* Driverlib headers */
#include <driverlib/gpio.h>

/* Board Header files */
#include <Board.h>
#include <EK_TM4C1294XL.h>

/* Application headers */
#include <local_inc/UART_Task.h>
#include <local_inc/HTUTask.h>
#include <local_inc/Poll_Task.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
/* XDCtools Header files */
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>

/* TI-RTOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/drivers/GPIO.h>
#include <ti/net/http/httpcli.h>

/* Example/Board Header file */
#include "Board.h"

#include <sys/socket.h>
#include <driverlib/flash.h>

//MOCK https://webhook.site/d6e1ab21-cb8e-4635-85f9-0046b7873f6b

#define HOSTNAME          "webhook.site"
#define REQUEST_URI       "/d6e1ab21-cb8e-4635-85f9-0046b7873f6b"
#define CONFIG_URI       "/d6e1ab21-cb8e-4635-85f9-0046b7873f6b/"

/*
//OUR SERVICE
#define HOSTNAME          "esesmarthome.azurewebsites.net"
#define CONFIG_URI       "/setup/"
#define REQUEST_URI       "/api/measurements"
*/
#define USER_AGENT        "HTTPCli (ARM; TI-RTOS)"
#define CONTENT_TYPE      "application/json"
#define HTTPTASKSTACKSIZE 4096
//************************************************************************************************************GLOBALS
static volatile char deviceID;
static volatile char g_devicemac[18];
//************************************************************************************************************PROTOTYPES
void printError(char *errString, int code);
int main(void);
void netIPAddrHook(unsigned int IPAddr, unsigned int IfIdx, unsigned int fAdd);
void getMacAddress(void);
void httpTask(UArg arg0, UArg arg1);
void HTTPPOSTTask(UArg arg0, UArg arg1);
char* concat(const char *s1, const char *s2);
//************************************************************************************************************FUNCTIONS

/*TODOS
 * -Get devideID by HTTP GET - Use MAC as Identifier in CONFIG_URI
 * --Include MAC in HTTP GET Request autom.
 * --Extract assigned devideID out auf HTTP GET and store it
 * -Post Values periodically using HTTP POST including deviceID
 * --Get Values from Sensors
 * --Build JSON String autom.
 * --Include deviceID in POST Requests
 * -Updating Timesheet
 */



//Copied Posttask from xxx
void HTTPPOSTTask(UArg arg0, UArg arg1)
{
    bool moreFlag = false;
    char data[128];
    int ret;
    int len;
    char CONTENT_LENGTH[3];
    struct sockaddr_in addr;
    char* try2 =concat("{\"deviceId\":", deviceID);
    char* try = concat(try2, ",\"temperature\": 20,\"humidity\": 20}");
    //Data to be sent
    strcpy(data, try);

    len = strlen(data);
    sprintf(CONTENT_LENGTH, "%d", len);

    System_printf("\nData: %s\n", data);
    System_printf("len(int): %d\n", len);
    System_printf("CONTENT_LENGTH: %s\n", CONTENT_LENGTH);

    HTTPCli_Struct cli;
    HTTPCli_Field fields[3] = {
        { HTTPStd_FIELD_NAME_HOST, HOSTNAME },
        { HTTPStd_FIELD_NAME_USER_AGENT, USER_AGENT },
        { NULL, NULL }
    };

    System_printf("Sending a HTTP POST request to '%s'\n", HOSTNAME);
    System_flush();

    HTTPCli_construct(&cli);

    HTTPCli_setRequestFields(&cli, fields);

    ret = HTTPCli_initSockAddr((struct sockaddr *)&addr, HOSTNAME, 0);
    if (ret < 0) {
        printError("POSTTask: address resolution failed", ret);
    }

    ret = HTTPCli_connect(&cli, (struct sockaddr *)&addr, 0, NULL);
    if (ret < 0) {
        printError("POSTTask: connect failed", ret);
    }

//************************************************************************************************************SEND
    ret = HTTPCli_sendRequest(&cli, HTTPStd_POST, REQUEST_URI, true);
    if (ret < 0) {
        printError("POSTTask: send failed", ret);
    }
    else {
        System_printf("sendRequest successful\n");
    }

    ret = HTTPCli_sendField(&cli, HTTPStd_FIELD_NAME_CONTENT_LENGTH, CONTENT_LENGTH, false);
    ret = HTTPCli_sendField(&cli, HTTPStd_FIELD_NAME_CONTENT_TYPE, CONTENT_TYPE, true);

    if (ret < 0) {
        printError("POSTTask: send failed", ret);
    }
    else {
        System_printf("sendField successful\n");
    }

        ret = HTTPCli_sendRequestBody(&cli, data, strlen(data));
    if (ret < 0) {
        printError("POSTTask: Variable data couldn't be sent", ret);
    }
    else {
        System_printf("Data sent successfully\n");
    }
//************************************************************************************************************RECEIVE

    ret = HTTPCli_getResponseStatus(&cli);
    if (ret != HTTPStd_OK) {
        if(ret != 204){
        printError("POSTTask: cannot get status", ret);

        }
    }
    System_printf("HTTP Response Status Code: %d\n", ret);

    ret = HTTPCli_getResponseField(&cli, data, sizeof(data), &moreFlag);

    if (ret != HTTPCli_FIELD_ID_END) {
        printError("POSTTask: response field processing failed", ret);
    }

    len = 0;
    do {
        ret = HTTPCli_readResponseBody(&cli, data, sizeof(data), &moreFlag);
        if (ret < 0) {
            printError("POSTTask: response body processing failed", ret);
        }

        len += ret;
    } while (moreFlag);

    System_printf("Received %d bytes of pay-load\n", len);
    System_flush();

    free(try);
    free(try2);
    HTTPCli_disconnect(&cli);
    HTTPCli_destruct(&cli);
}
//https://stackoverflow.com/questions/8465006/how-do-i-concatenate-two-strings-in-c
char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}
/*
 *  ======== httpTask ========
 *  Makes a HTTP GET request
 */
void httpTask(UArg arg0, UArg arg1)
{
    char* config = concat(CONFIG_URI,g_devicemac);
    bool moreFlag = false;
    char data[64];
    int ret;
    int len;
    struct sockaddr_in addr;

    HTTPCli_Struct cli;
    HTTPCli_Field fields[3] = {
        { HTTPStd_FIELD_NAME_HOST, HOSTNAME },
        { HTTPStd_FIELD_NAME_USER_AGENT, USER_AGENT },
        { NULL, NULL }
    };

    System_printf("Sending a HTTP GET request to '%s'\n", HOSTNAME);//POST
    System_flush();

    HTTPCli_construct(&cli);

    HTTPCli_setRequestFields(&cli, fields);

    ret = HTTPCli_initSockAddr((struct sockaddr *)&addr, HOSTNAME, 0);
    if (ret < 0) {
        printError("httpTask: address resolution failed", ret);
    }

    ret = HTTPCli_connect(&cli, (struct sockaddr *)&addr, 0, NULL);
    if (ret < 0) {
        printError("httpTask: connect failed", ret);
    }


    ret = HTTPCli_sendRequest(&cli, HTTPStd_GET, config, false);
    if (ret < 0) {
        printError("httpTask: send failed", ret);
    }

    ret = HTTPCli_getResponseStatus(&cli);
    if (ret != HTTPStd_OK) {
        printError("httpTask: cannot get status", ret);
    }

    System_printf("HTTP Response Status Code: %d\n", ret);

    ret = HTTPCli_getResponseField(&cli, data, sizeof(data), &moreFlag);
    if (ret != HTTPCli_FIELD_ID_END) {
        printError("httpTask: response field processing failed", ret);
    }

    len = 0;
    do {
        ret = HTTPCli_readResponseBody(&cli, data, sizeof(data), &moreFlag);
        if (ret < 0) {
            printError("httpTask: response body processing failed", ret);
        }
        if(ret>0){
            deviceID = data[0];
            System_printf("Recieved DeviceID: %c\n", data[0]);
            System_flush();

        }

        len += ret;
    } while (moreFlag);

    System_printf("Recieved %d bytes of payload\n", len);
    System_flush();

    free(config);

    HTTPCli_disconnect(&cli);
    HTTPCli_destruct(&cli);

    //TEST
    static Task_Handle taskHandle;
        Task_Params taskParams;
        Error_Block eb;
    if(deviceID!=0){
        Error_init(&eb);

        Task_Params_init(&taskParams);
        taskParams.stackSize = HTTPTASKSTACKSIZE;
        taskParams.priority = 1;
        //TODO: Add Posttask here

        taskHandle = Task_create((Task_FuncPtr)HTTPPOSTTask, &taskParams, &eb);

     if (taskHandle == NULL) {
         printError("netIPAddrHook: Failed to create HTTP Task\n", -1);
     }

    }
}
/*
 *  ======== getMacAddress ========
 */
void getMacAddress(void)
{
    uint32_t ui32User0, ui32User1;
    uint8_t pui8MACAddr[6];



      //
      // Get the MAC address from the UART0 and UART1 registers in NV ram.
      //
      FlashUserGet(&ui32User0, &ui32User1);

      if((ui32User0 == 0xffffffff) || (ui32User1 == 0xffffffff))
          {
          //
          // We should never get here. This is an error if the MAC address has
          // not been programmed into the device. Exit the program.
          //
              while(1)
              {
              }
          }

      //
      // Convert the 24/24 split MAC address from NV ram into a MAC address
      // array.
      //
      pui8MACAddr[0] = ui32User0 & 0xff;
      pui8MACAddr[1] = (ui32User0 >> 8) & 0xff;
      pui8MACAddr[2] = (ui32User0 >> 16) & 0xff;
      pui8MACAddr[3] = ui32User1 & 0xff;
      pui8MACAddr[4] = (ui32User1 >> 8) & 0xff;
      pui8MACAddr[5] = (ui32User1 >> 16) & 0xff;

      System_printf("MAC: %02x-%02x-%02x-%02x-%02x-%02x\n", pui8MACAddr[0], pui8MACAddr[1], pui8MACAddr[2], pui8MACAddr[3], pui8MACAddr[4], pui8MACAddr[5]);


      snprintf(g_devicemac, sizeof(g_devicemac), "%02x:%02x:%02x:%02x:%02x:%02x",
               pui8MACAddr[0], pui8MACAddr[1], pui8MACAddr[2], pui8MACAddr[3], pui8MACAddr[4], pui8MACAddr[5]);

      System_flush();


}

/*
 *  ======== netIPAddrHook ========
 *  This function is called when IP Addr is added/deleted
 */
void netIPAddrHook(unsigned int IPAddr, unsigned int IfIdx, unsigned int fAdd)
{
    static Task_Handle taskHandle;
    Task_Params taskParams;
    Error_Block eb;


    getMacAddress();

    /* Create a HTTP task when the IP address is added */
    if (fAdd && !taskHandle) {
        Error_init(&eb);

        Task_Params_init(&taskParams);
        taskParams.stackSize = HTTPTASKSTACKSIZE;
        taskParams.priority = 1;
        //TODO: Add Posttask here
        taskHandle = Task_create((Task_FuncPtr)httpTask, &taskParams, &eb);
        //taskHandle = Task_create((Task_FuncPtr)HTTPPOSTTask, &taskParams, &eb);

        if (taskHandle == NULL) {
            printError("netIPAddrHook: Failed to create HTTP Task\n", -1);
        }
    }
}

/*
 *  ======== main ========
 */
int main(void)
{
    /*EVA
   /* uint32_t ui32SysClock;
        ui32SysClock = Board_initGeneral(120000000);
*/
        //Init I2C
        //Board_initGPIO();


        /*TODO: Setup Mailboxes?*/
        /*TODO: Setup UART*/



    /* Call board init functions */
    Board_initGeneral();
    Board_initGPIO();
    Board_initEMAC();
Board_initI2C();

    /* Turn on user LED */
    GPIO_write(Board_LED0, Board_LED_ON);
    deviceID=0;
    System_printf("Starting the HTTP POST Code\n");

    /* SysMin will only print to the console when you call flush or exit */
    System_flush();
 System_printf("Poll Task setup\n");
        setup_Poll_Task();

        /* Start BIOS */
        System_printf("Start BIOS\n");
        System_flush();
    /* Start BIOS */
    BIOS_start();

    return (0);
}
/*
 *  ======== printError ========
 */
void printError(char *errString, int code)
{

    System_printf("Error! code = %d, desc = %s\n", code, errString);
    BIOS_exit(code);
}
