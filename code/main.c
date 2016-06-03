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

/************************************************************************************/
//	nAsrStatus 用来在main主程序中表示程序运行的状态，不是LD3320芯片内部的状态寄存器
//	LD_ASR_NONE:		表示没有在作ASR识别
//	LD_ASR_RUNING：		表示LD3320正在作ASR识别中
//	LD_ASR_FOUNDOK:		表示一次识别流程结束后，有一个识别结果
//	LD_ASR_FOUNDZERO:	表示一次识别流程结束后，没有识别结果
//	LD_ASR_ERROR:		表示一次识别流程中LD3320芯片内部出现不正确的状态
/***********************************************************************************/
uint8 idata nAsrStatus=0;	
uint8 idata nDemoFlag=0;
void MCU_init(); 
void ProcessInt0(); //识别处理函数
void User_handle(uint8 dat);//用户执行操作函数
void Delay1s();

void Led_test(void);//单片机工作指示
uint8_t G0_flag=DISABLE;//运行标志，ENABLE:运行。DISABLE:禁止运行 
sbit LED=P4^2;//信号指示灯

#define copy_cmd(cmd, sub_cmd) \
{\
	cmd[11] = sub_cmd[0]; \
  cmd[12] = sub_cmd[1]; \
  cmd[13] = sub_cmd[2]; \
  cmd[14] = sub_cmd[3]; \
  cmd[15] = sub_cmd[4]; \
}

