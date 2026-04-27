#ifndef INC_STM32F407XX_H_
#define INC_STM32F407XX_H_

#include <stdint.h>
#define __vo   volatile

/*
 * ARM Cortex Mx Processor NVIC ISERx register Addresses
 */
#define NVIC_ISER0             ((__vo uint32_t*)0xE000E100)
#define NVIC_ISER1             ((__vo uint32_t*)0xE000E104)
#define NVIC_ISER2             ((__vo uint32_t*)0xE000E108)
#define NVIC_ISER3             ((__vo uint32_t*)0xE000E10C)

/*
 * ARM Cortex Mx Processor NVIC ICERx register Addresses
 */
#define NVIC_ICER0             ((__vo uint32_t*)0XE000E180)
#define NVIC_ICER1             ((__vo uint32_t*)0XE000E184)
#define NVIC_ICER2             ((__vo uint32_t*)0XE000E188)
#define NVIC_ICER3             ((__vo uint32_t*)0XE000E18C)

/*
 * ARM Cortex Mx Processor priority register address
 */
#define NVIC_PR_BASE_ADDR      ((__vo uint32_t*)0xE000E400)

/*
 * ARM Cortex Mx Processor number of priority bits implemented in priority register
 */
#define NO_PR_BITS_IMPLEMENTED   4
/*
 *
 */
#define FLASH_BASEADDR           0x08000000U /**/
#define SRAM1_BASEADDR           0x20000000U /**/
#define SRAM2_BASEADDR           0x20001C00U /**/
#define ROM_BASEADDR             0x1FFF0000U /**/
#define SRAM                     SRAM1_BASEADDR  /**/

/*
 *
 */
#define PERIPH_BASE              0x40000000U
#define APB1PERIPH_BASEADDR      PERIPH_BASE
#define APB2PERIPH_BASEADDR      0x40010000U
#define AHB1PERIPH_BASEADDR      0x40020000U
#define AHB2PERIPH_BASEADDR      0x50000000U

/*
 * Base addresses of peripherals which are hanging on AHB1 bus
 */
#define GPIOA_BASEADDR          (AHB1PERIPH_BASEADDR + 0x0000)
#define GPIOB_BASEADDR          (AHB1PERIPH_BASEADDR + 0x0400)
#define GPIOC_BASEADDR          (AHB1PERIPH_BASEADDR + 0x0800)
#define GPIOD_BASEADDR          (AHB1PERIPH_BASEADDR + 0x0C00)
#define GPIOE_BASEADDR          (AHB1PERIPH_BASEADDR + 0x1000)
#define GPIOF_BASEADDR          (AHB1PERIPH_BASEADDR + 0x1400)
#define GPIOG_BASEADDR          (AHB1PERIPH_BASEADDR + 0x1800)
#define GPIOH_BASEADDR          (AHB1PERIPH_BASEADDR + 0x1C00)
#define GPIOI_BASEADDR          (AHB1PERIPH_BASEADDR + 0x2000)
#define RCC_BASEADDR            (AHB1PERIPH_BASEADDR + 0x3800)

/*
 * Base addresses of peripherals which are hanging on APB1 bus
 */
#define I2C1_BASEADDR           (APB1PERIPH_BASEADDR + 0x5400)
#define I2C2_BASEADDR           (APB1PERIPH_BASEADDR + 0x5800)
#define I2C3_BASEADDR           (APB1PERIPH_BASEADDR + 0x5C00)
#define SPI2_BASEADDR           (APB1PERIPH_BASEADDR + 0x3800)
#define SPI3_BASEADDR           (APB1PERIPH_BASEADDR + 0x3C00)
#define USART2_BASEADDR         (APB1PERIPH_BASEADDR + 0x4400)
#define USART3_BASEADDR         (APB1PERIPH_BASEADDR + 0x4800)
#define UART4_BASEADDR          (APB1PERIPH_BASEADDR + 0x4C00)
#define USART5_BASEADDR         (APB1PERIPH_BASEADDR + 0x5000)

/*
 * Base addresses of peripherals which are hanging on APB2 bus
 */
#define SPI1_BASEADDR           (APB2PERIPH_BASEADDR + 0x3000)
#define USART1_BASEADDR         (APB2PERIPH_BASEADDR + 0x1000)
#define USART6_BASEADDR         (APB2PERIPH_BASEADDR + 0x1400)
#define EXTI_BASEADDR           (APB2PERIPH_BASEADDR + 0x3C00)
#define SYSCFG_BASEADDR         (APB2PERIPH_BASEADDR + 0x3800)

