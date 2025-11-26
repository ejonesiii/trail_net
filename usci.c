/*
 * Author: Evan Jones III
 * Initial Commit: 11/25/2025
 * Last Commit: 11/25/2025
 *
 * A library for controlling the USCI (serial comm) interfaces. This is a combination of
 * the older i2c.h and spi.h libraries, which were removed for incompatability reasons when doing
 * SPI and I2C simultaneously due to the interrupts being shared across the USCI-A and USCI-B
 * peripherals.
 *
 * This work is covered under the MIT License
 * For license information, refer to the license file
 *
 * Written using Code Composer Studio v12. Have fun porting elsewhere :D
 */

// Include libraries
#include <msp430g2553.h>
#include <stdint.h>
#include "usci.h"

// Typedef for a USCI State machine
typedef enum USCI_ModeEnum{
    IDLE,
    SPI_TX,
    SPI_RX,
    I2C_TX,
    I2C_RX,
    UART_TX,
    UART_RX
} USCI_Mode;

USCI_Mode uscia0 = IDLE;
USCI_Mode uscib0 = IDLE;
/*
 * SPI Functions
 */
// Global Variables
uint8_t SPI_TX_BUF[MAX_BUF_SIZE] = {0};
uint8_t SPI_RX_BUF[MAX_BUF_SIZE] = {0};
uint8_t spiTxByteCtr = 0;
uint8_t spiRxByteCtr = 0;

/*
 * Initializes SPI in 4 wire mode, LSB first, Low SCLK, Low CS
 */
int spi_init(){
    uscia0 = IDLE;                          // Default to IDLE as to prevent transmissions of old data until ready
    P1SEL |= BIT1 + BIT2 + BIT4 + BIT5;     // Enable MISO, MOSI, SCLK, and CS
    P1SEL2 |= BIT1 + BIT2 + BIT4 + BIT5;    // Enable MISO, MOSI, SCLK, and CS
    UCA0CTL0 |= UCSYNC + UCMST + UCMODE_2 + UCCKPH; //Synchronous mode (Required), master mode, 4 wire mode with nCS, data captured on first clock edge
    UCA0CTL1 |= UCSSEL_3;                   // SMCLK
    UCA0BR0 = 16;                           // Divide SPI clock by 16 times
    UCA0BR1 = 0;
    UCA0MCTL = 0;
    UCA0CTL1 &= ~UCSWRST;                   // Enable USCI state machine
    //IE2 |= UCA0RXIE;                        // Enable RX interrupt
    return 0;
}

/*
 * Begins transmission of a byte or an array.
 * reg_addr is the address of the register you are writing to
 * data is the address of the first byte in the array
 * length is the number of bytes to be transmitted (counting does *NOT* start at zero, 5 byte array is length = 5)
 */
int spi_transmit(char reg_addr, char *data, char length){
    if(length > MAX_BUF_SIZE){
        return -1;                          // Error if trying to send array larger than max buffer size
    }
    else if(length == 1){

    }
    else{
        spiTxByteCtr = length - 2;
        uscia0 = SPI_TX;
    }


    return 0;
}


/*
 * I2C Functions TODO
 */

/*
 * UART Functions TODO
 */

/*
 * Interrupts
 */
// Tx interrupt vector (Being nice and supporting non-TI compilers
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCIAB0TX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0TX_VECTOR))) USCIAB0TX_ISR (void)
#else
#error Compiler not supported!
#endif
{
    if(IFG2 & UCA0TXIFG){                   // If USCI-A0 tx flag is tripped
        switch(uscia0){
        case SPI_TX:                        // SPI tx mode

            break;

        case UART_TX:                       // TODO UART

            break;
        }
        UCA0TXBUF = SPI_TX_BUF[spiTxByteCtr];      // Add byte to TX buffer and increment counter
        spiTxByteCtr++;
    }

    if(IFG2 & UCB0TXIFG){                   // if USCI-B0 tx flag is tripped
        switch(uscib0){
        case SPI_TX:

            break;

        case I2C_TX:

            break;
        }
    }

    __bic_SR_register_on_exit(CPUOFF);      // Exit LPM0
    IFG2 &= ~UCB0TXIFG;                     // Resets TX flag YOU MUST DO THIS OR SHIT BREAKS
}

// Rx interrupt vector
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCIAB0RX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCIAB0RX_ISR (void)
#else
#error Compiler not supported!
#endif
{

}
