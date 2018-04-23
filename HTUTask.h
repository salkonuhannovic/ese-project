/*
 * HTUTask.h
 *
 *  Created on: 2018. 04. 22.
 *      Author: Eva Gergely
 */

#ifndef LOCAL_INC_HTUTASK_H_
#define LOCAL_INC_HTUTASK_H_

#include <ti/drivers/I2C.h>

/* \fn HTU21DSoftreset
 * \brief
 *
 */
void HTU21DSoftreset(I2C_Handle *I2CHandle, I2C_Transaction *I2CTransaction);

/* \fn HTU21D_REG_read
 * \brief
 *
 */
void HTU21D_REG_read(I2C_Handle *I2CHandle, I2C_Transaction *I2CTransaction);

/* \fn HTU21D_REG_write
 * \brief
 *
 */
void HTU21D_REG_write(I2C_Handle *I2CHandle, I2C_Transaction *I2CTransaction);

/* \fn GetTemperature
 * \brief
 *
 */
float GetTemperature(I2C_Handle *I2CHandle, I2C_Transaction *I2CTransaction);

/* \fn I2CTransferConfig
 * \brief
 *
 */
int I2CTransferConfig(UChar *writeBuffer, size_t writeBufLen, UChar *readBuffer, size_t readBufLen, I2C_Handle *I2CHandle, I2C_Transaction *I2CTransaction);


#endif /* LOCAL_INC_HTUTASK_H_ */
