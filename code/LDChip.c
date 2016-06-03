/***************************飞音云电子****************************
**  工程名称：YS-V0.7语音识别模块驱动程序
**	CPU: STC11L08XE
**	晶振：22.1184MHZ
**	波特率：9600 bit/S
**	配套产品信息：YS-V0.7语音识别开发板
**                http://yuesheng001.taobao.com
**  作者：zdings
**  联系：751956552@qq.com
**  修改日期：2013.9.13
**  说明：口令模式： 即每次识别时都需要说“小杰”这个口令 ，才能够进行下一级的识别
/***************************飞音云电子******************************/
#include "config.h"

uint32 nMp3Size=0;
uint32 nMp3Pos=0;
uint32 soundIndex =1;
uint8  idata nLD_Mode = LD_MODE_IDLE;		//	用来记录当前是在进行ASR识别还是在播放MP3
uint8 bMp3Play=DISABLE;							//	用来记录播放MP3的状态
uint8 bAsrRun=DISABLE;							//	用来记录播放语音识别的状态
uint8 idata ucHighInt;
uint8 idata ucLowInt;
uint8 idata ucStatus;
uint8 idata ucSPVol=15; // MAX=15 MIN=0		//	Speaker喇叭输出的音量

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
  *  eeprom中前128字节的数据存储方式
  *  byte0     byte1    byte2     byte3      byte4     byte5    byte6     byte7
  *  addrH0    addrL0   lengthH0  lengthL0   addrH1    addrL1   lengthH1  lengthL1
  *  addrH0:第一个Mp3文件的起始地址的高8位
  *  addrL0:第一个Mp3文件的起始地址的低8位
  *  lengthH0：第一个Mp3文件长度的高8位
  *  lengthL0：第一个Mp3文件长度的低8位
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
功能描述： 	 复位LD模块
入口参数：	 none
返 回 值： 	 none
其他说明：	 none
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
功能描述： LD模块命令初始化
入口参数： none
返 回 值： none
其他说明： 该函数为出厂配置，一般不需要修改；
					 有兴趣的客户可对照开发手册根据需要自行修改。
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
功能描述： 	 LD模块 ASR功能初始化
入口参数：	 none
返 回 值： 	 none
其他说明：	 该函数为出厂配置，一般不需要修改；
					 有兴趣的客户可对照开发手册根据需要自行修改。
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
功能描述： 	中断处理函数
入口参数：	 none
返 回 值： 	 none
其他说明：	当LD模块接收到音频信号时，将进入该函数，
						判断识别是否有结果，如果没有从新配置寄
            存器准备下一次的识别。
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
		  LD_ReadReg(0xbf)==0x35)			/*识别成功*/
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
	  }															 /*没有识别结果*/
	  else
	  {	 
		  nAsrStatus=LD_ASR_FOUNDZERO;
	  }
		
		bAsrRun = DISABLE;
		
    LD_WriteReg(0x2b, 0);
    LD_WriteReg(0x1C,0);/*写0:ADC不可用*/
		EX0 =1;
		return;
	}
	else if (nLD_Mode == LD_MODE_MP3)
	{
		// 声音播放产生的中断，有三种：
	  // A. 声音数据已全部播放完。
	  // B. 声音数据已发送完毕。
	  // C. 声音数据暂时将要用完，需要放入新的数据。	
	  ucHighInt = LD_ReadReg(0x29); 
	  ucLowInt=LD_ReadReg(0x02); 
	  LD_WriteReg(0x29,0) ;
	  LD_WriteReg(0x02,0) ;
    if(LD_ReadReg(0xBA)&CAUSE_MP3_SONG_END)
    {
      // A. 声音数据已全部播放完。  
      LD_WriteReg(0x2B,  0);
      LD_WriteReg(0xBA, 0);	
		  LD_WriteReg(0xBC,0x0);	
		  LD_WriteReg(0x08,1);
		  delay(5);
      LD_WriteReg(0x08,0);
		  LD_WriteReg(0x33, 0);
			delay(5);
			bMp3Play=DISABLE;					// 声音数据全部播放完后，修改bMp3Play的变量		  //return;
			EX0=1;
		  return;	
    }		
	  else if(nMp3Pos>=nMp3Size)
	  {
	    // B. 声音数据已发送完毕
      LD_WriteReg(0xBC, 0x01);
		  LD_WriteReg(0x29, 0x10);
			EX0=1;
		  return;	
	  }		
    else
		{			
			// C. 声音数据暂时将要用完，需要放入新的数据。
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
功能描述： 	运行ASR识别流程
入口参数：	none
返 回 值：  asrflag：1->启动成功， 0—>启动失败
其他说明：	识别顺序如下:
						1、RunASR()函数实现了一次完整的ASR语音识别流程
						2、LD_Init_ASR() 函数实现了ASR初始化
						3、LD_AsrAddFixed() 函数实现了添加关键词语到LD3320芯片中
						4、LD_AsrRun()	函数启动了一次ASR语音识别流程					
						任何一次ASR识别流程，都需要按照这个顺序，从初始化开始
**************************************************************************/
uint8 RunASR(void)
{
	uint8 i=0;
	uint8 asrflag=0;
	
	while(bMp3Play || bAsrRun)    //等待播放和识别结束
	{
		delay(5);
		PrintLog("wait in RunASR\r\n");
	}
	bMp3Play = DISABLE;
	bAsrRun = ENABLE;
	
	for (i=0; i<5; i++)			//	防止由于硬件原因导致LD3320芯片工作不正常，所以一共尝试5次启动ASR识别流程
	{
		LD_Init_ASR();
		delay(50);
		if (LD_AsrAddFixed()==0)
		{
			LD_Reset();			//	LD3320芯片内部出现不正常，立即重启LD3320芯片
			delay(50);			//	并从初始化开始重新ASR识别流程
			continue;
		}
		delay(10);
		if (LD_AsrRun() == 0)
		{
			LD_Reset();			//	LD3320芯片内部出现不正常，立即重启LD3320芯片
			delay(50);			//	并从初始化开始重新ASR识别流程
			continue;
		}
		asrflag=1;
		bAsrRun = ENABLE;
		break;					//	ASR流程启动成功，退出当前for循环。开始等待LD3320送出的中断信号
	}

	return asrflag;
}
/************************************************************************
功能描述：  检测LD模块是否空闲
入口参数：	none
返 回 值： 	flag：1-> 空闲
其他说明：	none
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
功能描述： 	运行ASR
入口参数：	none
返 回 值： 	1：启动成功
其他说明：	none
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
	
  //LD_WriteReg(0xB6, 0xa); //识别时间	 1S
  //LD_WriteReg(0xB5, 0x1E); //背景音段时间 300ms
  //LD_WriteReg(0xB8, 10); //结束时间
  //LD_WriteReg(0x1C, 0x07); //配置双通道音频信号做为输入信号
	LD_WriteReg(0x1C, 0x0b); //配置麦克风做为输入信号
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
	while(bMp3Play || bAsrRun)    //等待播放和识别结束
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
功能描述： 向LD模块添加关键词
入口参数： none
返 回 值： flag：1->添加成功
其他说明： 用户修改.
					 1、根据如下格式添加拼音关键词，同时注意修改sRecog 和pCode 数组的长度
					 和对应变了k的循环置。拼音串和识别码是一一对应的。
					 2、开发者可以学习"语音识别芯片LD3320高阶秘籍.pdf"中
           关于垃圾词语吸收错误的用法，来提供识别效果。
					 3、”xiao jie “ 为口令，故在每次识别时，必须先发一级口令“小捷”
**************************************************************************/
uint8 LD_AsrAddFixed()
{
	uint8 k, flag;
	uint8 nAsrAddLength;
	#define DATE_A 20   /*数组二维数值*/
	#define DATE_B 20		/*数组一维数值*/
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
																			};	/*添加关键词，用户修改*/
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
														 };	/*添加识别码，用户修改*/	
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
功能描述： 	获取识别结果
入口参数：	none
返 回 值： 	LD_ReadReg(0xc5 )；  读取内部寄存器返回识别码。
其他说明：	none
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