/*
 * Peripheral register definition structure for GPIO
 */
typedef struct{
	__vo uint32_t  MODER;            /* Address Offset: 0x00*/
	__vo uint32_t  OTYPER;           /* Address Offset: 0x04*/
	__vo uint32_t  OSPEEDR;          /* Address Offset: 0x08*/
	__vo uint32_t  PUPDR;            /* Address Offset: 0x0C*/
	__vo uint32_t  IDR;              /* Address Offset: 0x10*/
	__vo uint32_t  ODR;              /* Address Offset: 0x14*/
	__vo uint32_t  BSRR;            /* Address Offset: 0x18*/
	__vo uint32_t  LCKR;             /* Address Offset: 0x1C*/
	__vo uint32_t  AFR[2];           /* Address Offset: 0x20*/
}GPIO_RegDef_t;

/*
 * Peripheral register definition structure for RCC
 */
typedef struct{
	__vo uint32_t CR;                /* Address Offset: 0x00*/
	__vo uint32_t PLLCFGR;           /* Address Offset: 0x04*/
	__vo uint32_t CFGR;              /* Address Offset: 0x08*/
	__vo uint32_t CIR;               /* Address Offset: 0x0C*/
	__vo uint32_t AHB1RSTR;          /* Address Offset: 0x10*/
	__vo uint32_t AHB2RSTR;          /* Address Offset: 0x14*/
	__vo uint32_t AHB3RSTR;          /* Address Offset: 0x18*/
		 uint32_t RESERVED0;         /* Reserved, 0x1C      */
	__vo uint32_t APB1RSTR;          /* Address Offset: 0x20*/
	__vo uint32_t APB2RSTR;          /* Address Offset: 0x24*/
		 uint32_t RESERVED1[2];      /* Reserved, 0x28-0x2C */
	__vo uint32_t AHB1ENR;           /* Address Offset: 0x30*/
	__vo uint32_t AHB2ENR;           /* Address Offset: 0x34*/
	__vo uint32_t AHB3ENR;           /* Address Offset: 0x38*/
		 uint32_t RESERVED2;         /* Reserved, 0x3C      */
	__vo uint32_t APB1ENR;           /* Address Offset: 0x40*/
	__vo uint32_t APB2ENR;           /* Address Offset: 0x44*/
	     uint32_t RESERVED3[2];      /* Reserved, 0x48-0x4C */
	__vo uint32_t AHB1LPENR;         /* Address Offset: 0x50*/
	__vo uint32_t AHB2LPENR;         /* Address Offset: 0x54*/
	__vo uint32_t AHB3LPENR;         /* Address Offset: 0x58*/
		 uint32_t RESERVED4;         /* Reserved, 0x5C      */
	__vo uint32_t APB1LPENR;         /* Address Offset: 0x60*/
	__vo uint32_t APB2LPENR;         /* Address Offset: 0x64*/
		 uint32_t RESERVED5[2];      /* Reserved, 0x68-0x6C */
	__vo uint32_t BDCR;              /* Address Offset: 0x70*/
	__vo uint32_t CSR;               /* Address Offset: 0x74*/
		 uint32_t RESERVED6[2];      /* Reserved, 0x78-0x7C */
	__vo uint32_t SSCGR;             /* Address Offset: 0x80*/
	__vo uint32_t PLLI2SCFGR;        /* Address Offset: 0x84*/
}RCC_RegDef_t;

/*
 * Peripheral register definition structure for EXTI
 */
typedef struct{
	__vo uint32_t IMR;
	__vo uint32_t EMR;
	__vo uint32_t RTSR;
	__vo uint32_t FTSR;
	__vo uint32_t SWIER;
	__vo uint32_t PR;

}EXTI_RegDef_t;

/*
 * Peripheral register definition structure for SYSCFG
 */
typedef struct{
	 __vo uint32_t MEMRMP;
	 __vo uint32_t PMC;
	 __vo uint32_t EXTICR[4];
	 __vo uint32_t CMPCR;
}SYSCFG_RegDef_t;
/*
 * Peripheral definition
 */
#define GPIOA        ((GPIO_RegDef_t*)GPIOA_BASEADDR)
#define GPIOB        ((GPIO_RegDef_t*)GPIOB_BASEADDR)
#define GPIOC        ((GPIO_RegDef_t*)GPIOC_BASEADDR)
#define GPIOD        ((GPIO_RegDef_t*)GPIOD_BASEADDR)
#define GPIOE        ((GPIO_RegDef_t*)GPIOE_BASEADDR)
#define GPIOF        ((GPIO_RegDef_t*)GPIOF_BASEADDR)
#define GPIOG        ((GPIO_RegDef_t*)GPIOG_BASEADDR)
#define GPIOH        ((GPIO_RegDef_t*)GPIOH_BASEADDR)
#define GPIOI        ((GPIO_RegDef_t*)GPIOI_BASEADDR)

