#include "config.h"

#define EnableIAP 0x83;
#define ReadIAP 0x1;
#define WriteIAP 0x2;
#define EraseIAP 0x3;

/***********************************************************
* ��    �ƣ� eerprom д�뺯��
* ��    �ܣ�
* ��ڲ�����  
* ���ڲ�����
* ˵    ���� 					 
* ���÷����� 
**********************************************************/ 
/*void   EepromWrite(uint8_t Dat, uint32_t addr)
{
 char i=0;
 IAP_CONTR = EnableIAP;
 IAP_CMD   = WriteIAP;
 IAP_ADDRL = addr;
 IAP_ADDRH = addr>>8;
 IAP_DATA  = Dat;
 
 IAP_TRIG = 0x5A;
 IAP_TRIG = 0xA5;
 for(i=0;i<3;i++);

 DisbleEeprom();
}
*/
/***********************************************************
* ��    �ƣ� eerprom ���뺯��
* ��    �ܣ�
* ��ڲ�����  
* ���ڲ�����
* ˵    ���� 					 
* ���÷����� 
**********************************************************/ 
uint8_t  EePromRead(uint32_t addr)
{
  uint8_t i=0;
  uint8_t Dat=0;
  IAP_CONTR = EnableIAP;
  IAP_CMD   = ReadIAP;
  IAP_ADDRL = addr;
  IAP_ADDRH = addr>>8;

  IAP_TRIG = 0x5A;
  IAP_TRIG = 0xA5;
  for(i=0;i<3;i++);
  Dat = IAP_DATA;

  DisbleEeprom();
  return (Dat);
}

/***********************************************************
* ��    �ƣ� eerprom ��������
* ��    �ܣ�
* ��ڲ�����  
* ���ڲ�����
* ˵    ���� 					 
* ���÷����� 
**********************************************************/ 
#if 0
void   EePromErase(uint32_t addr)
{
 char i=0;
 IAP_CONTR = EnableIAP;
 IAP_CMD   = EraseIAP;
 IAP_ADDRL = addr;
 IAP_ADDRH = addr>>8;

 IAP_TRIG = 0x5A;
 IAP_TRIG = 0xA5;
 for(i=0;i<3;i++);

 DisbleEeprom();
}
#endif
/***********************************************************
* ��    �ƣ� eerprom disable����
* ��    �ܣ�
* ��ڲ�����  
* ���ڲ�����
* ˵    ���� 					 
* ���÷����� 
**********************************************************/ 
void   DisbleEeprom(void)
{
 IAP_CONTR = 0x00;
 IAP_CMD   = 0x00;
 IAP_TRIG  = 0x00;
 IAP_ADDRH = 0x00;
 IAP_ADDRL = 0x00;
}
