/***************************�����Ƶ���****************************
**  �������ƣ�YS-V0.7����ʶ��ģ����������
**	CPU: STC11L08XE
**	����22.1184MHZ
**	�����ʣ�9600 bit/S
**	���ײ�Ʒ��Ϣ��YS-V0.7����ʶ�𿪷���
**                http://yuesheng001.taobao.com
**  ���ߣ�zdings
**  ��ϵ��751956552@qq.com
**  �޸����ڣ�2013.9.13
**  ˵��������ģʽ�� ��ÿ��ʶ��ʱ����Ҫ˵��С�ܡ�������� �����ܹ�������һ����ʶ��
/***************************�����Ƶ���******************************/
#include "config.h"

/************************************************************************************/
//	nAsrStatus ������main�������б�ʾ�������е�״̬������LD3320оƬ�ڲ���״̬�Ĵ���
//	LD_ASR_NONE:		��ʾû������ASRʶ��
//	LD_ASR_RUNING��		��ʾLD3320������ASRʶ����
//	LD_ASR_FOUNDOK:		��ʾһ��ʶ�����̽�������һ��ʶ����
//	LD_ASR_FOUNDZERO:	��ʾһ��ʶ�����̽�����û��ʶ����
//	LD_ASR_ERROR:		��ʾһ��ʶ��������LD3320оƬ�ڲ����ֲ���ȷ��״̬
/***********************************************************************************/
uint8 idata nAsrStatus=0;	
uint8 idata nDemoFlag=0;
void MCU_init(); 
void ProcessInt0(); //ʶ������
void User_handle(uint8 dat);//�û�ִ�в�������
void Delay1s();

void Led_test(void);//��Ƭ������ָʾ
uint8_t G0_flag=DISABLE;//���б�־��ENABLE:���С�DISABLE:��ֹ���� 
sbit LED=P4^2;//�ź�ָʾ��

#define copy_cmd(cmd, sub_cmd) \
{\
	cmd[11] = sub_cmd[0]; \
  cmd[12] = sub_cmd[1]; \
  cmd[13] = sub_cmd[2]; \
  cmd[14] = sub_cmd[3]; \
  cmd[15] = sub_cmd[4]; \
}

/***********************************************************
* ��    �ƣ� void  main(void)
* ��    �ܣ� ������	�������
* ��ڲ�����  
* ���ڲ�����
* ˵    ���� 					 
* ���÷����� 
**********************************************************/ 
void  main(void)
{
	uint8 idata nAsrRes;
	uint32_t i=0;
	uint8_t eeprom=0x11;

	Led_test();
	MCU_init();
	LD_Reset();
	UartIni(); /*���ڳ�ʼ��*/
	Sound_File_Parm_Init();
	CheckIRStatus();	
	nAsrStatus = LD_ASR_NONE;		//	��ʼ״̬��û������ASR

	while (1)
	{
		//�ȴ����⴫�������������˿���
		while (1)
		{
			if (IR_PIN == 1)
			{
				IR_STATUS = IR_IN;
				break;
			}
			
		}
		
		while(1)
		{	
			if (IR_STATUS == IR_OUT)
			{
				break;
			}
			switch(nAsrStatus)
			{
				case LD_ASR_RUNING:
				case LD_ASR_ERROR:
					break;
				case LD_ASR_NONE:
				{
					nAsrStatus=LD_ASR_RUNING;
					if (RunASR()==0)	//	����һ��ASRʶ�����̣�ASR��ʼ����ASR��ӹؼ��������ASR����
					{
						nAsrStatus = LD_ASR_ERROR;
					}
					break;
				}
				case LD_ASR_FOUNDOK: //	һ��ASRʶ�����̽�����ȥȡASRʶ����
				{	
					nAsrRes = LD_GetResult();		//��ȡ���
					User_handle(nAsrRes);//�û�ִ�к��� 
					nAsrStatus = LD_ASR_NONE;
					break;
				}
				case LD_ASR_FOUNDZERO:
				default:
				{
					nAsrStatus = LD_ASR_NONE;
					G0_flag=DISABLE;
					LED=1;
					//PrintChar("������\r\n");
					break;
				}
			}// switch		
		}// while
  }

}
/***********************************************************
* ��    �ƣ� 	 LED�Ʋ���
* ��    �ܣ� ��Ƭ���Ƿ���ָʾ
* ��ڲ����� �� 
* ���ڲ�������
* ˵    ���� 					 
**********************************************************/
void Led_test(void)
{
	LED=~ LED;
	Delay200ms();
	LED=~ LED;
	Delay200ms();
	LED=~ LED;
	Delay200ms();
	LED=~ LED;
	Delay200ms();
	LED=~ LED;
	Delay200ms();
	LED=~ LED;
}
/***********************************************************
* ��    �ƣ� void MCU_init()
* ��    �ܣ� ��Ƭ����ʼ��
* ��ڲ�����  
* ���ڲ�����
* ˵    ���� 					 
* ���÷����� 
**********************************************************/ 
void MCU_init()
{
	P0 = 0xff;
	P1 = 0xff;
	P2 = 0xff;
	P3 = 0xff;
	P4 = 0xff;

	LD_MODE = 0;		//	����MD�ܽ�Ϊ�ͣ�����ģʽ��д
	IE0=1;
	EX0=1;
	
	IT1 = 1; //�½��Ӵ���
	EX1 = 1; // INT1ʹ�ܣ��û������Ӧ
	EA=1;
	
	
}

