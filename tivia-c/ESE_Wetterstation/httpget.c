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
#include <string.h>
#include <stdio.h>

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

//MOCK
/*
#define HOSTNAME          "webhook.site"
#define REQUEST_URI       "/47567acc-32ec-4bf4-abd6-c03b7d5e5cef"
*/
//GET
#define HOSTNAME          "esesmarthome.azurewebsites.net"
#define CONFIG_URI       "/setup/00:23"
//POST
/*
#define HOSTNAME          "esesmarthome.azurewebsites.net"
*/
#define REQUEST_URI       "/api/measurements"

#define USER_AGENT        "HTTPCli (ARM; TI-RTOS)"
#define CONTENT_TYPE      "application/json"
#define HTTPTASKSTACKSIZE 4096


static volatile uint8_t device_mac[6];
/*
 *  ======== printError ========
 */
void printError(char *errString, int code)
{

    System_printf("Error! code = %d, desc = %s\n", code, errString);
    BIOS_exit(code);
}
//Copied Posttask from xxx
Void HTTPPOSTTask(UArg arg0, UArg arg1)
{
    bool moreFlag = false;
    char data[128];
    int ret;
    int len;
    char CONTENT_LENGTH[3];
    struct sockaddr_in addr;

    //Data to be sent
    strcpy(data, "{\"deviceId\": 1,\"timestamp\": \"2018-06-24T19:12:36.595Z\",\"temperature\": 25,\"humidity\": 45.6}");

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
        printError("httpTask: address resolution failed", ret);
    }

    ret = HTTPCli_connect(&cli, (struct sockaddr *)&addr, 0, NULL);
    if (ret < 0) {
        printError("httpTask: connect failed", ret);
    }

//************************************************************************************************************SEND
    ret = HTTPCli_sendRequest(&cli, HTTPStd_POST, REQUEST_URI, true);
    if (ret < 0) {
        printError("httpTask: send failed", ret);
    }
    else {
        System_printf("sendRequest successful\n");
    }

    ret = HTTPCli_sendField(&cli, HTTPStd_FIELD_NAME_CONTENT_LENGTH, CONTENT_LENGTH, false);
    ret = HTTPCli_sendField(&cli, HTTPStd_FIELD_NAME_CONTENT_TYPE, CONTENT_TYPE, true);

    if (ret < 0) {
        printError("httpTask: send failed", ret);
    }
    else {
        System_printf("sendField successful\n");
    }

        ret = HTTPCli_sendRequestBody(&cli, data, strlen(data));
    if (ret < 0) {
        printError("httpTask: Variable data couldn't be sent", ret);
    }
    else {
        System_printf("Data sent successfully\n");
    }
//*************************************************************************************************************RECEIVE

    ret = HTTPCli_getResponseStatus(&cli);
    if (ret != HTTPStd_OK) {
        if(ret != 204){
        printError("httpTask: cannot get status", ret);

        }
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

        len += ret;
    } while (moreFlag);

    System_printf("Received %d bytes of pay-load\n", len);
    System_flush();

    HTTPCli_disconnect(&cli);
    HTTPCli_destruct(&cli);
}

/*
 *  ======== httpTask ========
 *  Makes a HTTP GET request
 */
Void httpTask(UArg arg0, UArg arg1)
{
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

    ret = HTTPCli_sendRequest(&cli, HTTPStd_GET, CONFIG_URI, false);
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

        len += ret;
    } while (moreFlag);

    System_printf("Recieved %d bytes of payload\n", len);
    System_flush();

    HTTPCli_disconnect(&cli);
    HTTPCli_destruct(&cli);
}

getMacAddress()
{
    uint32_t ui32User0, ui32User1;
    uint8_t pui8MACAddr[6];
    uint8_t count = 0;

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
      System_flush();

      for(count =0; count<6;count++){
          device_mac[count] = pui8MACAddr[count];
      }

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
        //taskHandle = Task_create((Task_FuncPtr)httpTask, &taskParams, &eb);
        taskHandle = Task_create((Task_FuncPtr)HTTPPOSTTask, &taskParams, &eb);

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
    /* Call board init functions */
    Board_initGeneral();
    Board_initGPIO();
    Board_initEMAC();

    /* Turn on user LED */
    GPIO_write(Board_LED0, Board_LED_ON);

    System_printf("Starting the HTTP POST Code\n");

    /* SysMin will only print to the console when you call flush or exit */
    System_flush();

    /* Start BIOS */
    BIOS_start();

    return (0);
}
//*****************************************************************************
//
// Prints the current MAC address to the UART.
//
//*****************************************************************************

