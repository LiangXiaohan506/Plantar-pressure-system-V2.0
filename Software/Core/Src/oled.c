
//  ��������   : 0.96Ӣ��OLED��������
//              GND    ��Դ��
//              VCC  ��5V��3.3v��Դ
//              D0   ��    PB7
//              D1   ��    PB6
//              RES  ��    PB5
//              DC   ��    PB1
//              CS   ��    PB3
		   

#include "oled.h"
#include "oledfont.h"  

uint8_t OLED_GRAM[127][8];

//���ʹ�õ���ģ������Ҫ
//void OLED_Power_On(void)
//{
//	OLED_PWR_Clr();
//}
//void OLED_Power_Off(void)
//{
//	OLED_PWR_Set();
//}

//��SSD1106д��һ���ֽڡ�
//dat:Ҫд�������/����
//cmd:����/�����־ 0,��ʾ����;1,��ʾ����;
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

//���Ժ���
void OLED_ColorTurn(uint8_t i)
{
	if(i==0)
		{
			OLED_WR_Byte(0xA6,OLED_CMD);//������ʾ
		}
	if(i==1)
		{
			OLED_WR_Byte(0xA7,OLED_CMD);//��ɫ��ʾ
		}
}

//��Ļ��ת180��
void OLED_DisplayTurn(uint8_t i)
{
	if(i==0)
		{
			OLED_WR_Byte(0xC8,OLED_CMD);//������ʾ
			OLED_WR_Byte(0xA1,OLED_CMD);
		}
	if(i==1)
		{
			OLED_WR_Byte(0xC0,OLED_CMD);//��ת��ʾ
			OLED_WR_Byte(0xA0,OLED_CMD);
		}
}

//����OLED��ʾ    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}

//�ر�OLED��ʾ     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}	

//�����Դ浽OLED	
void OLED_Refresh(void)
{
	uint8_t i,n;
	for(i=0;i<8;i++)
	{
	   OLED_WR_Byte(0xb0+i,OLED_CMD); //��������ʼ��ַ
	   OLED_WR_Byte(0x00,OLED_CMD);   //���õ�����ʼ��ַ
	   OLED_WR_Byte(0x10,OLED_CMD);   //���ø�����ʼ��ַ
	   for(n=0;n<128;n++)
				OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA);
  }
}

//��������
void OLED_Clear(void)
{
	uint8_t i,n;
	for(i=0;i<8;i++)
	{
	   for(n=0;n<128;n++)
			{
			 OLED_GRAM[n][i]=0;//�����������
			}
  }
	OLED_Refresh();//������ʾ
}

//���� 
//x:0~127
//y:0~63
//t:1 ��� 0,���	
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

//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//size1:ѡ������ 6x8/6x12/8x16/12x24
//mode:0,��ɫ��ʾ;1,������ʾ
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size1,uint8_t mode)
{
	uint8_t i,m,temp,size2,chr1;
	uint8_t x0=x,y0=y;
	if(size1==8)
		size2=6;
	else 
		size2=(size1/8+((size1%8)?1:0))*(size1/2);  //�õ�����һ���ַ���Ӧ������ռ���ֽ���
	chr1=chr-' ';  //����ƫ�ƺ��ֵ
	for(i=0;i<size2;i++)
	{
		if(size1==8)
			  {temp=asc2_0806[chr1][i];} //����0806����
		else if(size1==12)
        {temp=asc2_1206[chr1][i];} //����1206����
		else if(size1==16)
        {temp=asc2_1608[chr1][i];} //����1608����
		else if(size1==24)
        {temp=asc2_2412[chr1][i];} //����2412����
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

//��ʾ�ַ���
//x,y:�������  
//size1:�����С 
//*chr:�ַ�����ʼ��ַ 
//mode:0,��ɫ��ʾ;1,������ʾ
void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr,uint8_t size1,uint8_t mode)
{
	while((*chr>=' ')&&(*chr<='~'))//�ж��ǲ��ǷǷ��ַ�!
	{
		OLED_ShowChar(x,y,*chr,size1,mode);
		if(size1==8)x+=6;
		else x+=size1/2;
		chr++;
  }
}

//m^n����
uint32_t OLED_Pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)
		result*=m;    
	return result;
}	

//��ʾ����
//x,y :�������
//num :Ҫ��ʾ������
//len :���ֵ�λ��
//size:�����С
//mode:0,��ɫ��ʾ;1,������ʾ
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

////��������
//void OLED_Set_Pos(uint8_t x, uint8_t y) 
//{ 
//	OLED_WR_Byte(0xb0+y,OLED_CMD);
//	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
//	OLED_WR_Byte((x&0x0f)|0x01,OLED_CMD); 
//} 

////���BMPͼƬ
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
////��ʾBMPͼƬ ����OLED_DrawBMP(0,0,128,8,BMP1);���ͼƬ
////x0,y0:��ʾ�������,x0(��Χ0��127),y0(��Χ0��7)*8
////x1,y1:��ʾͼƬ���ȺͿ��,x1(��Χ0��127),y1(��Χ0��7)*8
////BMP[]:ͼƬ�׵�ַ
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
//	//��ѡ��
//	
//	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
//	//OLED_WR_Byte((x&0x0f)|0x01,OLED_CMD); 	
//	OLED_WR_Byte((x&0x0f),OLED_CMD);
//	OLED_GRAM[x-2][y]=0;	
//	OLED_WR_Byte(OLED_GRAM[x-2][y],OLED_DATA);  	
//	}		
//}	



//-----------------------------------��ʾ����------------------
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
//	for(i=0;i<leng/4;i++)  //32Ϊ��Ƭ��4���ֽ�
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
//		chinese_ascii = chinese_ascii<<8|*str; 		//�ҵ���������	
//		p_address=getchinesecodepos(chinese_ascii);										
//		Get_Code(buffer,p_address,32);  					// ȡ��ģ���� 
//		OLED_Set_Pos(x,y);	
//		for(i=0;i<CHINESECHARSIZE;i++)
//		OLED_WR_Byte(buffer[i],OLED_DATA);
//		OLED_Set_Pos(x,y+1);
//		for(i=0;i<CHINESECHARSIZE;i++)
//		OLED_WR_Byte(buffer[i+CHINESECHARSIZE],OLED_DATA);
//}

//// * ��������LCD_Str_CH diao
//// * ����  ����ָ�����괦��ʾ16*16��С�����ַ���
//// * ����  : 	- x: ��ʾλ�ú�������	 
//// *         	- y: ��ʾλ���������� 
//// *			- str: ��ʾ�������ַ���
//// * ����  ��	
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

//��ʼ��SSD1306					    
void OLED_Init(void)
{ 	 	 
//���gpio�Ѿ���ʼ����������־Ͳ���Ҫ
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
	OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����
	OLED_WR_Byte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0���·��� 0xc8����
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



