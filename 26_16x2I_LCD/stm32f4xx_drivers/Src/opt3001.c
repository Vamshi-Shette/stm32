/*
 * opt3001.c
 *
 *  Created on: Apr 21, 2026
 *      Author: SANTOSH
 */

/*
 * opt3001.c  –  OPT3001 ambient light sensor driver
 *               Ported to custom STM32F407xx I2C driver (no HAL).
 *
 * I2C protocol for OPT3001:
 *   Write register:  START | ADDR+W | REG | MSB | LSB | STOP
 *   Read  register:  START | ADDR+W | REG | STOP
 *                    START | ADDR+R | MSB | LSB (NACK) | STOP
 */

#include "opt3001.h"
#include "stdio.h"
/* ─────────────────────────────────────────────────
 *  Private helpers
 * ───────────────────────────────────────────────── */

/**
 * @brief  Write a 16-bit value to a register.
 *         Transaction: START | ADDR+W | reg | value_MSB | value_LSB | STOP
 */
static OPT3001_Status _write_reg(OPT3001_Handle *dev,
                                 uint8_t         reg,
                                 uint16_t        value)
{
    uint8_t buf[3];
    buf[0] = reg;
    buf[1] = (uint8_t)(value >> 8);    /* MSB first */
    buf[2] = (uint8_t)(value & 0xFF);  /* LSB       */

    /*
     * I2C_MasterSendData(handle, txBuffer, length, 7-bit slave addr)
     * The driver shifts the address and appends the W bit internally.
     */
    I2C_MasterSendData(dev->hi2c, buf, 3, dev->addr);

    return OPT3001_OK;
}

/**
 * @brief  Read a 16-bit value from a register.
 *         Transaction 1 (pointer write):
 *           START | ADDR+W | reg | STOP
 *         Transaction 2 (data read):
 *           START | ADDR+R | MSB | LSB (NACK) | STOP
 */
static OPT3001_Status _read_reg(OPT3001_Handle *dev,
                                uint8_t         reg,
                                uint16_t       *value)
{
    uint8_t buf[2];

    /* Write the register pointer */
    I2C_MasterSendData(dev->hi2c, &reg, 1, dev->addr);

    /* Read 2 bytes back */
    I2C_MasterReceiveData(dev->hi2c, buf, 2, dev->addr);

    *value = ((uint16_t)buf[0] << 8) | buf[1];

    return OPT3001_OK;
}

/* ─────────────────────────────────────────────────
 *  Public API
 * ───────────────────────────────────────────────── */

/**
 * @brief  Verify device identity and apply initial configuration.
 * @param  dev     Pointer to OPT3001_Handle (hi2c and addr must be set).
 * @param  config  Initial value to write to the configuration register.
 *                 Use OPT3001_CFG_DEFAULT for a sane starting point.
 * @retval OPT3001_OK     – device found and configured.
 *         OPT3001_NODEV  – no device responded or wrong device ID.
 *         OPT3001_ERROR  – I2C communication error.
 */
OPT3001_Status OPT3001_Init(OPT3001_Handle *dev, uint16_t config)
{
    uint16_t dev_id = 0;
    if (_read_reg(dev, OPT3001_REG_DEVICE_ID, &dev_id) != OPT3001_OK)
    {
        return OPT3001_NODEV;
    }

    if (dev_id != 0x3001U)
    {
        return OPT3001_NODEV;   /* Wrong device on the bus */
    }

    return OPT3001_SetConfig(dev, config);
}

/**
 * @brief  Write the configuration register and update the shadow copy.
 */
OPT3001_Status OPT3001_SetConfig(OPT3001_Handle *dev, uint16_t config)
{
    OPT3001_Status s = _write_reg(dev, OPT3001_REG_CONFIGURATION, config);
    if (s == OPT3001_OK)
    {
        dev->config = config;
    }
    return s;
}

/**
 * @brief  Read back the configuration register from the device.
 */
OPT3001_Status OPT3001_GetConfig(OPT3001_Handle *dev, uint16_t *config_out)
{
    return _read_reg(dev, OPT3001_REG_CONFIGURATION, config_out);
}

