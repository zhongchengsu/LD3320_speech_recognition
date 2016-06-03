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

uint32 nMp3Size=0;
uint32 nMp3Pos=0;
uint32 soundIndex =1;
uint8  idata nLD_Mode = LD_MODE_IDLE;		//	������¼��ǰ���ڽ���ASRʶ�����ڲ���MP3
uint8 bMp3Play=DISABLE;							//	������¼����MP3��״̬
uint8 bAsrRun=DISABLE;							//	������¼��������ʶ���״̬
uint8 idata ucHighInt;
uint8 idata ucLowInt;
uint8 idata ucStatus;
uint8 idata ucSPVol=15; // MAX=15 MIN=0		//	Speaker�������������

extern uint8 idata nDemoFlag;
uint8 idata ucRegVal;
extern uint8 idata nAsrStatus;
void ProcessInt0(void);
soundPara_t xdata soundPara[32]; 

#ifdef DEBUG
#define PrintLog(print)   PrintChar(print)
#else
#define PrintLog(print)
#endif

void Sound_File_Parm_Init(void)
{
	/* 
  *  eeprom��ǰ128�ֽڵ����ݴ洢��ʽ
  *  byte0     byte1    byte2     byte3      byte4     byte5    byte6     byte7
  *  addrH0    addrL0   lengthH0  lengthL0   addrH1    addrL1   lengthH1  lengthL1
  *  addrH0:��һ��Mp3�ļ�����ʼ��ַ�ĸ�8λ
  *  addrL0:��һ��Mp3�ļ�����ʼ��ַ�ĵ�8λ
  *  lengthH0����һ��Mp3�ļ����ȵĸ�8λ
  *  lengthL0����һ��Mp3�ļ����ȵĵ�8λ
  */
	uint8_t *p;
	uint32_t i;
	
	p = (uint8_t *)soundPara;
	for(i=0; i< 128; i++)
	{
		*p = EePromRead(i);
		p++;
	}

}

/************************************************************************
���������� 	 ��λLDģ��
��ڲ�����	 none
�� �� ֵ�� 	 none
����˵����	 none
**************************************************************************/
void LD_Reset()
{
	RSTB=1;
	delay(5);
	RSTB=0;
	delay(5);
	RSTB=1;

	delay(5);
	CSB=0;
	delay(5);
	CSB=1;
	delay(5);
}

/************************************************************************
���������� LDģ�������ʼ��
��ڲ����� none
�� �� ֵ�� none
����˵���� �ú���Ϊ�������ã�һ�㲻��Ҫ�޸ģ�
					 ����Ȥ�Ŀͻ��ɶ��տ����ֲ������Ҫ�����޸ġ�
**************************************************************************/
void LD_Init_Common()
{
	LD_ReadReg(0x06);  
	LD_WriteReg(0x17, 0x35); 
	delay(10);
	LD_ReadReg(0x06);  

	LD_WriteReg(0x89, 0x03);  
	delay(5);
	LD_WriteReg(0xCF, 0x43);   
	delay(5);
	LD_WriteReg(0xCB, 0x02);
	
	/*PLL setting*/
	LD_WriteReg(0x11, LD_PLL_11);

	if (nLD_Mode == LD_MODE_MP3)
	{
		LD_WriteReg(0x1E, 0x00); 
		LD_WriteReg(0x19, LD_PLL_MP3_19);   
		LD_WriteReg(0x1B, LD_PLL_MP3_1B);   
		LD_WriteReg(0x1D, LD_PLL_MP3_1D);
	}
	else
	{
		LD_WriteReg(0x1E,0x00);
		LD_WriteReg(0x19, LD_PLL_ASR_19); 
		LD_WriteReg(0x1B, LD_PLL_ASR_1B);		
	  LD_WriteReg(0x1D, LD_PLL_ASR_1D);
	}	
	delay(10);
	
  LD_WriteReg(0xCD, 0x04);	
	LD_WriteReg(0x17, 0x4c); 
	delay(5);
	LD_WriteReg(0xB9, 0x00);
	LD_WriteReg(0xCF, 0x4F); 
	LD_WriteReg(0x6F, 0xFF); 
}

