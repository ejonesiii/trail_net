/*
 * Author: Evan Jones III
 * Initial Commit: 10/28/2025
 * Last Commit: 10/30/2025
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
#include "i2c.h"
#include "adc.h"

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	// Set up MCLK and SMCLK for a base speed of TBD MHz and ACLK to use the 32 kHz XTAL

	// TODO: Init ports
	i2c_init();
	adc_single_init(1);
	unsigned int val = adc_single_read();
	// TODO: Init sensors

	// TODO: Init wireless network

	// TODO: Init interrupts and LPM

	return 0;
}
