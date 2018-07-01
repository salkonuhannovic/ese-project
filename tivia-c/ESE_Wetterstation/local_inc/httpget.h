 /**
 * @file httpget.h
 * Embedded Systems Engineering
 *
 * Starts an HTTP Task after an IP-Address is received per DHCP
 * Uses HTTP POST to send temperature and humidity values to a defined server
 * Values to be sent are received through mailbox
 *
 *
 * @author Hannes Aurednik <ic15b004@technikum-wien.at>
 * @date 2018/07/01
 *
 * @version 1.1
 *
 *
 */

#ifndef LOCAL_INC_HTTPGET_H_
#define LOCAL_INC_HTTPGET_H_


#include <stdbool.h>
#include <stdint.h>

//************************************************************************************************************DEFINES

/*MOCK - Example https://webhook.site/d6e1ab21-cb8e-4635-85f9-0046b7873f6b
#define HOSTNAME          "webhook.site"
#define REQUEST_URI       "/d6e1ab21-cb8e-4635-85f9-0046b7873f6b"
#define CONFIG_URI       "/d6e1ab21-cb8e-4635-85f9-0046b7873f6b/"
*/

#define HOSTNAME          "esesmarthome.azurewebsites.net"
#define CONFIG_URI       "/setup/"
#define REQUEST_URI       "/api/measurements"

#define USER_AGENT        "HTTPCli (ARM; TI-RTOS)"
#define CONTENT_TYPE      "application/json"
#define HTTPTASKSTACKSIZE 4096

#define POLLINTVALL 20 //Polling Interval in Seconds

#define DEBUG

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
int doHttpPost(volatile float *temp,volatile float *humid);
char* concat(const char *s1, const char *s2);
int doHttpGet();


#endif /* LOCAL_INC_HTTPGET_H_ */
