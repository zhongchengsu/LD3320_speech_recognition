#include "config.h"

uint8_t idata IR_STATUS; //��¼IR�̵ܽ�ƽ״̬

void ProcessIR(void)
{
	EX1=0;

  IR_STATUS = IR_OUT;	
	
	EX1=1;
}

void CheckIRStatus(void)
{
	IR_STATUS = (IR_PIN == 0)?IR_OUT:IR_IN;
}
