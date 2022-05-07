
//  功能描述   : 0.96英寸OLED驱动程序
//              GND    电源地
//              VCC  接5V或3.3v电源
//              D0   接    PB7
//              D1   接    PB6
//              RES  接    PB5
//              DC   接    PB1
//              CS   接    PB3
		   

#include "oled.h"
#include "oledfont.h"  

uint8_t OLED_GRAM[127][8];

//如果使用的是模块则不需要
//void OLED_Power_On(void)
//{
//	OLED_PWR_Clr();
//}
//void OLED_Power_Off(void)
//{
//	OLED_PWR_Set();
//}

//向SSD1106写入一个字节。
//dat:要写入的数据/命令
//cmd:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(uint8_t dat,uint8_t cmd)
{	
	uint8_t i;			  
	if(cmd)
	  OLED_DC_Set();
	else 
	  OLED_DC_Clr();		  
	OLED_CS_Clr();
	for(i=0;i<8;i++)
	{			  
		OLED_SCLK_Clr();
		if(dat&0x80)
		   OLED_SDIN_Set();
		else 
		   OLED_SDIN_Clr();
		OLED_SCLK_Set();
		dat<<=1;   
	}				 		  
	OLED_CS_Set();
	OLED_DC_Set();   	  
} 

//反显函数
void OLED_ColorTurn(uint8_t i)
{
	if(i==0)
		{
			OLED_WR_Byte(0xA6,OLED_CMD);//正常显示
		}
	if(i==1)
		{
			OLED_WR_Byte(0xA7,OLED_CMD);//反色显示
		}
}

//屏幕旋转180度
void OLED_DisplayTurn(uint8_t i)
{
	if(i==0)
		{
			OLED_WR_Byte(0xC8,OLED_CMD);//正常显示
			OLED_WR_Byte(0xA1,OLED_CMD);
		}
	if(i==1)
		{
			OLED_WR_Byte(0xC0,OLED_CMD);//反转显示
			OLED_WR_Byte(0xA0,OLED_CMD);
		}
}

//开启OLED显示    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}

//关闭OLED显示     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}	

//更新显存到OLED	
void OLED_Refresh(void)
{
	uint8_t i,n;
	for(i=0;i<8;i++)
	{
	   OLED_WR_Byte(0xb0+i,OLED_CMD); //设置行起始地址
	   OLED_WR_Byte(0x00,OLED_CMD);   //设置低列起始地址
	   OLED_WR_Byte(0x10,OLED_CMD);   //设置高列起始地址
	   for(n=0;n<128;n++)
				OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA);
  }
}

//清屏函数
void OLED_Clear(void)
{
	uint8_t i,n;
	for(i=0;i<8;i++)
	{
	   for(n=0;n<128;n++)
			{
			 OLED_GRAM[n][i]=0;//清除所有数据
			}
  }
	OLED_Refresh();//更新显示
}

//画点 
//x:0~127
//y:0~63
//t:1 填充 0,清空	
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t)
{
	uint8_t i,m,n;
	i=y/8;
	m=y%8;
	n=1<<m;
	if(t)
	{
		OLED_GRAM[x][i]|=n;
	}
	else
	{
		OLED_GRAM[x][i]=~OLED_GRAM[x][i];
		OLED_GRAM[x][i]|=n;
		OLED_GRAM[x][i]=~OLED_GRAM[x][i];
	}
}

//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//size1:选择字体 6x8/6x12/8x16/12x24
//mode:0,反色显示;1,正常显示
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size1,uint8_t mode)
{
	uint8_t i,m,temp,size2,chr1;
	uint8_t x0=x,y0=y;
	if(size1==8)
		size2=6;
	else 
		size2=(size1/8+((size1%8)?1:0))*(size1/2);  //得到字体一个字符对应点阵集所占的字节数
	chr1=chr-' ';  //计算偏移后的值
	for(i=0;i<size2;i++)
	{
		if(size1==8)
			  {temp=asc2_0806[chr1][i];} //调用0806字体
		else if(size1==12)
        {temp=asc2_1206[chr1][i];} //调用1206字体
		else if(size1==16)
        {temp=asc2_1608[chr1][i];} //调用1608字体
		else if(size1==24)
        {temp=asc2_2412[chr1][i];} //调用2412字体
		else return;
		for(m=0;m<8;m++)
		{
			if(temp&0x01)
				OLED_DrawPoint(x,y,mode);
			else 
				OLED_DrawPoint(x,y,!mode);
			temp>>=1;
			y++;
		}
		x++;
		if((size1!=8)&&((x-x0)==size1/2))
		{
			x=x0;
			y0=y0+8;
		}
		y=y0;
  }
}

