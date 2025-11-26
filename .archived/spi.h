/*
 * Author: Evan Jones III
 * Initial Commit: 11/13/2025
 * Last Commit: 11/21/2025
 *
 * A simple SPI library for the MSP430G2553
 *
 * This work is covered under the MIT License
 * For license information, refer to the license file
 *
 * Written using Code Composer Studio v12. Have fun porting elsewhere :D
 */

#include <stdint.h>


#ifndef SPI_H_
#define SPI_H_

#define BUF_SIZE 16

int spi_init();
int spi_transmit(uint8_t reg_addr, uint8_t *data, uint8_t count);



#endif /* SPI_H_ */
