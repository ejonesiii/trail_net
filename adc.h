/*
 * Author: Evan Jones III
 * Initial Commit: 10/29/2025
 * Last Commit: 10/30/2025
 *
 * A simple library for using the ADC on the MSP430G2553
 *
 * This work is covered under the MIT License
 * For license information, refer to the license file
 *
 * Written using Code Composer Studio v12. Have fun porting elsewhere :D
 */

// Include MSP430G2553 library and change name of some defs
#include <msp430g2553.h>
#include <stdint.h>

#ifndef ADC_H_
#define ADC_H_

// ADC Channels to enable
#define ADC0 INCH_0
#define ADC1 INCH_1
#define ADC2 INCH_2
#define ADC3 INCH_3
#define ADC4 INCH_4
#define ADC5 INCH_5
#define ADC6 INCH_6
#define ADC7 INCH_7

// ADC functions
int adc_single_init(uint8_t channel);
unsigned int adc_single_read(void);


#endif /* ADC_H_ */
