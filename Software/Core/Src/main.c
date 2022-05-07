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
extern uint16_t Battery;//��ص�ѹֵ
uint16_t ADC_ConvertedValue[4];//DMA�洢���ݵ�����
uint16_t VoltageS[12][4];//���д�����������
volatile float pitch,roll,yaw;//ŷ����
uint8_t Row_list[4] = {1,0,3,2};//��˳��
//******************{7,6,8,5,9,4,10,3,11,2,12,1}//�������Խ�ǰ���к������Ҳ��֪��Ϊɶ������work�ˣ���
uint8_t Row_A[12] = {1,0,0,1,1,0,0, 1,1, 0,0, 1};
uint8_t Row_B[12] = {1,1,0,0,0,0,1, 1,1, 1,0, 0};
uint8_t Row_C[12] = {1,1,0,1,0,1,0, 0,0, 0,1, 0};
uint8_t Row_D[12] = {0,0,1,0,1,0,1, 0,1, 0,1, 0};//1~12
uint8_t MPU6050_Data_OK;//MPU6050���ݿ��ñ�־λ
uint8_t uart_buf_bat[2] = {0x00, 0x00};//���ڷ��͵�ص�ѹ����
uint8_t uart_buf_v[2] = {0x00, 0x00};//���ڷ������е�ѹ����
uint8_t uart_buf_imu[3] = {0x00, 0x00, 0x00};//���ڷ�����������������
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
	
	HAL_ADCEx_Calibration_Start(&hadc1);//����ADCУ׼
	HAL_ADCEx_Calibration_Start(&hadc2);
	
	OLED_Init();
	OLED_ColorTurn(0);//0������ʾ��1 ��ɫ��ʾ
  OLED_DisplayTurn(0);//0������ʾ 1 ��Ļ��ת��ʾ
	OLED_Refresh();

	MPU_Init();//��ʼ��MPU6050
	
	while(mpu_dmp_init())
	{
	}//�ȴ�mpu_dmp��ʼ��
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		DMA_Adc_collect();//��ȡѹ����������ѹ
		MPU6050_Get_Data();//��ȡ����������
		OLED_Show();//����չʾ
		Data_upload();//�����ϴ�
		
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
//�������ƣ�Adc_Battery()
//���ܸ�Ҫ����ȡ��ص�ѹֵ
//����˵������
//�������أ���
//=============================================================================
void Adc_Battery(void)
{
	HAL_ADC_Start(&hadc2);//����ADCװ��
	HAL_ADC_PollForConversion(&hadc2, 5);//�ȴ�ת����ɣ��ڶ���������ʾ��ʱʱ�䣬��λms.
	if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc2), HAL_ADC_STATE_REG_EOC))
	{
		Battery = HAL_ADC_GetValue(&hadc2);//��ȡADCת�����ݣ�����Ϊ12λ
		Battery = (float)Battery*(3.300/4096)*1000;
		Battery = Battery*4.0;
	}
}

//=============================================================================
//�������ƣ�DMA_Adc_collect()
//���ܸ�Ҫ����ȡDMA_ADֵ
//����˵������
//�������أ���
//=============================================================================
void DMA_Adc_collect(void)
{
	uint8_t r,i,m;
	
	for(r=0; r<12; r++)//��
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
			m = Row_list[i];//��˳��
			VoltageS[r][m]=(uint16_t)((float)ADC_ConvertedValue[i]/4096*3.300*1000)%10000;
		}
	}
}

//=============================================================================
//�������ƣ�delay_us()
//���ܸ�Ҫ����ʱ��������λus��
//����˵������
//�������أ���
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
//�������ƣ�MPU6050_Get_Data()
//���ܸ�Ҫ�����������ݶ�ȡ
//����˵������
//�������أ���
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
//�������ƣ�OLED_Show()
//���ܸ�Ҫ��OLEDչʾ
//����˵������
//�������أ���
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
//�������ƣ�Data_upload()
//���ܸ�Ҫ��������ͨ�������ϴ�����λ��
//����˵������
//�������أ���
//=============================================================================
void Data_upload(void)
{
	uint8_t i, j;
	
	uart_buf_bat[0] = Battery>>8;//���ݸ�λ
	uart_buf_bat[1] = Battery&0xff;//���ݵ�λ
	HAL_UART_Transmit(&huart1, (uint8_t *)uart_buf_bat, 2, 1);//�ϴ���ص�ѹ����
	
	HAL_UART_Transmit(&huart1, (uint8_t *)uart_buf_imu, 3, 1);//�ϴ�����������
	
	for(i=0; i<4; i++)//��
	{
		for(j=0; j<12; j++)//��
		{
			uart_buf_v[0] = VoltageS[j][i]>>8;//���ݸ�λ
			uart_buf_v[0] = VoltageS[j][i]&0xff;//���ݵ�λ
			HAL_UART_Transmit(&huart1, (uint8_t *)uart_buf_v, 2, 1);//�ϴ���������
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

