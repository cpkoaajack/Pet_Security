#include "bsp_beep.h"   

void BEEP_GPIO_Config(void)
{		
		
		GPIO_InitTypeDef GPIO_InitStructure;
		
		RCC_APB2PeriphClockCmd(macBEEP_GPIO_CLK, ENABLE); 														   
		GPIO_InitStructure.GPIO_Pin = macBEEP_GPIO_PIN;	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_Init(macBEEP_GPIO_PORT, &GPIO_InitStructure);			 
		GPIO_ResetBits(macBEEP_GPIO_PORT, macBEEP_GPIO_PIN);	 
}

