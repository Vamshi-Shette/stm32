
//SD card interfacing using SPI interface
//one onterrupt = one byte transfer

#include "main.h"
#include "fatfs.h"
#include<stdio.h>
#include<string.h>

volatile uint8_t spi_tx_data;
volatile uint8_t spi_rx_data;
volatile uint8_t spi_transfer_done = 0;
volatile uint32_t spi_irq_count = 0;

volatile uint8_t spi_sector_done = 0;
volatile uint32_t sector_irq_count = 0;
uint8_t dummy_tx_buffer[512];

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;

FATFS fs;
FRESULT fres;

FIL file;
UINT bytes_written;
UINT bytes_read;
char read_buffer[100];

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);

HAL_StatusTypeDef SPI_ReadSector_IT(uint8_t *rx_buffer)
{
    memset(dummy_tx_buffer, 0xFF, 512);

    spi_sector_done = 0;

    HAL_StatusTypeDef status =
        HAL_SPI_TransmitReceive_IT(&hspi1,
                                   dummy_tx_buffer,
                                   rx_buffer,
                                   512);
    sector_irq_count++;

    while(spi_sector_done == 0);

    return status;
}

HAL_StatusTypeDef SPI_WriteSector_IT(uint8_t *tx_buffer)
{
    static uint8_t dummy_rx_buffer[512];

    spi_sector_done = 0;

    HAL_StatusTypeDef status =
        HAL_SPI_TransmitReceive_IT(&hspi1,
                                   tx_buffer,
                                   dummy_rx_buffer,
                                   512);
    sector_irq_count++;
    while(spi_sector_done == 0);

    return status;
}

uint8_t SPI_SendByte_IT(uint8_t data)
{
    spi_tx_data = data;
    spi_transfer_done = 0;

    HAL_SPI_TransmitReceive_IT(&hspi1,
                               (uint8_t *)&spi_tx_data,
                               (uint8_t *)&spi_rx_data,
                               1);

    while(spi_transfer_done == 0);

    return spi_rx_data;
}

void SD_SendDummyClocks(void)
{
    SD_CS_HIGH();

    for(int i = 0; i < 10; i++)
    {
        SPI_SendByte_IT(0xFF);
    }
}

int __io_putchar(int ch)
{
    HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if(hspi->Instance == SPI1)
    {
        spi_transfer_done = 1;
        spi_sector_done = 1;
        spi_irq_count++;
    }
}

int main(void)
{

  HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  MX_FATFS_Init();

  HAL_Delay(100);
  SD_SendDummyClocks();

  fres = f_mount(&fs, "", 1);

  if(fres == FR_OK)
  {
      printf("Mount success\r\n");

      printf("Mount IRQ Count = %lu\r\n", spi_irq_count);
      spi_irq_count = 0;

      /* ---------- WRITE ---------- */
      fres = f_open(&file, "test.txt", FA_CREATE_ALWAYS | FA_WRITE);

      if(fres == FR_OK)
      {
          printf("File opened for write\r\n");

          fres = f_write(&file,
                         "Hello from STM32 SD Card!\r\n",
                         27,
                         &bytes_written);

          if(fres == FR_OK)
          {
              printf("Write success\r\n");
              printf("Bytes written = %d\r\n", bytes_written);

              printf("Write IRQ Count = %lu\r\n", spi_irq_count);
              spi_irq_count = 0;
          }

          f_sync(&file);
          f_close(&file);
      }

      /* ---------- READ ---------- */
      memset(&file, 0, sizeof(FIL));
      fres = f_open(&file, "test.txt", FA_READ);

      if(fres == FR_OK)
      {
          printf("File opened for read\r\n");

          memset(read_buffer, 0, sizeof(read_buffer));

          fres = f_read(&file,
                        read_buffer,
                        sizeof(read_buffer) - 1,
                        &bytes_read);

          if(fres == FR_OK)
          {
              printf("Read success\r\n");
              printf("Bytes read = %d\r\n", bytes_read);
              printf("Data = %s\r\n", read_buffer);
              //printf("SPI Interrupt Count = %lu\r\n", spi_irq_count);

              printf("Read IRQ Count = %lu\r\n", spi_irq_count);
              printf("Sector IRQ Count = %lu\r\n", sector_irq_count);
          }
          else
          {
              printf("Read failed = %d\r\n", fres);
          }

          f_close(&file);
      }
      else
      {
          printf("File read open failed = %d\r\n", fres);
      }
  }
  else
  {
      printf("Mount failed = %d\r\n", fres);
  }

  while (1)
  {
  }
}
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }


  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}


static void MX_SPI1_Init(void)
{

  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
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
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
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

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}
void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif /* USE_FULL_ASSERT */
