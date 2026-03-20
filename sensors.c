/*
 * Author: Evan Jones III
 * Initial Commit: 12/2/2025
 * Last Commit: 12/3/2025
 *
 * A library for controlling the sensors used on the trail_net project
 *
 * This work is covered under the MIT License
 * For license information, refer to the license file
 *
 * Written using Code Composer Studio v12. Have fun porting elsewhere :D
 */

#include "adc.h"

int distance(void){
    int raw;
    adc_single_init(ADC0);
    raw = adc_single_read();
    return raw;
}

int temperature(void){
    int raw;
    int temperature;
    adc_single_init(TEMP_SEN);
    raw = adc_single_read();
    temperature = ((raw - 673) * 423) / 1024;
    return temperature;
}

int moisture(void){
    int raw;
    adc_single_init(ADC3);
    raw = adc_single_read();
    return raw;
}
