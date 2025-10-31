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

#include <msp430g2553.h>
#include "adc.h"
#include <stdint.h>

// ADC single sample/read functions
int adc_single_init(uint8_t channel){
    // Reset ADC to prevent misconfiguration
    ADC10CTL0 = 0x0000;         // Must be done before other registers as ENC being set to 1 would prevent configuration
    ADC10CTL1 = 0x0000;
    ADC10AE0 = 0x00;
    ADC10DTC0 = 0x00;
    ADC10DTC1 = 0x00;
    ADC10SA = 0x0000;

    // Enable the ADC
    switch(channel){
        case 0:
            ADC10CTL1 = INCH_0 + ADC10DF;
            ADC10AE0 = 0b00000001;
            break;
        case 1:
            ADC10CTL1 = INCH_1 + ADC10DF;
            ADC10AE0 = 0b00000010;
            break;
        case 2:
            ADC10CTL1 = INCH_2 + ADC10DF;
            ADC10AE0 = 0b00000100;
            break;
        case 3:
            ADC10CTL1 = INCH_3 + ADC10DF;
            ADC10AE0 = 0b00001000;
            break;
        case 4:
            ADC10CTL1 = INCH_4 + ADC10DF;
            ADC10AE0 = 0b00010000;
            break;
        case 5:
            ADC10CTL1 = INCH_5 + ADC10DF;
            ADC10AE0 = 0b00100000;
            break;
        case 6:
            ADC10CTL1 = INCH_6 + ADC10DF;
            ADC10AE0 = 0b01000000;
            break;
        case 7:
            ADC10CTL1 = INCH_7 + ADC10DF;
            ADC10AE0 = 0b10000000;
            break;
        default:
            return -1;
    }
    // Final configuration, ADC not yet enabled
    ADC10CTL0 = SREF_1 + ADC10SHT_2 + ADC10SR + REF2_5V + REFON + ADC10ON + ADC10IE;
    return 0;
}

unsigned int adc_single_read(void){
    ADC10CTL0 = ENC + ADC10SC;
    LPM0;
    return ADC10MEM;
}



// TODO: ADC Sequence Functions
void adc_seq_init(short channels){


}

// TODO: ADC ISR
#pragma vector = ADC10_VECTOR           // Receive interrupts
__interrupt void ADC10_ISR (void){
    LPM0_EXIT;                          // Exit LPM0
}

