#ifndef DS1302_H_
#define DS1302_H_

void DS1302_Init(void);
void DS1302_WriteByte(unsigned char Command, unsigned char Data);
unsigned char DS1302_ReadByte(unsigned char Command);
void DS1302_SetTime(unsigned char year, unsigned char month, unsigned char date, 
										unsigned char hour,unsigned char minute, unsigned char second);
void DS1302_ReadTime(void);

#endif 
