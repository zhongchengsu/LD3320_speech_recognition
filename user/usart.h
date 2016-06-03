#ifndef __USART_H
#define __USART_H


void UartIni(void);//串口初始化
void UARTSendByte(uint8_t DAT);	//串口发送一字节数据
void PrintHex(uint8_t *DAT, uint8_t len); //打印串口字符串数据
void PrintChar(uint8_t *DAT); //打印字符


#endif
