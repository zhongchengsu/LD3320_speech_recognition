/***************************飞音云电子****************************
**  工程名称：YS-V0.7语音识别模块驱动程序
**	CPU: STC89LE52
**	晶振：22.1184MHZ
**	波特率：9600 bit/S
**	配套产品信息：YS-V0.7语音识别开发板
**                http://yuesheng001.taobao.com
**  作者：zdings
**  联系：751956552@qq.com
**  修改日期：2012.4.1
**  说明：口令模式： 即每次识别时都需要说“小杰”这个口令 ，才能够进行下一级的识别
/***************************飞音云电子******************************/


#ifndef LD_CHIP_H
#define LD_CHIP_H

#define uint8 unsigned char
#define uint16 unsigned int
#define uint32 unsigned long

//	以下三个状态定义用来记录程序是在运行ASR识别还是在运行MP3播放
#define LD_MODE_IDLE		0x00
#define LD_MODE_ASR_RUN		0x08
#define LD_MODE_MP3		 	0x40

//	以下五个状态定义用来记录程序是在运行ASR识别过程中的哪个状态
#define LD_ASR_NONE				0x00	 /*	表示没有在作ASR识别*/
#define LD_ASR_RUNING			0x01	/*	表示LD3320正在作ASR识别中*/
#define LD_ASR_FOUNDOK			0x10	/*表示一次识别流程结束后，有一个识别结果*/
#define LD_ASR_FOUNDZERO 		0x11	/*表示一次识别流程结束后，没有识别结果*/
#define LD_ASR_ERROR	 		0x31	/*表示一次识别流程中LD3320芯片内部出现不正确的状态*/


#define CLK_IN   		22.1184	/* 用户注意修改输入的晶振时钟大小 */
#define LD_PLL_11			(uint8)((CLK_IN/2.0)-1)
#define LD_PLL_MP3_19		0x0f
#define LD_PLL_MP3_1B		0x18
#define LD_PLL_MP3_1D   	(uint8)(((90.0*((LD_PLL_11)+1))/(CLK_IN))-1)

#define LD_PLL_ASR_19 		(uint8)(CLK_IN*32.0/(LD_PLL_11+1) - 0.51)
#define LD_PLL_ASR_1B 		0x48
#define LD_PLL_ASR_1D 		0x1f


// LD chip fixed values.
#define        RESUM_OF_MUSIC               0x01
#define        CAUSE_MP3_SONG_END           0x20

#define        MASK_INT_SYNC				0x10
#define        MASK_INT_FIFO				0x04
#define    	   MASK_AFIFO_INT				0x01
#define        MASK_FIFO_STATUS_AFULL		0x08


//函数声明
void LD_Reset();
void LD_Init_Common();
void LD_Init_ASR();
uint8 RunASR(void);
void LD_AsrStart();
uint8 LD_AsrRun();
uint8 LD_AsrAddFixed();
uint8 LD_GetResult();

void LD_Init_MP3();
//void LD_play(unit8 sound);
void LD_play();
void LD_AdjustMIX2SPVolume(uint8 value);
void LD_ReloadMp3Data( );
void LD_ReloadMp3Data_2();
uint8 LD_ProcessAsr(uint32 RecogAddr);
void LD_ReadMemoryBlock(uint8 dev, uint8 * ptr, uint32 addr, uint8 count);
void LD_WriteMemoryBlock(uint8 dev, uint8 * ptr, uint32 addr, uint8 count);
void PlayDemoSound_Once(uint8 index);
void Sound_File_Parm_Init(void);

extern uint32 nMp3StartPos;
extern uint32 nMp3Size;
extern uint32 nMp3Pos;
extern uint8 bMp3Play;
extern uint8 idata  nLD_Mode;

//识别码客户修改处 
#define	CODE_XIAO_TIAN_E		0x00
#define	CODE_GUAN_JI				0x01
#define	CODE_KAI_JI					0x02
#define	CODE_QI_DONG  	    	0x03
#define	CODE_ZAN_TING				0x04
#define	CODE_MIAN_MA				0x05
#define	CODE_HUN_HE					0x06
#define	CODE_YANG_MAO				0x07
#define	CODE_YU_RONG				0x08
#define	CODE_KUAI_XI				0x09
#define	CODE_JIE_NENG				0x0A
#define	CODE_DAN_TUO_SHUI		0x0B
#define	CODE_TONG_ZI_JIE		0x0C
#define	CODE_SHI_SHUI				0x0D
#define	CODE_JI_SHUI	      0x0E
#define	CODE_CHANG_GE       0x0F

void  delay(unsigned long uldata);
void  Delay200ms();

//数值越大越灵敏识别距离越远，但误识别率就越大， 根据自己的实际情况调节。
#define MIC_VOL 0x55	 //咪头增益（灵敏度调节） 范围：00-7f 
#endif
