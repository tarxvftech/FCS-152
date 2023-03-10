#include "main.h"

int main()
{	
	Sys_Init();
	log_init();	
//	printf("<<<<<<<    APP   >>>>>>\n");	//Open the serial port and initialize the printing information
	TIM6_Init();
//	while(1)
//	UART1_PrintBack();
	
	LEDK_Init(); 				//Keyboard light
	LCDK_Init(); 				//lx12864 backlight
	BackLight_SetVal(50);
	
	LCD_Init();				//lx12864 initialization
	LCD_ShowPICALL(pic_HARRIS);
//	LCD_ShowPICALL(pic_BaoTong);
	
	TIM2_Init(); 				//Universal timer
	KEY_Init();				//Key initialization
	
//	while(1)Key_Test();
//	waitToUpdate();
//	TOOLS_Menu();

	delay_ms(1000);
	LCD_ShowString0608(29, 5, VERSION_KDU, 1, 128);
	delay_ms(1000);
	LCD_Clear(GLOBAL64);
	LCD_ShowString0608(28, 3, "LOADING DATA ", 1, 128);
	

//Variable initialization
#ifndef DEBUG
	wait152StartUp();			//Wait to receive real-time change data to determine that 152 has been turned on and obtain all parameters
    chan_arv[TMP] = chan_arv[NOW];
	sendTo152(_ASKA);
    sendTo152(_ASKB);
#endif
	flagLineTemp = FLAG_LINE;		//The status after the detection is completed, there is no need to enter the mutual difference to refresh again
	KDU_Clear();				//Display initialization
	
	while (1)
	{
		KDU_Reflash();
		MY_GLOBAL_FUN(1);
		if(Main_Event_KEY(KEY_SCAN(0)))		//After returning from other interfaces or operating from the main interface
			KDU_Clear();

		if(flagLineTemp != FLAG_LINE)   	//State change
		{
			BackLight_SetVal(50);
			Home_Mode = MAIN_MODE;
			if(FLAG_LINE == ON)
				KDU_Clear();
			else
				LCD_ShowString0608(0, 0, "LINK FAILED ", 1, 128);
			flagLineTemp = FLAG_LINE;
		}
	}
}
//

