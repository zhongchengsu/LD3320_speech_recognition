#ifndef EEPROM_H
#define EEPROM_H

void delay_us(int  n);
void delay_ms(int  n); 
void   EepromWrite(uint8_t Dat, uint32_t addr);
uint8_t  EePromRead(uint32_t addr);
void   EePromErase(uint32_t addr);
void   DisbleEeprom(void);

#endif