void LD_Init_MP3()
{
	if (nLD_Mode != LD_MODE_MP3)
	{
	  //LD_Reset();
	}
	nLD_Mode = LD_MODE_MP3;
	LD_Init_Common();

	LD_WriteReg(0xBD,0x02);
	LD_WriteReg(0x17, 0x48);
	delay(10);

	LD_WriteReg(0x85, 0x52); 
	LD_WriteReg(0x8F, 0x00);  
	LD_WriteReg(0x81, 0x00);
	LD_WriteReg(0x83, 0x00);
	LD_WriteReg(0x8E, 0xff);
	LD_WriteReg(0x8D, 0xff);
  delay(1);
	LD_WriteReg(0x87, 0xff);
	LD_WriteReg(0x89, 0xff);
	delay(1);
	LD_WriteReg(0x22, 0x00);    
	LD_WriteReg(0x23, 0x00);
	LD_WriteReg(0x20, 0xef);    
	LD_WriteReg(0x21, 0x07);
	LD_WriteReg(0x24, 0x77);          
  LD_WriteReg(0x25, 0x03);
  LD_WriteReg(0x26, 0xbb);    
  LD_WriteReg(0x27, 0x01); 
}


/************************************************************************
���������� 	 LDģ�� ASR���ܳ�ʼ��
��ڲ�����	 none
�� �� ֵ�� 	 none
����˵����	 �ú���Ϊ�������ã�һ�㲻��Ҫ�޸ģ�
					 ����Ȥ�Ŀͻ��ɶ��տ����ֲ������Ҫ�����޸ġ�
**************************************************************************/
void LD_Init_ASR()
{
	if (nLD_Mode != LD_MODE_ASR_RUN)
	{
	  //LD_Reset();
	}
	nLD_Mode=LD_MODE_ASR_RUN;
	
	LD_Init_Common();
	LD_WriteReg(0xBD, 0x00);
	LD_WriteReg(0x17, 0x48);
	delay( 10 );
	
	LD_WriteReg(0x3C, 0x80);    
	LD_WriteReg(0x3E, 0x07);
	LD_WriteReg(0x38, 0xff);    
	LD_WriteReg(0x3A, 0x07);
	
  LD_WriteReg(0x40, 0);          
	LD_WriteReg(0x42, 8);
	LD_WriteReg(0x44, 0);    
	LD_WriteReg(0x46, 8); 
	delay( 1 );
}

