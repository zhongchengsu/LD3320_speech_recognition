#ifndef __USART_H
#define __USART_H


void UartIni(void);//���ڳ�ʼ��
void UARTSendByte(uint8_t DAT);	//���ڷ���һ�ֽ�����
void PrintHex(uint8_t *DAT, uint8_t len); //��ӡ�����ַ�������
void PrintChar(uint8_t *DAT); //��ӡ�ַ�


#endif
