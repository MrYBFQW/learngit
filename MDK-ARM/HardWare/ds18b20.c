#include "stm32f1xx_hal.h"
#include "delay.h"

#define SkipRom             				 0xCC /* 因为我们只操控一个DS18B20,不需要进行rom匹配,跳过这一步指令 */
#define ConvertTemperature  				 0x44 /* 启动DS18B20进行温度转换指令,结果（温度）存入RAM */
#define ReadScratchpad      				 0xBE /* 读取读取RAM中9个字节的内容(读取温度) */
#define DS18B20_GPIO_Port            GPIOA
#define DS18B20_Pin   							 GPIO_PIN_15
#define DS18B20_Set(x) 							 HAL_GPIO_WritePin(DS18B20_GPIO_Port, DS18B20_Pin, (GPIO_PinState)(x))
#define DS18B20_Get()	 							 HAL_GPIO_ReadPin(DS18B20_GPIO_Port, DS18B20_Pin)


unsigned char OneWireInit(void)/* 单总线初始化 */
{
	unsigned char ACK;
	DS18B20_Set(1);
	DS18B20_Set(0);   		   /* 主机将总线拉低 */
	delay_us(500);   /* 500us(拉低过程最少480us) */
	DS18B20_Set(1);            /*主机释放主线*/
	delay_us(70);   	/*从机等待(15~60us)70us*/
	/*从机自己拉低主线,以响应主机，*/
	ACK = DS18B20_Get(); /*读取总线电平,如果是0,说明从机的确响应主机了,如果是1，则是未响应*/
	delay_us(500);   /*(从机拉低过程60~240us)500us*/
	/*之后从机自己将释放总线*/
	return ACK;/*返回值*/
}

/* 主机发送一位(1/8个字节)数据给从机 */
void OneWireSendOne(unsigned char byte)
{
	DS18B20_Set(1); 
	DS18B20_Set(0);/* 主机拉低总线 */
	delay_us(10);       /* (延时10us)从机将在主机拉低总线后15~60us内读取数据 */
	DS18B20_Set(byte);          /*主机发送数据到总线，从机读取 */
	/* 如果发送0，后面的50us内总线都是0；最后拉高 */
	/* 如果发送1，则后面50us内总线都是1，并且顺带释放 */
	delay_us(50);        /* 50us */
	DS18B20_Set(1);      /* 主机释放总线 */
}

/*主机读取从机发来的一位（1/8个字节）数据*/
unsigned char OneWireReceiveOne(void)
{
	unsigned char byte;
	DS18B20_Set(1); 
	DS18B20_Set(0);/* 主机拉低总线 */
	delay_us(5);       /* 5us(主机拉低总线最少1us,) */
	DS18B20_Set(1);           /*主机必须释放总线给从机，让从机放入数据 */
/* 若从机发送1，则保持总线为高电平,若发送 0,则从机拉低总线 */
	delay_us(5);       /* 5us */
	byte = DS18B20_Get();       /* 主机读取从机放入总线的数据(主机在自己拉低总线后15us内必须读取完数据) */
	delay_us(50);        /* 50us(满足整个过程要大于60us) */
	return byte;   /* 结束后从机会自动释放总线，所以结束后OneWire = 1 */
}

/* 主机发送八位数据(一个字节)给从机 */
void OneWireSendByte(unsigned char byte)
{
	unsigned char i;
	for(i = 0; i < 8; i++)/* 8次循环，每次循环对应一位数据 */
	{
		OneWireSendOne( byte & ( 0x01<<i ) );
	}
}

/* 主机接收从机发送的八位数据(一个字节) */
unsigned char OneWireReceiveByte(void)
{
	unsigned char i;
	unsigned char byte = 0x00;
	for(i = 0; i < 8; i++)/* 8次循环，每次循环对应一位数据 */
	{
		if( OneWireReceiveOne() )/* 对应每一位数据，如果该位数据是1 */
		{
			byte = byte | (0x01<<i);/* 赋值到该位为1 */
		}/* 否则就是默认的0x00 */
	}
	return byte;/*返回*/
}

void DS18B20ConvertTemperature(void)/* 温度转换函数 */
{ 
	OneWireInit();/* 初始化 */
	OneWireSendByte( SkipRom );
	OneWireSendByte( ConvertTemperature );
	delay_ms(750);
}
float DS18B20ReadTemperature(void)
{
	unsigned char TemperatureLSB, TemperatureMSB;
	float FinalTemperature;
	int Change;
	OneWireInit();/* 初始化 */
	OneWireSendByte( SkipRom );
	OneWireSendByte( ReadScratchpad );
/* 读取温度，是一个转换完成后的温度值就以16位带符号扩展的二进制补码形式 */
	TemperatureLSB = OneWireReceiveByte();/*第一次读取是低八位字节，低四位表示小数 */
	TemperatureMSB = OneWireReceiveByte();/*第二次读取是高八位字节，前五位是补码 */
	Change = (TemperatureMSB << 8) | TemperatureLSB;/*拼接成一个16位数据 */
/* 为了让最低位表示小数，最低位变成了2^-4,正常应该是2^0的 */
	FinalTemperature = Change / 16.0;/* 相当于扩大了16倍，所以要除以16.0(float) */
	return FinalTemperature;
}