#define RCC         ((RCC_RegDef_t*)RCC_BASEADDR)

#define EXTI        ((EXTI_RegDef_t*)EXTI_BASEADDR)

#define SYSCFG      ((SYSCFG_RegDef_t*)SYSCFG_BASEADDR)

/*
 * Clock enable macros for GPIOx peripherals
 */
#define GPIOA_PCLK_EN()   (RCC->AHB1ENR |= (1<<0))
#define GPIOB_PCLK_EN()   (RCC->AHB1ENR |= (1<<1))
#define GPIOC_PCLK_EN()   (RCC->AHB1ENR |= (1<<2))
#define GPIOD_PCLK_EN()   (RCC->AHB1ENR |= (1<<3))
#define GPIOE_PCLK_EN()   (RCC->AHB1ENR |= (1<<4))
#define GPIOF_PCLK_EN()   (RCC->AHB1ENR |= (1<<5))
#define GPIOG_PCLK_EN()   (RCC->AHB1ENR |= (1<<6))
#define GPIOH_PCLK_EN()   (RCC->AHB1ENR |= (1<<7))
#define GPIOI_PCLK_EN()   (RCC->AHB1ENR |= (1<<8))

/*
 * Clock enable macros for I2Cx peripherals
 */
#define I2C1_PCLK_EN()    (RCC->APB1ENR |= (1<<21))
#define I2C2_PCLK_EN()    (RCC->APB1ENR |= (1<<22))
#define I2C3_PCLK_EN()    (RCC->APB1ENR |= (1<<23))

/*
 * Clock enable macros for SPIx peripherals
 */
#define SPI1_PCLK_EN()    (RCC->APB2ENR |= (1<<12))
#define SPI2_PCLK_EN()    (RCC->APB1ENR |= (1<<14))
#define SPI3_PCLK_EN()    (RCC->APB1ENR |= (1<<15))

/*
 * Clock enable macros for USARTx peripherals
 */
#define USART1_PCLK_EN()    (RCC->APB2ENR |= (1<<4))
#define USART2_PCLK_EN()    (RCC->APB1ENR |= (1<<17))
#define USART3_PCLK_EN()    (RCC->APB1ENR |= (1<<18))
#define UART4_PCLK_EN()     (RCC->APB1ENR |= (1<<19))
#define UART5_PCLK_EN()     (RCC->APB1ENR |= (1<<20))
#define USART6_PCLK_EN()    (RCC->APB2ENR |= (1<<6))

/*
 * Clock enable macros for SYSCFG peripherals
 */
#define SYSCFG_PCLK_EN()    (RCC->APB2ENR |= (1<<14))

/*
 * Clock diable macros for GPIOx peripherals
 */
#define GPIOA_PCLK_DI()   (RCC->AHB1ENR &= ~(1<<0))
#define GPIOB_PCLK_DI()   (RCC->AHB1ENR &= ~(1<<1))
#define GPIOC_PCLK_DI()   (RCC->AHB1ENR &= ~(1<<2))
#define GPIOD_PCLK_DI()   (RCC->AHB1ENR &= ~(1<<3))
#define GPIOE_PCLK_DI()   (RCC->AHB1ENR &= ~(1<<4))
#define GPIOF_PCLK_DI()   (RCC->AHB1ENR &= ~(1<<5))
#define GPIOG_PCLK_DI()   (RCC->AHB1ENR &= ~(1<<6))
#define GPIOH_PCLK_DI()   (RCC->AHB1ENR &= ~(1<<7))
#define GPIOI_PCLK_DI()   (RCC->AHB1ENR &= ~(1<<8))

/*
 * Clock disable macros for I2Cx peripherals
 */
#define I2C1_PCLK_DI()    (RCC->APB1ENR &= ~(1<<21))
#define I2C2_PCLK_DI()    (RCC->APB1ENR &= ~(1<<22))
#define I2C3_PCLK_DI()    (RCC->APB1ENR &= ~(1<<23))

/*
 * Clock disable macros for SPIx peripherals
 */
#define SPI1_PCLK_DI()    (RCC->APB2ENR &= ~(1<<12))
#define SPI2_PCLK_DI()    (RCC->APB1ENR &= ~(1<<14))
#define SPI3_PCLK_DI()    (RCC->APB1ENR &= ~(1<<15))

