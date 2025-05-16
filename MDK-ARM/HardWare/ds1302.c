#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "Time_Task.h"

extern Task_Timer Time_Now;
//寄存器写入地址/指令定义
#define DS1302_SECOND	               0x80
#define DS1302_MINUTE	               0x82
#define DS1302_HOUR		               0x84
#define DS1302_DATE		               0x86
#define DS1302_MONTH	               0x88
#define DS1302_DAY		               0x8A
#define DS1302_YEAR		               0x8C
#define DS1302_WP			               0x8E
#define input_mode 									 1
#define output_mode 								 0
#define ds1302_io_Port     					 GPIOA
#define ds1302_io_Pin     					 GPIO_PIN_9

void ds1302_io_work(unsigned char function)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(ds1302_io_Port == GPIOA)	__HAL_RCC_GPIOA_CLK_ENABLE();
	else __HAL_RCC_GPIOB_CLK_ENABLE();
	if(function == input_mode)
	{
		GPIO_InitStruct.Pin = ds1302_io_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(ds1302_io_Port, &GPIO_InitStruct);
	}
	else if(function == output_mode)
	{
		HAL_GPIO_WritePin(ds1302_io_Port, ds1302_io_Pin, GPIO_PIN_RESET);
		GPIO_InitStruct.Pin = ds1302_io_Pin;   
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(ds1302_io_Port, &GPIO_InitStruct);		
	}
}
//时间数组，索引0~6分别为年、月、日、时、分、秒、星期
unsigned char DS1302_Time[]={25,5,6,19,20,0,2};

/**
  * @brief  DS1302初始化
  * @param  无
  * @retval 无
  */
void DS1302_Init(void)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
}

/**
  * @brief  DS1302写一个字节
  * @param  Command 命令字/地址
  * @param  Data 要写入的数据
  * @retval 无
  */
void DS1302_WriteByte(unsigned char Command, unsigned char Data)
{
	unsigned char i;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
	ds1302_io_work(output_mode);
	for(i=0;i<8;i++)
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, (GPIO_PinState)(Command&(0x01<<i)) );
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
	}
	for(i=0;i<8;i++)
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, (GPIO_PinState)(Data&(0x01<<i)) );
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
	}
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
}

/**
  * @brief  DS1302读一个字节
  * @param  Command 命令字/地址
  * @retval 读出的数据
  */
unsigned char DS1302_ReadByte(unsigned char Command)
{
	unsigned char i,Data=0x00;
	Command|=0x01;	//将指令转换为读指令
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
	for(i=0;i<8;i++)
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, (GPIO_PinState)(Command&(0x01<<i)) );
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
	}
	for(i=0;i<8;i++)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
		ds1302_io_work(input_mode);
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9) == 1){Data|=(0x01<<i);}
	}
	ds1302_io_work(output_mode);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);	//读取后将IO设置为0，否则读出的数据会出错
	return Data;
}

/**
  * @brief  DS1302设置时间，调用之后，DS1302_Time数组的数字会被设置到DS1302中
  * @param  无
  * @retval 无
  */
void DS1302_SetTime(unsigned char year, unsigned char month, unsigned char date, 
										unsigned char hour,unsigned char minute, unsigned char second)
{
	DS1302_WriteByte(DS1302_WP,0x00);
	DS1302_WriteByte(DS1302_YEAR,   year/10*16   + year%10);//十进制转BCD码后写入
	DS1302_WriteByte(DS1302_MONTH,  month/10*16  + month%10);
	DS1302_WriteByte(DS1302_DATE,   date/10*16   + date%10);
	DS1302_WriteByte(DS1302_HOUR,   hour/10*16   + hour%10);
	DS1302_WriteByte(DS1302_MINUTE, minute/10*16 + minute%10);
	DS1302_WriteByte(DS1302_SECOND, second/10*16 + second%10);
	DS1302_WriteByte(DS1302_WP,0x80);
}

/**
  * @brief  DS1302读取时间，调用之后，DS1302中的数据会被读取到DS1302_Time数组中
  * @param  无
  * @retval 无
  */
void DS1302_ReadTime(void)
{
	unsigned char Temp;
	Temp = DS1302_ReadByte(DS1302_YEAR);
	Time_Now.year = Temp/16*10 + Temp%16;//BCD码转十进制后读取
	
	Temp = DS1302_ReadByte(DS1302_MONTH);
	Time_Now.month = Temp/16*10 + Temp%16;
	
	Temp = DS1302_ReadByte(DS1302_DATE);
	Time_Now.date = Temp/16*10 + Temp%16;
	
	Temp = DS1302_ReadByte(DS1302_HOUR);
	Time_Now.hour = Temp/16*10 + Temp%16;
	
	Temp = DS1302_ReadByte(DS1302_MINUTE);
	Time_Now.minute = Temp/16*10 + Temp%16;
	
	Temp = DS1302_ReadByte(DS1302_SECOND);
	Time_Now.second = Temp/16*10 + Temp%16;
	
}
