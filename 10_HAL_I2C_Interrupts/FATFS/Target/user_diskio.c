

#ifdef USE_OBSOLETE_USER_CODE_SECTION_0
#endif

#include <string.h>
#include "ff_gen_drv.h"
#include "main.h"

extern uint8_t SPI_SendByte_IT(uint8_t data);

extern HAL_StatusTypeDef SPI_ReadSector_IT(uint8_t *rx_buffer);
extern HAL_StatusTypeDef SPI_WriteSector_IT(uint8_t *tx_buffer);

static volatile DSTATUS Stat = STA_NOINIT;

DSTATUS USER_initialize(BYTE pdrv);
DSTATUS USER_status(BYTE pdrv);
DRESULT USER_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count);

#if _USE_WRITE == 1
DRESULT USER_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count);
#endif

#if _USE_IOCTL == 1
DRESULT USER_ioctl(BYTE pdrv, BYTE cmd, void *buff);
#endif

Diskio_drvTypeDef USER_Driver =
{
    USER_initialize,
    USER_status,
    USER_read,
#if _USE_WRITE
    USER_write,
#endif
#if _USE_IOCTL
    USER_ioctl,
#endif
};

/* ================= SD INIT ================= */
DSTATUS USER_initialize(BYTE pdrv)
{
    uint8_t response;
    uint8_t cmd[6];

    SD_CS_HIGH();
    HAL_Delay(20);

    /* 80 dummy clocks */
    for (int i = 0; i < 10; i++)
    {
        SPI_SendByte_IT(0xFF);
    }

    /* ---------------- CMD0 ---------------- */
    cmd[0] = 0x40;   // CMD0
    cmd[1] = 0x00;
    cmd[2] = 0x00;
    cmd[3] = 0x00;
    cmd[4] = 0x00;
    cmd[5] = 0x95;

    SD_CS_LOW();

    for (int i = 0; i < 6; i++)
        SPI_SendByte_IT(cmd[i]);

    for (int i = 0; i < 100; i++)
    {
        response = SPI_SendByte_IT(0xFF);
        if (response == 0x01)
            break;
    }

    SD_CS_HIGH();
    SPI_SendByte_IT(0xFF);

    if (response != 0x01)
    {
        Stat = STA_NOINIT;
        return Stat;
    }

    /* ---------------- CMD8 ---------------- */
    cmd[0] = 0x48;   // CMD8
    cmd[1] = 0x00;
    cmd[2] = 0x00;
    cmd[3] = 0x01;
    cmd[4] = 0xAA;
    cmd[5] = 0x87;

    SD_CS_LOW();

    for (int i = 0; i < 6; i++)
        SPI_SendByte_IT(cmd[i]);

    for (int i = 0; i < 100; i++)
    {
        response = SPI_SendByte_IT(0xFF);
        if (response == 0x01)
            break;
    }

    /* discard 4 response bytes */
    for (int i = 0; i < 4; i++)
        SPI_SendByte_IT(0xFF);

    SD_CS_HIGH();
    SPI_SendByte_IT(0xFF);

    /* ---------------- ACMD41 LOOP ---------------- */
    for (int retry = 0; retry < 1000; retry++)
    {
        /* CMD55 */
        SD_CS_LOW();

        SPI_SendByte_IT(0x77);
        SPI_SendByte_IT(0x00);
        SPI_SendByte_IT(0x00);
        SPI_SendByte_IT(0x00);
        SPI_SendByte_IT(0x00);
        SPI_SendByte_IT(0xFF);

        while (SPI_SendByte_IT(0xFF) == 0xFF);

        SD_CS_HIGH();
        SPI_SendByte_IT(0xFF);

        /* ACMD41 */
        SD_CS_LOW();

        SPI_SendByte_IT(0x69);
        SPI_SendByte_IT(0x40);
        SPI_SendByte_IT(0x00);
        SPI_SendByte_IT(0x00);
        SPI_SendByte_IT(0x00);
        SPI_SendByte_IT(0xFF);

        for (int i = 0; i < 100; i++)
        {
            response = SPI_SendByte_IT(0xFF);

            if (response == 0x00)
            {
                SD_CS_HIGH();
                SPI_SendByte_IT(0xFF);

                Stat = 0;
                return Stat;
            }
        }

        SD_CS_HIGH();
        SPI_SendByte_IT(0xFF);

        HAL_Delay(1);
    }

    Stat = STA_NOINIT;
    return Stat;
}
/* ================= STATUS ================= */
DSTATUS USER_status(BYTE pdrv)
{
    return Stat;
}