/************************************************************************
���������� 	�жϴ�����
��ڲ�����	 none
�� �� ֵ�� 	 none
����˵����	��LDģ����յ���Ƶ�ź�ʱ��������ú�����
						�ж�ʶ���Ƿ��н�������û�д������ü�
            ����׼����һ�ε�ʶ��
**************************************************************************/
void ProcessInt0(void)
{
	uint8 nAsrResCount=0;
	EX0=0;
	
	if(nLD_Mode == LD_MODE_ASR_RUN)
	{	
		ucRegVal = LD_ReadReg(0x2B);
	  LD_WriteReg(0x29,0) ;
	  LD_WriteReg(0x02,0) ;
	  if((ucRegVal & 0x10) &&
		  LD_ReadReg(0xb2)==0x21 && 
		  LD_ReadReg(0xbf)==0x35)			/*ʶ��ɹ�*/
	  {	
		  nAsrResCount = LD_ReadReg(0xba);
		  if(nAsrResCount>0 && nAsrResCount<=4) 
		  {
			  nAsrStatus=LD_ASR_FOUNDOK;
		  }
		  else
	    {
			  nAsrStatus=LD_ASR_FOUNDZERO;
		  }	
	  }															 /*û��ʶ����*/
	  else
	  {	 
		  nAsrStatus=LD_ASR_FOUNDZERO;
	  }
		
		bAsrRun = DISABLE;
		
    LD_WriteReg(0x2b, 0);
    LD_WriteReg(0x1C,0);/*д0:ADC������*/
		EX0 =1;
		return;
	}
	else if (nLD_Mode == LD_MODE_MP3)
	{
		// �������Ų������жϣ������֣�
	  // A. ����������ȫ�������ꡣ
	  // B. ���������ѷ�����ϡ�
	  // C. ����������ʱ��Ҫ���꣬��Ҫ�����µ����ݡ�	
	  ucHighInt = LD_ReadReg(0x29); 
	  ucLowInt=LD_ReadReg(0x02); 
	  LD_WriteReg(0x29,0) ;
	  LD_WriteReg(0x02,0) ;
    if(LD_ReadReg(0xBA)&CAUSE_MP3_SONG_END)
    {
      // A. ����������ȫ�������ꡣ  
      LD_WriteReg(0x2B,  0);
      LD_WriteReg(0xBA, 0);	
		  LD_WriteReg(0xBC,0x0);	
		  LD_WriteReg(0x08,1);
		  delay(5);
      LD_WriteReg(0x08,0);
		  LD_WriteReg(0x33, 0);
			delay(5);
			bMp3Play=DISABLE;					// ��������ȫ����������޸�bMp3Play�ı���		  //return;
			EX0=1;
		  return;	
    }		
	  else if(nMp3Pos>=nMp3Size)
	  {
	    // B. ���������ѷ������
      LD_WriteReg(0xBC, 0x01);
		  LD_WriteReg(0x29, 0x10);
			EX0=1;
		  return;	
	  }		
    else
		{			
			// C. ����������ʱ��Ҫ���꣬��Ҫ�����µ����ݡ�
	    LD_ReloadMp3Data();
	    LD_WriteReg(0x29,ucHighInt); 
	    LD_WriteReg(0x02,ucLowInt);
		  EX0=1;
		  return;	
		}
	}
	else
	{
		  EX0=1;
	}	
}

/************************************************************************
���������� 	����ASRʶ������
��ڲ�����	none
�� �� ֵ��  asrflag��1->�����ɹ��� 0��>����ʧ��
����˵����	ʶ��˳������:
						1��RunASR()����ʵ����һ��������ASR����ʶ������
						2��LD_Init_ASR() ����ʵ����ASR��ʼ��
						3��LD_AsrAddFixed() ����ʵ������ӹؼ����ﵽLD3320оƬ��
						4��LD_AsrRun()	����������һ��ASR����ʶ������					
						�κ�һ��ASRʶ�����̣�����Ҫ�������˳�򣬴ӳ�ʼ����ʼ��
**************************************************************************/
uint8 RunASR(void)
{
	uint8 i=0;
	uint8 asrflag=0;
	
	while(bMp3Play || bAsrRun)    //�ȴ����ź�ʶ�����
	{
		delay(5);
		PrintLog("wait in RunASR\r\n");
	}
	bMp3Play = DISABLE;
	bAsrRun = ENABLE;
	
	for (i=0; i<5; i++)			//	��ֹ����Ӳ��ԭ����LD3320оƬ����������������һ������5������ASRʶ������
	{
		LD_Init_ASR();
		delay(50);
		if (LD_AsrAddFixed()==0)
		{
			LD_Reset();			//	LD3320оƬ�ڲ����ֲ���������������LD3320оƬ
			delay(50);			//	���ӳ�ʼ����ʼ����ASRʶ������
			continue;
		}
		delay(10);
		if (LD_AsrRun() == 0)
		{
			LD_Reset();			//	LD3320оƬ�ڲ����ֲ���������������LD3320оƬ
			delay(50);			//	���ӳ�ʼ����ʼ����ASRʶ������
			continue;
		}
		asrflag=1;
		bAsrRun = ENABLE;
		break;					//	ASR���������ɹ����˳���ǰforѭ������ʼ�ȴ�LD3320�ͳ����ж��ź�
	}

	return asrflag;
}
/************************************************************************
����������  ���LDģ���Ƿ����
��ڲ�����	none
�� �� ֵ�� 	flag��1-> ����
����˵����	none
**************************************************************************/
uint8 LD_Check_ASRBusyFlag_b2()
{
	uint8 j;
	uint8 flag = 0;
	for (j=0; j<10; j++)
	{
		if (LD_ReadReg(0xb2) == 0x21)
		{
			flag = 1;
			break;
		}
		delay(10);		
	}
	return flag;
}


