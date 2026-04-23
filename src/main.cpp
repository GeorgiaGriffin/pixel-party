/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.cpp
  * @brief          : Combined Main program body (HAL + C++ Game Engine)
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
// Wrap C headers in extern "C" so the C++ compiler doesn't mangle their names
extern "C" {
    #include "main.h"
    #include "usb_device.h"
    #include "usbd_customhid.h"
    #include <stdio.h>
}

// C++ Headers
#include "hardware.h"
#include "game.h"



/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;
extern USBD_HandleTypeDef hUsbDeviceFS;

/* Private function prototypes -----------------------------------------------*/
extern "C" {
    void SystemClock_Config(void);

    #ifdef NUCLEO
    static void MX_GPIO_Init(void);
    static void MX_USART2_UART_Init(void);
    void Error_Handler(void);
    #endif

    extern void __libc_init_array(void); // C++ global constructors
}

GameMachine game;

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    // 1. Run C++ constructors before anything else
    __libc_init_array();  

    // 2. ST HAL Initialization
    HAL_Init();
    // Add a tiny delay here to let the MAX7375 stabilize power
    for(volatile int i=0; i<50000; i++); 
    SystemClock_Config();

    #ifdef NUCLEO
    MX_USB_DEVICE_Init();
    MX_GPIO_Init();
    MX_USART2_UART_Init();
    #endif

    MX_USB_DEVICE_Init();
   

    #ifdef PCB
    USART6_Init();
    USART1_Init();
    #endif

    // 3. Custom / Lower-Level Peripheral Initialization
    GPIO_Init();
    dma_setup();
    ADC_Init();
    EXTI_Init();

    // 4. Setup DMA & ADC for Joystick
    // dma_setup();  // GA: where is the declaration???

    // 5. Initialize Game Engine
    HAL_Delay(2000); // give it 3 secs to open serial monitor 
    // g_machine = &game;
    // game.setState(&game.regState);

    // Variable for non-blocking USB delay
    uint32_t last_usb_send = HAL_GetTick();

    /* Infinite loop */
    while (1)
    {
        // // Advance the game state machine
        // handleTokenEvents(&game);
        // // Only advance the game if the button was actually pressed for more than a few ms
        // if (game.startPressed) {
        //     HAL_Delay(50); // Simple debounce
        //     // Check if the pin is still low (active low)
        //     if ((GPIOB->IDR & (1 << START_BUTTON_PIN)) == 0) {
        //         game.advance();
        //     } else {
        //         game.startPressed = false; // It was just noise
        //     }
        // } else {
        //     game.advance();
        // }
        // Add a tiny delay to prevent the loop from "tight-polling" the UART 
        // which can sometimes starve the hardware of processing time
        HAL_Delay(100);

       // printf("X: %d  Y: %d\r\n",  (uint8_t)(joystick_data[0]),   (uint8_t)(joystick_data[1]));

        // Non-blocking timer: Only send USB data every 10ms
        // if ((HAL_GetTick() - last_usb_send) >= 10) 
        // {
            uint8_t report_buffer[2];
            report_buffer[0] = (uint8_t)(joystick_data[0] >> 4);
            report_buffer[1] = (uint8_t)(joystick_data[1] >> 4);

            USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, report_buffer, 2);
            
        //     // Optional: Printf for debugging (might slow down the loop slightly)
        //     printf("X: %d  Y: %d\r\n", report_buffer[0], report_buffer[1]);

        //     last_usb_send = HAL_GetTick(); // Reset the timer
        // }
    }
}

// ==============================================================================
// USER DEFINED FUNCTIONS (C-Linkage required for HAL callbacks/syscalls)
// ==============================================================================
#ifdef NUCLEO
int _write(int file, char *ptr, int len)
{
	int DataIdx;

	for (DataIdx = 0; DataIdx < len; DataIdx++)
	{
		__io_putchar(*ptr++);
	}
	return len;
}

extern "C" {

    int __io_putchar(int ch) {
        HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
        return ch;
    }


    static void MX_USART2_UART_Init(void)
    {
        huart2.Instance = USART2;
        huart2.Init.BaudRate = 115200;
        huart2.Init.WordLength = UART_WORDLENGTH_8B;
        huart2.Init.StopBits = UART_STOPBITS_1;
        huart2.Init.Parity = UART_PARITY_NONE;
        huart2.Init.Mode = UART_MODE_TX_RX;
        huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
        huart2.Init.OverSampling = UART_OVERSAMPLING_16;
        if (HAL_UART_Init(&huart2) != HAL_OK)
        {
            Error_Handler();
        }
    }

    static void MX_GPIO_Init(void)
    {
        GPIO_InitTypeDef GPIO_InitStruct = {0};

        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_GPIOH_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();

        GPIO_InitStruct.Pin = B1_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_5;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }


} // End extern "C"

#endif

    void SystemClock_Config(void)
    {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
     */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 4;
    RCC_OscInitStruct.PLL.PLLN = 72;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 3;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
    }

void Error_Handler(void) {
    __disable_irq();
    while (1) { }
}

#ifdef PCB
extern "C" {
    int _write(int file, char *ptr, int len) {
        for (int i = 0; i < len; i++) {
            USART6_SendChar(ptr[i]); // use USART6 for serial monitor, 1 for Pi
        }
        return len;
    }
}
#endif