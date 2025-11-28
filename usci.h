/*
 * Author: Evan Jones III
 * Initial Commit: 11/25/2025
 * Last Commit: 11/26/2025
 *
 * A library for controlling the USCI (serial comm) interfaces on the MSP430G2553. This is a combination of
 * the older i2c.h and spi.h libraries, which were removed for incompatability reasons when doing
 * SPI and I2C simultaneously due to the interrupts being shared across the USCI-A and USCI-B
 * peripherals.
 *
 * This work is covered under the MIT License
 * For license information, refer to the license file
 *
 * Written using Code Composer Studio v12. Have fun porting elsewhere :D
 */

#ifndef USCI_H_
#define USCI_H_

#define MAX_BUF_SIZE    32

int A0_spi_init();
int A0_spi_transmit(char reg, char *data, char length);
int A0_spi_receive(char reg, char *data, char length);
int B0_spi_init();
int B0_spi_transmit(char reg, char *data, char length);
int B0_spi_receive(char reg, char *data, char length);

#endif /* USCI_H_ */
