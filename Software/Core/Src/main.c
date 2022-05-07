/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern uint16_t Battery;//电池电压值
uint16_t ADC_ConvertedValue[4];//DMA存储数据的容器
uint16_t VoltageS[12][4];//阵列传感器的数据
volatile float pitch,roll,yaw;//欧拉角
uint8_t Row_list[4] = {1,0,3,2};//列顺序
//******************{7,6,8,5,9,4,10,3,11,2,12,1}//经过调试将前两列后最后，我也不知道为啥，但是work了！！
uint8_t Row_A[12] = {1,0,0,1,1,0,0, 1,1, 0,0, 1};
uint8_t Row_B[12] = {1,1,0,0,0,0,1, 1,1, 1,0, 0};
uint8_t Row_C[12] = {1,1,0,1,0,1,0, 0,0, 0,1, 0};
uint8_t Row_D[12] = {0,0,1,0,1,0,1, 0,1, 0,1, 0};//1~12
uint8_t MPU6050_Data_OK;//MPU6050数据可用标志位
uint8_t uart_buf_bat[2] = {0x00, 0x00};//串口发送电池电压容器
uint8_t uart_buf_v[2] = {0x00, 0x00};//串口发送阵列电压容器
uint8_t uart_buf_imu[3] = {0x00, 0x00, 0x00};//串口发送陀螺仪数据容器
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Adc_Battery(void);
void DMA_Adc_collect(void);
void delay_us(uint32_t us);
void MPU6050_Get_Data(void);
void OLED_Show(void);
void Data_upload(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC2_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_I2C2_Init();
  MX_USART1_UART_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start_IT(&htim1);
	
	HAL_ADCEx_Calibration_Start(&hadc1);//开启ADC校准
	HAL_ADCEx_Calibration_Start(&hadc2);
	
	OLED_Init();
	OLED_ColorTurn(0);//0正常显示，1 反色显示
  OLED_DisplayTurn(0);//0正常显示 1 屏幕翻转显示
	OLED_Refresh();

	MPU_Init();//初始化MPU6050
	
	while(mpu_dmp_init())
	{
	}//等待mpu_dmp初始化
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		DMA_Adc_collect();//获取压力传感器电压
		MPU6050_Get_Data();//获取陀螺仪数据
		OLED_Show();//数据展示
		Data_upload();//数据上传
		
		HAL_GPIO_TogglePin(GPIOA,Run_Pin);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
//=============================================================================
//函数名称：Adc_Battery()
//功能概要：读取电池电压值
//参数说明：无
//函数返回：无
//=============================================================================
void Adc_Battery(void)
{
	HAL_ADC_Start(&hadc2);//启动ADC装换
	HAL_ADC_PollForConversion(&hadc2, 5);//等待转换完成，第二个参数表示超时时间，单位ms.
	if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc2), HAL_ADC_STATE_REG_EOC))
	{
		Battery = HAL_ADC_GetValue(&hadc2);//读取ADC转换数据，数据为12位
		Battery = (float)Battery*(3.300/4096)*1000;
		Battery = Battery*4.0;
	}
}

//=============================================================================
//函数名称：DMA_Adc_collect()
//功能概要：读取DMA_AD值
//参数说明：无
//函数返回：无
//=============================================================================
void DMA_Adc_collect(void)
{
	uint8_t r,i,m;
	
	for(r=0; r<12; r++)//行
	{
		HAL_GPIO_WritePin(GPIOA, R_A_Pin, Row_A[0]);
		HAL_GPIO_WritePin(GPIOA, R_B_Pin, Row_B[0]);
		HAL_GPIO_WritePin(GPIOA, R_C_Pin, Row_C[0]);
		HAL_GPIO_WritePin(GPIOA, R_D_Pin, Row_D[0]);
		delay_us(50);
		HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&ADC_ConvertedValue, 4);
		delay_us(50);
		for(i=0;i<4;i++)
		{
			m = Row_list[i];//列顺序
			VoltageS[r][m]=(uint16_t)((float)ADC_ConvertedValue[i]/4096*3.300*1000)%10000;
		}
	}
}

//=============================================================================
//函数名称：delay_us()
//功能概要：延时函数（单位us）
//参数说明：无
//函数返回：无
//=============================================================================
void delay_us(uint32_t us)
{
    uint32_t delay = (HAL_RCC_GetHCLKFreq() / 4000000 * us);
    while (delay--)
	{
		;
	}
}

//=============================================================================
//函数名称：MPU6050_Get_Data()
//功能概要：陀螺仪数据读取
//参数说明：无
//函数返回：无
//=============================================================================
void MPU6050_Get_Data(void)
{
	if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)
	{ 
		MPU6050_Data_OK = 1;
		uart_buf_imu[0] = pitch;
		uart_buf_imu[1] = roll;
		uart_buf_imu[2] = yaw;
	}
	else
	{
		MPU6050_Data_OK = 0;
	}
}	

//=============================================================================
//函数名称：OLED_Show()
//功能概要：OLED展示
//参数说明：无
//函数返回：无
//=============================================================================
void OLED_Show(void)
{
	OLED_ShowString(0,0,"Battery",8,1);
	OLED_ShowNum(1,8,Battery,4,8,1);
	OLED_ShowNum(1,16,VoltageS[1][0],4,8,1);
	OLED_ShowNum(1,24,VoltageS[1][1],4,8,1);
	OLED_ShowNum(1,32,VoltageS[1][2],4,8,1);
	OLED_ShowNum(1,40,VoltageS[1][3],4,8,1);
	
	if(MPU6050_Data_OK)
	{
		OLED_ShowString(50,0,"IMU",8,1);
		OLED_ShowNum(50,8,pitch,4,8,1);
		OLED_ShowNum(50,16,roll,4,8,1);
		OLED_ShowNum(50,24,yaw,4,8,1);
	}
		
	OLED_Refresh();
}	

//=============================================================================
//函数名称：Data_upload()
//功能概要：将数据通过串口上传到上位机
//参数说明：无
//函数返回：无
//=============================================================================
void Data_upload(void)
{
	uint8_t i, j;
	
	uart_buf_bat[0] = Battery>>8;//数据高位
	uart_buf_bat[1] = Battery&0xff;//数据低位
	HAL_UART_Transmit(&huart1, (uint8_t *)uart_buf_bat, 2, 1);//上传电池电压数据
	
	HAL_UART_Transmit(&huart1, (uint8_t *)uart_buf_imu, 3, 1);//上传陀螺仪数据
	
	for(i=0; i<4; i++)//列
	{
		for(j=0; j<12; j++)//行
		{
			uart_buf_v[0] = VoltageS[j][i]>>8;//数据高位
			uart_buf_v[0] = VoltageS[j][i]&0xff;//数据低位
			HAL_UART_Transmit(&huart1, (uint8_t *)uart_buf_v, 2, 1);//上传阵列数据
		}
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

