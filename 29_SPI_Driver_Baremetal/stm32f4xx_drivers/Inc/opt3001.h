/*
 * opt3001.h
 *
 *  Created on: Apr 21, 2026
 *      Author: SANTOSH
 */
/*
 * opt3001.h  –  OPT3001 ambient light sensor driver
 *               Ported to custom STM32F407xx I2C driver (no HAL).
 */

#ifndef OPT3001_H
#define OPT3001_H

#include <stdint.h>
#include "stm32f407xx.h"   /* brings in I2C_Handle_t via stm32f407xx_i2c_driver.h */

/* ─── I2C Addresses (7-bit, un-shifted) ─── */
#define OPT3001_I2C_ADDR_GND   0x44U   /* ADDR pin -> GND */
#define OPT3001_I2C_ADDR_VCC   0x45U   /* ADDR pin -> VCC */
#define OPT3001_I2C_ADDR_SDA   0x46U   /* ADDR pin -> SDA */
#define OPT3001_I2C_ADDR_SCL   0x47U   /* ADDR pin -> SCL */

/* ─── Register Map ─── */
#define OPT3001_REG_RESULT        0x00U
#define OPT3001_REG_CONFIGURATION 0x01U
#define OPT3001_REG_LOW_LIMIT     0x02U
#define OPT3001_REG_HIGH_LIMIT    0x03U
#define OPT3001_REG_MANUFACTURER  0x7EU   /* Expected: 0x5449 */
#define OPT3001_REG_DEVICE_ID     0x7FU   /* Expected: 0x3001 */

/* ─── Configuration Register Bit Fields ─── */

/* Range Number (bits [15:12]) */
#define OPT3001_CFG_RN_AUTO       (0xCU << 12)   /* Auto full-scale (recommended) */
#define OPT3001_CFG_RN_40LUX      (0x0U << 12)
#define OPT3001_CFG_RN_80LUX      (0x1U << 12)
#define OPT3001_CFG_RN_160LUX     (0x2U << 12)
#define OPT3001_CFG_RN_320LUX     (0x3U << 12)
#define OPT3001_CFG_RN_640LUX     (0x4U << 12)
#define OPT3001_CFG_RN_1280LUX    (0x5U << 12)
#define OPT3001_CFG_RN_2560LUX    (0x6U << 12)
#define OPT3001_CFG_RN_5120LUX    (0x7U << 12)
#define OPT3001_CFG_RN_10240LUX   (0x8U << 12)
#define OPT3001_CFG_RN_20480LUX   (0x9U << 12)
#define OPT3001_CFG_RN_40960LUX   (0xAU << 12)
#define OPT3001_CFG_RN_83865LUX   (0xBU << 12)

/* Conversion Time (bit 11) */
#define OPT3001_CFG_CT_100MS      (0U << 11)
#define OPT3001_CFG_CT_800MS      (1U << 11)

/* Mode of Conversion (bits [10:9]) */
#define OPT3001_CFG_MODE_SHUTDOWN (0x0U << 9)
#define OPT3001_CFG_MODE_SINGLE   (0x1U << 9)
#define OPT3001_CFG_MODE_CONT     (0x2U << 9)   /* Continuous */

/* Result-ready flag (bit 7, read-only) */
#define OPT3001_CFG_RDY_FLAG      (1U << 7)

/* ─── Default Config: Auto-range, 800 ms, Continuous ─── */
#define OPT3001_CFG_DEFAULT  \
    (OPT3001_CFG_RN_AUTO | OPT3001_CFG_CT_800MS | OPT3001_CFG_MODE_CONT)

/* ─── Software delay macro ───
 * Replace with your own delay_ms() implementation if available.
 * Simple busy-wait shown here; for RTOS projects use vTaskDelay etc. */
#ifndef OPT3001_DELAY_MS
#  define OPT3001_DELAY_MS(ms)   do { \
        volatile uint32_t _d = (ms) * 8000U; \
        while (_d--) { __asm("NOP"); } \
    } while (0)
#endif

/* ─── Status Codes ─── */
typedef enum {
    OPT3001_OK    =  0,
    OPT3001_ERROR = -1,
    OPT3001_BUSY  = -2,
    OPT3001_NODEV = -3,
} OPT3001_Status;

/* ─── Device Handle ─── */
typedef struct {
    I2C_Handle_t *hi2c;   /* Pointer to your I2C_Handle_t (custom driver) */
    uint8_t       addr;   /* 7-bit I2C address (un-shifted)               */
    uint16_t      config; /* Shadow of last written config register        */
} OPT3001_Handle;

/* ─── Public API ─── */
OPT3001_Status OPT3001_Init(OPT3001_Handle *dev, uint16_t config);

OPT3001_Status OPT3001_ReadLux(OPT3001_Handle *dev, float *lux_out);
OPT3001_Status OPT3001_ReadLuxSingleShot(OPT3001_Handle *dev, float *lux_out);
OPT3001_Status OPT3001_ReadRaw(OPT3001_Handle *dev, uint16_t *raw);

float          OPT3001_RawToLux(uint16_t raw);

OPT3001_Status OPT3001_SetConfig(OPT3001_Handle *dev, uint16_t config);
OPT3001_Status OPT3001_GetConfig(OPT3001_Handle *dev, uint16_t *config_out);

OPT3001_Status OPT3001_ReadManufacturerID(OPT3001_Handle *dev, uint16_t *id);
OPT3001_Status OPT3001_ReadDeviceID(OPT3001_Handle *dev, uint16_t *id);

OPT3001_Status OPT3001_Shutdown(OPT3001_Handle *dev);

#endif /* OPT3001_H */
