// hardware.cpp
#include "hardware.h"
#include "game.h"
#include <stdio.h>
#include <stdlib.h>

volatile uint16_t joystick_data[8] = {0};
volatile uint16_t button_state[5] = {0};
volatile uint32_t sys_ticks = 0;

uint32_t last_token_time[4]  = {0};
uint32_t last_button_time[5] = {0};

extern "C" {
    void SysTick_Handler(void) {
        sys_ticks++;
    }
}

uint32_t get_tick(void) {
    return sys_ticks;
}


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

extern "C" {
    int _write(int file, char *ptr, int len) {
        for (int i = 0; i < len; i++) {
            USART6_SendChar(ptr[i]);
        }
        return len;
    }
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

    EXTI->RTSR |= (1 << START_BUTTON_PIN) | (1 << BUTTON1_PIN) | (1 << BUTTON2_PIN) |
                  (1 << BUTTON3_PIN)      | (1 << BUTTON4_PIN);

    
    // Both edges for token sensors (placed and removed)
    EXTI->FTSR |= (1 << TOKEN1_PIN) | (1 << TOKEN2_PIN) |
                (1 << TOKEN3_PIN) | (1 << TOKEN4_PIN);
    EXTI->RTSR |= (1 << TOKEN1_PIN) | (1 << TOKEN2_PIN) |
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
    // 1. Enable ADC Clock
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    ADC1->CR1 = ADC_CR1_SCAN; // Scan mode for multiple channels
    ADC1->CR2 = 0;

    // 2. Set sample times to maximum 
    ADC1->SMPR2 |= (7 << (JOY1_X_ADC * 3)) | (7 << (JOY1_Y_ADC * 3));
    ADC1->SMPR2 |= (7 << (JOY2_X_ADC * 3)) | (7 << (JOY2_Y_ADC * 3));
    ADC1->SMPR2 |= (7 << (JOY3_X_ADC * 3)) | (7 << (JOY3_Y_ADC * 3));
    ADC1->SMPR2 |= (7 << (JOY4_X_ADC * 3)) | (7 << (JOY4_Y_ADC * 3));

    // 3. Sequence Length: 8 Conversions
    ADC1->SQR1 &= ~ADC_SQR1_L; 
    ADC1->SQR1 |= (7 << 20); 

    // 4. Channel Order
    // SQR3 handles conversions 1 through 6
    ADC1->SQR3 = (JOY1_X_ADC << 0)  | 
                 (JOY1_Y_ADC << 5) |
                 (JOY2_X_ADC << 10) | 
                 (JOY2_Y_ADC << 15) | 
                 (JOY3_X_ADC << 20) | 
                 (JOY3_Y_ADC << 25);
                 
    // SQR2 handles conversions 7 and 8
   ADC1->SQR2 = (JOY4_X_ADC << 0)  | 
                 (JOY4_Y_ADC << 5);

    // 5. DMA & Continuous Settings 
    ADC1->CR2 |= ADC_CR2_DMA | ADC_CR2_DDS | ADC_CR2_CONT | ADC_CR2_ADON;

    // Wait a tiny bit for ADC to stabilize
    for(volatile int i=0; i<1000; i++); 

    // 6. Start Conversion
    ADC1->CR2 |= ADC_CR2_SWSTART;
}

// ----DMA init--------


void dma_setup(void) {
    // 1. Enable DMA2 Clock (ADC1 is connected to DMA2)
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;

    // 2. Configure DMA2 Stream 0 (ADC1 is on Channel 0)
    DMA2_Stream0->CR &= ~DMA_SxCR_EN; // Turn off before config
    while(DMA2_Stream0->CR & DMA_SxCR_EN); 

    // Set Peripheral Address (Source): The ADC Data Register
    DMA2_Stream0->PAR = (uint32_t)&(ADC1->DR);

    // Set Memory Address (Destination): Our C array
    DMA2_Stream0->M0AR = (uint32_t)joystick_data;

    // Total items to transfer: 8 (4 Joysticks, X and Y)
    DMA2_Stream0->NDTR = 8;

    // Configure Control Register (CR)
    DMA2_Stream0->CR = (0 << 25) |        // Channel 0
                       (0b01 << 16) |     // Priority Medium
                       (0b01 << 13) |     // Memory Size 16-bit
                       (0b01 << 11) |     // Peripheral Size 16-bit
                       DMA_SxCR_MINC |    // Memory Increment
                       DMA_SxCR_CIRC;
    // 3. Enable the DMA Stream
    DMA2_Stream0->CR |= DMA_SxCR_EN;
}


// ---- LED helpers ----

