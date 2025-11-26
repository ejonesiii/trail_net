/*
 * Author: Evan Jones III
 * Initial Commit: 5/1/2022
 * Last Commit: 11/21/2025
 *
 * A simple I2C library for the MSP430Gxxxx family of microcontrollers
 * Theoretically can be ported for other MSP430s
 *
 * This work is covered under the MIT License
 * For license information, refer to the license file
 *
 * Written using Code Composer Studio v12. Have fun porting elsewhere :D
 */

#include <msp430g2553.h>
#include <i2c.h>
#include <stdint.h>

volatile unsigned char I2C_TX_BUF[];
volatile unsigned char I2C_RX_BUF[];

static unsigned int ByteCtr;                // Create a counter


// Initializes the I2C bus
void i2c_init(void){
    P1SEL  |= BIT6 + BIT7;                  // Initialize pins 1.6 (SCL) & 1.7 (SDA)
    P1SEL2 |= BIT6 + BIT7;

    UCB0CTL1 |= UCSWRST;                    // Reset module before configuring
    UCB0CTL0 |= UCMST + UCMODE_3 + UCSYNC;  // Master mode, I2C, synchronous mode
    UCB0CTL1 |= UCSSEL_2 + UCSWRST;         // SMCLK clock source, keep reset
    UCB0BR0 = 12;                           // Transmit clock divisor (~100 kHz I2C)
    UCB0BR1 = 0;                            // N/A
    UCB0CTL1 &= ~UCSWRST;                   // Release reset after configuring
}


void i2c_tx_single(char addr, char tx_buf){ // Sends a single byte of data
    ByteCtr = 0;                            // Reset counter
    int WDT = 0;
    UCB0I2CSA = addr;                       // Set slave address

    IE2 |= UCB0TXIE;                        // Enable interrupts
    I2C_TX_BUF[0] = tx_buf;                     // Load buffer PRIOR to transmission. This seems to matter.
    UCB0CTL1 |= UCTXSTT + UCTR;             // Start transmission
    __enable_interrupt();                   // Enable interrupts

    while((ByteCtr <= 1) && (WDT <= 1000)){ // Wait until transmission finishes or times out
        WDT++;
    }

    UCB0CTL1 |= UCTXSTP;                    // Stop transmission

}


void i2c_tx_double(char addr, char tx_buf0, char tx_buf1){  // Sends two bytes of data
    ByteCtr = 0;                            // Reset counter
    int WDT = 0;                            // Simple WDT
    UCB0I2CSA = addr;                       // Set slave address

    IE2 |= UCB0TXIE;                        // Enable interrupts
    I2C_TX_BUF[0] = tx_buf0;                    // Load buffer PRIOR to transmission. This seems to matter.
    I2C_TX_BUF[1] = tx_buf1;                    // Load second byte cause why not, its midnight anyhow
    UCB0CTL1 |= UCTXSTT + UCTR;             // Start transmission
    __enable_interrupt();                   // Enable interrupts

    while((ByteCtr <= 2) && (WDT <= 2000)){ // Wait until transmission finishes or times out
        WDT++;
    }

    UCB0CTL1 |= UCTXSTP;                    // Stop transmission

}

void i2c_tx_mult(char addr, volatile unsigned char tx_buf[], uint8_t elmt){ // Sends an 'n' long array of data
    uint8_t i = 0;
    int WDT = 0;
    ByteCtr = 0;
    UCB0I2CSA = addr;

    IE2 |= UCB0TXIE;                        // Enable interrupts
    I2C_TX_BUF[0] = tx_buf[0];                  // Pre-load the first byte before starting transmission. This seems to matter
    UCB0CTL1 |= UCTXSTT + UCTR;             // Start transmission
    for(i=1; i <= elmt; i++){
        I2C_TX_BUF[i] = tx_buf[i];              // Load the buffer
    }

    __enable_interrupt();                   // Enable interrupts

    while((ByteCtr <= elmt) && (WDT <= elmt*1000)){// Wait until transmission finishes
        WDT++;
    }

    UCB0CTL1 |= UCTXSTP;                    // Stop Transmission
}

