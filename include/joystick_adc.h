#ifndef JOYSTICK_ADC_H
#define JOYSTICK_ADC_H

#include <stdint.h>

extern volatile uint16_t joystick_data[2];
void dma_setup(void);
void joystick_pin_adc_setup(void);

#endif