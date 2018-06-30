/*
 * httpget.h
 *
 *  Created on: 29.06.2018
 *      Author: Hannes
 */

#ifndef LOCAL_INC_HTTPGET_H_
#define LOCAL_INC_HTTPGET_H_

/*
 *  ======== httpget.h ========
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
//************************************************************************************************************DEFINES
//MOCK https://webhook.site/d6e1ab21-cb8e-4635-85f9-0046b7873f6b

#define HOSTNAME          "webhook.site"
#define REQUEST_URI       "/d6e1ab21-cb8e-4635-85f9-0046b7873f6b"
#define CONFIG_URI       "/d6e1ab21-cb8e-4635-85f9-0046b7873f6b/"

//OUR SERVICE
/*
#define HOSTNAME          "esesmarthome.azurewebsites.net"
#define CONFIG_URI       "/setup/"
#define REQUEST_URI       "/api/measurements"
*/
#define USER_AGENT        "HTTPCli (ARM; TI-RTOS)"
#define CONTENT_TYPE      "application/json"
#define HTTPTASKSTACKSIZE 4096

#define POLLINTVALL 2 //Polling Intervall in Seconds
//************************************************************************************************************GLOBALS
static volatile uint8_t deviceID;
static volatile char g_devicemac[18];
static volatile float g_temperature;
static volatile float g_rhval;

//************************************************************************************************************PROTOTYPES
void printError(char *errString, int code);
int main(void);
void netIPAddrHook(unsigned int IPAddr, unsigned int IfIdx, unsigned int fAdd);
void getMacAddress(void);
void httpTask(UArg arg0, UArg arg1);
int doHttpPost(float temp, float humid);
char* concat(const char *s1, const char *s2);

int doHttpGet();


#endif /* LOCAL_INC_HTTPGET_H_ */
