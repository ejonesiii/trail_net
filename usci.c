/*
 * Author: Evan Jones III
 * Initial Commit: 11/25/2025
 * Last Commit: 11/26/2025
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
uint8_t A0_TX_BUF[MAX_BUF_SIZE] = {0};
uint8_t A0_RX_BUF[MAX_BUF_SIZE] = {0};
uint8_t B0_TX_BUF[MAX_BUF_SIZE] = {0};
uint8_t B0_RX_BUF[MAX_BUF_SIZE] = {0};
uint8_t A0TxByteCtr = 0;
uint8_t A0RxByteCtr = 0;
uint8_t B0TxByteCtr = 0;
uint8_t B0RxByteCtr = 0;

/*
 * Initializes the USCI-A peripherial in SPI mode
 * Currenty initializes SPI in 4 wire mode, LSB first, Low SCLK, Low CS
 * TODO, add support for USCI-B SPI, changing SPI baud, and for other SPI modes (3 wire, MSB, phase, polarity)
 */
int A0_spi_init(){
    UCA0CTL1 = UCSWRST;                     // Reset before configuration
    uscia0 = IDLE;                          // Default to IDLE as to prevent transmissions of old data until ready
    P1SEL |= BIT1 + BIT2 + BIT4 + BIT5;     // Enable MISO, MOSI, SCLK, and CS
    P1SEL2 |= BIT1 + BIT2 + BIT4 + BIT5;    // Enable MISO, MOSI, SCLK, and CS
    UCA0CTL0 |= UCSYNC + UCMST + UCMODE_2 + UCCKPH; //Synchronous mode (Required), master mode, 4 wire mode with nCS, data captured on first clock edge
    UCA0CTL1 |= UCSSEL_3;                   // SMCLK
    UCA0BR0 = 16;                           // Divide SPI clock by 16 times
    UCA0BR1 = 0;
    UCA0MCTL = 0;
    UCA0CTL1 &= ~UCSWRST;                   // Enable USCI state machine
    return 0;
}

/*
 * Begins transmission of a byte or an array of bytes.
 *
 * reg is the register the data will be written to and is the first byte transmitted
 * data is the address of the first byte in the array
 * length is the number of bytes to be transmitted. If only sending a char and not a char array, set length = 1
 */
int A0_spi_transmit(char reg, char *data, char length){
    if(length > MAX_BUF_SIZE){
        return -1;                          // Error if trying to send array larger than max buffer size
    }
    else{
        char i;
        for(i=0; i<length; i++){       // Create a sort of FIFO buffer where the first byte out is the highest index in the array
            A0_TX_BUF[i] = *(data+(length-1)-i);
        }
        A0TxByteCtr = length - 1;          // Set counter for bytes remaining to be transmitted
        UCA0TXBUF = reg;                    // Transmit register
        IE2 |= UCA0TXIE;                    // Enable tx interrupt
        uscia0 = SPI_TX;                    // Set state machine to SPI_TX mode
        LPM0;                               // Enter low power mode
    }
    return 0;
}

/*
 * Receive data over the SPI bus and send it to an array beginning at a provided address
 *
 * reg is the register the data will be written to and is the first byte transmitted
 * data is the address of the first byte of the receive array
 * length is the number of bytes to be transmitted. If only sending a char and not a char array, set length = 1
 */
int A0_spi_receive(char reg, char *data, char length){
    if(length > MAX_BUF_SIZE){
        return -1;                          // Error if trying to send array larger than max buffer size
    }
    else{
        A0RxByteCtr = length-1;            // Set counter for expected bytes to be received
        UCA0TXBUF = reg;                    // Transmit register byte
        IE2 |= UCA0RXIE;                    // Enable rx interrupt
        uscia0 = SPI_RX;                    // Set state machine to SPI_RX mode
        LPM0;                               // Enter low power mode until reception is complete
        char i;
        for(i=0;i<length;i++){              // RX buf will be backwards from expected, so data is transferred in reverse indexing
            *(data+(length-1)-i) = A0_RX_BUF[i];
        }
    }
    return 0;
}

/*
 * Initializes the USCI-A peripherial in SPI mode
 * Currenty initializes SPI in 4 wire mode, LSB first, Low SCLK, Low CS
 * TODO, add support for USCI-B SPI, changing SPI baud, and for other SPI modes (3 wire, MSB, phase, polarity)
 */
int B0_spi_init(){
    UCB0CTL1 = UCSWRST;                     // Reset before configuration
    uscib0 = IDLE;                          // Default to IDLE as to prevent transmissions of old data until ready
    //P1REN = BIT7 + BIT6 + BIT4 + BIT5;
    UCB0CTL0 |= UCSYNC + UCMST + UCMODE_2 + UCCKPH; //Synchronous mode (Required), master mode, 4 wire mode with nCS, data captured on first clock edge
    UCB0CTL1 |= UCSSEL_3;                   // SMCLK
    UCB0BR0 = 16;                           // Divide SPI clock by 16 times
    UCB0BR1 = 0;
    P1SEL |= BIT7 + BIT6 + BIT4 + BIT5;     // Enable MISO, MOSI, SCLK, and CS
    P1SEL2 |= BIT7 + BIT6 + BIT4 + BIT5;    // Enable MISO, MOSI, SCLK, and CS
    UCB0CTL1 &= ~UCSWRST;                   // Enable USCI by removing reset bit
    return 0;
}

/*
 * Begins transmission of a byte or an array of bytes.
 *
 * reg is the register the data will be written to and is the first byte transmitted
 * data is the address of the first byte in the array
 * length is the number of bytes to be transmitted. If only sending a char and not a char array, set length = 1
 */