void Delay200ms()		//@22.1184MHz
{
	unsigned char i, j, k;

	i = 17;
	j = 208;
	k = 27;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

void Delay1s()
{
	Delay200ms();
	Delay200ms();
	Delay200ms();
	Delay200ms();
	Delay200ms();
}
/***********************************************************
* ��    �ƣ� �жϴ�����
* ��    �ܣ�
* ��ڲ�����  
* ���ڲ�����
* ˵    ���� 					 
* ���÷����� 
**********************************************************/ 
void ExtInt0Handler(void) interrupt 0  
{ 	
	ProcessInt0();				
}


/***********************************************************
* ��    �ƣ� �жϴ����� �����жϺ���
* ��    �ܣ�
* ��ڲ�����  
* ���ڲ�����
* ˵    ���� 					 
* ���÷����� 
**********************************************************/ 
void ExtInt1Handler(void) interrupt 1  
{ 	
	ProcessIR();				
}


/***********************************************************
* ��    �ƣ��û�ִ�к��� 
* ��    �ܣ�ʶ��ɹ���ִ�ж������ڴ˽����޸� 
* ��ڲ����� �� 
* ���ڲ�������
* ˵    ���� 		
						LED=1;
            copy_cmd(cmd, dan_tuo_shui);						
						PrintHex(cmd); 
						break;

AA 0F DB 00 00 00 00 00 00 02 02 00 00 00 00 12 �ػ�
AA 0F DB 00 00 00 00 00 00 02 02 01 00 00 00 11 ����
AA 0F DB 00 00 00 00 00 00 02 02 01 01 00 FF 11 ����
AA 0F DB 00 00 00 00 00 00 02 02 01 00 00 FF 12 ��ͣ
AA 0F DB 00 00 00 00 00 00 02 02 01 00 00 00 11 ����
AA 0F DB 00 00 00 00 00 00 02 02 01 00 00 03 0E ���
AA 0F DB 00 00 00 00 00 00 02 02 01 00 00 05 0C ��ë
AA 0F DB 00 00 00 00 00 00 02 02 01 00 00 0F 02 ���޷�
AA 0F DB 00 00 00 00 00 00 02 02 01 00 00 02 0F ��ϴ
AA 0F DB 00 00 00 00 00 00 02 02 01 00 00 01 10 ����
AA 0F DB 00 00 00 00 00 00 02 02 01 00 00 09 08 ����ˮ
AA 0F DB 00 00 00 00 00 00 02 02 01 00 00 07 0A Ͳ�Խ�
**********************************************************/
void 	User_handle(uint8 dat)
{
     uint8_t cmd[16]={0xAA, 0x0F, 0xDB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x12};	
		 uint8_t guan_ji[5]=      {0x00, 0xff, 0xff, 0xff, 0x15};
		 uint8_t kai_ji[5]=       {0x01, 0xff, 0xff, 0xff, 0x14};
	   uint8_t qi_dong[5]=      {0xff, 0x01, 0xff, 0xff, 0x14};
	   uint8_t zan_ting[5]=     {0xff, 0x00, 0xff, 0xff, 0x15};
	   uint8_t mian_ma[5]=      {0xff, 0xff, 0xff, 0x00, 0x15};
	   uint8_t hun_he[5]=       {0xff, 0xff, 0xff, 0x03, 0x12};
	   uint8_t yang_mao[5]=     {0xff, 0xff, 0xff, 0x05, 0x10};
	   uint8_t yu_rong_fu[5]=   {0xff, 0xff, 0xff, 0x0F, 0x06};
	   uint8_t kuai_xi[5]=      {0xff, 0xff, 0xff, 0x02, 0x13};
	   uint8_t jie_neng[5]=     {0xff, 0xff, 0xff, 0x01, 0x14};
     uint8_t dan_tuo_shui[5]= {0xff, 0xff, 0xff, 0x09, 0x0c};
     uint8_t tong_zi_jie[5]=  {0xff, 0xff, 0xff, 0x07, 0x0E};

		 switch(dat)		   /*�Խ��ִ����ز���,�ͻ���ɾ��PrintHex �����������滻Ϊ������Ҫ���ƵĴ���*/
		 {
			 case CODE_SHI_SHUI:		/*����˭*/		
				  PlayDemoSound_Once(WO_SHIXIAO_TIAN_E);
			    break;		
			 case CODE_JI_SHUI:		/*�㼸��*/		
					PlayDemoSound_Once(ZHE_SHI_MI_MI);
					break;	
			 case CODE_CHANG_GE:		/*���׸�*/		
				  PlayDemoSound_Once(HAO_DE);
			    Delay1s();
					PlayDemoSound_Once(XIAO_PING_GUO);
					break;
       case CODE_XIAO_TIAN_E:   /*С����ǿ���ϴ�»���һ��ָ��*/
				 	G0_flag=ENABLE;
			    LED=0;
			    PlayDemoSound_Once(NI_HAO_ZHU_REN);
			    break;
			 default:	
				  if(ENABLE==G0_flag)
				  {	
					  G0_flag=DISABLE;
					  LED=1;
					  //PrintHex("����\r\n", 6);
					  switch(dat)		   /*�Խ��ִ����ز���,�ͻ���ɾ��PrintHex �����������滻Ϊ������Ҫ���ƵĴ���*/
						{
						 	case CODE_GUAN_JI:	/*�ػ�*/
							 	PlayDemoSound_Once(HAO_DE);
								copy_cmd(cmd, guan_ji);
								PrintHex(cmd, 16); 					  
								break;
							case CODE_KAI_JI:	 /*����*/
								PlayDemoSound_Once(HAO_DE);
								copy_cmd(cmd, kai_ji);
								PrintHex(cmd, 16); 
								break;
							case CODE_QI_DONG:		/*����*/
								PlayDemoSound_Once(HAO_DE);						
								copy_cmd(cmd, qi_dong);						
								PrintHex(cmd, 16); 
								break;
							case CODE_ZAN_TING:		/*��ͣ*/	
								PlayDemoSound_Once(HAO_DE);
								copy_cmd(cmd, zan_ting);						
								PrintHex(cmd, 16); 
								break;
							case CODE_MIAN_MA:		/*����*/	
								PlayDemoSound_Once(HAO_DE);
								copy_cmd(cmd, mian_ma);						
								PrintHex(cmd, 16); 
								break;
							case CODE_HUN_HE:		/*���*/
								PlayDemoSound_Once(HAO_DE);
								copy_cmd(cmd, hun_he);						
								PrintHex(cmd, 16); 
								break;
							case CODE_YANG_MAO:		/*��ë*/
								PlayDemoSound_Once(HAO_DE);
								copy_cmd(cmd, yang_mao);						
								PrintHex(cmd, 16); 
								break;
							case CODE_YU_RONG:		/*���޷�*/	
								PlayDemoSound_Once(HAO_DE);
								copy_cmd(cmd, yu_rong_fu);						
								PrintHex(cmd, 16); 
								break;		
							case CODE_KUAI_XI:		/*��ϴ*/
								PlayDemoSound_Once(HAO_DE);
								copy_cmd(cmd, kuai_xi);						
								PrintHex(cmd, 16); 
								break;
							case CODE_JIE_NENG:		/*����*/	
								PlayDemoSound_Once(HAO_DE);
								copy_cmd(cmd, jie_neng);						
								PrintHex(cmd, 16); 
								break;
							case CODE_DAN_TUO_SHUI:		/*����ˮ*/	
								PlayDemoSound_Once(HAO_DE);
								copy_cmd(cmd, dan_tuo_shui);						
								PrintHex(cmd, 16); 
								break;
							case CODE_TONG_ZI_JIE:		/*Ͳ�Խ�*/		
								PlayDemoSound_Once(HAO_DE);
								copy_cmd(cmd, tong_zi_jie);		
								PrintHex(cmd, 16); 
								break;	
							default:
								PrintChar("�����·���ָ��\r\n"); /*text.....*/
								break;
						}	
					}	
					else 	
					{
								//PrintChar("��˵��һ������\r\n"); /*text.....*/	
					}				 
				  break;
		 }
}	 

