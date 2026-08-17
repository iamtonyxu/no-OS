/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "radio_hw_spi.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ARM_CMD_IRQ_BIT   0x80000000U  /* write '1' to arm_cmd_0 bit31 -> pulse spi_mailbox_irq */
#define CONSOLE_LINE_MAX  32U
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void radio_hw_console_poll(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* Retarget printf to USART1 (PA9/PA10, 115200).
   - newlib (GCC / STM32CubeIDE): syscalls.c _write() calls __io_putchar().
   - IAR DLib: printf() calls __write(). */
int __io_putchar(int ch)
{
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1U, HAL_MAX_DELAY);
  return ch;
}

int __io_getchar(void)
{
  uint8_t ch = 0U;
  HAL_UART_Receive(&huart1, &ch, 1U, HAL_MAX_DELAY);
  return (int)ch;
}

/* IAR DLib low-level output: printf/fprintf route here. */
size_t __write(int handle, const unsigned char *buf, size_t bufSize)
{
  if (buf == 0)                       /* flush request */
    return 0U;

  if ((handle != 1) && (handle != 2)) /* stdout / stderr */
    return 0U;

  if (HAL_UART_Transmit(&huart1, (uint8_t *)buf, (uint16_t)bufSize,
                        HAL_MAX_DELAY) != HAL_OK)
    return 0U;

  return bufSize;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  /* Configure the software chip-select pin (PA4, idle high). */
  radio_hw_spi_init();

  /* Run the radio_hw register read/write self-test. */
  if (radio_hw_spi_self_test() == 0)
  {
    /* LED0 on = pass. */
    HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);
  }
  else
  {
    /* LED1 on = fail. */
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
#if 0
    radio_hw_console_poll();
#else
  radio_hw_spi_check_rw(RADIO_HW_REG_ARM_CMD_0, 0xFFFFFFFFU, RADIO_HW_ARM_CMD_RESET, "arm_cmd_0");
  radio_hw_spi_check_rw(RADIO_HW_REG_ARM_CMD_1, 0xF1111111U, RADIO_HW_ARM_CMD_RESET, "arm_cmd_1");
  radio_hw_spi_check_rw(RADIO_HW_REG_ARM_CMD_2, 0xF2222222U, RADIO_HW_ARM_CMD_RESET, "arm_cmd_2");
  radio_hw_spi_check_rw(RADIO_HW_REG_ARM_CMD_3, 0xF3333333U, RADIO_HW_ARM_CMD_RESET, "arm_cmd_3");
  radio_hw_spi_check_rw(RADIO_HW_REG_ARM_CMD_4, 0xF4444444U, RADIO_HW_ARM_CMD_RESET, "arm_cmd_4");
  radio_hw_spi_check_rw(RADIO_HW_REG_ARM_CMD_5, 0xF5555555U, RADIO_HW_ARM_CMD_RESET, "arm_cmd_5");
  radio_hw_spi_check_rw(RADIO_HW_REG_ARM_CMD_6, 0xF6666666U, RADIO_HW_ARM_CMD_RESET, "arm_cmd_6");
  radio_hw_spi_check_rw(RADIO_HW_REG_ARM_CMD_7, 0xF7777777U, RADIO_HW_ARM_CMD_RESET, "arm_cmd_7");
  HAL_Delay(3000u);
  HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin); // Indicates that the SPI master is running and polling the ARM command registers.
  
#endif
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
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
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 64;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* Read one CR/LF-terminated line from USART1 with echo. */
static uint16_t console_read_line(uint8_t *buf, uint16_t max)
{
  uint16_t i = 0U;
  uint8_t ch;

  while (i < (max - 1U))
  {
    if (HAL_UART_Receive(&huart1, &ch, 1U, HAL_MAX_DELAY) != HAL_OK)
      break;

    if ((ch == '\r') || (ch == '\n'))
    {
      buf[i] = '\0';
      HAL_UART_Transmit(&huart1, (uint8_t *)"\r\n", 2U, HAL_MAX_DELAY);
      return i;
    }

    if ((ch == 0x08U) || (ch == 0x7FU))  /* backspace */
    {
      if (i > 0U) i--;
      continue;
    }

    buf[i++] = ch;
    HAL_UART_Transmit(&huart1, &ch, 1U, HAL_MAX_DELAY);  /* echo */
  }

  buf[i] = '\0';
  return i;
}

/* Match a command keyword followed by end-of-line or a space. */
static uint8_t console_keyword(const uint8_t *line, const char *kw)
{
  size_t n = strlen(kw);
  if (strncmp((const char *)line, kw, n) != 0)
    return 0U;
  return (line[n] == '\0') || (line[n] == ' ');
}

/* Poll USART1 for a command and act on it. Called from the main loop. */
void radio_hw_console_poll(void)
{
  uint8_t line[CONSOLE_LINE_MAX];
  uint32_t val;
  static uint32_t cmd_seq = 0U;   /* auto payload for a bare "cmd" */

  if (console_read_line(line, sizeof(line)) == 0U)
    return;

  if (console_keyword(line, "cmd"))
  {
    uint32_t payload = cmd_seq++;
    if (line[3] == ' ')
      payload = (uint32_t)strtoul((const char *)&line[4], NULL, 16);

    if (radio_hw_spi_write_reg(RADIO_HW_REG_ARM_CMD_0,
                               ARM_CMD_IRQ_BIT | payload) != HAL_OK)
      printf("[FAIL] arm_cmd_0 write error\n");
    else
      printf("arm_cmd_0 <- 0x%08X  (bit31 set -> ZC706 PS IRQ)\n",
             (unsigned int)(ARM_CMD_IRQ_BIT | payload));
  }
  else if (console_keyword(line, "read"))
  {
    uint16_t addr = 0U;
    if (line[4] == ' ')
      addr = (uint16_t)strtoul((const char *)&line[5], NULL, 16);

    if (radio_hw_spi_read_reg(addr, &val) == HAL_OK)
      printf("reg[0x%04X] = 0x%08X\n", (unsigned int)addr, (unsigned int)val);
    else
      printf("[FAIL] read reg error\n");
  }
  else if (console_keyword(line, "test"))
  {
    radio_hw_spi_self_test();
  }
  else if (console_keyword(line, "help") || line[0] == '?')
  {
    printf("commands:\n"
           "  cmd [hex]  write arm_cmd_0 (bit31 -> ZC706 PS IRQ)\n"
           "  read <hex> read a radio_hw register\n"
           "  test       re-run self-test\n"
           "  help       this list\n");
  }
  else
  {
    printf("unknown '%s' (type help)\n", (const char *)line);
  }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
