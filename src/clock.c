#include "stm32f4xx.h"

void internal_clock()
{
    // 1. Enable HSI (Internal High Speed oscillator) - 16 MHz
    RCC->CR |= RCC_CR_HSION;
    while(!(RCC->CR & RCC_CR_HSIRDY));

    // 2. Set Flash Latency (3 Wait States required for 96MHz at 3.3V)
    FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_3WS;

    // 3. Configure the Main PLL
    // Formula: SYSCLK = (Source / M) * N / P
    // For 96 MHz: (16MHz / 16) * 192 / 2 = 96 MHz
    // We also set Q to 4 for the USB Clock: (16MHz / 16) * 192 / 4 = 48 MHz
    RCC->PLLCFGR = (16 << RCC_PLLCFGR_PLLM_Pos) | // M = 16
                   (192 << RCC_PLLCFGR_PLLN_Pos) | // N = 192
                   (0 << RCC_PLLCFGR_PLLP_Pos) |   // P = 2 (bits 00)
                   (4 << RCC_PLLCFGR_PLLQ_Pos) |   // Q = 4
                   RCC_PLLCFGR_PLLSRC_HSI;         // Source = HSI

    // 4. Enable PLL and wait for ready
    RCC->CR |= RCC_CR_PLLON;
    while(!(RCC->CR & RCC_CR_PLLRDY));

    // 5. Configure Bus Prescalers
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;  // AHB = 96 MHz
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2; // APB1 = 48 MHz (Max 50MHz)
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1; // APB2 = 96 MHz (Max 100MHz)

    // 6. Select PLL as System Clock
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}