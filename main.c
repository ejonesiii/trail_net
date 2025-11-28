/*
 * Author: Evan Jones III
 * Initial Commit: 10/28/2025
 * Last Commit: 11/26/2025
 *
 * A wireless sensor network for measuring trail conditions of a nordic ski trail
 * For a detailed explanation, see README.md
 *
 * This work is covered under the MIT License
 * For license information, refer to the license file
 *
 * Written using Code Composer Studio v12. Have fun porting elsewhere :D
 */

#include <msp430g2553.h>
#include "adc.h"
#include "usci.h"


int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	// Set up MCLK and SMCLK for a base speed of 16 MHz and ACLK to use the 32 kHz XTAL
	DCOCTL = 0x00;          // Stop clock before changing system
	BCSCTL3 = LFXT1S_2;     // Set LFXT1 to 32k XTAL TODO: Solder 32k XTAL to board and switch from VLO to 32k
	BCSCTL2 = 0x00;         // MCLK is set to DCO with no division
	BCSCTL1 = CALBC1_16MHZ; // Set DCO to 16 MHz
	DCOCTL = CALDCO_16MHZ;


	// TODO: Init ports
	adc_single_init(4);
	//TODO TODO TODO SET PIN HIGH TO ENABLE nRF24
	char samp_array[5] = {0x43,0x22,0x44,0x19,0xFF};
	__enable_interrupt();
	// TODO: Init sensors


	// TODO: Init wireless network
    B0_spi_init();             // Init SPI peripheral
    while(1){
        __delay_cycles(10000);
        B0_spi_receive(0x07,&samp_array[0],1);           // Read nRF24 status to check for operation
    }
	// TODO: Init interrupts and LPM
    //__no_operation();
	return 0;
}
