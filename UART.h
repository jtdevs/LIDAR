#ifndef UART_H
#define UART_H

extern void InitUART0 (BOOL fast_mode);
extern unsigned char U0Read(void);
extern void U0Write(char data);
extern void U0Write_text(char * msg);

#endif
