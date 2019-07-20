/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "api.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

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

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void Delay(__IO uint32_t nCount)
{
    while (nCount--);
}

// print array of bytes in hex
void printBytes(unsigned char *data, int length) {
    char buffer[16];
    int i;
    for (i = 0; i < length; ++i) {
        HAL_UART_Transmit(&huart2, (uint8_t *)buffer, sprintf(buffer, "%02X", data[i]), 0xFFFFFF);
    }
}

volatile unsigned int *DWT_CYCCNT  ;
volatile unsigned int *DWT_CONTROL ;
volatile unsigned int *SCB_DEMCR   ;

void reset_timer() {
    DWT_CYCCNT    = (int *)0xE0001004; //address of the register
    DWT_CONTROL   = (int *)0xE0001000; //address of the register
    SCB_DEMCR     = (int *)0xE000EDFC; //address of the register
    *SCB_DEMCR   |= 0x01000000;
    *DWT_CYCCNT   = 0; // reset the counter
    *DWT_CONTROL  = 0; 
}

void start_timer() {
    *DWT_CONTROL |= 1; // enable the counter
}

void stop_timer() {
    *DWT_CONTROL &= ~1; // disable the counter    
}

unsigned int getCycles() {
    return *DWT_CYCCNT;
}

void update(int *count, double *mean, double *m2, unsigned int newValue) {
    double delta, delta2;
    
    *count += 1;
    delta = newValue - *mean;
    *mean += delta / *count;
    delta2 = newValue - *mean;
    *m2 += delta * delta2;
}

