 /**
 * @file httpget.c
 * Embedded Systems Engineering
 *
 * Code is based on TI-RTOS HTTP GET Example
 *
 *
 * @author Hannes Aurednik <ic15b004@technikum-wien.at>
 * @date 2018/07/01
 *
 * @version 1.1
 *
 *
 */

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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <inc/hw_memmap.h>
#include <sys/socket.h>

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Memory.h>

/* TI-RTOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <ti/sysbios/hal/Timer.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/GPIO.h>
#include <ti/net/http/httpcli.h>

/* Driverlib headers */
#include <driverlib/gpio.h>
#include <driverlib/flash.h>

/* Board Header files */
#include <Board.h>
#include <EK_TM4C1294XL.h>

/* Application headers */
#include <local_inc/UART_Task.h>
#include <local_inc/HTUTask.h>
#include <local_inc/Poll_Task.h>
#include <local_inc/httpget.h>
// #include <local_inc/jsmn.h> //not used

//************************************************************************************************************GLOBALS
Mailbox_Handle g_temp_mailbox;
Mailbox_Handle g_humi_mailbox;

//************************************************************************************************************FUNCTIONS


/*
 *  ======== initTempBox ========
 *  Initializes Mailbox for Temperature Values
 */
void initTempBox(){

        Error_Block eb;
        Mailbox_Params mailboxParams;
        Mailbox_Params_init(&mailboxParams);
        Error_init(&eb);
        g_temp_mailbox = Mailbox_create(sizeof(float), 100, &mailboxParams, &eb);
        if (g_temp_mailbox == NULL) {
            System_abort("Mailbox create failed");
        }

}
/*
 *  ======== initHumiBox ========
 *  Initializes Mailbox for Humidity Values
 */
void initHumiBox(){

        Error_Block eb;
        Mailbox_Params mailboxParams;
        Mailbox_Params_init(&mailboxParams);
        Error_init(&eb);
        g_humi_mailbox = Mailbox_create(sizeof(float), 100, &mailboxParams, &eb);
        if (g_humi_mailbox == NULL) {
            System_abort("Mailbox create failed");
        }

}




/*
 *  ======== httpTask ========
 *  DeviceID GET
 *  Value POST
 */
void httpTask(UArg arg0, UArg arg1)
{

    #ifdef DEBUG
    uint8_t debugger = 1;
    #endif
    Task_sleep(POLLINTVALL*1000); //Initial Sleep

    if(doHttpGet()==0){
        printError("httpTask: No ID received. Aborting.",1);
    }

    while(1){
        Task_sleep(POLLINTVALL*1000);

        Mailbox_pend(g_temp_mailbox, (float *) &g_temperature, BIOS_WAIT_FOREVER);
        Mailbox_pend(g_humi_mailbox, (float *) &g_rhval, BIOS_WAIT_FOREVER);

        #ifdef DEBUG
        System_printf("Trying to POST: ID:%c temperature: %0.2f humidity: %f ROUND: %d\n", deviceID, g_temperature, g_rhval, debugger);
        System_flush();
        debugger++;
        #endif

        doHttpPost(&g_temperature, &g_rhval);


    }

}
/*
 *  ======== doHttpPost ========
 *  HTTP POST Request
 *  POSTs temperature and humidity to the server
 */