void LED_Set(uint8_t pin, uint8_t state) {
    if (state)
        GPIOC->ODR |= (1 << pin);
    else
        GPIOC->ODR &= ~(1 << pin);
}

// ---- EXTI interrupt handlers ----
extern "C" { // needed to use cpp with platform io
    // Handles EXTI4: TOKEN2 (PC4)
    void EXTI4_IRQHandler(void) {
        if (EXTI->PR & (1 << TOKEN2_PIN)) {
            EXTI->PR |= (1 << TOKEN2_PIN);
            if (g_machine) {
                g_machine->tokenState[1] = (GPIOC->IDR & (1 << TOKEN2_PIN)) ? 0 : 1;
                g_machine->tokenEvent |= (1 << 1);
            }
        }
    }

    // Handles EXTI5-9: TOKEN1 (PC5), EXTI8: TOKEN3 (PC8), EXTI9: TOKEN4 (PC9)
    void EXTI9_5_IRQHandler(void) {
        if (EXTI->PR & (1 << TOKEN1_PIN)) {
            EXTI->PR |= (1 << TOKEN1_PIN);
            if (g_machine) {
                g_machine->tokenState[0] = (GPIOC->IDR & (1 << TOKEN1_PIN)) ? 0 : 1;
                g_machine->tokenEvent |= (1 << 0);
            }
        }
        if (EXTI->PR & (1 << TOKEN3_PIN)) {
            EXTI->PR |= (1 << TOKEN3_PIN);
            if (g_machine) {
                g_machine->tokenState[2] = (GPIOC->IDR & (1 << TOKEN3_PIN)) ? 0 : 1;
                g_machine->tokenEvent |= (1 << 2);
            }
        }
        if (EXTI->PR & (1 << TOKEN4_PIN)) {
            EXTI->PR |= (1 << TOKEN4_PIN);
            if (g_machine) {
                g_machine->tokenState[3] = (GPIOC->IDR & (1 << TOKEN4_PIN)) ? 0 : 1;
                g_machine->tokenEvent |= (1 << 3);
            }
        }
    }

    // Handles EXTI10-15: BUTTON1 (PB10), BUTTON2 (PB12), BUTTON3 (PB13),
    //                    BUTTON4 (PB14), START_BUTTON (PB15)
    void EXTI15_10_IRQHandler(void) {
        uint32_t current_time = get_tick();

        if (EXTI->PR & (1 << START_BUTTON_PIN)) {
            EXTI->PR |= (1 << START_BUTTON_PIN);
            if (g_machine) g_machine->startPressed = true;
            
            if (current_time - last_button_time[0] > 50) {
                button_state[0] = (GPIOB->IDR & (1 << START_BUTTON_PIN)) ? 0 : 1;
                last_button_time[0] = current_time;
            }
        }
        if (EXTI->PR & (1 << BUTTON1_PIN)) {
            EXTI->PR |= (1 << BUTTON1_PIN);
            if (current_time - last_button_time[1] > 50) {
                button_state[1] = (GPIOB->IDR & (1 << BUTTON1_PIN)) ? 0 : 1;
                last_button_time[1] = current_time;
            }
        }
        if (EXTI->PR & (1 << BUTTON2_PIN)) {
            EXTI->PR |= (1 << BUTTON2_PIN);
            if (current_time - last_button_time[2] > 50) {
                button_state[2] = (GPIOB->IDR & (1 << BUTTON2_PIN)) ? 0 : 1;
                last_button_time[2] = current_time;
            }
        }
        if (EXTI->PR & (1 << BUTTON3_PIN)) {
            EXTI->PR |= (1 << BUTTON3_PIN);
            if (current_time - last_button_time[3] > 50) {
                button_state[3] = (GPIOB->IDR & (1 << BUTTON3_PIN)) ? 0 : 1;
                last_button_time[3] = current_time;
            }
        }
        if (EXTI->PR & (1 << BUTTON4_PIN)) {
            EXTI->PR |= (1 << BUTTON4_PIN);
            if (current_time - last_button_time[4] > 50) {
                button_state[4] = (GPIOB->IDR & (1 << BUTTON4_PIN)) ? 0 : 1;
                last_button_time[4] = current_time;
            }
        }
    }
}


char USART6_ReadChar(void) {
    while (!(USART6->SR & USART_SR_RXNE));
    return (char)USART6->DR;
}

void USART6_ReadLine(char* buf, int maxlen) {
    int i = 0;
    while (i < maxlen - 1) {
        char c = USART6_ReadChar();
        if (c == '\r' || c == '\n') {
            if (i == 0) continue; // Skip leading newlines
            break;
        }
        buf[i++] = c;
    }
    buf[i] = '\0';
}