void finalize(int *count, double *mean, double *m2) {
    *m2 = *m2/(*count - 1);
}

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
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  
  char *test = "\n\r# LEDA";
  HAL_UART_Transmit(&huart2, (uint8_t *)test, strlen(test), 0xFFFFFF);
  
  char *NEWLINE = "\n\r";
  char *COUNT = "\n\rcount = ";
  char *SEED = "\n\rseed = ";
  char *KEYPAIR_TIME = "\n\rcycles in crypto_kem_keypair = ";
  char *ENC_TIME = "\n\rcycles in crypto_kem_enc = ";
  char *DEC_TIME = "\n\rcycles in crypto_kem_dec = ";
  char *KEYPAIR = "\n\rKEY GENERATION";
  char *ENC = "\n\rENCRYPTION";
  char *DEC = "\n\rDECRYPTION";
  char *MEAN_IS = "\n\r\tmean is ";
  char *VARIANCE_IS = "\n\r\tvariance is ";
  
  unsigned char msg[CRYPTO_CIPHERTEXTBYTES];
  unsigned char pk[CRYPTO_PUBLICKEYBYTES];
  unsigned char sk[CRYPTO_SECRETKEYBYTES];
  unsigned char ss[CRYPTO_BYTES];
  
  char number[16];
  int count_keypair, count_enc, count_dec;
  double mean_keypair, mean_enc, mean_dec;
  double m2_keypair, m2_enc, m2_dec;
  
  count_keypair = count_enc = count_dec = 0;
  mean_keypair = mean_enc = mean_dec = 0;
  m2_keypair = m2_enc = m2_dec = 0;
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  
  // mi sa hce ci vogliono i long ma con segno perche' se no si minchia tutto
  while (count_keypair < 30)
  {
    /* USER CODE END WHILE */
    
    // seed
    unsigned char entropy_string[96];
    HAL_UART_Receive(&huart2, (uint8_t *)entropy_string, 96, 0xFFFFFF);
    
    unsigned char entropy[48];
    unsigned char *tmp = entropy_string;
    for (size_t i = 0; i < 48; i++) {
      char high = entropy_string[i*2];
      char low = entropy_string[i*2 + 1];
      entropy[i] = 0;
    
      if (high >= '0' && high <= '9') {
          entropy[i] = (high - '0') << 4;
      } else if (high >= 'A' && high <= 'F') {
          entropy[i] = (high - 'A' + 10) << 4;
      } else if (high >= 'a' && high <= 'f') {
          entropy[i] = (high - 'a' + 10) << 4;
      }
    
      if (low >= '0' && low <= '9') {
          entropy[i] |= low - '0';
      } else if (low >= 'A' && low <= 'F') {
          entropy[i] |= low - 'A' + 10;
      } else if (low >= 'a' && low <= 'f') {
          entropy[i] |= low - 'a' + 10;
      }
    }
    randombytes_init(entropy, NULL, 1);
    HAL_UART_Transmit(&huart2, (uint8_t *)SEED, strlen(SEED), 0xFFFFFF);
    printBytes(entropy, 48);
    
    // pk and sk
    reset_timer();
    start_timer();
    crypto_kem_keypair(pk, sk);
    stop_timer();
    long long keypair_time = getCycles();
    update(&count_keypair, &mean_keypair, &m2_keypair, keypair_time);
    
    HAL_UART_Transmit(&huart2, (uint8_t *)KEYPAIR_TIME, strlen(KEYPAIR_TIME), 0xFFFFFF);
    //itoa(keypair_time, number, 10);
    //number[strlen(number)] = 0;
    //HAL_UART_Transmit(&huart2, (uint8_t *)number, strlen(number), 0xFFFFFF);
    //HAL_UART_Transmit(&huart2, (uint8_t *)number, sprintf(number, "%d", keypair_time), 0xFFFFFF);
    printBytes(&keypair_time, 8);

    // ct and ss
    reset_timer();
    start_timer();
    crypto_kem_enc(msg, ss, pk);
    stop_timer();
    long long enc_time = getCycles();
    update(&count_enc, &mean_enc, &m2_enc, enc_time);
    
    HAL_UART_Transmit(&huart2, (uint8_t *)ENC_TIME, strlen(ENC_TIME), 0xFFFFFF);
    //itoa(enc_time, number, 10);
    //number[strlen(number)] = 0;
    //HAL_UART_Transmit(&huart2, (uint8_t *)number, strlen(number), 0xFFFFFF);
    //HAL_UART_Transmit(&huart2, (uint8_t *)number, sprintf(number, "%d", enc_time), 0xFFFFFF);
    printBytes(&enc_time, 8);
    
    // decryption testing
    reset_timer();
    start_timer();
    //
    crypto_kem_dec(ss, msg, sk);
    HAL_UART_Transmit(&huart2, (uint8_t *)test, strlen(test), 0xFFFFFF);
    //
    stop_timer();
    long long dec_time = getCycles();
    update(&count_dec, &mean_dec, &m2_dec, dec_time);
    
    HAL_UART_Transmit(&huart2, (uint8_t *)DEC_TIME, strlen(DEC_TIME), 0xFFFFFF);
    //itoa(dec_time, number, 10);
    //number[strlen(number)] = 0;
    //HAL_UART_Transmit(&huart2, (uint8_t *)number, strlen(number), 0xFFFFFF);
    //HAL_UART_Transmit(&huart2, (uint8_t *)number, sprintf(number, "%d", dec_time), 0xFFFFFF);
    printBytes(&dec_time, 8);
  
    // newline
    HAL_UART_Transmit(&huart2, (uint8_t *)NEWLINE, strlen(NEWLINE), 0xFFFFFF);
    
    HAL_GPIO_TogglePin(Ld2_GPIO_Port, Ld2_Pin); // Toggle LED
    HAL_Delay(1000); // Delay 1 Seconds
    /* USER CODE BEGIN 3 */
  }
  
  // results for crypto_kem_keypair
  HAL_UART_Transmit(&huart2, (uint8_t *)KEYPAIR, strlen(KEYPAIR), 0xFFFFFF);
  finalize(&count_keypair, &mean_keypair, &m2_keypair);
  
  long long i_mean_keypair = (long long)mean_keypair;
  long long i_mean_enc = (long long)mean_enc;
  long long i_mean_dec = (long long)mean_dec;
  long long i_m2_keypair = (long long)m2_keypair;
  long long i_m2_enc = (long long)m2_enc;
  long long i_m2_dec = (long long)m2_dec;
  
  HAL_UART_Transmit(&huart2, (uint8_t *)MEAN_IS, strlen(MEAN_IS), 0xFFFFFF);
  //itoa((unsigned int)mean_keypair, number, 10);
  //number[strlen(number)] = 0;
  //HAL_UART_Transmit(&huart2, (uint8_t *)number, strlen(number), 0xFFFFFF);
  //HAL_UART_Transmit(&huart2, (uint8_t *)number, sprintf(number, "%d", (int)mean_keypair), 0xFFFFFF);
  printBytes(&i_mean_keypair, 8);
  
  HAL_UART_Transmit(&huart2, (uint8_t *)VARIANCE_IS, strlen(VARIANCE_IS), 0xFFFFFF);
  //itoa((unsigned int)m2_keypair, number, 10);
  //number[strlen(number)] = 0;
  //HAL_UART_Transmit(&huart2, (uint8_t *)number, strlen(number), 0xFFFFFF);
  //HAL_UART_Transmit(&huart2, (uint8_t *)number, sprintf(number, "%d", (int)m2_keypair), 0xFFFFFF);
  printBytes(&i_m2_keypair, 8);
  
  // results for crypto_kem_enc  
  HAL_UART_Transmit(&huart2, (uint8_t *)ENC, strlen(ENC), 0xFFFFFF);
  finalize(&count_enc, &mean_enc, &m2_enc);
  
  HAL_UART_Transmit(&huart2, (uint8_t *)MEAN_IS, strlen(MEAN_IS), 0xFFFFFF);
  //itoa((unsigned int)mean_enc, number, 10);
  //number[strlen(number)] = 0;
  //HAL_UART_Transmit(&huart2, (uint8_t *)number, strlen(number), 0xFFFFFF);
  //HAL_UART_Transmit(&huart2, (uint8_t *)number, sprintf(number, "%d", (int)mean_enc), 0xFFFFFF);
  printBytes(&i_mean_enc, 8);
  
  HAL_UART_Transmit(&huart2, (uint8_t *)VARIANCE_IS, strlen(VARIANCE_IS), 0xFFFFFF);
  //itoa((unsigned int)m2_enc, number, 10);
  //number[strlen(number)] = 0;
  //HAL_UART_Transmit(&huart2, (uint8_t *)number, strlen(number), 0xFFFFFF);
  //HAL_UART_Transmit(&huart2, (uint8_t *)number, sprintf(number, "%d", (int)m2_enc), 0xFFFFFF);
  printBytes(&i_m2_enc, 8);
  
  // results for crypto_kem_dec
  HAL_UART_Transmit(&huart2, (uint8_t *)DEC, strlen(DEC), 0xFFFFFF);
  finalize(&count_dec, &mean_dec, &m2_dec);
  
  HAL_UART_Transmit(&huart2, (uint8_t *)MEAN_IS, strlen(MEAN_IS), 0xFFFFFF);
  //itoa((unsigned int)mean_dec, number, 10);
  //number[strlen(number)] = 0;
  //HAL_UART_Transmit(&huart2, (uint8_t *)number, strlen(number), 0xFFFFFF);
  //HAL_UART_Transmit(&huart2, (uint8_t *)number, sprintf(number, "%d", (int)mean_dec), 0xFFFFFF);
  printBytes(&i_mean_dec, 8);
  
  HAL_UART_Transmit(&huart2, (uint8_t *)VARIANCE_IS, strlen(VARIANCE_IS), 0xFFFFFF);
  //itoa((unsigned int)m2_dec, number, 10);
  //number[strlen(number)] = 0;
  //HAL_UART_Transmit(&huart2, (uint8_t *)number, strlen(number), 0xFFFFFF);
  //HAL_UART_Transmit(&huart2, (uint8_t *)number, sprintf(number, "%d", (int)m2_dec), 0xFFFFFF);
  printBytes(&i_m2_dec, 8);
  
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
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

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