int doHttpPost(volatile float *temp,volatile float *humid)
{
    bool moreFlag = false;
    char data[64];
    int ret;
    int len;
    char CONTENT_LENGTH[3];
    struct sockaddr_in addr;


    snprintf(data, sizeof(data), "{\"deviceId\":%d,\"temperature\": %.2f,\"humidity\": %.2f}", deviceID, *temp, *humid);


    len = strlen(data);
    sprintf(CONTENT_LENGTH, "%d", len);
    #ifdef DEBUG
    System_printf("\nData: %s\n", data);
    System_printf("len(int): %d\n", len);
    System_printf("CONTENT_LENGTH: %s\n", CONTENT_LENGTH);
    #endif

    HTTPCli_Struct cli;
    HTTPCli_Field fields[3] = {
        { HTTPStd_FIELD_NAME_HOST, HOSTNAME },
        { HTTPStd_FIELD_NAME_USER_AGENT, USER_AGENT },
        { NULL, NULL }
    };
    #ifdef DEBUG
    System_printf("Sending a HTTP POST request to '%s'\n", HOSTNAME);
    System_flush();
    #endif
    HTTPCli_construct(&cli);

    HTTPCli_setRequestFields(&cli, fields);

    ret = HTTPCli_initSockAddr((struct sockaddr *)&addr, HOSTNAME, 0); //-106 failed to resolve Host Name
    if (ret < 0) {
        printError("POSTTask: address resolution failed", ret);
    }

    ret = HTTPCli_connect(&cli, (struct sockaddr *)&addr, 0, NULL);
    if (ret < 0) {
        printError("POSTTask: connect failed", ret);
    }

    //SEND
    ret = HTTPCli_sendRequest(&cli, HTTPStd_POST, REQUEST_URI, true);
    if (ret < 0) {
        printError("POSTTask: send failed", ret);
    }
    else {
        System_printf("sendRequest successful\n");
        System_flush();
    }

    ret = HTTPCli_sendField(&cli, HTTPStd_FIELD_NAME_CONTENT_LENGTH, CONTENT_LENGTH, false);
    ret = HTTPCli_sendField(&cli, HTTPStd_FIELD_NAME_CONTENT_TYPE, CONTENT_TYPE, true);

    if (ret < 0) {
        printError("POSTTask: send failed", ret);
    }
    else {
        System_printf("sendField successful\n");
        System_flush();
    }

        ret = HTTPCli_sendRequestBody(&cli, data, strlen(data));
    if (ret < 0) {
        printError("POSTTask: Variable data couldn't be sent", ret);
    }
    else {
        System_printf("Data sent successfully\n");
        System_flush();
    }

    //RECEIVE
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


    HTTPCli_disconnect(&cli);
    HTTPCli_destruct(&cli);
    return 0;
}
/*
 *  ======== doHttpGet ========
 *  HTTP GET Request
 *  GETs deviceID from Server based on MAC-Address
 */
int doHttpGet()
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
    #ifdef DEBUG
    System_printf("Sending a HTTP GET request to '%s'\n", HOSTNAME);
    System_flush();
    #endif
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
    #ifdef DEBUG
    System_printf("HTTP Response Status Code: %d\n", ret);
    #endif
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
            deviceID = (int) (data[0]-'0');
            System_printf("Received DeviceID: %c\n", data[0]);
            System_flush();

        }


        len += ret;
    } while (moreFlag);
    #ifdef DEBUG
    System_printf("Received %d bytes of Payload\n", len);
    System_flush();
    #endif
    free(config);

    HTTPCli_disconnect(&cli);
    HTTPCli_destruct(&cli);

    return deviceID;

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

    /* Create a HTTP task when the IP address is added*/
    if (fAdd && !taskHandle) {
        Error_init(&eb);

        Task_Params_init(&taskParams);
        taskParams.stackSize = HTTPTASKSTACKSIZE;
        taskParams.priority = 1;

        taskHandle = Task_create((Task_FuncPtr)httpTask, &taskParams, &eb);


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
    Board_initI2C();

    /* Turn on user LED */
    GPIO_write(Board_LED0, Board_LED_ON);

    deviceID=0;
    initTempBox();
    initHumiBox();

    System_printf("Poll Task setup\n");
    System_flush();
    setup_Poll_Task(g_temp_mailbox, g_humi_mailbox);

    /* Start BIOS */
    System_printf("Start BIOS\n");
    System_flush();
    /* Start BIOS */
    BIOS_start();

    return (0);
}
/*
 *  ======== concat ========
 *  Combines two Strings
 */
char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    if(result==NULL)
        {
            printError("concat: memory not allocated\n", 0);
        }
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}
/*
 *  ======== getMacAddress ========
 *  Gets, prints and stores the MAC Address of the device
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
 *  ======== printError ========
 */
void printError(char *errString, int code)
{

    System_printf("Error! code = %d, desc = %s\n", code, errString);
    BIOS_exit(code);
}
