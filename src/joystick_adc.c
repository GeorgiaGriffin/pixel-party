#include "joystick_adc.h"
#include "stm32f4xx.h" // Required for register definitions

// This array acts as the "bridge" between DMA and your Main loop
// Index 0 = Joystick X (PA0), Index 1 = Joystick Y (PA1)
volatile uint16_t joystick_data[2] = {0, 0};

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

    // Total items to transfer: 2 (X and Y)
    DMA2_Stream0->NDTR = 2;

    // Configure Control Register (CR)
    // Ch 0, 16-bit data, Circular Mode, Memory Increment, Transfer Complete Interrupt
    DMA2_Stream0->CR = (0 << 25) |        // Channel 0
                       (0b01 << 16) |     // Priority Medium
                       (0b01 << 13) |     // Memory Size 16-bit
                       (0b01 << 11) |     // Peripheral Size 16-bit
                       DMA_SxCR_MINC |    // Memory Increment
                       DMA_SxCR_CIRC |    // Circular Mode
                       DMA_SxCR_TCIE;     // Enable Interrupt (optional)

    // 3. Enable the DMA Stream
    DMA2_Stream0->CR |= DMA_SxCR_EN;
}

void joystick_pin_adc_setup(void) {
    // 1. Clocks
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;  
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; 

    // 2. GPIO: Set PA0 and PA1 to Analog Mode (0x3)
    GPIOA->MODER |= (0x3 << (0*2)) | (0x3 << (1*2));

    // 3. ADC Common Init
    ADC1->CR1 = ADC_CR1_SCAN; // Scan mode for multiple channels

    // 4. Sequence Length: 2 Conversions (L=1 writes '0001' to bits)
    ADC1->SQR1 &= ~ADC_SQR1_L; 
    ADC1->SQR1 |= (1 << ADC_SQR1_L_Pos); 

    // 5. Channel Order: Rank 1 = Ch0 (PA0), Rank 2 = Ch1 (PA1)
    ADC1->SQR3 = (0 << 0) | (1 << 5);

    // 6. DMA & Continuous Settings
    // DDS: Keep DMA request active. CONT: Don't stop after one loop.
    ADC1->CR2 |= ADC_CR2_DMA | ADC_CR2_DDS | ADC_CR2_CONT | ADC_CR2_ADON;

    // Wait a tiny bit for ADC to stabilize
    for(volatile int i=0; i<1000; i++); 

    // 7. Start Conversion
    ADC1->CR2 |= ADC_CR2_SWSTART;
}