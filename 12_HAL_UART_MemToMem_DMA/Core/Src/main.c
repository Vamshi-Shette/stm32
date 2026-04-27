#include "stm32l4xx_hal.h"
#include <string.h>

/* ================= HANDLES ================= */
DMA_HandleTypeDef hdma_memtomem;

/* ================= BUFFERS ================= */
uint8_t srcBuffer[] = "DMA_MEM_TO_MEM_L476";
uint8_t dstBuffer[sizeof(srcBuffer)];

/* ================= FLAGS ================= */
volatile uint8_t dmaDone = 0;

/* ================= PROTOTYPES ================= */
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);

/* ================= MAIN ================= */
int main(void)
{
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_DMA_Init();

  while (1)
  {
    dmaDone = 0;

    memset(dstBuffer, 0, sizeof(dstBuffer));

    /* Start DMA Mem-to-Mem */
    HAL_DMA_Start_IT(&hdma_memtomem,
                     (uint32_t)srcBuffer,
                     (uint32_t)dstBuffer,
                     sizeof(srcBuffer));

    /* Wait for completion */
    while (!dmaDone);

    /* Verify */
    if (memcmp(srcBuffer, dstBuffer, sizeof(srcBuffer)) == 0)
    {
      HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5); // success
    }
    else
    {
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
    }

    HAL_Delay(1000);
  }
}

/* ================= CALLBACK ================= */
void HAL_DMA_XferCpltCallback(DMA_HandleTypeDef *hdma)
{
  if (hdma->Instance == DMA1_Channel1)
  {
    dmaDone = 1;
  }
}

/* ================= DMA INIT ================= */
static void MX_DMA_Init(void)
{
  __HAL_RCC_DMA1_CLK_ENABLE();

  hdma_memtomem.Instance = DMA1_Channel1;
  hdma_memtomem.Init.Request = DMA_REQUEST_MEM2MEM;
  hdma_memtomem.Init.Direction = DMA_MEMORY_TO_MEMORY;
  hdma_memtomem.Init.PeriphInc = DMA_PINC_ENABLE;
  hdma_memtomem.Init.MemInc = DMA_MINC_ENABLE;
  hdma_memtomem.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_memtomem.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  hdma_memtomem.Init.Mode = DMA_NORMAL;
  hdma_memtomem.Init.Priority = DMA_PRIORITY_LOW;

  HAL_DMA_Init(&hdma_memtomem);

  /* NVIC */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}

/* ================= GPIO ================= */
static void MX_GPIO_Init(void)
{
  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/* ================= CLOCK ================= */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                              | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;

  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);
}

/* ================= IRQ ================= */
void DMA1_Channel1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_memtomem);
}