/***********************************************************
* 名    称： void  main(void)
* 功    能： 主函数	程序入口
* 入口参数：  
* 出口参数：
* 说    明： 					 
* 调用方法： 
**********************************************************/ 
void  main(void)
{
	uint8 idata nAsrRes;
	uint32_t i=0;
	uint8_t eeprom=0x11;

	Led_test();
	MCU_init();
	LD_Reset();
	UartIni(); /*串口初始化*/
	Sound_File_Parm_Init();
	CheckIRStatus();	
	nAsrStatus = LD_ASR_NONE;		//	初始状态：没有在作ASR

	while (1)
	{
		//等待红外传感器触发，有人靠近
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
					if (RunASR()==0)	//	启动一次ASR识别流程：ASR初始化，ASR添加关键词语，启动ASR运算
					{
						nAsrStatus = LD_ASR_ERROR;
					}
					break;
				}
				case LD_ASR_FOUNDOK: //	一次ASR识别流程结束，去取ASR识别结果
				{	
					nAsrRes = LD_GetResult();		//获取结果
					User_handle(nAsrRes);//用户执行函数 
					nAsrStatus = LD_ASR_NONE;
					break;
				}
				case LD_ASR_FOUNDZERO:
				default:
				{
					nAsrStatus = LD_ASR_NONE;
					G0_flag=DISABLE;
					LED=1;
					//PrintChar("错误了\r\n");
					break;
				}
			}// switch		
		}// while
  }

}
/***********************************************************
* 名    称： 	 LED灯测试
* 功    能： 单片机是否工作指示
* 入口参数： 无 
* 出口参数：无
* 说    明： 					 
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
* 名    称： void MCU_init()
* 功    能： 单片机初始化
* 入口参数：  
* 出口参数：
* 说    明： 					 
* 调用方法： 
**********************************************************/ 
void MCU_init()
{
	P0 = 0xff;
	P1 = 0xff;
	P2 = 0xff;
	P3 = 0xff;
	P4 = 0xff;

	LD_MODE = 0;		//	设置MD管脚为低，并行模式读写
	IE0=1;
	EX0=1;
	
	IT1 = 1; //下降延触发
	EX1 = 1; // INT1使能，用户红外感应
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
* 名    称： 中断处理函数
* 功    能：
* 入口参数：  
* 出口参数：
* 说    明： 					 
* 调用方法： 
**********************************************************/ 
void ExtInt0Handler(void) interrupt 0  
{ 	
	ProcessInt0();				
}


/***********************************************************
* 名    称： 中断处理函数 红外中断函数
* 功    能：
* 入口参数：  
* 出口参数：
* 说    明： 					 
* 调用方法： 
**********************************************************/ 
void ExtInt1Handler(void) interrupt 1  
{ 	
	ProcessIR();				
}


/***********************************************************
* 名    称：用户执行函数 
* 功    能：识别成功后，执行动作可在此进行修改 
* 入口参数： 无 
* 出口参数：无
* 说    明： 		
						LED=1;
            copy_cmd(cmd, dan_tuo_shui);						
						PrintHex(cmd); 
						break;

AA 0F DB 00 00 00 00 00 00 02 02 00 00 00 00 12 关机
AA 0F DB 00 00 00 00 00 00 02 02 01 00 00 00 11 开机
AA 0F DB 00 00 00 00 00 00 02 02 01 01 00 FF 11 启动
AA 0F DB 00 00 00 00 00 00 02 02 01 00 00 FF 12 暂停
AA 0F DB 00 00 00 00 00 00 02 02 01 00 00 00 11 棉麻
AA 0F DB 00 00 00 00 00 00 02 02 01 00 00 03 0E 混合
AA 0F DB 00 00 00 00 00 00 02 02 01 00 00 05 0C 羊毛
AA 0F DB 00 00 00 00 00 00 02 02 01 00 00 0F 02 羽绒服
AA 0F DB 00 00 00 00 00 00 02 02 01 00 00 02 0F 快洗
AA 0F DB 00 00 00 00 00 00 02 02 01 00 00 01 10 节能
AA 0F DB 00 00 00 00 00 00 02 02 01 00 00 09 08 单脱水
AA 0F DB 00 00 00 00 00 00 02 02 01 00 00 07 0A 筒自洁
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

		 switch(dat)		   /*对结果执行相关操作,客户可删除PrintHex 串口输出语句替换为其他需要控制的代码*/
		 {
			 case CODE_SHI_SHUI:		/*你是谁*/		
				  PlayDemoSound_Once(WO_SHIXIAO_TIAN_E);
			    break;		
			 case CODE_JI_SHUI:		/*你几岁*/		
					PlayDemoSound_Once(ZHE_SHI_MI_MI);
					break;	
			 case CODE_CHANG_GE:		/*唱首歌*/		
				  PlayDemoSound_Once(HAO_DE);
			    Delay1s();
					PlayDemoSound_Once(XIAO_PING_GUO);
					break;
       case CODE_XIAO_TIAN_E:   /*小天鹅是控制洗衣机的一级指令*/
				 	G0_flag=ENABLE;
			    LED=0;
			    PlayDemoSound_Once(NI_HAO_ZHU_REN);
			    break;
			 default:	
				  if(ENABLE==G0_flag)
				  {	
					  G0_flag=DISABLE;
					  LED=1;
					  //PrintHex("命令\r\n", 6);
					  switch(dat)		   /*对结果执行相关操作,客户可删除PrintHex 串口输出语句替换为其他需要控制的代码*/
						{
						 	case CODE_GUAN_JI:	/*关机*/
							 	PlayDemoSound_Once(HAO_DE);
								copy_cmd(cmd, guan_ji);
								PrintHex(cmd, 16); 					  
								break;
							case CODE_KAI_JI:	 /*开机*/
								PlayDemoSound_Once(HAO_DE);
								copy_cmd(cmd, kai_ji);
								PrintHex(cmd, 16); 
								break;
							case CODE_QI_DONG:		/*启动*/
								PlayDemoSound_Once(HAO_DE);						
								copy_cmd(cmd, qi_dong);						
								PrintHex(cmd, 16); 
								break;
							case CODE_ZAN_TING:		/*暂停*/	
								PlayDemoSound_Once(HAO_DE);
								copy_cmd(cmd, zan_ting);						
								PrintHex(cmd, 16); 
								break;
							case CODE_MIAN_MA:		/*棉麻*/	
								PlayDemoSound_Once(HAO_DE);
								copy_cmd(cmd, mian_ma);						
								PrintHex(cmd, 16); 
								break;
							case CODE_HUN_HE:		/*混合*/
								PlayDemoSound_Once(HAO_DE);
								copy_cmd(cmd, hun_he);						
								PrintHex(cmd, 16); 
								break;
							case CODE_YANG_MAO:		/*羊毛*/
								PlayDemoSound_Once(HAO_DE);
								copy_cmd(cmd, yang_mao);						
								PrintHex(cmd, 16); 
								break;
							case CODE_YU_RONG:		/*羽绒服*/	
								PlayDemoSound_Once(HAO_DE);
								copy_cmd(cmd, yu_rong_fu);						
								PrintHex(cmd, 16); 
								break;		
							case CODE_KUAI_XI:		/*快洗*/
								PlayDemoSound_Once(HAO_DE);
								copy_cmd(cmd, kuai_xi);						
								PrintHex(cmd, 16); 
								break;
							case CODE_JIE_NENG:		/*节能*/	
								PlayDemoSound_Once(HAO_DE);
								copy_cmd(cmd, jie_neng);						
								PrintHex(cmd, 16); 
								break;
							case CODE_DAN_TUO_SHUI:		/*单脱水*/	
								PlayDemoSound_Once(HAO_DE);
								copy_cmd(cmd, dan_tuo_shui);						
								PrintHex(cmd, 16); 
								break;
							case CODE_TONG_ZI_JIE:		/*筒自洁*/		
								PlayDemoSound_Once(HAO_DE);
								copy_cmd(cmd, tong_zi_jie);		
								PrintHex(cmd, 16); 
								break;	
							default:
								PrintChar("请重新发送指令\r\n"); /*text.....*/
								break;
						}	
					}	
					else 	
					{
								//PrintChar("请说出一级口令\r\n"); /*text.....*/	
					}				 
				  break;
		 }
}	 