/**
 * @brief  Read the raw result register (16-bit).
 *         Use OPT3001_RawToLux() to convert.
 */
OPT3001_Status OPT3001_ReadRaw(OPT3001_Handle *dev, uint16_t *raw)
{
    return _read_reg(dev, OPT3001_REG_RESULT, raw);
}

/**
 * @brief  Convert a raw register value to lux.
 *         Formula: lux = 0.01 * 2^exponent * mantissa
 */
float OPT3001_RawToLux(uint16_t raw)
{
    uint8_t  exponent = (uint8_t)((raw >> 12) & 0x0FU);
    uint16_t mantissa = raw & 0x0FFFU;

    float lux = 0.01f * (float)(1U << exponent) * (float)mantissa;
    return lux;
}

/**
 * @brief  Poll for conversion-ready then return the lux result.
 *         Assumes the device is already running in continuous or single mode.
 *
 * @note   Uses a software busy-wait via OPT3001_DELAY_MS().
 *         Timeout is ~1000 ms for 800 ms conversion time, ~200 ms for 100 ms.
 */
OPT3001_Status OPT3001_ReadLux(OPT3001_Handle *dev, float *lux_out)
{
    uint16_t cfg = 0;
    uint16_t raw = 0;
    uint32_t elapsed_ms = 0;
    uint32_t timeout_ms;

    timeout_ms = (dev->config & OPT3001_CFG_CT_800MS) ? 1000U : 200U;

    do
    {
        OPT3001_DELAY_MS(5);
        elapsed_ms += 5U;

        if (_read_reg(dev, OPT3001_REG_CONFIGURATION, &cfg) != OPT3001_OK)
        {
            return OPT3001_ERROR;
        }

        if (elapsed_ms > timeout_ms)
        {
            return OPT3001_BUSY;   /* Conversion timed out */
        }
    }
    while (!(cfg & OPT3001_CFG_RDY_FLAG));

    if (_read_reg(dev, OPT3001_REG_RESULT, &raw) != OPT3001_OK)
    {
        return OPT3001_ERROR;
    }

    *lux_out = OPT3001_RawToLux(raw);
    return OPT3001_OK;
}

/**
 * @brief  Trigger a single-shot conversion, wait for result, then shutdown.
 *         Safe to call repeatedly in low-power polling loops.
 */
OPT3001_Status OPT3001_ReadLuxSingleShot(OPT3001_Handle *dev, float *lux_out)
{
    /* Build single-shot config from the current config shadow */
    uint16_t ss_cfg = (dev->config & ~(0x3U << 9)) | OPT3001_CFG_MODE_SINGLE;

    if (OPT3001_SetConfig(dev, ss_cfg) != OPT3001_OK)
    {
        return OPT3001_ERROR;
    }

    OPT3001_Status s = OPT3001_ReadLux(dev, lux_out);

    /* Restore shutdown mode in shadow (device auto-returns to shutdown after
     * single-shot, but keep the shadow consistent) */
    dev->config = (ss_cfg & ~(0x3U << 9)) | OPT3001_CFG_MODE_SHUTDOWN;

    return s;
}

/**
 * @brief  Put the device into shutdown mode (lowest power).
 */
OPT3001_Status OPT3001_Shutdown(OPT3001_Handle *dev)
{
    uint16_t cfg = (dev->config & ~(0x3U << 9)) | OPT3001_CFG_MODE_SHUTDOWN;
    return OPT3001_SetConfig(dev, cfg);
}

/**
 * @brief  Read the Manufacturer ID register (expected: 0x5449 = "TI").
 */
OPT3001_Status OPT3001_ReadManufacturerID(OPT3001_Handle *dev, uint16_t *id)
{
    return _read_reg(dev, OPT3001_REG_MANUFACTURER, id);
}

/**
 * @brief  Read the Device ID register (expected: 0x3001).
 */
OPT3001_Status OPT3001_ReadDeviceID(OPT3001_Handle *dev, uint16_t *id)
{
    return _read_reg(dev, OPT3001_REG_DEVICE_ID, id);
}
