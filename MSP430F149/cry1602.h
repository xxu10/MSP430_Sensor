#ifndef Cry_1602
#define Cry_1602
void DispNChar(unsigned char x,unsigned char y, unsigned char n,unsigned char *ptr);
void LocateXY(unsigned char x,unsigned char y);
void Disp1Char(unsigned char x,unsigned char y,unsigned char data);
void LcdReset(void);
void LcdWriteCommand(unsigned char cmd,unsigned char chk);
void LcdWriteData( unsigned char data );
void WaitForEnable(void);
void Delay5ms(void);
void Delay400ms(void);
void ClearPing();
#endif 