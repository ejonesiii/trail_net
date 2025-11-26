/*
 * Author: Evan Jones III
 * Initial Commit: 5/1/2022
 * Last Commit: 10/28/2025
 *
 * A simple I2C library for the MSP430Gxxxx family of microcontrollers
 * Theoretically can be ported for other MSP430s
 *
 * This work is covered under the MIT License
 * For license information, refer to the license file
 *
 * Written using Code Composer Studio v12. Have fun porting elsewhere :D
 */

#include <stdint.h>

#ifndef I2C_H_
#define I2C_H_

/* Init functions */
void i2c_init(void);
void i2c_tx_single(char addr, char tx_buf);
void i2c_tx_double(char addr, char tx_buf0, char tx_buf1);
void i2c_tx_mult(char addr, volatile unsigned char tx_buf[],uint8_t elmt);
char i2c_tx_rx_single(char addr, char tx_buf);
char i2c_rx_single(char addr);



#endif /* I2C_H_ */
