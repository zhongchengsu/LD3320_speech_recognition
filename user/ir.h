#ifndef __IR_H
#define __IR_H

#define IR_IN   0
#define IR_OUT  1

sbit IR_PIN=P3^3;//–≈∫≈÷∏ æµ∆

void ProcessIR(void);	
void CheckIRStatus(void);

extern uint8_t idata IR_STATUS;

#endif