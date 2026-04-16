#include "stm32f4xx.h"
#include <stdio.h>

// ---- Pin configurations ----
// Buttons
#define START_BUTTON_PIN    15  // PB15
#define BUTTON1_PIN         10  // PB10
#define BUTTON2_PIN         12  // PB12
#define BUTTON3_PIN         13  // PB13
#define BUTTON4_PIN         14  // PB14

// Joysticks
#define JOY1_X_ADC  1   // PA1 = ADC channel 1
#define JOY1_Y_ADC  2   // PA2 = ADC channel 2
#define JOY1_X_PIN  1   // PA1
#define JOY1_Y_PIN  2   // PA2

#define JOY2_X_ADC  3   // PA3 = ADC channel 3
#define JOY2_Y_ADC  4   // PA4 = ADC channel 4
#define JOY2_X_PIN  3   // PA3
#define JOY2_Y_PIN  4   // PA4

#define JOY3_X_ADC  5   // PA5 = ADC channel 5
#define JOY3_Y_ADC  6   // PA6 = ADC channel 6
#define JOY3_X_PIN  5   // PA5
#define JOY3_Y_PIN  6   // PA6

#define JOY4_X_ADC  8   // PB0 = ADC channel 8
#define JOY4_Y_ADC  9   // PB1 = ADC channel 9
#define JOY4_X_PIN  0   // PB0
#define JOY4_Y_PIN  1   // PB1

// Sensors
#define TOKEN1_PIN  5   // PC5
#define TOKEN2_PIN  4   // PC4
#define TOKEN3_PIN  8   // PC8
#define TOKEN4_PIN  9   // PC9

// On-board LEDs
#define LED_1_PIN   2   // PC2
#define LED_2_PIN   3   // PC3
// ---------------------------

void delay(volatile uint32_t count) {
    while(count--) {
        __asm__("nop");
    }
}

// ---- USART6 ----

void USART6_Init(void) {
    RCC->AHB1ENR  |= RCC_AHB1ENR_GPIOCEN;
    RCC->APB2ENR  |= RCC_APB2ENR_USART6EN;

    // PC6 = TX (AF8), PC7 = RX (AF8)
    GPIOC->MODER  &= ~((3 << (6 * 2)) | (3 << (7 * 2)));
    GPIOC->MODER  |=  ((2 << (6 * 2)) | (2 << (7 * 2)));
    GPIOC->AFR[0] &= ~((0xF << (6 * 4)) | (0xF << (7 * 4)));
    GPIOC->AFR[0] |=  ((8   << (6 * 4)) | (8   << (7 * 4)));

    // 9600 baud @ 16MHz
    USART6->BRR = 0x683;
    USART6->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}

void USART6_SendChar(char c) {
    while (!(USART6->SR & USART_SR_TXE));
    USART6->DR = c;
}

int _write(int file, char *ptr, int len) {
    for (int i = 0; i < len; i++) {
        USART6_SendChar(ptr[i]);
    }
    return len;
}

// ---- GPIO init ----

void GPIO_Init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

    // Buttons on GPIOB: input with pull-up
    uint8_t button_pins[] = {START_BUTTON_PIN, BUTTON1_PIN, BUTTON2_PIN, BUTTON3_PIN, BUTTON4_PIN};
    for (int i = 0; i < 5; i++) {
        GPIOB->MODER &= ~(3 << (button_pins[i] * 2));
        GPIOB->PUPDR &= ~(3 << (button_pins[i] * 2));
        GPIOB->PUPDR |=  (1 << (button_pins[i] * 2));
    }

    // Token sensors on GPIOC: input with pull-up
    uint8_t token_pins[] = {TOKEN1_PIN, TOKEN2_PIN, TOKEN3_PIN, TOKEN4_PIN};
    for (int i = 0; i < 4; i++) {
        GPIOC->MODER &= ~(3 << (token_pins[i] * 2));
        GPIOC->PUPDR &= ~(3 << (token_pins[i] * 2));
        GPIOC->PUPDR |=  (1 << (token_pins[i] * 2));
    }

    // LEDs on GPIOC: output
    GPIOC->MODER &= ~((3 << (LED_1_PIN * 2)) | (3 << (LED_2_PIN * 2)));
    GPIOC->MODER |=  ((1 << (LED_1_PIN * 2)) | (1 << (LED_2_PIN * 2)));

    // Joystick analog pins on GPIOA
    GPIOA->MODER |= (3 << (JOY1_X_PIN * 2)) | (3 << (JOY1_Y_PIN * 2));
    GPIOA->MODER |= (3 << (JOY2_X_PIN * 2)) | (3 << (JOY2_Y_PIN * 2));
    GPIOA->MODER |= (3 << (JOY3_X_PIN * 2)) | (3 << (JOY3_Y_PIN * 2));

    // Joystick 4 analog pins on GPIOB
    GPIOB->MODER |= (3 << (JOY4_X_PIN * 2)) | (3 << (JOY4_Y_PIN * 2));
}