/************************************************************************
���������� 	����ASR
��ڲ�����	none
�� �� ֵ�� 	1�������ɹ�
����˵����	none
**************************************************************************/
uint8 LD_AsrRun()
{
	EX0=0;
	LD_WriteReg(0x35, MIC_VOL);
	LD_WriteReg(0x1C, 0x09);
	LD_WriteReg(0xBD, 0x20);
	LD_WriteReg(0x08, 0x01);
	delay( 1 );
	LD_WriteReg(0x08, 0x00);
	delay( 1 );

	if(LD_Check_ASRBusyFlag_b2() == 0)
	{
		return 0;
	}
	
  //LD_WriteReg(0xB6, 0xa); //ʶ��ʱ��	 1S
  //LD_WriteReg(0xB5, 0x1E); //��������ʱ�� 300ms
  //LD_WriteReg(0xB8, 10); //����ʱ��
  //LD_WriteReg(0x1C, 0x07); //����˫ͨ����Ƶ�ź���Ϊ�����ź�
	LD_WriteReg(0x1C, 0x0b); //������˷���Ϊ�����ź�
	LD_WriteReg(0xB2, 0xff);
	delay( 1);	
	LD_WriteReg(0x37, 0x06);
	delay( 1 );
  LD_WriteReg(0x37, 0x06);
  delay( 5 );
	LD_WriteReg(0x29, 0x10);	
	LD_WriteReg(0xBD, 0x00);
	EX0=1;
	return 1;
}

void LD_play()
{
	nMp3Pos=0;
	nMp3Size = soundPara[soundIndex].dataSize;

	if (nMp3Pos >=  nMp3Size)
		return ; 

	LD_ReloadMp3Data();

  LD_WriteReg(0xBA, 0x00);
	LD_WriteReg(0x17, 0x48);
	LD_WriteReg(0x33, 0x01);
	LD_WriteReg(0x29, 0x04);
	
	LD_WriteReg(0x02, 0x01); 
	LD_WriteReg(0x85, 0x5A);

	EX0=1;

}

void LD_AdjustMIX2SPVolume(uint8 val)
{
	ucSPVol = val;
	val = ((15-val)&0x0f) << 2;
	LD_WriteReg(0x8E, val | 0xc3); 
	LD_WriteReg(0x87, 0x78); 
}

void LD_ReloadMp3Data( )
{
	uint8_t val;

  PrintLog("LD_ReloadMp3Data\r\n");
	ucStatus = LD_ReadReg(0x06);
  while ( !(ucStatus&MASK_FIFO_STATUS_AFULL) && (nMp3Pos<nMp3Size) )
	{
		val = EePromRead(soundPara[soundIndex].addr + nMp3Pos);
		//val = val = EePromRead(addr + nMp3Pos);
		nMp3Pos++;
		LD_WriteReg(0x01,val);
		//if (nMp3Pos == DEMO_SOUND_SIZE)
		//	nMp3Pos = 0;

		ucStatus = LD_ReadReg(0x06);
	}
}

void PlayDemoSound_Once(uint8 index)
{
	while(bMp3Play || bAsrRun)    //�ȴ����ź�ʶ�����
	{
		delay(5);
		PrintLog("wait in PlayDemoSound_Once\r\n");
	}
	bMp3Play = ENABLE;
	bAsrRun = DISABLE;
	soundIndex = index;
	
	LD_Init_MP3();
	LD_AdjustMIX2SPVolume(1);
	LD_play();
}

