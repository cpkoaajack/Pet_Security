#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"
#include "string.h"

void SysTick_Init_Config(void);
void delay_ms(u32 nms);
void CLR_Buf(void);
void USART2_Init_Config(u32 bound);
void UART2_SendString(char* s);
void UART2_SendLR(void);
void UART2_Send_Command(char* s);
void USART2_IRQHandler(void);
u8 UART2_Send_AT_Command(char *b,char *a,u8 wait_time,u32 interval_time);
u8 Find(char *a);

#define UART2_SendLR() UART2_SendString("\r\n")
											
#endif