// ---- EXTI interrupt init ----
// On STM32F4, EXTI lines map to pins by number regardless of port.
// Only one port can use a given pin number at a time.
// EXTI line is selected per pin via SYSCFG_EXTICRx.
// Port mapping: 0=PA, 1=PB, 2=PC

void EXTI_Init(void) {
    // Enable SYSCFG clock for EXTI mux
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    // ---- Buttons on GPIOB ----
    // PB10 -> EXTI10, PB12 -> EXTI12, PB13 -> EXTI13, PB14 -> EXTI14, PB15 -> EXTI15
    // SYSCFG_EXTICR3 controls EXTI8-11, SYSCFG_EXTICR4 controls EXTI12-15
    SYSCFG->EXTICR[2] |= (1 << (2 * 4));   // EXTI10 = PB (bits 11:8)
    SYSCFG->EXTICR[3] |= (1 << (0 * 4));   // EXTI12 = PB (bits 3:0)
    SYSCFG->EXTICR[3] |= (1 << (1 * 4));   // EXTI13 = PB (bits 7:4)
    SYSCFG->EXTICR[3] |= (1 << (2 * 4));   // EXTI14 = PB (bits 11:8)
    SYSCFG->EXTICR[3] |= (1 << (3 * 4));   // EXTI15 = PB (bits 15:12)

    // ---- Token sensors on GPIOC ----
    // PC4 -> EXTI4, PC5 -> EXTI5, PC8 -> EXTI8, PC9 -> EXTI9
    SYSCFG->EXTICR[1] |= (2 << (0 * 4));   // EXTI4  = PC (bits 3:0)
    SYSCFG->EXTICR[1] |= (2 << (1 * 4));   // EXTI5  = PC (bits 7:4)
    SYSCFG->EXTICR[2] |= (2 << (0 * 4));   // EXTI8  = PC (bits 3:0)
    SYSCFG->EXTICR[2] |= (2 << (1 * 4));   // EXTI9  = PC (bits 7:4)

    // Falling edge trigger for all (active low with pull-up)
    EXTI->FTSR |= (1 << START_BUTTON_PIN) | (1 << BUTTON1_PIN) | (1 << BUTTON2_PIN) |
                  (1 << BUTTON3_PIN)      | (1 << BUTTON4_PIN);
    EXTI->FTSR |= (1 << TOKEN1_PIN) | (1 << TOKEN2_PIN) |
                  (1 << TOKEN3_PIN) | (1 << TOKEN4_PIN);

    // Unmask all EXTI lines
    EXTI->IMR  |= (1 << START_BUTTON_PIN) | (1 << BUTTON1_PIN) | (1 << BUTTON2_PIN) |
                  (1 << BUTTON3_PIN)      | (1 << BUTTON4_PIN);
    EXTI->IMR  |= (1 << TOKEN1_PIN) | (1 << TOKEN2_PIN) |
                  (1 << TOKEN3_PIN) | (1 << TOKEN4_PIN);

    // Enable NVIC interrupts
    // EXTI4          -> IRQ6
    // EXTI5-9        -> IRQ23
    // EXTI10-15      -> IRQ40
    NVIC_EnableIRQ(EXTI4_IRQn);
    NVIC_EnableIRQ(EXTI9_5_IRQn);
    NVIC_EnableIRQ(EXTI15_10_IRQn);
}

// ---- ADC init ----

void ADC_Init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    ADC1->CR1 = 0;
    ADC1->CR2 = 0;
    ADC1->SMPR2 |= (7 << (JOY1_X_ADC * 3)) | (7 << (JOY1_Y_ADC * 3));
    ADC1->SMPR2 |= (7 << (JOY2_X_ADC * 3)) | (7 << (JOY2_Y_ADC * 3));
    ADC1->SMPR2 |= (7 << (JOY3_X_ADC * 3)) | (7 << (JOY3_Y_ADC * 3));
    ADC1->SMPR2 |= (7 << (JOY4_X_ADC * 3)) | (7 << (JOY4_Y_ADC * 3));

    ADC1->CR2 |= ADC_CR2_ADON;
}

uint16_t ADC_Read(uint8_t channel) {
    ADC1->SQR3 = channel;
    ADC1->CR2 |= ADC_CR2_SWSTART;
    while (!(ADC1->SR & ADC_SR_EOC));
    return ADC1->DR;
}