/*
 * Clock disable macros for USARTx peripherals
 */
#define USART1_PCLK_DI()    (RCC->APB2ENR &= ~(1<<4))
#define USART2_PCLK_DI()    (RCC->APB1ENR &= ~(1<<17))
#define USART3_PCLK_DI()    (RCC->APB1ENR &= ~(1<<18))
#define UART4_PCLK_DI()     (RCC->APB1ENR &= ~(1<<19))
#define UART5_PCLK_DI()     (RCC->APB1ENR &= ~(1<<20))
#define USART6_PCLK_DI()    (RCC->APB2ENR &= ~(1<<6))

/*
 * Clock disable macros for SYSCFG peripherals
 */
#define SYSCFG_PCLK_DI()    (RCC->APB2ENR &= ~(1<<14))

/*
 * Macros to reset GPIOx peripherals
 */
#define GPIOA_REG_RESET()   do{ (RCC->AHB1RSTR |= (1<<0)); (RCC->AHB1RSTR &= ~(1<<0)); }while(0)
#define GPIOB_REG_RESET()   do{ (RCC->AHB1RSTR |= (1<<1)); (RCC->AHB1RSTR &= ~(1<<1)); }while(0)
#define GPIOC_REG_RESET()   do{ (RCC->AHB1RSTR |= (1<<2)); (RCC->AHB1RSTR &= ~(1<<2)); }while(0)
#define GPIOD_REG_RESET()   do{ (RCC->AHB1RSTR |= (1<<3)); (RCC->AHB1RSTR &= ~(1<<3)); }while(0)
#define GPIOE_REG_RESET()   do{ (RCC->AHB1RSTR |= (1<<4)); (RCC->AHB1RSTR &= ~(1<<4)); }while(0)
#define GPIOF_REG_RESET()   do{ (RCC->AHB1RSTR |= (1<<5)); (RCC->AHB1RSTR &= ~(1<<5)); }while(0)
#define GPIOG_REG_RESET()   do{ (RCC->AHB1RSTR |= (1<<6)); (RCC->AHB1RSTR &= ~(1<<6)); }while(0)
#define GPIOH_REG_RESET()   do{ (RCC->AHB1RSTR |= (1<<7)); (RCC->AHB1RSTR &= ~(1<<7)); }while(0)
#define GPIOI_REG_RESET()   do{ (RCC->AHB1RSTR |= (1<<8)); (RCC->AHB1RSTR &= ~(1<<8)); }while(0)

/*
 * return port code for given GPIOx base address
 */
#define GPIO_BASEADDR_TO_CODE(x)     (x == GPIOA) ? 0 :\
		                             (x == GPIOB) ? 1 :\
		                             (x == GPIOC) ? 2 :\
		               				 (x == GPIOD) ? 3 :\
		               				 (x == GPIOE) ? 4 :\
		               			     (x == GPIOF) ? 5 :\
		               			     (x == GPIOG) ? 6 :\
		               				 (x == GPIOH) ? 7 :8


/*
 * IRQ(interrupt request) number
 */
#define IRQ_NO_EXTI0         6
#define IRQ_NO_EXTI1         7
#define IRQ_NO_EXTI2         8
#define IRQ_NO_EXTI3         9
#define IRQ_NO_EXTI4         10
#define IRQ_NO_EXTI9_5       23
#define IRQ_NO_EXTI15_10     40

/*
 * NVIC priority number
 */
#define NVIC_IRQ_PR0         0
#define NVIC_IRQ_PR1         1
#define NVIC_IRQ_PR2         2
#define NVIC_IRQ_PR3         3
#define NVIC_IRQ_PR4         4
#define NVIC_IRQ_PR5         5
#define NVIC_IRQ_PR6         6
#define NVIC_IRQ_PR7         7
#define NVIC_IRQ_PR8         8
#define NVIC_IRQ_PR9         9
#define NVIC_IRQ_PR10        10
#define NVIC_IRQ_PR11        11
#define NVIC_IRQ_PR12        12
#define NVIC_IRQ_PR13        13
#define NVIC_IRQ_PR14        14
#define NVIC_IRQ_PR15        15

/*
 * Some generic macros
 */
#define ENABLE              1
#define DISABLE             0
#define SET                 ENABLE
#define RESET               DISABLE
#define GPIO_PIN_SET        SET
#define GPIO_PIN_RESET      RESET

#include "stm32f4xx_gpio_driver.h"

#endif /* INC_STM32F407XX_H_ */
