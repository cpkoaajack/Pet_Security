#include "usart.h"
	
#define STABLE_TIMES  10  

char Uart2_Buf[100];
u8 First_Int = 0;

static u32 SysTickDelayTime;

void SysTick_Init_Config(void)
{
	while(SysTick_Config(SystemCoreClock / 1000));				 
	SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;		 
}

void delay_ms(u32 nms)
{ 
	nms = nms * 20;
	SysTickDelayTime = nms;		 
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;	 
	while(SysTickDelayTime != 0);				
}

void USART2_Init_Config(u32 bound)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	

	USART_DeInit(USART2);  


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;		
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(GPIOA, &GPIO_InitStructure);				


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;				
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	
	GPIO_Init(GPIOA, &GPIO_InitStructure);			


	
	USART_InitStructure.USART_BaudRate = bound;										
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							
	USART_InitStructure.USART_Parity = USART_Parity_No;								
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					
	USART_Init(USART2, &USART_InitStructure);										

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);			

	USART_Cmd(USART2, ENABLE);                    			 
	USART_ClearFlag(USART2, USART_FLAG_TC);					
}

void UART2_SendString(char* s)
{
	while(*s)
	{
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC)==RESET); 
		USART_SendData(USART2 ,*s++);
	}
}

void UART2_Send_Command(char* s)
{
	CLR_Buf(); 
	UART2_SendString(s); 
	UART2_SendString("\r\n"); 
}


u8 UART2_Send_AT_Command(char *b,char *a,u8 wait_time,u32 interval_time)         
{
	u8 i;
	i = 0;
	while(i < wait_time)                    
	{
		UART2_Send_Command(b);
		delay_ms(interval_time); 
		if(Find(a))         
		{
			return 1;
		}
		i++;
	}
	
	return 0;
}

void USART2_IRQHandler(void)                	
{
	u8 Res=0;
	Res = USART_ReceiveData(USART2);
	Uart2_Buf[First_Int] = Res;  	  
	First_Int++;                	
	if(First_Int > 100)     
	{
		First_Int = 0;
	}    
} 	



void CLR_Buf(void)
{
	u16 k;
	for(k=0;k<100;k++)     
	{
		Uart2_Buf[k] = 0x00;
	}
    First_Int = 0;            
}


u8 Find(char *a)
{ 
	if(strstr(Uart2_Buf, a)!=NULL) 
	{
		return 1;
	}	
	else
	{
		return 0;
	}
		
}