void Joy_Read(uint8_t x_channel, uint8_t y_channel, uint16_t *x, uint16_t *y) {
    *x = ADC_Read(x_channel);
    *y = ADC_Read(y_channel);
}

// ---- LED helpers ----

void LED_Set(uint8_t pin, uint8_t state) {
    if (state)
        GPIOC->ODR |= (1 << pin);
    else
        GPIOC->ODR &= ~(1 << pin);
}

// ---- EXTI interrupt handlers ----

// Handles EXTI4: TOKEN2 (PC4)
void EXTI4_IRQHandler(void) {
    if (EXTI->PR & (1 << TOKEN2_PIN)) {
        EXTI->PR |= (1 << TOKEN2_PIN); // clear flag
        printf("TOKEN2 triggered (PC4)\r\n");
    }
}

// Handles EXTI5-9: TOKEN1 (PC5), EXTI8: TOKEN3 (PC8), EXTI9: TOKEN4 (PC9)
void EXTI9_5_IRQHandler(void) {
    if (EXTI->PR & (1 << TOKEN1_PIN)) {
        EXTI->PR |= (1 << TOKEN1_PIN);
        printf("TOKEN1 triggered (PC5)\r\n");
    }
    if (EXTI->PR & (1 << TOKEN3_PIN)) {
        EXTI->PR |= (1 << TOKEN3_PIN);
        printf("TOKEN3 triggered (PC8)\r\n");
    }
    if (EXTI->PR & (1 << TOKEN4_PIN)) {
        EXTI->PR |= (1 << TOKEN4_PIN);
        printf("TOKEN4 triggered (PC9)\r\n");
    }
}

// Handles EXTI10-15: BUTTON1 (PB10), BUTTON2 (PB12), BUTTON3 (PB13),
//                    BUTTON4 (PB14), START_BUTTON (PB15)
void EXTI15_10_IRQHandler(void) {
    if (EXTI->PR & (1 << BUTTON1_PIN)) {
        EXTI->PR |= (1 << BUTTON1_PIN);
        printf("BUTTON1 pressed (PB10)\r\n");
    }
    if (EXTI->PR & (1 << BUTTON2_PIN)) {
        EXTI->PR |= (1 << BUTTON2_PIN);
        printf("BUTTON2 pressed (PB12)\r\n");
    }
    if (EXTI->PR & (1 << BUTTON3_PIN)) {
        EXTI->PR |= (1 << BUTTON3_PIN);
        printf("BUTTON3 pressed (PB13)\r\n");
    }
    if (EXTI->PR & (1 << BUTTON4_PIN)) {
        EXTI->PR |= (1 << BUTTON4_PIN);
        printf("BUTTON4 pressed (PB14)\r\n");
    }
    if (EXTI->PR & (1 << START_BUTTON_PIN)) {
        EXTI->PR |= (1 << START_BUTTON_PIN);
        printf("START BUTTON pressed (PB15)\r\n");
    }
}

// ---- Main ----

int main(void) {
    GPIO_Init();
    USART6_Init();
    ADC_Init();
    EXTI_Init();

    printf("System ready.\r\n");

    while (1) {
        uint16_t joy1_x, joy1_y;
        uint16_t joy2_x, joy2_y;
        uint16_t joy3_x, joy3_y;
        uint16_t joy4_x, joy4_y;

        Joy_Read(JOY1_X_ADC, JOY1_Y_ADC, &joy1_x, &joy1_y);
        Joy_Read(JOY2_X_ADC, JOY2_Y_ADC, &joy2_x, &joy2_y);
        Joy_Read(JOY3_X_ADC, JOY3_Y_ADC, &joy3_x, &joy3_y);
        Joy_Read(JOY4_X_ADC, JOY4_Y_ADC, &joy4_x, &joy4_y);

        if (joy1_x < 1000 || joy1_x > 3000) printf("JOY1 X: %4d\r\n", joy1_x);
        if (joy1_y < 1000 || joy1_y > 3000) printf("JOY1 Y: %4d\r\n", joy1_y);

        if (joy2_x < 1000 || joy2_x > 3000) printf("JOY2 X: %4d\r\n", joy2_x);
        if (joy2_y < 1000 || joy2_y > 3000) printf("JOY2 Y: %4d\r\n", joy2_y);

        if (joy3_x < 1000 || joy3_x > 3000) printf("JOY3 X: %4d\r\n", joy3_x);
        if (joy3_y < 1000 || joy3_y > 3000) printf("JOY3 Y: %4d\r\n", joy3_y);

        if (joy4_x < 1000 || joy4_x > 3000) printf("JOY4 X: %4d\r\n", joy4_x);
        if (joy4_y < 1000 || joy4_y > 3000) printf("JOY4 Y: %4d\r\n", joy4_y);

        delay(100000);
    }
}
