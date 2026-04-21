#pragma once
#include "stm32f4xx.h"

// Pin definitions
#define START_BUTTON_PIN    15
#define BUTTON1_PIN         10
#define BUTTON2_PIN         12
#define BUTTON3_PIN         13
#define BUTTON4_PIN         14

#define JOY1_X_ADC  1
#define JOY1_Y_ADC  2
#define JOY1_X_PIN  1
#define JOY1_Y_PIN  2

#define JOY2_X_ADC  3
#define JOY2_Y_ADC  4
#define JOY2_X_PIN  3
#define JOY2_Y_PIN  4

#define JOY3_X_ADC  5
#define JOY3_Y_ADC  6
#define JOY3_X_PIN  5
#define JOY3_Y_PIN  6

#define JOY4_X_ADC  8
#define JOY4_Y_ADC  9
#define JOY4_X_PIN  0
#define JOY4_Y_PIN  1

#define TOKEN1_PIN  5
#define TOKEN2_PIN  4
#define TOKEN3_PIN  8
#define TOKEN4_PIN  9

#define LED_1_PIN   2
#define LED_2_PIN   3

// Init
void GPIO_Init(void);
void USART6_Init(void);
void ADC_Init(void);
void EXTI_Init(void);

// Helpers
void delay(volatile uint32_t count);
void LED_Set(uint8_t pin, uint8_t state);
void Joy_Read(uint8_t x_channel, uint8_t y_channel, uint16_t *x, uint16_t *y);
uint16_t ADC_Read(uint8_t channel);

// UART 
char USART6_ReadChar(void);
void USART6_ReadLine(char* buf, int maxlen);

void USART6_SendChar(char c);
