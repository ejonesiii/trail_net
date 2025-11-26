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

#include <msp430g2553.h>
#include <stdint.h>
#include "spi.h"

// Global Variables
uint8_t SPI_TX_BUF[BUF_SIZE] = {0};
uint8_t SPI_RX_BUF[BUF_SIZE] = {0};
uint8_t txByteCtr = 0;
uint8_t rxByteCtr = 0;

/*
 * Initializes SPI in 4 wire mode, LSB first, Low SCLK, Low CS
 */
int spi_init(){
    P1SEL |= BIT1 + BIT2 + BIT4 + BIT5;     // Enable MISO, MOSI, SCLK, and CS
    P1SEL2 |= BIT1 + BIT2 + BIT4 + BIT5;    // Enable MISO, MOSI, SCLK, and CS
    UCA0CTL0 |= UCSYNC + UCMST + UCMODE_2 + UCCKPH; //Synchronous mode (Required), master mode, 4 wire mode with nCS, data captured on first clock edge
    UCA0CTL1 |= UCSSEL_3;                   // SMCLK
    UCA0BR0 = 16;                           // Divide SPI clock by 16 times
    UCA0BR1 = 0;
    UCA0MCTL = 0;
    UCA0CTL1 &= ~UCSWRST;                   // Enable USCI state machine
    IE2 |= UCA0RXIE;                        // Enable RX interrupt
    return 0;
}

int spi_transmit(uint8_t reg_addr, uint8_t *data, uint8_t count){

    return 0;
}

#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCIA0TX_ISR(void){
    UCA0TXBUF = SPI_TX_BUF[txByteCtr];      // Add byte to TX buffer and increment counter
    txByteCtr++;
    __bic_SR_register_on_exit(CPUOFF);      // Exit LPM0
    IFG2 &= ~UCB0TXIFG;                     // Resets TX flag YOU MUST DO THIS OR SHIT BREAKS
}