char i2c_rx_single(char addr){
    ByteCtr = 0;
    UCB0I2CSA = addr;
    int WDT = 0;

    IE2 |= UCB0RXIE;                        // Enable interrupts
    UCB0CTL1 &= ~UCTR;                      // Set to read mode
    UCB0CTL1 |= UCTXSTT;                    // Start transmission
    __enable_interrupt();                   // Enable interrupts

    while(UCB0CTL1 & UCTXSTT);
    UCB0CTL1 |= UCTXSTP;                    // Stop transmission
    while((ByteCtr < 1) && (WDT <= 1000)){  // Wait until transmission finishes or times out
        WDT++;
    }
    return I2C_RX_BUF[0];
}


// TODO: ACTUALLY DO MULTIPLE RX
/*
unsigned char * i2c_rx_mult(char addr, uint8_t elmt){
    ByteCtr = 0;
    UCB0I2CSA = addr;
    unsigned int WDT = 0;
    unsigned char ret[elmt];

    IE2 |= UCB0RXIE;                        // Enable interrupts
    UCB0CTL1 &= ~UCTR;                      // Set to read mode
    UCB0CTL1 |= UCTXSTT;                    // Start transmission
    __enable_interrupt();                   // Enable interrupts

    while(UCB0CTL1 & UCTXSTT);
    UCB0CTL1 |= UCTXSTP;                    // Stop transmission
    while((ByteCtr < elmt-1) && (WDT <= 1000*elmt)){    // Wait until transmission finishes or times out
        WDT++;
    }
    UCB0CTL1 |= UCTXSTP;                    // Stop transmission
    for(WDT = 0; WDT <= elmt; WDT++){
        ret[WDT] = I2C_RX_BUF[WDT];
    }
    return ret;
}
*/

char i2c_tx_rx_single(char addr, char tx_buf){  // Sends a single byte of data to set register, then reads the register
    ByteCtr = 0;                            // Reset counter
    int WDT = 0;
    UCB0I2CSA = addr;                       // Set slave address

    IE2 |= UCB0TXIE;                        // Enable interrupts
    I2C_TX_BUF[0] = tx_buf;                     // Load buffer PRIOR to transmission. This seems to matter.
    UCB0CTL1 |= UCTXSTT + UCTR;             // Start transmission
    __enable_interrupt();                   // Enable interrupts

    while((ByteCtr <= 1) && (WDT <= 1000)){ // Wait until transmission finishes or times out
        WDT++;
    }

    UCB0CTL1 |= UCTXSTP;                    // Stop transmission

    ByteCtr = 0;
    WDT = 0;
    IE2 |= UCB0RXIE;                        // Enable interrupts
    UCB0CTL1 &= ~UCTR;                      // Set to read mode
    UCB0CTL1 |= UCTXSTT;                    // Start transmission
    __enable_interrupt();                   // Enable interrupts

    while(UCB0CTL1 & UCTXSTT);              // Wait until start condition is sent
    UCB0CTL1 |= UCTXSTP;                    // Stop transmission
    while((ByteCtr < 1) && (WDT <= 1000)){  // Wait until transmission finishes or times out
        WDT++;
    }
    return I2C_RX_BUF[0];
}

#pragma vector = USCIAB0TX_VECTOR           // Transmit interrupts
__interrupt void USCIAB0TX_ISR (void){
    if(UCB0CTL1 & UCTR){
        UCB0TXBUF = I2C_TX_BUF[ByteCtr];        // Add byte to TX buffer and increment counter
        ByteCtr++;
    }
    else{
        I2C_RX_BUF[ByteCtr] = UCB0RXBUF;        // Add byte to RX buffer and decrement counter
        ByteCtr++;
    }
    __bic_SR_register_on_exit(CPUOFF);      // Exit LPM0
    IFG2 &= ~UCB0TXIFG;                     // Resets TX flag YOU MUST DO THIS OR SHIT BREAKS
}

// TODO: ACTUALLY WRITE I2C RECEIVE FLAGS
#pragma vector = USCIAB0RX_VECTOR           // Receive interrupts
__interrupt void USCIAB0RX_ISR (void){
    __bic_SR_register_on_exit(CPUOFF);      // Exit LPM0
    IFG2 &= ~UCB0RXIFG;                     // Resets RX flag YOU MUST DO THIS OR SHIT BREAKS
}