/* ================= READ SECTOR ================= */
DRESULT USER_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count)
{
    uint8_t response;
    uint8_t cmd[6];
    uint32_t address;

    if (count != 1)
        return RES_PARERR;

    address = sector;

    /* CMD17 = READ_SINGLE_BLOCK */
    cmd[0] = 0x51;
    cmd[1] = (address >> 24) & 0xFF;
    cmd[2] = (address >> 16) & 0xFF;
    cmd[3] = (address >> 8) & 0xFF;
    cmd[4] = address & 0xFF;
    cmd[5] = 0xFF;

    SD_CS_LOW();

    /* Send command */
    for (int i = 0; i < 6; i++)
    {
        SPI_SendByte_IT(cmd[i]);
    }

    /* Wait for command response 0x00 */
    for (int i = 0; i < 1000; i++)
    {
        response = SPI_SendByte_IT(0xFF);
        if (response == 0x00)
            break;
    }

    if (response != 0x00)
    {
        SD_CS_HIGH();
        SPI_SendByte_IT(0xFF);
        return RES_ERROR;
    }

    /* Wait for data token 0xFE */
    for (int i = 0; i < 10000; i++)
    {
        response = SPI_SendByte_IT(0xFF);
        if (response == 0xFE)
            break;
    }

    if (response != 0xFE)
    {
        SD_CS_HIGH();
        SPI_SendByte_IT(0xFF);
        return RES_ERROR;
    }

    /* Read 512 bytes */

    /*for (int i = 0; i < 512; i++)
    {
        buff[i] = SPI_SendByte_IT(0xFF);
    }*/

    SPI_ReadSector_IT(buff);

    /* Ignore CRC */
    SPI_SendByte_IT(0xFF);
    SPI_SendByte_IT(0xFF);

    SD_CS_HIGH();
    SPI_SendByte_IT(0xFF);

    return RES_OK;
}

/* ================= WRITE ================= */
#if _USE_WRITE == 1
DRESULT USER_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
{
    uint8_t response;
    uint8_t cmd[6];
    uint32_t address;

    if (count != 1)
        return RES_PARERR;

    /* SDHC uses block addressing */
    address = sector;

    /* CMD24 = WRITE_SINGLE_BLOCK */
    cmd[0] = 0x58;
    cmd[1] = (address >> 24) & 0xFF;
    cmd[2] = (address >> 16) & 0xFF;
    cmd[3] = (address >> 8) & 0xFF;
    cmd[4] = address & 0xFF;
    cmd[5] = 0xFF;

    SD_CS_LOW();

    /* Send command */
    for (int i = 0; i < 6; i++)
    {
        SPI_SendByte_IT(cmd[i]);
    }

    /* Wait for response */
    for (int i = 0; i < 1000; i++)
    {
        response = SPI_SendByte_IT(0xFF);
        if (response == 0x00)
            break;
    }

    if (response != 0x00)
    {
        SD_CS_HIGH();
        SPI_SendByte_IT(0xFF);
        return RES_ERROR;
    }

    /* Send one dummy byte */
    SPI_SendByte_IT(0xFF);

    /* Data token */
    SPI_SendByte_IT(0xFE);

    /* Send 512 bytes */
    /*
    for (int i = 0; i < 512; i++)
    {
        SPI_SendByte_IT(buff[i]);
    }*/

    SPI_WriteSector_IT((uint8_t *)buff);

    /* Dummy CRC */
    SPI_SendByte_IT(0xFF);
    SPI_SendByte_IT(0xFF);

    /* Check data response token */
    response = SPI_SendByte_IT(0xFF);

    if ((response & 0x1F) != 0x05)
    {
        SD_CS_HIGH();
        SPI_SendByte_IT(0xFF);
        return RES_ERROR;
    }

    /* Wait until card is not busy */
    while (SPI_SendByte_IT(0xFF) == 0x00);

    SD_CS_HIGH();
    SPI_SendByte_IT(0xFF);

    return RES_OK;
}
#endif

/* ================= IOCTL ================= */
#if _USE_IOCTL == 1
DRESULT USER_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
    switch (cmd)
    {
        case GET_SECTOR_SIZE:
            *(WORD *)buff = 512;
            return RES_OK;

        case GET_BLOCK_SIZE:
            *(DWORD *)buff = 1;
            return RES_OK;

        case GET_SECTOR_COUNT:
            *(DWORD *)buff = 32768;
            return RES_OK;

        default:
            return RES_PARERR;
    }
}
#endif
