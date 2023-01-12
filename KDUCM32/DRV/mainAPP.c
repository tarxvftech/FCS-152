#include "main.h"

int main()
{	
	Sys_Init();
	log_init();	
	//printf("<<<<<<<    APP   >>>>>>\n");	//1.打开串口,初始化打印信息
	TIM6_Init();
//	while(1)
//		UART1_PrintBack();
	
	LEDK_Init(); 									//键盘灯
	LCDK_Init(); 									//lx12864背光
	BackLight_SetVal(50);
	
	LCD_Init();										//lx12864初始化
	LCD_ShowPICALL(pic_HARRIS);
	//	LCD_ShowPICALL(pic_BaoTong);
	
	TIM2_Init(); 									//通用定时器
	KEY_Init();										//按键初始化
	
//	while(1)Key_Test();
//	waitToUpdate();
//	TOOLS_Menu();

	delay_ms(1000);
	LCD_ShowString0608(29, 5, VERSION_KDU, 1, 128);
	delay_ms(1000);
	LCD_Clear(GLOBAL64);
	LCD_ShowString0608(28, 3, "LOADING DATA ", 1, 128);
	

//变量初始化
#ifndef DEBUG
	wait152StartUp();					//等待接收实时变化数据,从而确定152已经开机, 并且获取所有参数
    chan_arv[TMP] = chan_arv[NOW];
	sendTo152(_ASKA);
    sendTo152(_ASKB);
#endif
	flagLineTemp = FLAG_LINE;		//检测完成后的状态, 无需再进入 互异 刷新一次
	KDU_Clear();					//显示初始化
	
	while (1)
	{
		KDU_Reflash();
		MY_GLOBAL_FUN(1);
		if(Main_Event_KEY(KEY_SCAN(0)))	//从其他界面返回  或者  主界面操作后
			KDU_Clear();

		if(flagLineTemp != FLAG_LINE)   //状态改变
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

