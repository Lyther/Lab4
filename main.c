#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "key.h"
#include <string.h>

int main(void) {	
	u8 t;
	u8 len;
	u16 times = 0;
	delay_init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	uart_init(9600);
	LED_Init();
	KEY_Init();

	while (1) {
		//Scanning key press event.
		t = KEY_Scan(0);
		switch (t) {
			case KEY0_PRES:
				printf("Key0 Pressed.\r\n");
				break;
			case KEY1_PRES:
				printf("Key1 Pressed.\r\n");
				break;
			case WKUP_PRES:
				printf("WKUP Pressed.\r\n");
				break;
			default:
				break;
		}
		if (USART_RX_STA & 0x8000) {
			//Receiving command, and control led.
			len = USART_RX_STA & 0x3fff;
			USART_RX_BUF[len] = '\0';
			if (strcmp("led0 on", USART_RX_BUF) == 0)
				GPIO_ResetBits(GPIOA, GPIO_Pin_8);
			else if (strcmp("led0 off", USART_RX_BUF) == 0)
				GPIO_SetBits(GPIOA, GPIO_Pin_8);
			else if (strcmp("led1 on", USART_RX_BUF) == 0)
				GPIO_ResetBits(GPIOD, GPIO_Pin_2);
			else if (strcmp("led1 off", USART_RX_BUF) == 0)
				GPIO_SetBits(GPIOD, GPIO_Pin_2);
			else {	//Display name and welcome message.
				printf("\r\nHello, \r\n");
				for (t=0; t<len; t++) {
					USART1->DR = USART_RX_BUF[t];
					while ((USART1->SR & 0X40) == 0);
				}
				printf("\r\n");
			}
			USART_RX_STA = 0;
		} else {	//Make sure assistant is alive.
			times++;
			if (times%100 == 0)	printf(">");
			delay_ms(10);
		}
	}
}
