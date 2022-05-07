#ifndef __OLED_H
#define __OLED_H			

#include "main.h"
#include "stm32f1xx_hal_gpio.h"

#define X_WIDTH 	128
#define Y_WIDTH 	64	   

//-----------------OLED端口定义----------------  					   
#define OLED_CS_Clr()  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_RESET)//CS
#define OLED_CS_Set()  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_SET)

#define OLED_DC_Clr() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET)//DC
#define OLED_DC_Set() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET)

#define OLED_RST_Clr() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET)//RES
#define OLED_RST_Set() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET)

#define OLED_SDIN_Clr() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_RESET)//DI
#define OLED_SDIN_Set() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_SET)
		     
#define OLED_SCLK_Clr() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET)//DO
#define OLED_SCLK_Set() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_SET)

//#define OLED_PWR_Clr() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,0)//CLK 如果使用的是模块则不需要
//#define OLED_PWR_Set() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,1)

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

//OLED控制用函数
//void OLED_Power_On(void);
//void OLED_Power_Off(void);
void OLED_WR_Byte(uint8_t dat,uint8_t cmd);
void OLED_ColorTurn(uint8_t i);
void OLED_DisplayTurn(uint8_t i);
void OLED_Display_On(void);
void OLED_Display_Off(void);	
void OLED_Refresh(void);
void OLED_Clear(void);
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t);
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size1,uint8_t mode);
void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr,uint8_t size1,uint8_t mode);	 
uint32_t OLED_Pow(uint8_t m,uint8_t n);
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size1,uint8_t mode);
//void OLED_Set_Pos(uint8_t x, uint8_t y);
//void OLED_DrawBMP(uint8_t x0, uint8_t y0,uint8_t x1,uint8_t y1,const uint8_t BMP[]);
//void OLED_ClearBMP(uint8_t x0, uint8_t y0,uint8_t x1,uint8_t y1);
//void LCD_Str_CHinese(uint8_t x,uint8_t y,uint8_t *str);
//void OLED_ClearLine(uint8_t x);
void OLED_Init(void);

#endif 
