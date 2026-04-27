/*
 * 013_16x2_lcd.c
 */
#include "stm32f407xx.h"
#include <string.h>

/********* LCD PIN CONFIG *********/
#define LCD_GPIO_PORT GPIOA

#define LCD_RS GPIO_PIN_NO_0
#define LCD_EN GPIO_PIN_NO_1

#define LCD_D4 GPIO_PIN_NO_4
#define LCD_D5 GPIO_PIN_NO_5
#define LCD_D6 GPIO_PIN_NO_6
#define LCD_D7 GPIO_PIN_NO_7

/********* FUNCTION PROTOTYPES *********/
void LCD_GPIO_Init(void);
void LCD_Init(void);
void LCD_SendCommand(uint8_t cmd);
void LCD_SendData(uint8_t data);
void LCD_PrintString(char *msg);
void LCD_SetCursor(uint8_t row, uint8_t col);
void delay(void);

/********* SIMPLE DELAY *********/
void delay(void)
{
    for(uint32_t i=0;i<50000;i++);
}

/********* WRITE 4 BITS *********/
void LCD_Write4Bits(uint8_t data)
{
    GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_D4, (data >> 0) & 1);
    GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_D5, (data >> 1) & 1);
    GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_D6, (data >> 2) & 1);
    GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_D7, (data >> 3) & 1);

    // Enable pulse
    GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_EN, 1);
    delay();
    GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_EN, 0);
}

/********* SEND COMMAND *********/
void LCD_SendCommand(uint8_t cmd)
{
    GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_RS, 0);

    LCD_Write4Bits(cmd >> 4);
    LCD_Write4Bits(cmd & 0x0F);

    delay();
}

/********* SEND DATA *********/
void LCD_SendData(uint8_t data)
{
    GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_RS, 1);

    LCD_Write4Bits(data >> 4);
    LCD_Write4Bits(data & 0x0F);

    delay();
}

/********* PRINT STRING *********/
void LCD_PrintString(char *msg)
{
    while(*msg)
    {
        LCD_SendData(*msg++);
    }
}

/********* SET CURSOR *********/
void LCD_SetCursor(uint8_t row, uint8_t col)
{
    uint8_t addr;

    if(row == 0)
        addr = 0x80 + col;
    else
        addr = 0xC0 + col;

    LCD_SendCommand(addr);
}

/********* INIT LCD *********/
void LCD_Init(void)
{
    delay();

    LCD_SendCommand(0x33); // Init
    LCD_SendCommand(0x32); // 4-bit mode
    LCD_SendCommand(0x28); // 2 line, 5x7
    LCD_SendCommand(0x0C); // Display ON
    LCD_SendCommand(0x06); // Cursor move
    LCD_SendCommand(0x01); // Clear
    delay();
}

/********* GPIO INIT *********/
void LCD_GPIO_Init(void)
{
    GPIO_Handle_t lcd;

    lcd.pGPIOx = LCD_GPIO_PORT;
    lcd.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    lcd.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    lcd.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    lcd.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

    // RS
    lcd.GPIO_PinConfig.GPIO_PinNumber = LCD_RS;
    GPIO_Init(&lcd);

    // EN
    lcd.GPIO_PinConfig.GPIO_PinNumber = LCD_EN;
    GPIO_Init(&lcd);

    // D4-D7
    lcd.GPIO_PinConfig.GPIO_PinNumber = LCD_D4;
    GPIO_Init(&lcd);

    lcd.GPIO_PinConfig.GPIO_PinNumber = LCD_D5;
    GPIO_Init(&lcd);

    lcd.GPIO_PinConfig.GPIO_PinNumber = LCD_D6;
    GPIO_Init(&lcd);

    lcd.GPIO_PinConfig.GPIO_PinNumber = LCD_D7;
    GPIO_Init(&lcd);
}

/********* MAIN *********/
int main(void)
{
    LCD_GPIO_Init();
    LCD_Init();

    LCD_SetCursor(0,0);
    LCD_PrintString("STM32 LCD");

    LCD_SetCursor(1,0);
    LCD_PrintString("Working!");

    while(1);
}


