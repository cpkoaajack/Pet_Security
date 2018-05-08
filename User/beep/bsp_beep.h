#ifndef __BEEP_H
#define	__BEEP_H

#include "stm32f10x.h"

#define macBEEP_GPIO_PORT    	GPIOC			              
#define macBEEP_GPIO_CLK 	    RCC_APB2Periph_GPIOC		
#define macBEEP_GPIO_PIN		  GPIO_Pin_8			        

#define ON  1
#define OFF 0

#define macBEEP(a)	if (a)	\
					GPIO_SetBits(macBEEP_GPIO_PORT,macBEEP_GPIO_PIN);\
					else		\
					GPIO_ResetBits(macBEEP_GPIO_PORT,macBEEP_GPIO_PIN)

void BEEP_GPIO_Config(void);
					
#endif /* __BEEP_H */
