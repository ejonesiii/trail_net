/*
 * sensors.c
 *
 *  Created on: Dec 2, 2025
 *      Author: ejjonesiii
 */

#include "adc.h"

int distance(void){
    int raw;
    adc_single_init(ADC0);
    raw = adc_single_read();

}

int temperature(void){
    int raw;
    float voltage, temperature;
    adc_single_init(10);
    raw = adc_single_read();
    voltage = raw*(2.5/1024);
    temperature = (raw-0.986)/0.00355;
    return int(temperature);
}

int moisture(){


}