//显示字符串
//x,y:起点坐标  
//size1:字体大小 
//*chr:字符串起始地址 
//mode:0,反色显示;1,正常显示
void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr,uint8_t size1,uint8_t mode)
{
	while((*chr>=' ')&&(*chr<='~'))//判断是不是非法字符!
	{
		OLED_ShowChar(x,y,*chr,size1,mode);
		if(size1==8)x+=6;
		else x+=size1/2;
		chr++;
  }
}

//m^n函数
uint32_t OLED_Pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)
		result*=m;    
	return result;
}	

//显示数字
//x,y :起点坐标
//num :要显示的数字
//len :数字的位数
//size:字体大小
//mode:0,反色显示;1,正常显示
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size1,uint8_t mode)
{
	uint8_t t,temp,m=0;
	if(size1==8)m=2;
	for(t=0;t<len;t++)
	{
		temp=(num/OLED_Pow(10,len-t-1))%10;
			if(temp==0)
			{
				OLED_ShowChar(x+(size1/2+m)*t,y,'0',size1,mode);
      }
			else 
			{
			  OLED_ShowChar(x+(size1/2+m)*t,y,temp+'0',size1,mode);
			}
  }
}

////坐标设置
//void OLED_Set_Pos(uint8_t x, uint8_t y) 
//{ 
//	OLED_WR_Byte(0xb0+y,OLED_CMD);
//	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
//	OLED_WR_Byte((x&0x0f)|0x01,OLED_CMD); 
//} 

////清除BMP图片
//void OLED_ClearBMP(uint8_t x0, uint8_t y0,uint8_t x1,uint8_t y1)
//{
//	
//	 uint8_t x,y;
//  
//  //if(y1%8==0) y1=y1/8;      
//  //else y1=y1/8+1;
//	
//	for(y=y0;y<y1;y++)
//	{
//		OLED_Set_Pos(x0,y);
//    for(x=x0;x<x0+x1;x++)
//		{      
//			OLED_WR_Byte(0,OLED_DATA);	    	
//		}
//	}
//}	
////显示BMP图片 调用OLED_DrawBMP(0,0,128,8,BMP1);最大图片
////x0,y0:显示起点坐标,x0(范围0～127),y0(范围0～7)*8
////x1,y1:显示图片长度和宽度,x1(范围0～127),y1(范围0～7)*8
////BMP[]:图片首地址
//void OLED_DrawBMP(uint8_t x0, uint8_t y0,uint8_t x1, uint8_t y1,const uint8_t BMP[])
//{ 	
//	 uint16_t j=0;
//	 uint8_t x,y;
//  
//  //if(y1%8==0) y1=y1/8;      
//  //else y1=y1/8+1;
//	
//	for(y=y0;y<y1;y++)
//	{
//		OLED_Set_Pos(x0,y);
//    for(x=x0;x<x0+x1;x++)
//		{      
//			OLED_WR_Byte(BMP[j++],OLED_DATA);	    	
//		}
//	}
//} 

//void OLED_ClearLine(uint8_t x)
//{	uint8_t y;
//	if(x==128)
//		x=0;
//	
//	x+=2;
//	
//	for(y=2;y<6;y++){
//	
//	OLED_WR_Byte(0xb0+y,OLED_CMD);
//	//列选择
//	
//	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
//	//OLED_WR_Byte((x&0x0f)|0x01,OLED_CMD); 	
//	OLED_WR_Byte((x&0x0f),OLED_CMD);
//	OLED_GRAM[x-2][y]=0;	
//	OLED_WR_Byte(OLED_GRAM[x-2][y],OLED_DATA);  	
//	}		
//}	