int B0_spi_transmit(char reg, char *data, char length){
    if(length > MAX_BUF_SIZE){
        return -1;                          // Error if trying to send array larger than max buffer size
    }
    else{
        uscib0 = SPI_TX;                    // Set state machine to SPI_TX mode
        char i;
        for(i=0; i<length; i++){            // Create a sort of FIFO buffer where the first byte out is the highest index in the array
            B0_TX_BUF[i] = *(data+(length-1)-i);
        }
        B0TxByteCtr = length - 1;           // Set counter for bytes remaining to be transmitted
        while (!(IFG2 & UCB0TXIFG));        // Wait until TX buffer ready
        UCB0TXBUF = reg;                    // Transmit register
        IE2 |= UCB0TXIE;                    // Enable tx interrupt
        LPM0;                               // Enter low power mode
    }
    return 0;
}

/*
 * Receive data over the SPI bus and send it to an array beginning at a provided address
 *
 * reg is the register the data will be written to and is the first byte transmitted
 * data is the address of the first byte of the receive array
 * length is the number of bytes to be transmitted. If only sending a char and not a char array, set length = 1
 */
int B0_spi_receive(char reg, char *data, char length){
    if(length > MAX_BUF_SIZE){
        return -1;                          // Error if trying to send array larger than max buffer size
    }
    else{
        B0RxByteCtr = length-1;             // Set counter for expected bytes to be received
        //while (!(IFG2 & UCB0TXIFG));        // Wait until TX buffer ready
        UCB0TXBUF = reg;                    // Transmit register byte
        IE2 |= UCB0RXIE;                    // Enable rx interrupt
        uscib0 = SPI_RX;                    // Set state machine to SPI_RX mode
        LPM0;                               // Enter low power mode until reception is complete
        char i;
        for(i=0;i<length;i++){              // RX buf will be backwards from expected, so data is transferred in reverse indexing
            *(data+(length-1)-i) = B0_RX_BUF[i];
        }
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
// Tx interrupt vector (Being nice and supporting non-TI compilers)
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
            if(A0TxByteCtr == 0){          // Last byte remaining; switch to idle after transmitting last byte
                UCA0TXBUF = A0_TX_BUF[A0TxByteCtr];
                uscia0 = IDLE;
                IE2 &= ~UCA0TXIE;           // Disable tx interrupts
                IFG2 &= ~UCA0TXIFG;         // Resets tx flag
                LPM0_EXIT;                  // Exit LPM0
            }
            else{
                UCA0TXBUF = A0_TX_BUF[A0TxByteCtr];      // Add byte to TX buffer and increment counter
                A0TxByteCtr--;
                IFG2 &= ~UCA0TXIFG;         // Resets tx flag
            }

            break;

        case UART_TX:                       // TODO UART

            break;
        }

    }

    if(IFG2 & UCB0TXIFG){                   // if USCI-B0 tx flag is tripped
        switch(uscib0){
        case SPI_TX:                        // TODO USCI-B SPI
            if(B0TxByteCtr == 0){          // Last byte remaining; switch to idle after transmitting last byte
                UCB0TXBUF = B0_TX_BUF[B0TxByteCtr];
                uscib0 = IDLE;
                IE2 &= ~UCB0TXIE;           // Disable tx interrupts
                IFG2 &= ~UCB0TXIFG;         // Resets tx flag
                LPM0_EXIT;                  // Exit LPM0
            }
            else{
                UCB0TXBUF = B0_TX_BUF[B0TxByteCtr];      // Add byte to TX buffer and increment counter
                B0TxByteCtr--;
                IFG2 &= ~UCB0TXIFG;         // Resets tx flag
            }
            break;

        case I2C_TX:                        // TODO I2C

            break;
        }
    }

    //__bic_SR_register_on_exit(CPUOFF);      // Exit LPM0
    //IFG2 &= ~UCB0TXIFG;                     // Resets TX flag YOU MUST DO THIS OR SHIT BREAKS
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
    if(IFG2 & UCA0RXIFG){                   // If USCI-A0 rx flag is tripped
        switch(uscia0){
        case SPI_RX:                        // SPI rx mode
            if(A0RxByteCtr == 0){          // Last byte remaining; switch to idle after transmitting last byte
                A0_RX_BUF[A0RxByteCtr] = UCA0RXBUF;
                uscia0 = IDLE;
                IE2 &= ~UCA0RXIE;           // Disable tx interrupts
                IFG2 &= ~UCA0RXIFG;         // Resets tx flag
                LPM0_EXIT;                  // Exit LPM0
            }
            else{
                A0_RX_BUF[A0RxByteCtr] = UCA0RXBUF;      // Add byte to TX buffer and increment counter
                A0RxByteCtr--;
                IFG2 &= ~UCA0RXIFG;         // Resets TX flag
            }

            break;

        case UART_RX:                       // TODO UART

            break;
        }

    }

    if(IFG2 & UCB0RXIFG){
        switch(uscib0){
        case SPI_RX:
            if(A0RxByteCtr == 0){          // Last byte remaining; switch to idle after transmitting last byte
                B0_RX_BUF[B0RxByteCtr] = UCB0TXBUF;
                uscib0 = IDLE;
                IE2 &= ~UCB0RXIE;           // Disable tx interrupts
                IFG2 &= ~UCB0RXIFG;         // Resets tx flag
                LPM0_EXIT;                  // Exit LPM0
            }
            else{
                B0_RX_BUF[B0RxByteCtr] = UCB0TXBUF;      // Add byte to TX buffer and increment counter
                B0RxByteCtr--;
                IFG2 &= ~UCB0RXIFG;         // Resets TX flag
            }
            break;

        case I2C_RX:                        // TODO I2C

            break;

        }
    }
}