/************************************************************************
���������� ��LDģ����ӹؼ���
��ڲ����� none
�� �� ֵ�� flag��1->��ӳɹ�
����˵���� �û��޸�.
					 1���������¸�ʽ���ƴ���ؼ��ʣ�ͬʱע���޸�sRecog ��pCode ����ĳ���
					 �Ͷ�Ӧ����k��ѭ���á�ƴ������ʶ������һһ��Ӧ�ġ�
					 2�������߿���ѧϰ"����ʶ��оƬLD3320�߽��ؼ�.pdf"��
           ���������������մ�����÷������ṩʶ��Ч����
					 3����xiao jie �� Ϊ�������ÿ��ʶ��ʱ�������ȷ�һ�����С�ݡ�
**************************************************************************/
uint8 LD_AsrAddFixed()
{
	uint8 k, flag;
	uint8 nAsrAddLength;
	#define DATE_A 20   /*�����ά��ֵ*/
	#define DATE_B 20		/*����һά��ֵ*/
	uint8 code sRecog[DATE_A][DATE_B] = {
																				"xiao tian e",\		
																				"guan ji",\
																				"kai ji",\
																				"qi dong",\
																				"zan ting",\
																				"xuan ze mian ma",\
																				"xuan ze hun he",\
																				"xuan ze yang mao",\
																				"xuan ze yu rong fu",\
																				"xuan ze kuai xi",\
																				"xuan ze jie neng",\
																				"xuan ze dan tuo shui",\
																				"xuan ze tong zi jie", \
		                                    "ni shi shui", \
		                                    "ni ji sui", \
		                                    "chang shou ge ba"
																			};	/*��ӹؼ��ʣ��û��޸�*/
	uint8 code pCode[DATE_A] = {
                              CODE_XIAO_TIAN_E, \
                              CODE_GUAN_JI, \
                             	CODE_KAI_JI, \
                             	CODE_QI_DONG, \
                             	CODE_ZAN_TING, \
                             	CODE_MIAN_MA, \
                             	CODE_HUN_HE, \
                             	CODE_YANG_MAO, \
                             	CODE_YU_RONG, \
                             	CODE_KUAI_XI, \
                             	CODE_JIE_NENG, \
                             	CODE_DAN_TUO_SHUI, \
                             	CODE_TONG_ZI_JIE, \
                             	CODE_SHI_SHUI, \
                             	CODE_JI_SHUI, \
                             	CODE_CHANG_GE
														 };	/*���ʶ���룬�û��޸�*/	
	flag = 1;
	for (k=0; k<DATE_A; k++)
	{
			
		if(LD_Check_ASRBusyFlag_b2() == 0)
		{
			flag = 0;
			break;
		}
		
		LD_WriteReg(0xc1, pCode[k] );
		LD_WriteReg(0xc3, 0 );
		LD_WriteReg(0x08, 0x04);
		delay(1);
		LD_WriteReg(0x08, 0x00);
		delay(1);

		for (nAsrAddLength=0; nAsrAddLength<DATE_B; nAsrAddLength++)
		{
			if (sRecog[k][nAsrAddLength] == 0)
				break;
			LD_WriteReg(0x5, sRecog[k][nAsrAddLength]);
		}
		LD_WriteReg(0xb9, nAsrAddLength);
		LD_WriteReg(0xb2, 0xff);
		LD_WriteReg(0x37, 0x04);
	}
    return flag;
}
/************************************************************************
���������� 	��ȡʶ����
��ڲ�����	none
�� �� ֵ�� 	LD_ReadReg(0xc5 )��  ��ȡ�ڲ��Ĵ�������ʶ���롣
����˵����	none
**************************************************************************/
uint8 LD_GetResult()
{		
	return LD_ReadReg(0xc5 );
}

void  delay(unsigned long uldata)
{
	unsigned int j  =  0;
	unsigned int g  =  0;
	for (j=0;j<5;j++)
	{
		for (g=0;g<uldata;g++)
		{
			_nop_();
			_nop_();
			_nop_();
		}
	}
}