//-----------------------------------显示中文------------------
//uint32_t *getchinesecodepos(uint16_t ac)
//{   
//		uint16_t min,max,mid,midc;
//    min=0;
//    max=CHINESECHARNUMBER-1;
//    while(1)
//    {   if(max-min<2)
//        {   if(ChineseCode[min]==ac)
//                mid=min;
//            else if(ChineseCode[max]==ac)
//                mid=max;
//            else
//                mid=0;
//            break;
//        }
//        mid=(max+min)/2;
//        midc=ChineseCode[mid];
//        if(midc==ac)
//            break;
//        else if(midc>ac)
//            max=mid-1;
//        else
//            min=mid+1;
//    }
//    return (uint32_t*)(ChineseCharDot+mid*CHINESECHARDOTSIZE);
//}

//void Get_Code(uint8_t* dis_buf,uint32_t* address,uint8_t leng)
//{
//	uint8_t i;
//	for(i=0;i<leng/4;i++)  //32为单片机4个字节
//	{
//		*dis_buf=*address;
//		dis_buf++;
//		*dis_buf=*address>>8;
//		dis_buf++;
//		*dis_buf=*address>>16;
//		dis_buf++;
//		*dis_buf=*address>>24;
//		dis_buf++;
//		address++;
//	}
//}

//void LCD_Char_CH(uint8_t x,uint8_t y,uint8_t *str)
//{
//    uint8_t  i,buffer[CHINESECHARDOTSIZE]={0};
//		uint16_t  chinese_ascii;
//		uint32_t *p_address;
//		chinese_ascii = *str;
//		str++;
//		chinese_ascii = chinese_ascii<<8|*str; 		//找到汉字内码	
//		p_address=getchinesecodepos(chinese_ascii);										
//		Get_Code(buffer,p_address,32);  					// 取字模数据 
//		OLED_Set_Pos(x,y);	
//		for(i=0;i<CHINESECHARSIZE;i++)
//		OLED_WR_Byte(buffer[i],OLED_DATA);
//		OLED_Set_Pos(x,y+1);
//		for(i=0;i<CHINESECHARSIZE;i++)
//		OLED_WR_Byte(buffer[i+CHINESECHARSIZE],OLED_DATA);
//}

//// * 函数名：LCD_Str_CH diao
//// * 描述  ：在指定坐标处显示16*16大小汉字字符串
//// * 输入  : 	- x: 显示位置横向坐标	 
//// *         	- y: 显示位置纵向坐标 
//// *			- str: 显示的中文字符串
//// * 举例  ：	
//void LCD_Str_CHinese(uint8_t x,uint8_t y,uint8_t *str)  
//{   
//    while(*str != '\0')
//    {
//					LCD_Char_CH(x,y,str);      
//					str += 2 ;
//					x += 16 ;	
//    }
//}
//-----------------------------------------------------------------------------

//初始化SSD1306					    
void OLED_Init(void)
{ 	 	 
//如果gpio已经初始化了则除部分就不需要
// 	GPIO_InitTypeDef  GPIO_InitStructure;
// 	
//	GPIO_InitStructure.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
//	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
//	GPIO_InitStructure.Pull = GPIO_NOPULL;
//	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
//	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
//	
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
	
//	OLED_Power_On();
	
	OLED_RST_Clr();
	HAL_Delay(200);
	OLED_RST_Set(); 
					  
	OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel
	OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
	OLED_WR_Byte(0xCF,OLED_CMD);// Set SEG Output Current Brightness
	OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	OLED_WR_Byte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display
	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
	OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WR_Byte(0x00,OLED_CMD);//-not offset
	OLED_WR_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
	OLED_WR_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WR_Byte(0xD9,OLED_CMD);//--set pre-charge period
	OLED_WR_Byte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WR_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration
	OLED_WR_Byte(0x12,OLED_CMD);
	OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
	OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM Deselect Level
	OLED_WR_Byte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WR_Byte(0x02,OLED_CMD);//
	OLED_WR_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
	OLED_WR_Byte(0x14,OLED_CMD);//--set(0x10) disable
	OLED_WR_Byte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
	OLED_WR_Byte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7) 
	OLED_Clear();
	OLED_WR_Byte(0xAF,OLED_CMD);
}  



