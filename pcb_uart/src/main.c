#include "stm32f4xx.h"

void uart1_init(void);
void uart1_write(char c);
void uart1_write_string(const char *s);

void delay(volatile uint32_t d)
{
    while (d--) __asm__("nop");
}

void uart1_init(void)
{
    // 1. Enable clocks
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;   // GPIOB (PB6 = TX)
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    // 2. Configure PB6 as AF (TX only)
    GPIOB->MODER &= ~(3 << (6 * 2));
    GPIOB->MODER |=  (2 << (6 * 2));        // Alternate function mode
    GPIOB->AFR[0] |= (7 << (6 * 4));        // AF7 = USART1 on PB6

    // 3. UART config — 9600 baud @ 16MHz HSI
    // BRR = 16000000 / 9600 = 1666.67 → Mantissa = 104 (0x68), Fraction = 3 (out of 16)
    // So BRR = (104 << 4) | 3 = 0x683
    USART1->BRR = 0x683;
    USART1->CR1 = USART_CR1_TE | USART_CR1_UE;  // TX only, no RE
}

void uart1_write(char c)
{
    while (!(USART1->SR & USART_SR_TXE));
    USART1->DR = c;
}

void uart1_write_string(const char *s)
{
    while (*s)
        uart1_write(*s++);
}

int main(void)
{
    uart1_init();

    while (1)
    {
        uart1_write_string("hi\r\n");
        delay(1600000);   // ~100ms at 16MHz HSI
    }
}