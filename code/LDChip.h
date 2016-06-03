/***************************�����Ƶ���****************************
**  �������ƣ�YS-V0.7����ʶ��ģ����������
**	CPU: STC89LE52
**	����22.1184MHZ
**	�����ʣ�9600 bit/S
**	���ײ�Ʒ��Ϣ��YS-V0.7����ʶ�𿪷���
**                http://yuesheng001.taobao.com
**  ���ߣ�zdings
**  ��ϵ��751956552@qq.com
**  �޸����ڣ�2012.4.1
**  ˵��������ģʽ�� ��ÿ��ʶ��ʱ����Ҫ˵��С�ܡ�������� �����ܹ�������һ����ʶ��
/***************************�����Ƶ���******************************/


#ifndef LD_CHIP_H
#define LD_CHIP_H

#define uint8 unsigned char
#define uint16 unsigned int
#define uint32 unsigned long

//	��������״̬����������¼������������ASRʶ����������MP3����
#define LD_MODE_IDLE		0x00
#define LD_MODE_ASR_RUN		0x08
#define LD_MODE_MP3		 	0x40

//	�������״̬����������¼������������ASRʶ������е��ĸ�״̬
#define LD_ASR_NONE				0x00	 /*	��ʾû������ASRʶ��*/
#define LD_ASR_RUNING			0x01	/*	��ʾLD3320������ASRʶ����*/
#define LD_ASR_FOUNDOK			0x10	/*��ʾһ��ʶ�����̽�������һ��ʶ����*/
#define LD_ASR_FOUNDZERO 		0x11	/*��ʾһ��ʶ�����̽�����û��ʶ����*/
#define LD_ASR_ERROR	 		0x31	/*��ʾһ��ʶ��������LD3320оƬ�ڲ����ֲ���ȷ��״̬*/


#define CLK_IN   		22.1184	/* �û�ע���޸�����ľ���ʱ�Ӵ�С */
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


//��������
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

//ʶ����ͻ��޸Ĵ� 
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

//��ֵԽ��Խ����ʶ�����ԽԶ������ʶ���ʾ�Խ�� �����Լ���ʵ��������ڡ�
#define MIC_VOL 0x55	 //��ͷ���棨�����ȵ��ڣ� ��Χ��00-7f 
#endif
