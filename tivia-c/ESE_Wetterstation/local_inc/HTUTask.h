/*
 * HTUTask.h
 *
 *  Author: Hannes Aurednik, Eva Gergely, Stephan Nöhrer, Salko Nuhanovic
 *
 */

#ifndef LOCAL_INC_HTUTASK_H_
#define LOCAL_INC_HTUTASK_H_

//#define DEBUG //for debugging only

#include <ti/drivers/I2C.h>

/* \fn HTU21DSoftreset
 * \brief HTU21DSoftreset
 *
 * Performs soft reset which is recommended at the start of the device (p. 10ff - documentation HTU21D)
 * Reboots the HTU21D sensor switching the power off and on again.
 * Upon reception of this command, the HTU21D sensor system reinitializes
 * and starts operation according to the default settings (with the exception of the heater bit in the userreg)
 *
 * \param I2CHandle - I2C config struct
 * \param I2CTransaction - defines the nature of the I2C transaction. Specifies the buffer and the buffer's size
 *                         that is to be written to or read from the I2C slave peripheral
 *
 * \return void
 */
void HTU21DSoftreset(I2C_Handle *I2CHandle, I2C_Transaction *I2CTransaction);

/* \fn HTU21D_REG_read
 * \brief HTU21D_REG_read
 *
 * Reads the user register of the slave peripheral
 *
 * \param I2CHandle - I2C config struct
 * \param I2CTransaction - defines the nature of the I2C transaction. Specifies the buffer and the buffer's size
 *                         that is to be written to or read from the I2C slave peripheral
 *
 * \return void
 */
void HTU21D_REG_read(I2C_Handle *I2CHandle, I2C_Transaction *I2CTransaction);

/* \fn HTU21D_REG_write
 * \brief HTU21D_REG_write
 *
 * Performs configuration. Sets measurement resolution.
 *
 * \param I2CHandle - I2C config struct
 * \param I2CTransaction - defines the nature of the I2C transaction. Specifies the buffer and the buffer's size
 *                         that is to be written to or read from the I2C slave peripheral
 *
 * \return void
 */
void HTU21D_REG_write(I2C_Handle *I2CHandle, I2C_Transaction *I2CTransaction);

/* \fn CalcValues
 * \brief CalcValues
 *
 * Calculates correct temperature and relative humidity values.
 * For more details refer to HTU21D example code.
 *
 * \param readBuffer - value to be converted
 *
 * \return calculated value
 */
unsigned CalcValues(UChar *readBuffer);

/* \fn GetTemperature
 * \brief GetTemperature
 *
 * Determines temperature value provided by the slave peripheral.
 *
 * \param I2CHandle - I2C config struct
 * \param I2CTransaction - defines the nature of the I2C transaction. Specifies the buffer and the buffer's size
 *                         that is to be written to or read from the I2C slave peripheral
 *
 * \return float temperature (in Celsius)
 */
float GetTemperature(I2C_Handle *I2CHandle, I2C_Transaction *I2CTransaction);

/* \fn GetRelativeHumidity
 * \brief GetRelativeHumidity
 *
 * Determines relative humidity value provided by the slave peripheral.
 *
 * \param I2CHandle - I2C config struct
 * \param I2CTransaction - defines the nature of the I2C transaction. Specifies the buffer and the buffer's size
 *                         that is to be written to or read from the I2C slave peripheral
 *
 * \return float relative humidity (in percentage)
 */
float GetRelativeHumidity(I2C_Handle *I2CHandle, I2C_Transaction *I2CTransaction);


/* \fn I2CTransferConfig
 * \brief I2CTransferConfig
 *
 * I2C Transfer to the I2C Interface
 *
 * \param writeBuffer - buffer holding data which is to be written
 * \param writeBufLen - number of bytes to be written to the slave peripheral
 * \param readBuffer - buffer which will hold the data that is read
 * \param readBufLen - number of bytes to be read from the slave peripheral
 * \param I2CHandle - I2C config struct
 * \param I2CTransaction - defines the nature of the I2C transaction. Specifies the buffer and the buffer's size
 *                         that is to be written to or read from the I2C slave peripheral
 *
 */
int I2CTransferConfig(UChar *writeBuffer, size_t writeBufLen, UChar *readBuffer, size_t readBufLen, I2C_Handle *I2CHandle, I2C_Transaction *I2CTransaction);



#endif /* LOCAL_INC_HTUTASK_H_ */
