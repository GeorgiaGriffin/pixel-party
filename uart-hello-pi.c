#include "stm32f4xx.h"

void uart1_init(void);
void uart1_write(char c);
void uart1_write_string(const char *s);

#define UARTTEST

void delay(volatile uint32_t d)
{
    while (d--) __asm__("nop");
}

void uart1_init(void)
{
    // 1. Enable clocks
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    // 2. Configure PA9 (TX) and PA10 (RX) alternate function
    GPIOA->MODER &= ~((3 << (9 * 2)) | (3 << (10 * 2)));
    GPIOA->MODER |=  ((2 << (9 * 2)) | (2 << (10 * 2)));

    GPIOA->AFR[1] |= (7 << ((9 - 8) * 4));   // AF7 USART1 TX
    GPIOA->AFR[1] |= (7 << ((10 - 8) * 4));  // AF7 USART1 RX

    // 3. UART config
    USART1->BRR = 0x8B; // ~115200 @ 16MHz (HSI default-ish safe approximation)

    USART1->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}

void uart1_write(char c)
{
    while (!(USART1->SR & USART_SR_TXE));
    USART1->DR = c;
}

void uart1_write_string(const char *s)
{
    while (*s)
    {
        uart1_write(*s++);
    }
}

int main(void)
{
    #ifdef UARTTEST
    uart1_init();

    while (1)
    {
        uart1_write_string("Hello Raspberry Pi!\r\n");
        delay(8000000);
    }
    #endif
}
