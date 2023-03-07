#include "main.h"

u8 MACHINE_OPTION = 152;

volatile char FLAG_LINE = OFF;		//OFF: NO data, ON: connected
char flagLineTemp = OFF;

CHAN_ARV chan_arv[ARV_MEM_COUNT]={1, 0, 0, 1, 0, 0, 435.025, 435.025, "nihao"};   //"nihao" - variable translated by KDU in a text string as a name


volatile int FM_FREQ = 1036;
volatile u8  
			CF = 1, VU = 1, CHAN = 1,
			STEP_LEVEL = 0, SQL = 4, AUD = 0, MIC = 1, ENC = 0, TOT = 0, BL = 40, SC=3, OP = 0,
			VOLUME = 4, PRE_TONE = 0, END_TONE = 0, 
			VOLTAGE = 100,
			RSSI = 0,
			PTT_READ = 1,
			A002_SQ_READ=1,
			SQUELCH_STATE = 1,

			WFM = 0,
			FM_CHAN = 0,
			ENTER_FM = 0;

int LAMP_TIME = 10000;
extern u8  LCD_INVERTED;		//LCD upside down display

char 
	Home_Mode = 0,			//Homepage display mode	   //0: Regular, 1: Large characters 2: Double guard 
	Flag_Main_Page = 1, 		//Homepage display mode
	SQL_MODE = 0;			//Long squelch mode

double STEP[3] = {0.005, 0.0100, 0.0125};
char send_buf[128]={0};
char freq_buf[8]={0};
u8 key_lock = 0;

extern void LightBacklight(void);

void loadData(void)
{
	sendTo152(_ASKALL);			//Get all parameters
	chan_arv[TMP] = chan_arv[NOW];
}
//
void MY_GLOBAL_FUN(unsigned char volume_change_en)		//Global function function
{
	FeedDog(); 						//Feed the dog - watchdog timer initialization
	receiveProcess();
	
	if(LAMP_TIME>0 && bsp_CheckTimer(TMR_FLOW))		//It's time to turn off the lights when the backlight is set to 10S automatic mode
		BackLight_SetVal(0);

	if(volume_change_en)
		VOL_Reflash();
	
	if(bsp_CheckTimer(TMR_ASK))
    {
        sendCommand(_ASKALL);                              	//Get all parameters
        bsp_StartTimer(TMR_REPLY, TMR_PERIOD_500MS);        	//Start time limit
        waitToReceive(2000);
    }

    if(bsp_CheckTimer(TMR_REPLY))
    {
        D_printf("Timeout but no receive\n");
        FLAG_LINE=OFF;
    }
	
}
//
void VOL_Reflash(void)
{
	static u8 key_up=1;
	u8 change=0;
	if(key_up && (VOL_SUB_READ==0||VOL_ADD_READ==0))
	{
		delay_ms(5);
		key_up=0;
		if(LCD_INVERTED?VOL_SUB_READ==0:VOL_ADD_READ==0)
		{
			LightBacklight();
			if (VOLUME < 7)
			{
				VOLUME++;
				change = 1;
			}
		}
		else if(LCD_INVERTED?VOL_ADD_READ==0:VOL_SUB_READ==0)
		{
			LightBacklight();
			if (VOLUME > 0)
			{
				VOLUME--;
				change = 1;
			}
		}
		if(change)
		{
			change = 0;
			LCD_ShowVolume(VOLUME);
			sendTo152(_SETVOLU);
		}
	}
	else if(VOL_ADD_READ  &&  VOL_SUB_READ)
		key_up = 1;
	if (bsp_CheckTimer(TMR_DELAY_FLASH) && PTT_READ)
	{
		LCD_ShowBattery(VOLTAGE);
	}
}
//


//Main interface format
void KDU_Clear(void)
{
	LCD_Clear(GLOBAL64);
	LCD_ShowString0408(0, 2, "R BAT", 1);
	LCD_ShowBattery(VOLTAGE);
	LCD_ShowString0408(48, 2, "VULOS MOI", 1);
	LCD_ShowString0408(88, 2, "------- PT", 1);
	LCD_ShowVolume(VOLUME);
	if(!FLAG_LINE)
		LCD_ShowString0608(0, 0, "LINK FAILED ", 1, 128);
	
	//DualMode_Clear
	//DUAL mode clear screen
	if(Home_Mode == DUAL_MODE)
	{
		LCD_ShowString0608(0, 3,  "A:                    ", 1, 128);
		LCD_ShowString0608(0, 4,  "B:                    ", 1, 128);
		LCD_ShowString0608(0, 5,  "                      ", 1, 128);
		LCD_ShowString0408(0, 5,  "DUAL STANDBY MODE.    ", 1);
	}

}
//
//Homepage interface refresh
char BIG_MODE_buf[12]={0};
char 
	 sele_pos=0, 			//In dual-guard and dual-standby mode, the selected channel
	 rcv_chan=0;			//Currently set channel
void KDU_Reflash()
{
	switch(Home_Mode)
	{
		case MAIN_MODE:
			if (CF) 	//Frequency
				LCD_ShowString0608(66, 3,  VU?"UHF ":"VHF ", 1, 128);
			else 		//Channel
				LCD_ShowString0608(66, 3, "CHAN", 1, 128);
			
			LCD_ShowChan(83, 4, CHAN, 1);
			
			if(PTT_READ)	//Display frequency
			{
				LCD_ShowFreq(0, 3, chan_arv[NOW].RX_FREQ, 1);
				LCD_ShowString0608(56, 4,  WFM?"WFM":"FM ", 1, 128);
			}
			else
				LCD_ShowFreq(0, 3, chan_arv[NOW].TX_FREQ, 1);

			//Send and receive different frequency signs
			if (chan_arv[NOW].RX_FREQ != chan_arv[NOW].TX_FREQ)
				LCD_ShowPIC0808(92, 3, 0);
			else
				LCD_ShowString0408(92, 3, "  ", 1);
			
			LCD_ShowString0408(0, 5, "TYPE   TRF    MOD    CHAN  KEY  ", 1); 
			if(key_lock)
				LCD_ShowString0408(108, 5, "KEY", 0);
			
			LCD_ShowString0608(0, 4, "LOS", 1, 128);

			LCD_ShowString0608(26, 4, TRF_Show[MIC], 1, 128);
			//
			break;
			
			
		case BIG_MODE:	//BIG_MODE
            memset(BIG_MODE_buf, 0, 12);
			sprintf(BIG_MODE_buf, "%02d-%s", CHAN, chan_arv[NOW].NN);
			LCD_ShowString1016(0, 3,  BIG_MODE_buf, 1, 128);
			LCD_ShowString0408(0, 5,  "THE NICKNAME OF YOUR CHANNEL.", 1);
			break;
		
		
		case DUAL_MODE:
            memset(BIG_MODE_buf, 0, 12);
			sprintf(BIG_MODE_buf, "%02d-%s", chan_arv[CHANA].CHAN, chan_arv[CHANA].NN);
			LCD_ShowString0608(13, 3, BIG_MODE_buf, 1, 82);	
			sprintf(BIG_MODE_buf, "%02d-%s", chan_arv[CHANB].CHAN, chan_arv[CHANB].NN);
			LCD_ShowString0608(13, 4, BIG_MODE_buf, 1, 82);
		
			LCD_ShowString0608	(82, 2-sele_pos+2,  "  ", 1, 128);		//Hook
			LCD_ShowPIC0808	(82, sele_pos+1+2, 1);
		
			if(A002_SQ_READ)
			{
				LCD_ShowString0608	(74, 2-rcv_chan+2,  " ", 1, 95);
				LCD_ShowPIC0608	(74, rcv_chan+1+2, 0, 1);			//Arrow table reception
				//Get signal
			}
			break;
		
		
		default:
			break;
	}
	if (PTT_READ)
	{
		LCD_ShowAscii0408(0, 2, 'R');
		LCD_ShowSignal(RSSI);		//Signal detection
	}
	else
	{
		LCD_ShowAscii0408(0, 2, 'T');
		LCD_ShowSignal(100);
	}
	
}
//

//Key event handling	
//When FLAG_LINE==0, some key functions are disabled
int Main_Event_KEY(u8 key)
{
	if(!PTT_READ)
		return NO_OPERATE;
	
	
	u8 pre_mode = 200;
	switch (key)
	{
		case key_0:
			if(CF || !FLAG_LINE)
				return NO_OPERATE;
			//Do not jump for the time being, wait for a reply before jumping (?) - What?
			sendCommand(_SETHOMEMODE);
			LCD_Clear(EDITZONE64);
			LCD_ShowString1016(14, 3, "WAITING...", 1, 128);
			wait152StartUp();
				
			KDU_Clear();
			return NO_OPERATE;

		case key_1:
			if(Home_Mode || !FLAG_LINE)
				return NO_OPERATE;
			
			D_printf("NOW : V_SETTING\n");
			pre_mode=VU;  		//The current VU frequency band is used to retain the current frequency band
						//Not used as a reason to "enter different frequency band settings":
						//Reduce the judgment after confirmation (since you need to judge whether to enter or not to confirm and exit, 
						//then operate directly in the judgment), and ensure that the memory of VU is only modified in FREQ mode.	
						//Change the display in FREQ mode.
						//If it is currently a U segment, change the display to "VHF" 
						//Arrows are displayed in CHANNEL mode
			if(CF)	      		
			{
				if (pre_mode)	
				{
					VU = 0;		//Modify VU
					LCD_ShowString0608(66, 3,  "VHF ", 1, 128);
					CHAN = 0;
				}
			}
			else				
				LCD_ShowPIC0608(60, 3, 1, 1);
			
			switch(RT_FREQ_Set(0, 3, (double *)&chan_arv[NOW].RX_FREQ, 0))		//In any case, the V segment is set
			{
				case ENT2LAST:
					chan_arv[NOW].TX_FREQ = chan_arv[NOW].RX_FREQ;
					sendTo152(_SETCHAN);
				return BACK2MAIN;
				
				case CLR2LAST:
					if(pre_mode)
					{
						if(CF)
						{
							VU=1;
							LCD_ShowString0608(66, 3,  "VHF ", 1, 128);	//No, "KDU_Reflash()" will be updated
							CHAN = 100;
						}
						
					}
					return BACK2MAIN;
			}
			break;
			
		case key_2:
			Light_Mode_Set();
			return BACK2MAIN;;
		
		case key_3:
			if(!FLAG_LINE)
				return NO_OPERATE;
			Enter_Radio();
			return BACK2MAIN;
				
		case key_4:
			if(Home_Mode || !FLAG_LINE)
				return NO_OPERATE;
			pre_mode= VU;
			D_printf("NOW : U_SETTING\n");
			if(CF)
			{
				if (pre_mode == 0)
				{
					VU=1;
					LCD_ShowString0608(66, 3,  "UHF ", 1, 128);	
					CHAN = 100;
				}
			}
			else
				LCD_ShowPIC0608(60, 3, 1, 1);
			
			switch(RT_FREQ_Set(0, 3, (double *)&chan_arv[NOW].RX_FREQ, 1))
			{
				case ENT2LAST: 
					chan_arv[NOW].TX_FREQ = chan_arv[NOW].RX_FREQ;
					sendTo152(_SETCHAN);
					return BACK2MAIN;
								
				case CLR2LAST:
					if(pre_mode == 0)
					{
						if(CF)
						{
							VU=0;
							CHAN = 0;
							LCD_ShowString0608(66, 3,  "VHF ", 1, 128);	//No, "KDU_Reflash()" will be updated
						}
					}
					return BACK2MAIN;
			}
			break;
		
		case key_5:
			if(!FLAG_LINE)
				return NO_OPERATE;
			Zero_Menu();
			return BACK2MAIN;
		
		case key_7:
			OPTION_Menu();
			if (Flag_Main_Page == 0)		
				return BACK2MAIN;
			else
				return NO_OPERATE;
			
			
		case key_8:
			if(!FLAG_LINE)
				return NO_OPERATE;
			PGM_Menu();
			return BACK2MAIN;
   
		case key_a:
		case key_pre_add:
			if(!FLAG_LINE)
				return NO_OPERATE;
			
			if(Home_Mode == DUAL_MODE)
			{
				if(sele_pos)
				{
					if(++chan_arv[CHANB].CHAN>99)
						chan_arv[CHANB].CHAN = 1;
					sendTo152(_RELB);
				}
				else 
				{
					if(++chan_arv[CHANA].CHAN>99)
						chan_arv[CHANA].CHAN = 1;
					sendTo152(_RELA);
				}
				return NO_OPERATE;
			}
			if (CF)		//Frequency mode
			{
				if ((chan_arv[NOW].RX_FREQ >= 136.0 && chan_arv[NOW].RX_FREQ + STEP[STEP_LEVEL] <= 174.0) || 
					(chan_arv[NOW].RX_FREQ >= 400.0 && chan_arv[NOW].RX_FREQ + STEP[STEP_LEVEL] <= 480.0)) //U
					chan_arv[NOW].RX_FREQ += STEP[STEP_LEVEL];
				chan_arv[NOW].TX_FREQ = chan_arv[NOW].RX_FREQ;
				sendTo152(_SETCHAN);
				return 0;
			}
			else   		//Channel mode
			{
				if(Home_Mode==0)
					LCD_ShowPIC0608(60, 3, 1, 1);
				else if(Home_Mode == 1)
				{
					LCD_ShowString1016(20, 3, "       ", 1, 80);
					sprintf(BIG_MODE_buf, "%02d-%s", CHAN, chan_arv[NOW].NN);
					LCD_ShowString1016(0, 3, BIG_MODE_buf, 1, 128);
				}
				if (CHAN < 99)
					CHAN++;
				else
					CHAN = 1;
				sendTo152(_RELOAD);
				return 0;
			}
			//
			
		case key_d:
		case key_pre_sub:
			if(!FLAG_LINE)
				return NO_OPERATE;
			
			if(Home_Mode == DUAL_MODE)
			{
				if(sele_pos)
				{
					if(--chan_arv[CHANB].CHAN<1)
						chan_arv[CHANB].CHAN = 99;
					sendTo152(_RELB);
				}
				else 
				{
					if(--chan_arv[CHANA].CHAN<1)
						chan_arv[CHANA].CHAN = 99;
					sendTo152(_RELA);
				}
				return NO_OPERATE;
			}
			if (CF) 		//Frequency mode
			{
				if ((chan_arv[NOW].RX_FREQ - STEP[STEP_LEVEL] >= 400.0 && chan_arv[NOW].RX_FREQ <= 480.0) || 
					(chan_arv[NOW].RX_FREQ - STEP[STEP_LEVEL] >= 136.0 && chan_arv[NOW].RX_FREQ <= 174.0))
					chan_arv[NOW].RX_FREQ -= STEP[STEP_LEVEL];
				chan_arv[NOW].TX_FREQ = chan_arv[NOW].RX_FREQ;
				sendTo152(_SETCHAN);
				return NO_OPERATE;
			}
			else 			//Channel mode
			{
				if(Home_Mode==0)
					LCD_ShowPIC0608(60, 3, 1, 1);
				else if(Home_Mode == 1)
				{
					LCD_ShowString1016(20, 3, "       ", 1, 80);
					sprintf(BIG_MODE_buf, "%02d-%s", CHAN, chan_arv[NOW].NN);
					LCD_ShowString1016(0, 3, BIG_MODE_buf, 1, 128);
				}
				
				if (CHAN > 1)
					CHAN --;
				else
					CHAN = 99;
				sendTo152(_RELOAD);
				return NO_OPERATE;
			}
			//

		case key_b:
			if(Home_Mode==BIG_MODE || !FLAG_LINE)
				return NO_OPERATE;
			else if(Home_Mode == DUAL_MODE)
			{
				sele_pos = (sele_pos+1)%2;
                sendTo152(_SETDUALPOS);
				return NO_OPERATE;
			}
			
			D_printf("{<}\n");
			CF = !CF;
			CHAN = CF?0:1;
			LCD_ShowPIC0608(60, 3, CF?5:1, 1);
			sendTo152(_RELOAD);
			return NO_OPERATE; 	//BACK2MAIN;	//It can be 0, after all, "KDU_Reflash()" will be updated here

		case key_c:
			if(Home_Mode==BIG_MODE || !FLAG_LINE)
				return NO_OPERATE;
			else if(Home_Mode == DUAL_MODE)
			{
				sele_pos = (sele_pos+1)%2;
                sendTo152(_SETDUALPOS);
				return NO_OPERATE;
			}
			
			D_printf("{>}\n");
			if(Home_Mode)
				return NO_OPERATE;
			
			if(CF)
			{			
				VU = !VU;		//VU is used to obtain the channel number, 0 is V, 1 is U
				CHAN = VU*100;
				sendTo152(_RELOAD);
				return NO_OPERATE; 	//BACK2MAIN;	//It can be 0, after all, "KDU_Reflash()" will be updated here
			}
			else
				LCD_ShowAscii0608(60, 3, ' ', 1);
			
			break;


		case key_clr:
			if(CF || !FLAG_LINE)
				return NO_OPERATE;
			LCD_ShowAscii0608(60, 3, ' ', 1);
			return NO_OPERATE;

		case key_n:
		case key_ent:
			if(Home_Mode || !FLAG_LINE)
				return NO_OPERATE;
			LCD_ShowAscii0608(60, 3, ' ', 1);
			return ShortCut_Menu();			//The lock screen returns "NO_OPERATE", and the rest returns "BACK2MAIN"
		
		case key_tools:
			TOOLS_Menu();			
			return BACK2MAIN;
		}
	return NO_OPERATE;
}
//
//Enter the main interface menu	
//BACK2MAIN main menu when there is no response after sending the confirmation option
int  ShortCut_Menu()
{
	u8 option_num = 0, cursor_change = 1, ENTER = 0;
	
	while (1)
	{
		if(!FLAG_LINE)
			return BACK2MAIN;
		
		MY_GLOBAL_FUN(1);
		switch (KEY_SCAN(0))
		{
			case key_clr: 		//Exit
				LCD_ShowString0408(0, 5, "TYPE   TRF    MOD    CHAN  KEY  ", 1);
				return CLR2LAST;

			case key_ent:
				ENTER = 1;
				break;

			case key_d:
			case key_b:
			case key_pre_sub:
				option_num = (option_num+5-1)%5;
				cursor_change = 1;
				break;

			case key_a:
			case key_c:
			case key_pre_add:
				option_num = (option_num+1)%5;
				cursor_change = 1;
				break;
		}

		if (cursor_change) 		//Display arrows to avoid repeated refreshes
		{
			cursor_change = 0;
			LCD_ShowString0408(0, 5, "TYPE   TRF    MOD    CHAN  KEY  ", 1);
			switch (option_num)
			{
			case 0:
				LCD_ShowPIC0608(16, 5, 0, 1);
				break;
			case 1:
				LCD_ShowPIC0608(40, 5, 0, 1);
				break;
			case 2:
				LCD_ShowPIC0608(70, 5, 0, 1);
				break;
			case 3:
				LCD_ShowPIC0608(100, 5, 0, 1);
				break;
			case 4:
				LCD_ShowPIC0608(122, 5, 0, 1);
				break;
			}
		}
		if(ENTER)
		{
			ENTER = 0;
			switch (option_num)
			{
			case 0: 		//Graphic menu
				RT_Menu();
				return BACK2MAIN;
			
			case 1: 		//Microphone mode
				if(AUD==0)
					break;
				ShortCut_MICGAIN_Select();
				return BACK2MAIN;

			
			case 2: 		//FM broadcast radio
				D_printf("ENTER FM\n");
				ShortCut_FM_Select();
				return BACK2MAIN;

			case 3: 		//Channel number selection in channel mode
				ShortCut_CHAN_Select();
				return BACK2MAIN;

			case 4: 		//Lock screen
				printf("KEY\n");
				LCD_ShowString0408(0, 5, "TYPE   TRF    MOD    CHAN  KEY  ", 1);
				Lock_Screen_KeyBoard();
				LightBacklight();
				while(VOL_ADD_READ == 0 || VOL_SUB_READ == 0)FeedDog();
				return NO_OPERATE;
			}
		}
	}
}
//
int ShortCut_MICGAIN_Select(void)		//The return value is meaningless
{
	char trf = MIC, trf_old = MIC;
	
	LCD_ShowString0608(26, 4, TRF_Show[trf], 0, 128); 	//Display the selected level

	while (1)
	{
		if(FLAG_LINE == OFF)
			return BACK2MAIN;
		
		MY_GLOBAL_FUN(1);
		switch (KEY_SCAN(0))
		{
		case key_a:
		case key_c:
		case key_pre_add:
			trf = (trf + 1) % 3;
			LCD_ShowString0608(26, 4, TRF_Show[trf], 0, 128);
			break;

		case key_b:
		case key_d:
		case key_pre_sub:
			trf = (trf + 2) % 3;
			LCD_ShowString0608(26, 4, TRF_Show[trf], 0, 128);
			break;

		case key_clr:
			LCD_ShowString0608(26, 4, TRF_Show[trf_old], 1, 128);
			return CLR2LAST;

		case key_ent:
		case key_n:
			LCD_ShowString0608(26, 4, TRF_Show[trf], 1, 128);
			MIC = trf * 3 + 1;
			sendTo152(_SETAUD);
			return ENT2LAST;
		}
		//
	}
	//
}
//To modify (?)
int  ShortCut_FM_Select(void)		//The return value is meaningless
{
    char  FM_now = WFM, FM_old = WFM;
	LCD_ShowString0608(56, 4, FM_Show[FM_now], 0, 128);
	while (1)
	{
		MY_GLOBAL_FUN(1);
		switch (KEY_SCAN(0))
		{
			case key_pre_sub:
			case key_pre_add:
			case key_a:
			case key_b:
			case key_c:
			case key_d:
				FM_now = !FM_now;
				LCD_ShowString0608(56, 4, FM_Show[FM_now], 0, 128);
				break;

			case key_n:
			case key_ent:
				LCD_ShowString0608(56, 4, FM_Show[FM_now], 1, 128);
				if(WFM == FM_now)
					return ENT2LAST;

				WFM = FM_now;
				sendTo152(_SETFM);	//Wait for a reply before further processing
				if(WFM)
				 Enter_Radio();		//When not in FM mode, turn on FM to send a request and after receiving a reply, 
							//enter FM mode (same as 152 processing)
				return ENT2LAST;

			case key_clr:
				LCD_ShowString0608(56, 4, FM_Show[FM_old], 0, 128);
				return 0;
		}
	}
	//
}
//Channel selection information display on the main page
void Channel_Info_Show(u8 channel)	
{
	sendTo152(_ASKCHAN);
	LCD_ShowFreq(0, 3, chan_arv[NOW].RX_FREQ, 1);
	LCD_ShowChan(83, 4, CHAN, 0);		//Display channel number
}

int  ShortCut_CHAN_Select(void)			//The return value is meaningless
{
	int channel = 1;
	channel = CHAN; 			//Keep the current channel number, if other channels are displayed when exiting, reload
	LCD_ShowChan(83, 4, channel, 0);
	
	chan_arv[TMP]=chan_arv[NOW];
	
	while (1)
	{
		MY_GLOBAL_FUN(1);
		if(FLAG_LINE==0)
			return BACK2MAIN;
		switch (KEY_SCAN(0))
		{
		case key_0:
			CHAN = 1;
			Channel_Info_Show(CHAN);
			break;

		case key_a:
		case key_pre_add:
			CHAN++;
			if (CHAN > 99) CHAN = 1;
			Channel_Info_Show(CHAN);
			break;

		case key_d:
		case key_pre_sub:
			CHAN--;
			if (CHAN < 1) CHAN = 99;
			Channel_Info_Show(CHAN);
			break;
		
		case key_n:
		case key_ent:
			LCD_ShowAscii0608(60, 3, ' ', 1);
			sendTo152(_SETCHAN);
			return ENT2LAST; 

		case key_clr:
			if (CHAN != channel)
				chan_arv[NOW] = chan_arv[TMP];
			LCD_ShowAscii0608(60, 3, ' ', 1);
			return CLR2LAST;
		}
	}
}



//


//Send and receive settings
//The return value is meaningless BACK2MAIN
int  RT_Menu()
{
	unsigned char 			
		now_mode = 0,						//Currently set frequency band
		pre_mode = 0,						//Enter the previous frequency band		
		change = 0,						//Flag: Save data modification
		FLAG_Modify = 0,					//Flag: Confirm the option and enter the modification
		pos = 0;						//The option where the cursor is located, MATRIX_MENU1_TOTALN1 is the maximum number of items
	pre_mode = (chan_arv[NOW].RX_FREQ>174);
	now_mode = pre_mode;
	
	RT_Menu_Clear();
	LCD_ShowMatrixMenu22(matrix_menu1, 8, pos);
	while (1)
	{
		if(FLAG_LINE == 0)
			return BACK2MAIN;
		MY_GLOBAL_FUN(1);
		switch (KEY_SCAN(0))
		{
			case key_1:
				if(pos==0)
				{
					pre_mode = (chan_arv[NOW].RX_FREQ>174);		//Get the frequency band before entering
					now_mode = 0;					//Press [1] to enter the V-segment settings
					FLAG_Modify = 1;	
				}
				else if(pos==2)
				{
					if(chan_arv[NOW].RX_FREQ>174)			//If the receiving frequency is U segment, it is ignored
											//Determine the transmission band setting according to the receiving band
						break;
					now_mode = 0;					//Press [1] to enter the V-segment settings
					FLAG_Modify = 1;	
				}
				break;
				
			case key_4:
				if(pos==0)
				{
					pre_mode = (chan_arv[NOW].RX_FREQ>174);		//Get the frequency band before entering
					now_mode = 1;					//Press [4] to enter the U-segment settings
					FLAG_Modify = 1;
				}
				else if(pos==2)
				{
					if(chan_arv[NOW].RX_FREQ<400)			//If the receiving frequency is in the V segment, it is ignored
						break;
					now_mode = 1;					//Press [4] to enter the U-segment settings
					FLAG_Modify = 1;
				}
				
				break;
				
			case key_n:
			case key_ent:
				if(pos==6 && CF)
					break;
				
				if(pos == 0 || pos == 2)
				{
					pre_mode =  (chan_arv[NOW].RX_FREQ>174);	//Get the frequency band before entering
					now_mode = pre_mode;
				}
				FLAG_Modify = 1;
				break;	
				
				
			case key_clr:
				printf("Return\n");
				return BACK2MAIN;

			case key_a:
			case key_pre_add:
				D_printf("Turn Up\n");
				pos = (pos + 8 - 2) % 8;	
				LCD_ShowMatrixMenu22(matrix_menu1, 8, pos);
				break;

			case key_d:
			case key_pre_sub:
				D_printf("Turn Down\n");
				pos = (pos + 2) % 8;
				LCD_ShowMatrixMenu22(matrix_menu1, 8, pos);
				break;

			case key_b:
				D_printf("Turn Left\n");
				pos = (pos + 8 - 1) % 8;	
				LCD_ShowMatrixMenu22(matrix_menu1, 8, pos);
				break;

			case key_c:
				D_printf("Turn Right\n");
				pos = (pos + 1) % 8;
				LCD_ShowMatrixMenu22(matrix_menu1, 8, pos);
				break;
		};
		if (FLAG_Modify)
		{
			switch (pos)
			//Enter the menu according to the current position pressed
			{
			case 0:
				D_printf("\t\t\t\tRx FREQ SETTING\n");
				if (now_mode)
					LCD_ShowString0408(0, 5, "  FRE SET:400-480MHZ,CLR OR ENT ", 1);
				else
					LCD_ShowString0408(0, 5, "  FRE SET:136-174MHZ,CLR OR ENT ", 1);

				if (RT_FREQ_Set(12, 3, (double *)&chan_arv[NOW].RX_FREQ, now_mode)==ENT2LAST)
				{
					chan_arv[NOW].TX_FREQ = chan_arv[NOW].RX_FREQ;
					change = 1;
				}
				else
					LCD_ShowMatrixMenu22(matrix_menu1, 8, pos);
				
				break;

			case 1:
				D_printf("\t\t\t\tRS CTCSS SETTING\n");
				chan_arv[NOW].RS = RT_SubVoice_Set(3, chan_arv[NOW].RS);

				change = 1;
				break;

			case 2:
				D_printf("\t\t\t\tTx FREQ SETTING\n");
				if (now_mode)
					LCD_ShowString0408(0, 5, "  FRE SET:400-480MHZ,CLR OR ENT ", 1);
				else
					LCD_ShowString0408(0, 5, "  FRE SET:136-174MHZ,CLR OR ENT ", 1);

				if(RT_FREQ_Set(12, 4, (double *)&chan_arv[NOW].TX_FREQ, now_mode)==ENT2LAST)
					change = 1;
				else
					LCD_ShowMatrixMenu22(matrix_menu1, 8, pos);
				
				break;

			case 3:
				D_printf("\t\t\t\tTS CTCSS SETTING\n");
				chan_arv[NOW].TS = RT_SubVoice_Set(4, chan_arv[NOW].TS);	//The return value will not be affected, and it will only be clear 
												//whether the connection has been disconnected after the data is sent.

				change = 1;
				break;

			case 4:
				D_printf("\t\t\t\tPower SETTINF\n");
				RT_TX_POWER_Set(chan_arv[NOW].POWER);
			
				change = 1;
				break;

			case 5:
				D_printf("\t\t\t\tGBW SETTING\n");
				RT_GBW_Set(chan_arv[NOW].GBW);
			
				change = 1;
				break;

			case 6:
				D_printf("\t\t\t\tNickName SETTING\n");
				RT_NICKNAME_Set();
			
				change = 1;
				break;

			case 7:
				D_printf("\t\t\t\tChannel Select\n");		//No need to enter "setchan"

				RT_CHAN_Switch();
				RT_Menu_Clear();
				LCD_ShowMatrixMenu22(matrix_menu1, 8, pos);
				break;
			};

			if (change)						//The parameters have changed, enter the settings
			{
				change = 0;
				sendTo152(_SETCHAN);
				RT_Menu_Clear();
				LCD_ShowMatrixMenu22(matrix_menu1, 8, pos);
			}
			FLAG_Modify = 0;
		}
	}
}
void RT_Menu_Clear()				//Interface display
{
	sprintf(matrix_menu1[0][0], "R:%3.4f", chan_arv[NOW].RX_FREQ);
	sprintf(matrix_menu1[0][1], "%s", menu_subvoice[chan_arv[NOW].RS]);
	sprintf(matrix_menu1[1][0], "T:%3.4f", chan_arv[NOW].TX_FREQ);
	sprintf(matrix_menu1[1][1], "%s", menu_subvoice[chan_arv[NOW].TS]);

	matrix_menu1[3][0][0] = 'C';
	matrix_menu1[3][0][1] = 'N';
	matrix_menu1[3][0][2] = ':';
	
	memset(matrix_menu1[3][0]+3, 32, 7);
	if (CF) 				//FREQ
	{
		sprintf((char *)matrix_menu1[3][1], "FREQ    ");
		sprintf(matrix_menu1[3][0]+3, "%s", VU ? "UHF  " : "VHF  ");
	}
	else 					//CHAN
	{
		sprintf((char *)matrix_menu1[3][1], "CH-%02d   ", CHAN);
		memcpy(matrix_menu1[3][0]+3, (char *)chan_arv[NOW].NN, 7);
	}
	sprintf((char *)matrix_menu1[2][0], chan_arv[NOW].POWER ? "PWR:LOW    " : "PWR:HIGH  ");
	sprintf((char *)matrix_menu1[2][1], chan_arv[NOW].GBW   ? "BW:WIDE "    : "BW:NARR ");
}
//
//Frequency verification
//    0: Frequency does not need to be corrected
//Other: Corrected frequency
int checkFreq(int freq_tmp)
{
	int step_temp[3]={50, 100, 125};
	int mul = 0;
	D_printf("freq_tmp:%d\n", freq_tmp);
	if(CF)
	{
		if(freq_tmp%step_temp[STEP_LEVEL]==0)
			return 0;
	}
	else
	{
		for(int i=0; i<3; i++)
		{
			if(freq_tmp%step_temp[i]==0)
				return 0;
		}
	}
	//The default is to set the step correction frequency
	mul = freq_tmp/step_temp[STEP_LEVEL]+0.5;
	D_printf("mul:%d, return: %d\n", mul, mul*step_temp[STEP_LEVEL]);
	return (mul*step_temp[STEP_LEVEL]);
}
//

//Return value: ENT2LAST/CLR2LAST
int RT_FREQ_Set(int x, int y, double *result, int vu_mode)
{
	unsigned char 
		locate = x + 6, 
		bit = 1, 
		flag_finish = 0,		  /* "flag_finish" Stop entering the flag (complete) */
		key_result = 23, 
		num_input = 0, 
		freq_buf[8] = {0};
	
	int int_freq = 0;
	double freq = *result;
	LCD_ShowString0608(x, y, "         ", 1, x+ 48);
	LCD_ShowPIC0608(x + 48, y, 0, 1);	
		
	if(vu_mode)
		freq_buf[0] = 4;
	else
		freq_buf[0] = 1;
	
	LCD_ShowAscii0608(x, y, freq_buf[0]+'0', 1); 		//After entering, the first place is set to "4"
	//
	
	while (1)
	{
		MY_GLOBAL_FUN(1);
		if (locate == (18 + x)) 			//Add a '.' after the 3-unit integer
		{
			LCD_ShowAscii0608(locate, y, '.', 1);
			locate += 6;
			bit++;
		}
		if (locate > x && locate < 48 + x) 		//The unset bit is set to '_'
			LCD_ShowAscii0608(locate, y, '_', 1);
		if (locate == 48 + x || bit == 8) 		//8 units set up
			flag_finish = 1;
		
		key_result = KEY_SCAN(0);
		switch (key_result)
		{
			case key_0:
				D_printf("Press{0}");
				if (
					(vu_mode && freq_buf[1] <= 8 && bit >= 1) || 				//  400-480
					(vu_mode ==0 && 
						(bit > 1 && 
							((freq_buf[1] >= 4 && freq_buf[1] < 8) || 		//  140-170
							(freq_buf[1] == 3 && freq_buf[2] >= 6) || 		//  136.0
							(freq_buf[1] == 7 && freq_buf[2] <= 4))))		//  174.0
					)
					num_input = 1;
				break;

				
			case key_1:
			case key_2:
				D_printf("Press{%d}", key_result);
				if (
					(vu_mode && (bit == 1 || freq_buf[1] <= 7))	||				//  41x - 47x
					(vu_mode ==0 &&
						((bit==2 && freq_buf[1] > 3 && freq_buf[1] < 8)  || 			//  141 142 	171	 172	
						 (bit>2 && ((freq_buf[1] > 3  && freq_buf[1] < 7) ||
									(freq_buf[1] == 3 && freq_buf[2] > 5) || 	//  136.1 136.2	
									(freq_buf[1] == 7 && freq_buf[2] < 4)))))	//  173.1 173.2
					)
					num_input = 1;
				
				break;
				
				
			case key_3:
				D_printf("Press{3}");
				if (
					(vu_mode && (bit == 1 || freq_buf[1] <= 7))	||				//  43x/45x  473、475
					(vu_mode==0 && 
						(bit == 1 || 								//  13x 15x
						(bit == 2 && (freq_buf[1] > 3 && freq_buf[1] < 8)) 		|| 	//  143 173
						(bit > 2  && ((freq_buf[1] == 3 && freq_buf[2] >= 6) 	|| 		//  136.x
									  (freq_buf[1] == 7 && freq_buf[2] < 4)))))	//  173.X
					)
					num_input = 1;
				break;
				
			case key_5:
				D_printf("Press{5}");
				if (
					(vu_mode && (bit == 1 || freq_buf[1] <= 7))	||		//  43x/45x  473、475
					(vu_mode==0 && 							
					(	 bit == 1 || 						//  13x 15x
						(freq_buf[1] > 3 && freq_buf[1] < 7) 	|| 		//  145 165
						(bit > 2  &&
						 ((freq_buf[1] == 3 && freq_buf[2] >= 6)|| 		//  136.x
						  (freq_buf[1] == 7 && freq_buf[2] <  4)))	
					)
					)								//  173.X
				   )		
					num_input = 1;
				break;
				
				
			case key_4:
				D_printf("Press{4}");
				if (
					(vu_mode && (bit <= 1 || freq_buf[1] <= 7)) ||
					(vu_mode==0 && 
						(bit == 1 || 
						(freq_buf[1] >= 4 && freq_buf[1] < 7) 	|| 
						(freq_buf[1] == 7 && bit == 2) 			|| 
						(freq_buf[1] == 3 && freq_buf[2] >= 6) 	|| 
						(freq_buf[1] == 7 && freq_buf[1] < 4)))
					)
					num_input = 1;
				break;	
				
				
			case key_6:
			case key_7:
				D_printf("Press{%d}", key_result);
				if (
					(vu_mode && (bit == 1 || freq_buf[1] <= 7)) ||			//  47X
					(vu_mode == 0 && 
						(bit == 1 || 						//  16X  17X
						(freq_buf[1] > 2 && freq_buf[1] < 7) 	|| 		//  136
						(bit > 2 && freq_buf[1] == 7 && freq_buf[2] < 4)))	//  173.X
					)
					num_input = 1;
				break;

				
			case key_8:
				D_printf("Press{%d}", key_result);
				if (
					(vu_mode && (bit == 1 || freq_buf[1] <= 7)) ||
					(vu_mode ==0 && 
						((freq_buf[1] >= 3 && freq_buf[1] <= 6) || 
						 (bit > 2 && freq_buf[1] == 7 && freq_buf[2] < 4)))
					)
					num_input = 1;
				break;


			case key_9:
				D_printf("Press{9}");
				if (
					(vu_mode && freq_buf[1] <= 7) ||
					(vu_mode==0 &&
						((freq_buf[1] >= 3 && freq_buf[1] <= 6) || 
						 (bit > 2 && freq_buf[1] == 7 && freq_buf[2] < 4)))
				)
					num_input = 1;
				break;

				
			case key_clr:
				if (locate == x + 6) 					//exit
				{
					D_printf("Press{CLR} : Exit\n");
					LCD_ShowFreq(x, y, *result, 1);
					return CLR2LAST;
				}
				else 							//clear
				{
					D_printf("Press{CLR} : %d\n", locate); 		//A channel
					LCD_ShowString0608(x, y,  "        ", 1, 120);

					while (bit--)
						freq_buf[bit] = 0;
					if(vu_mode)
					{
						freq_buf[0] = 4;
						LCD_ShowAscii0608(x, y, '4', 1);
					}
					else
					{
						freq_buf[0] = 1;
						LCD_ShowAscii0608(x, y, '1', 1);
					}
					locate = x + 6;
					bit = 1;
				}
				break;

			case key_n:
			case key_ent:
				D_printf("Press{ENT}\n"); 				//B channel
				flag_finish = 1;
				break;
			
		}
		if (num_input) 								//Digital deposit (?)
		{
			num_input = 0;
			D_printf("_[%d]_: %d\n", bit, key_result);
			freq_buf[bit] = key_result;
			bit++;
			LCD_ShowAscii0608(locate, y, key_result + '0', 1);
			locate += 6;
		}

		//
		if (flag_finish) 							//Input is complete
		{
			LCD_ShowString0608(x + 48, y,  " ", 1, x+54);
			if (locate < 48 + x || bit < 7) 				//Press the 'OK' button, the zero is not written and the zero is filled
			{
				for (; bit < 8; bit++)
					freq_buf[bit] = 0;
			}
			freq = (vu_mode ? 400 : 100);
			freq = freq + freq_buf[1] * 10 + freq_buf[2] + freq_buf[4] * 0.1 + freq_buf[5] * 0.01 + freq_buf[6] * 0.001 + freq_buf[7] * 0.0001;
			
			int_freq = freq_buf[0]*1000000 + freq_buf[1]*100000 + freq_buf[2]*10000 + 
								freq_buf[4]*1000 + freq_buf[5]*100 + freq_buf[6]*10 + freq_buf[7];
			if(freq<136.0)
				freq = 136.0;
			else if ((freq>174.0&&freq<400.0) || freq > 480.0 )
				freq = *result;
			else
			{
				int res = checkFreq(int_freq);
				if(res>0)
					freq = (double)res/10000;
			}
			*result = freq;
			LCD_ShowFreq(x, y, freq, 0); 			//Frequency display
			return ENT2LAST;
		}
	}
}
//
//Subsonic setting  
//The return value is the set sub-tone
int  RT_SubVoice_Set(int row, int subvoice) 				//The first row shows the first subvoice subvoice
{
	int subvoice_temp = subvoice;
	
	LCD_ShowPIC0608(116, row, 0, 1);
	LCD_ShowString0408(0, 5, " CTCSS SET,   OR   ,CLR OR ENT  ", 1);
	LCD_ShowPIC0408(48, 5, 0);
	LCD_ShowPIC0408(68, 5, 1);
	
	while (1)
	{
		MY_GLOBAL_FUN(1);
		switch (KEY_SCAN(0))
		{
		case key_0:
			subvoice = 0;
			LCD_ShowString0608(68, row, "        ", 1, 128);
			LCD_ShowString0608(68, row, menu_subvoice[0], 0, 128);
			break;

		case key_clr:						//Cancel the setting and return
			LCD_ShowAscii0608(116, row, ' ', 1);
			return subvoice_temp;
		
		case key_ent:						//Confirm
			LCD_ShowAscii0608(116, row, ' ', 1);
			if (subvoice < 0 || subvoice > 121)
				return 0;
			else
				return subvoice;

		case key_b:
		case key_d:
		case key_pre_sub:	
			//Select the previous sub-tone
			if (subvoice > 0)
				subvoice--;
			LCD_ShowString0608(68, row, "        ", 1, 128);
			LCD_ShowString0608(68, row, menu_subvoice[subvoice], 0, 128);
			break;

		case key_a:		
		case key_c:
		case key_pre_add:
			//Select the latter sub-tone
			if (subvoice < 121)
				subvoice++;
			LCD_ShowString0608(68, row, "        ", 1, 128);
			LCD_ShowString0608(68, row, menu_subvoice[subvoice], 0, 128);
			break;

		case key_5:
			subvoice = RT_SubVoice_Matrix_Menu_Select(subvoice);
			return subvoice;
		}
	}
}


int  RT_SubVoice_Matrix_Menu_Select(int subvoice)		//Sub-tone setting: matrix sub-tone selection
{
	int subvoice_temp = subvoice;

	LCD_Clear(EDITZONE64);
	LCD_ShowMatrixMenu33(matrix_menu_subvoice, 122, subvoice);
	while (1)
	{
		MY_GLOBAL_FUN(1);
		switch (KEY_SCAN(0))
		{
		case key_clr:
			return subvoice_temp;

		case key_ent:
			if (0 > subvoice || subvoice >= 122)
				return 0;
			return subvoice;
			
		case key_a:
		case key_pre_add:
			switch(subvoice)
			{
				case 0: 
					subvoice = 120;
				break;
				case 1: 
					subvoice = 121;
				break;
				case 2: 
					subvoice = 119;
				break;
				
				default:
					subvoice-=3;
				break;
			}
			LCD_ShowMatrixMenu33(matrix_menu_subvoice, 122, subvoice);
			break;
			
		case key_d:
		case key_pre_sub:
			switch(subvoice)
			{
				case 119: 
					subvoice = 2;
				break;
				case 120: 
					subvoice = 0;
				break;
				case 121: 
					subvoice = 1;
				break;
				
				default:
					subvoice+=3;
			}
			LCD_ShowMatrixMenu33(matrix_menu_subvoice, 122, subvoice);
		break;
			
		case key_b:
			if(subvoice-1 < 0)
				subvoice = 121;
			else
				subvoice --;
			
			LCD_ShowMatrixMenu33(matrix_menu_subvoice, 122, subvoice);
			break;	
			
		case key_c:
			if(subvoice+1 > 121)
				subvoice = 0;
			else
				subvoice ++;
			LCD_ShowMatrixMenu33(matrix_menu_subvoice, 122, subvoice);
			
			break;
			
		case key_0:
			subvoice = 0;;
			LCD_ShowMatrixMenu33(matrix_menu_subvoice, 122, subvoice);
		}
	}
}
//Transmit power setting
//The return value is meaningless
int  RT_TX_POWER_Set(int power)	
{
	unsigned char power_temp = chan_arv[NOW].POWER;
	LCD_ShowPIC0608(60, 3, 0, 1);
	while (1)
	{
		MY_GLOBAL_FUN(1);
		switch (KEY_SCAN(0))
		{
		case key_clr:
			return CLR2LAST;

		case key_ent:
			chan_arv[NOW].POWER = power_temp;
			return ENT2LAST;

		case key_a:
		case key_b:
		case key_c:
		case key_d:
		case key_pre_add:
		case key_pre_sub:
			power_temp = !power_temp;
			LCD_ShowString0608(24, 3, POWER_SHOW[power_temp], 0, 128);
			break;
		}
	}
}
//Bandwidth setting
//The return value is meaningless
int  RT_GBW_Set(int gbw)		
{
	char gbw_temp = chan_arv[NOW].GBW;
	LCD_ShowPIC0608(116, 3, 0, 1);
	while (1)
	{
		MY_GLOBAL_FUN(1);
		switch (KEY_SCAN(0))
		{
		case key_clr:
			return CLR2LAST;

		case key_ent:
			chan_arv[NOW].GBW = gbw_temp;
			return ENT2LAST;

		case key_a:
		case key_b:
		case key_c:
		case key_d:
		case key_pre_add:
		case key_pre_sub:
			gbw_temp = (gbw_temp + 1) % 2;
			LCD_ShowString0608(86, 3, gbw_temp == 1 ? "WIDE" : "NARR", 0, 128);
			break;
		}
	}
}

//Channel alias
//The return value is meaningless
int  RT_NICKNAME_Set()
{
	char nn[7] = "";
	u8 result_matrix,				//Current trigger button
		key_old = key_no, 			//Last key value
		nn_locate = 0,				//Current cursor position
		nn_locate_change = 0,			//The cursor position changes, used to refresh 'press_times'
		locate_change = 1,			//Refresh the selected location
		press_times = 0,			//Current number of button presses
		clear = 0;				//The edit bar is cleared, 0 is not cleared, 1 is cleared

	if (CF)
		return NO_OPERATE;

	for (u8 i = 0; i < 7; i++)
		nn[i] = chan_arv[NOW].NN[i];

	LCD_ShowString0608(0, 4, "CN:", 1, 128);
	LCD_ShowPIC0608(60, 4, 0, 1);

	while (1)
	{
		MY_GLOBAL_FUN(1);
		result_matrix = KEY_SCAN(0);
		switch (result_matrix)
		{
		case key_clr:
			if (clear) 			//Cleared, returns the initial value
				return CLR2LAST;
			else 				//Not cleared, clear the edit bar
			{
				memset(nn, ' ', 7);
				locate_change = 1;
				nn_locate = 0;
				clear = 1;
			}
			break;

		case key_n:
		case key_ent:
			strcpy((char *)chan_arv[NOW].NN, nn);
			return ENT2LAST;

		case key_0:
		case key_1:
		case key_2:
		case key_3:
		case key_4:
		case key_5:
		case key_6:
		case key_7:
		case key_8:
		case key_9:
			
			clear = 0;
			press_times++;
			press_times %= 9;

			if(result_matrix != key_old || nn_locate_change )
			{
				press_times = 0;
				key_old = result_matrix;
				nn_locate_change = 0;
			}
			nn[nn_locate] = square_9[result_matrix][press_times];
			LCD_ShowAscii0608(18 + nn_locate * 6, 4, nn[nn_locate], 0);
			break;

		case key_b:
			nn_locate--;
			locate_change = 1;
			if (nn_locate > 6)
				nn_locate = 6;
			break;

		case key_c:
			nn_locate++;
			locate_change = 1;
			if (nn_locate > 6)
				nn_locate = 0;
			break;

		case key_a:
		case key_pre_add:
			nn[nn_locate] = (nn[nn_locate] - 32 + 1) % 95 + 32;
			clear = 0;
			LCD_ShowAscii0608(18 + nn_locate * 6, 4, nn[nn_locate], 0);
			break;

		case key_d:
		case key_pre_sub:
			nn[nn_locate] = nn[nn_locate] + 95 - 1 % 95 > 127 ? nn[nn_locate] - 1 % 95 : nn[nn_locate] + 95 - 1 % 95;
			clear = 0;
			LCD_ShowAscii0608(18 + nn_locate * 6, 4, nn[nn_locate], 0);
			break;
		};
		//
		
		if (locate_change) 			//Cursor position modification
		{
			locate_change = 0;
			nn_locate_change = 1;
			LCD_ShowString0608(18, 4, "       ", 1, 128);
			LCD_ShowString0608(18, 4, (char *)nn, 1, 128);
			if(nn[nn_locate])
				LCD_ShowAscii0608(18 + nn_locate * 6, 4, nn[nn_locate], 0); 		//Show currently selected
			else
				LCD_ShowAscii0608(18 + nn_locate * 6, 4, ' ', 0);
		}
	}
}
//Channel switching
int  RT_CHAN_Switch()
{
	u8 change = 0,
	   chan_temp = CHAN,
	   pre_chan  = 1;
	
	char chan_buf[7]={0};
	
	u8 	
		pre_cf    = CF,
		pre_vu    = VU,
		now_cf	  = pre_cf,
		now_vu	  = pre_vu;
	
	chan_arv[TMP] = chan_arv[NOW];
	LCD_ShowPIC0608(116, 4, 0, 1);

	while (1)
	{
		MY_GLOBAL_FUN(1);
		if(FLAG_LINE==0)
			return BACK2MAIN;
		switch (KEY_SCAN(0))
		{
			case key_0:
				if (now_cf)
					break;
				change = 1;
				CHAN = 1;
				break;

			case key_clr:
				//Cancel the setting and request the original channel parameters
				if(CHAN != chan_temp)
				{
					CHAN = chan_temp;
					sendTo152(_ASKCHAN);
					return CLR2LAST;
				}
				return CLR2LAST;
			
			case key_b:
				now_cf = !now_cf;
				if(now_cf)
				{
					pre_chan = CHAN;
					CHAN = now_vu*100;
				}
				else
					CHAN = pre_chan;
				change=1;
				break;
			
			case key_c:
				if(now_cf)
				{			
					now_vu = !now_vu;
					CHAN = now_vu*100;
					change=1;
				}
			break;		
				
			
		case key_n:
		case key_ent:
			//Confirm the current channel and overload the parameters
			CF = now_cf;
			VU = now_vu;
			sendTo152(_RELOAD);
			return ENT2LAST;

		case key_d:
		case key_pre_sub:
			if (CHAN > 1)
			{
				change = 1;
				CHAN--;
			}
			break;

		case key_a:
		case key_pre_add:
			if (CHAN < 99)
			{
				change = 1;
				CHAN++;
			}
			break;
		}

		if (change)
		{
			change = 0;
			
			//Request to modify the current channel number parameter
			sendTo152(_ASKCHAN);
			

			LCD_ShowString0608(86, 3, chan_arv[NOW].GBW ?   "WIDE" : "NARR", 1, 128);
			LCD_ShowString0608(24, 3, chan_arv[NOW].POWER ? "LOW " : "HIGH", 1, 128);
			
			if(now_cf)
				strcpy((char *)chan_buf, "FREQ    ");
			else
				sprintf((char *)chan_buf, "CH-%02d  ", CHAN);
			
			LCD_ShowString0608(18, 4, "       ", 1, 128);
			LCD_ShowString0608(68, 4, "       ", 0, 128);
			LCD_ShowString0608(18, 4, (char *)chan_arv[NOW].NN, 1, 128);
			LCD_ShowString0608(68, 4, (char *)chan_buf, 0, 128);
		}
	}
}
//

//Button '2' to enter the backlight and brightness settings
//CLR2LAST: Return to the previous level
//ENT2LAST: Set to exit successfully
//BACK2MAIN: Button '2(LT)' radio/KDU control exit
u8 Screen_Contrast_Set(void)
{
	LCD_ShowString0608(19, 3, "SCREEN CONTRAST", 1, 128);
	LCD_ShowContrast(SC);
	while(1)
	{
		MY_GLOBAL_FUN(1);
		switch (KEY_SCAN(0))
		{
			case key_2:
				return BACK2MAIN;
			case key_clr:
				return CLR2LAST;
			
			case key_n:
			case key_ent:
				return ENT2LAST;
			
			case key_a:
			case key_c:
			case key_pre_add:
				if(SC+1>6)
					SC=6;
				else
					SC ++;
//				LCD_LightRatio(36+SC*3);
				LCD_LightRatio(21+SC*5);
				LCD_ShowContrast(SC);
				break;

			case key_b:
			case key_d:
			case key_pre_sub:
				if(SC-1<0)
					SC=0;
				else
					SC --;
				LCD_LightRatio(21+SC*5);
				LCD_ShowContrast(SC);
				break;		
		}
	}
}



//CLR2LAST: Return to the previous level
//BACK2MAIN: Button '2(LT)' radio/KDU control exit/enter the contrast setting after the setting is complete
u8 Light_Intensity_set(void)
{
	u8 ENTER = 0;
	LCD_ShowString0608(19, 3, "LIGHT INTENSITY", 1, 128);
	LCD_ShowString0608(0,  4, "                      ", 1, 128);	
	LCD_ShowBackLight(BL);
	while(1)
	{
		MY_GLOBAL_FUN(1);
		switch (KEY_SCAN(0))
		{
			case key_2:
				return BACK2MAIN;
			case key_clr:
				return CLR2LAST;
			
			case key_n:
			case key_ent:
				ENTER = 1;
				break;
			
			case key_a:
			case key_c:
			case key_pre_add:
				if(BL+10>95)
					BL=100;
				else
					BL+=10;
			 	BackLight_SetVal(BL);
				LCD_ShowBackLight(BL);
				break;

			case key_b:
			case key_d:
			case key_pre_sub:
				if(BL-10<0)
					BL=0;
				else
					BL-=10;
				BackLight_SetVal(BL);
				LCD_ShowBackLight(BL);
				break;		
		}
		if(ENTER)
		{
			ENTER = 0;
			if(Screen_Contrast_Set())
				return 1;
			LCD_ShowString0608(19, 3, "LIGHT INTENSITY", 1, 128);
			LCD_ShowBackLight(BL);
		}
	}
}

void Light_Mode_Clear(u8 sel_pos)
{
	LCD_Clear(EDITZONE64);
	LCD_ShowString0608(34, 3, "LIGHT MODE", 1, 128);
	LCD_ShowOption(37, 4, LIGHT_SHOW, 2, sel_pos);
	LCD_ShowString0408(20, 5, " TO CHANGE / ENT TO CONT", 1);
	
	LCD_ShowPIC0408(10, 5, 0);
	LCD_ShowPIC0408(15, 5, 1);

}
void Light_Mode_Set(void)
{
	u8 BL_temp = BL;
	static u8 pos=0;
	Light_Mode_Clear(pos);
	
	while(1)
	{
		MY_GLOBAL_FUN(1);
		switch (KEY_SCAN(0))
		{
			case key_2:
			case key_clr:
				BackLight_SetVal(BL_temp);
				return;
			
			case key_n:
			case key_ent:
				if(pos)		//OFF
				{
					if(Screen_Contrast_Set())
						return;
				}
				else
				{
					if(Light_Intensity_set())
						return;
				}
				Light_Mode_Clear(pos);
				break;
			
			case key_a:
			case key_c:
			case key_b:
			case key_d:
				pos = !pos;
				BL = pos?0: BL_temp;
				LCD_ShowString0608(0,  4, "                      ", 1, 128);
				LightBacklight();
				Light_Mode_Clear(pos);
				break;		
		}
		//		
	}
	//
}
// 


//Button '5(Zero)' radio's memory zeroize initialization
void Zero_Menu(void)
{
	u8 pos = 0;
	int ENSURE=0;
	LCD_Clear(EDITZONE64);
	LCD_ShowMenu31(MENU_ZERO, MENU_ZERO_NUM, pos);
	
	while(1)
	{
		if(!FLAG_LINE)
			return;
		MY_GLOBAL_FUN(1);
		switch (KEY_SCAN(0))
		{
			case key_5:
			case key_clr:
				return;
			
			case key_n:
			case key_ent:
				ENSURE=1;
				break;
			
			case key_a:
			case key_c:
				pos = (pos + 1) % MENU_ZERO_NUM;
				LCD_ShowMenu31(MENU_ZERO, MENU_ZERO_NUM, pos);
				break;

			case key_b:
			case key_d:
				pos = (pos + MENU_ZERO_NUM - 1) % MENU_ZERO_NUM;
				LCD_ShowMenu31(MENU_ZERO, MENU_ZERO_NUM, pos);
				break;
		}
///////////////////////////////////////////////////////
		if(ENSURE)
		{
			ENSURE=0;
			switch(pos)
			{
				case 0:
					D_printf("ZEROIZE ALL\n");
					ENSURE = Zeroize_All();
					break;
			}
			if(ENSURE == 2)
				return;
			ENSURE = 0;
			LCD_ShowMenu31(MENU_ZERO, MENU_ZERO_NUM, pos);
		}
	}
}
//The return value has no special meaning
int Zeroize_All(void)
{
	u8 pos = 0;

	LCD_Clear(EDITZONE64);
	LCD_ShowString0608(25 ,  3, "ZEROIZE RADIO",  1, 128);
	LCD_ShowOption(55, 4, CONFIRM_OPT, 2, pos);
	LCD_ShowString0408(18 ,  5, "TO SCROLL / ENT TO CONT",  1);

	while(1)
	{
		MY_GLOBAL_FUN(1);
		switch(KEY_SCAN(0))
		{
			case key_a:
			case key_b:
			case key_c:
			case key_d:
            case key_pre_add:
            case key_pre_sub:
				pos = !pos;
				LCD_ShowOption(55, 4, CONFIRM_OPT, 2, pos);
				break;
			
			case key_5:
				return BACK2MAIN;
			
			case key_clr:
				return CLR2LAST;
			
			case key_n:
			case key_ent:
				if(pos)
				{
					sendCommand(_SETZERO);
					LCD_ShowString1016(14, 3, "WAITING...", 1, 128);
					delay_ms(33000);
					ResetSystem();
				}
				else
					return ENT2LAST;
		}
		//
	}
}
//

//Button '7(OPT)' option menu
void OPTION_Menu(void)
{
	Flag_Main_Page = 0;
	u8 num = 0;
	int ENSURE=0;
	
	LCD_Clear(EDITZONE64);
	
	LCD_ShowMenu31(MENU_OPT, MENU_OPT_NUM, num);
	while (1)
	{
		MY_GLOBAL_FUN(1);
		switch (KEY_SCAN(0))
		{
			case key_7:
			case key_clr:
				return;
			
			case key_n:
			case key_ent:
				ENSURE=1;
				break;
			
			case key_a:
			case key_c:
			case key_pre_add:
				num = (num + 1) % MENU_OPT_NUM;
				LCD_ShowMenu31(MENU_OPT, MENU_OPT_NUM, num);	
				break;

			case key_b:
			case key_d:
			case key_pre_sub:
				num = (num + MENU_OPT_NUM - 1) % MENU_OPT_NUM;
				LCD_ShowMenu31(MENU_OPT, MENU_OPT_NUM, num);
				break;
		}
////////////////////////////////////////////		
		if(ENSURE)
		{
			ENSURE=0;
			switch(num)
			{
				case 0:
					D_printf("LOCK KEY PAD\n");
					Flag_Main_Page = 0;
					ENSURE = Lock_Screen_KeyBoard();
					LightBacklight();
					while (VOL_ADD_READ == 0 || VOL_SUB_READ == 0)FeedDog();	//Feed the dog - watch dog timer initialization;
													//Prevent a volume increase or decrease from being performed when long-pressing to exit
					if(ENSURE == 2)
					{
						Flag_Main_Page = 1;
						return;
					}
					break;

				case 1:
					D_printf("KEY TEST\n");
					Key_Test();
					break;
			}
			ENSURE = 0;
			LCD_ShowMenu31(MENU_OPT, MENU_OPT_NUM, num);
			while (VOL_ADD_READ == 0 || VOL_SUB_READ == 0)FeedDog();			//Feed the dog - watch dog timer initialization;
													//Prevent a volume increase or decrease from being performed when you press and hold to exit
		}
		//
	}



}
//
//Button '7(OPT)' testing option menu
void Key_Test()
{
	int delay_f1_f2 = 0, result = 0;
	LCD_ShowString0608(0, 3, "     KEY TESTING      ", 1, 128);
	LCD_ShowString0608(0, 4, "                      ", 1, 128);
	LCD_ShowString0608(0, 5, "VALUE:                ", 1, 128);
	while (1)
	{
		FeedDog(); 					//Feed the dog - watch dog timer initialization
		result = KEY_SCAN(0);
		if (result != key_no)
			printf("%d\n", result);
		switch (result)
		{
		case key_0:
			LCD_ShowString0608(36, 5, "    KEY  0    ", 1, 128);
			break;
		case key_1:                                    
			LCD_ShowString0608(36, 5, "    KEY  1    ", 1, 128);
			break;                                     
		case key_2:                                    
			LCD_ShowString0608(36, 5, "    KEY  2    ", 1, 128);
			break;                                     
		case key_3:                                    
			LCD_ShowString0608(36, 5, "    KEY  3    ", 1, 128);
			break;                                     
		case key_4:                                    
			LCD_ShowString0608(36, 5, "    KEY  4    ", 1, 128);
			break;                                     
		case key_5:                                    
			LCD_ShowString0608(36, 5, "    KEY  5    ", 1, 128);
			break;                                     
		case key_6:                                    
			LCD_ShowString0608(36, 5, "    KEY  6    ", 1, 128);
			break;                                     
		case key_7:                                    
			LCD_ShowString0608(36, 5, "    KEY  7    ", 1, 128);
			break;                                     
		case key_8:                                    
			LCD_ShowString0608(36, 5, "    KEY  8    ", 1, 128);
			break;                                     
		case key_9:                                    
			LCD_ShowString0608(36, 5, "    KEY  9    ", 1, 128);
			break;
		case key_clr:
			LCD_ShowString0608(36, 5, "   KEY  CLR   ", 1, 128);
			break;
		case key_ent:
			LCD_ShowString0608(36, 5, "   KEY  ENT   ", 1, 128);
			break;
		case key_n:
			LCD_ShowString0608(36, 5, "  KEY CENTER  ", 1, 128);
			break;
		case key_a:
			LCD_ShowString0608(36, 5, "    KEY UP    ", 1, 128);
			break;
		case key_d:
			LCD_ShowString0608(36, 5, "   KEY DOWN   ", 1, 128);
			break;
		case key_b:
			LCD_ShowString0608(36, 5, "   KEY LEFT   ", 1, 128);
			break;
		case key_c:
			LCD_ShowString0608(36, 5, "  KEY  RIGHT  ", 1, 128);
			break;
		case key_tools:
			LCD_ShowString0608(36, 5, "  KEY  TOOLS  ", 1, 128);
			break;
		case key_pre_add:
			LCD_ShowString0608(36, 5, " KEY  PRE  ADD", 1, 128);
			break;
		case key_pre_sub:
			LCD_ShowString0608(36, 5, " KEY  PRE  SUB", 1, 128);
			break;
		case key_vol_add:
			LCD_ShowString0608(36, 5, " KEY  VOL  ADD", 1, 128);
			break;
		case key_vol_sub:
			LCD_ShowString0608(36, 5, " KEY  VOL  SUB", 1, 128);
			break;
		}

		if (VOL_ADD_READ == 0 || VOL_SUB_READ == 0)
		{
			delay_f1_f2++;
			if (delay_f1_f2 == 120)			//870000
			{
				D_printf("delay_f1_f2:%d\n", delay_f1_f2);
				delay_f1_f2 = 0;
				return;
			}
			if(!VOL_ADD_READ)
				LCD_ShowString0608(36, 5, " KEY  VOL  ADD", 1, 128);
			else
				LCD_ShowString0608(36, 5, " KEY  VOL  SUB", 1, 128);
		}
		else
			delay_f1_f2 = 0;
		

		if (!PTT_READ)
			LCD_ShowString0608(36, 5, " PUSH TO TALK ", 1, 128);
		if (!SQUELCH_STATE)
			LCD_ShowString0608(36, 5, " SQUELCH_READ ", 1, 128);
	}
}
//Function 0: Lock screen, lock disk
int  Lock_Screen_KeyBoard()			//The return value is meaningless
{
	int f1 = 0, f2 = 0;
	int change = 0;
	
	if (Flag_Main_Page == 0)
		KDU_Clear();
	
	key_lock = ON;
	LCD_ShowSignal(0);
	LCD_ShowString0408(108, 5, "KEY", 0);
	while (1)
	{
		if(LAMP_TIME>0 && bsp_CheckTimer(TMR_FLOW))		//It's time to turn off the lights when the backlight is set to 10 sec automatic mode
			BackLight_SetVal(0);
		receiveProcess();
		KDU_Reflash();
		
		if(VOL_ADD_READ == 0)
			f1++;
		else
		{
			if(f1 > 0)
			{
				f1=0;
				if (VOLUME < 7)
				{
					VOLUME++;
					change = 1;
				}
			}
		}
		//
		if(VOL_SUB_READ == 0)
			f2++;
		else
		{
			if(f2>0)
			{
				f2 = 0;
				if (VOLUME > 0)
				{
					VOLUME--;
					change = 1;
				}
			}
		}
		//
		if(change)
		{
			change = 0;
			LCD_ShowVolume(VOLUME);
			sendTo152(_SETVOLU);
		}
		
		if (f1>30 || f2>30)
		{
			key_lock = OFF;
			LCD_ShowString0408(108, 5, "KEY", 1);
			return 2;
		}
	}
}
//


//Button '8(PGM)' parameter settings menu
void PGM_Menu()
{
	u8 num = 0, ENSURE=0;
	LCD_Clear(EDITZONE64);
	LCD_ShowString0408(0, 2, "PGM                             ", 1);
	LCD_ShowMenu31(MENU_PGM, MENU_PGM_NUM, num);
	
	while (1)
	{
		if(!FLAG_LINE)
			return;
		MY_GLOBAL_FUN(0);
		switch (KEY_SCAN(0))
		{
			case key_8:
				return;
			
			case key_clr:
				return;

			case key_n:
			case key_ent:
				ENSURE = 1;
				break;

			case key_c:
			case key_d:
			case key_pre_sub:
				num = (num + 1) % MENU_PGM_NUM;
				LCD_ShowMenu31(MENU_PGM, MENU_PGM_NUM, num);
				break;

			case key_a:
			case key_b:
			case key_pre_add:
				num = (num -1 + MENU_PGM_NUM) % MENU_PGM_NUM;
				LCD_ShowMenu31(MENU_PGM, MENU_PGM_NUM, num);
				break;
		}
		//
		if(ENSURE)
		{
			ENSURE=0;
			switch(num)
			{
				case normal_set_mic_gain:
					D_printf("AUDIO SELECT & MIC GAIN\n");		//Select audio source (IN, TOP, SIDE) and mic gain menu
					ENSURE = PGM_AUDIO_Select(num%3+3);
					break;

				case normal_set_sql:
					D_printf("Squelch Level\n");			//Squelch level menu
					ENSURE = PGM_SQL_Set(num%3+3);
					break;

				case normal_set_step:
					D_printf("STEP SET\n");				//Set step menu
					ENSURE = PGM_STEP_Set(num%3+3);
					break;

				case normal_set_tot:
					D_printf("TOT SET\n");				//Timeout interval menu
					ENSURE = PGM_TOT_Set(num%3+3);
					break;
				
				case normal_set_lamptime:
					D_printf("LampTime SET\n");			//Backlite mode menu
					ENSURE = PGM_LAMP_TIME_Set(num%3+3);
					break;
				
				case normal_set_powerout:
					D_printf("POWER OUT\n");			//Dynamic (Boom) microphone amplifier ON/OFF menu
					ENSURE = PGM_POWEROUT_Set(num%3+3);
					break;

				case normal_set_ptttone:
					D_printf("PTT TONE\n");				//PTT pre-/end- tone menu
					ENSURE = PGM_TONE_Select(num%3+3);
					break;
				default:
					break;
			}
			if(ENSURE == BACK2MAIN)
				return;
			ENSURE = 0;
			LCD_ShowMenu31(MENU_PGM, MENU_PGM_NUM, num);
			while (VOL_ADD_READ == 0 || VOL_SUB_READ == 0)FeedDog(); 	//Feed the dog - watch dog timer initialization;
											//Prevent a volume increase or decrease from being performed when you press and hold to exit
//
		}
	}
}

//Button '8(PGM)' parameter settings menu
//CLR2LAST: Do not modify, re-select the audio channel that needs to be strobe	  
//ENT2LAST: Confirm the modification and exit the audio settings to return to the PDM menu	  
//BACK2MAIN: Button '8'/KDU control to exit directly
//Select the output channel and set the sensitivity
int  AUDIO_SET(u8 _audio)
{
	u8 temp = MIC;

	LCD_ShowString0608(60, _audio + 3,            ":", 0, 128);
	LCD_ShowString0608(66, _audio + 3, TRF_Show[temp], 0, 128);
	LCD_ShowPIC0608(116,   _audio + 3, 0, 0);
	while (1)
	{
		MY_GLOBAL_FUN(0);
		switch (KEY_SCAN(0))
		{
		case key_8:
			return BACK2MAIN;
		
		case key_clr:
			return CLR2LAST;

		case key_n:
		case key_ent:
			MIC = temp;
			AUD = _audio;
			sendTo152(_SETAUD);
			return ENT2LAST;
				
		case key_0:
			temp = 0;
			LCD_ShowString0608(66, _audio + 3, TRF_Show[temp], 0, 128);
			break;

		case key_a:
		case key_c:
		case key_pre_add:
			temp = (temp + 1) % 3;
			LCD_ShowString0608(66, _audio + 3, TRF_Show[temp], 0, 128);
			break;

		case key_b:
		case key_d:
		case key_pre_sub:	
			temp = (temp + 2) % 3;
			LCD_ShowString0608(66, _audio + 3, TRF_Show[temp], 0, 128);
			break;
		}
	}
}

//Button '8(PGM)' parameter settings menu
//CLR2LAST: Return to PGM menu without modification  
//ENT2LAST: Confirm the modification and return to the PGM menu
//BACK2MAIN: Button '8'/KDU control to exit directly
int  PGM_AUDIO_Select(u8 row)
{
	u8 index = AUD, ENTER = 0;
	LCD_ShowMenu31(menu_audio, 3, index);
	while (1)
	{
		MY_GLOBAL_FUN(0);
		if(!FLAG_LINE)
			return BACK2MAIN;
		
		switch (KEY_SCAN(0))
		{
		case key_8:
			return BACK2MAIN;
			
		case key_c:
		case key_d:
		case key_pre_sub:
			index = (index + 1) % 3;
			LCD_ShowMenu31(menu_audio, 3, index);
			break;

		case key_a:
		case key_b:
		case key_pre_add:
			index = (index + 2) % 3;
			LCD_ShowMenu31(menu_audio, 3, index);
			break;

		case key_ent:
			ENTER = 1;
			break;

		case key_clr:
			return CLR2LAST;
		}
		if(ENTER)
		{
			if(index == 0)
			{
				AUD = 0;
				MIC = 4;
				sendTo152(_SETAUD);
				return ENT2LAST;
			}
			ENTER  = AUDIO_SET(index);
			if(ENTER==0)
				LCD_ShowMenu31(menu_audio, 3, index);
			else
				return ENTER;
			
		}
	}
}
//
//Button '8(PGM)' parameter settings menu
//Related setting 2: squelch level
int  PGM_SQL_Set(u8 row)
{
	u8 sql_temp = SQL;
	LCD_ShowString0608(30, row, ":LEVEL", 0, 128);
	LCD_ShowAscii0608(66, row, sql_temp + '0', 0);
	LCD_ShowPIC0608(116, row, 0, 0);
	while (1)
	{
		MY_GLOBAL_FUN(0);
		switch (KEY_SCAN(0))
		{
			case key_8:
				return BACK2MAIN;
			
			case key_clr:
				return CLR2LAST;
		
			case key_ent:
				SQL = sql_temp;
				sendTo152(_SETSQL);
				return ENT2LAST;

			case key_0:
				sql_temp = 0;
				LCD_ShowAscii0608(66, row, sql_temp + '0', 0);
				break;

			case key_a:
			case key_c:
			case key_pre_add:
				if (sql_temp <= 7)
				{
					sql_temp++;
					LCD_ShowAscii0608(66, row, sql_temp + '0', 0);
				}
				break;

			case key_b:
			case key_d:
			case key_pre_sub:	
				if (sql_temp > 0)
				{
					sql_temp--;
					LCD_ShowAscii0608(66, row, sql_temp + '0', 0);
				}
				break;
		}
	}
}

//
//Button '8(PGM)' parameter settings menu
//Related settings 3: Step
int  PGM_STEP_Set(u8 row)
{
	u8 step_temp = STEP_LEVEL;

	LCD_ShowAscii0608 (36, row, ':', 0);
	LCD_ShowString0608(42, row, STEP_SHOW[STEP_LEVEL], 0, 128);
	LCD_ShowPIC0608(116, row, 0, 0);

	while (1)
	{
		MY_GLOBAL_FUN(0);
		switch (KEY_SCAN(0))
		{
			case key_8:
				return BACK2MAIN;
			
			case key_clr:
				return CLR2LAST;

			case key_ent:
				STEP_LEVEL = step_temp;
				sendTo152(_SETSTEP);
				return ENT2LAST;
	 
			case key_0:
				step_temp = 0;
				LCD_ShowString0608(42, row, STEP_SHOW[step_temp], 0, 128);
				break;
			
			case key_a:
			case key_c:
			case key_pre_add:
				if (step_temp < 2)
				{
					step_temp++;
					LCD_ShowString0608(42, row, STEP_SHOW[step_temp], 0, 128);
				}
				break;

			case key_b:
			case key_d:
			case key_pre_sub:	
				if (step_temp > 0)
				{
					step_temp--;
					LCD_ShowString0608(42, row, STEP_SHOW[step_temp], 0, 128);
				}
				break;
				
		}
	}
}
//
//Button '8(PGM)' parameter settings menu - in firmware ver. 2.0.0000 / 2.1.1226 this function doesn't realised!
//Related settings 4: Encryption
int  PGM_ENCRPY_Set(u8 row)
{
	unsigned char SCRAM_LEVEL_temp = ENC;
	LCD_ShowAscii0608(48, row, ':', 0);
	LCD_ShowAscii0608(54, row, SCRAM_LEVEL_temp + '0', 0);
	LCD_ShowPIC0608(116, row, 0, 0);
	while (1)
	{
		MY_GLOBAL_FUN(0);
		switch (KEY_SCAN(0))
		{
			case key_8:
				return BACK2MAIN;
			
			case key_clr:
				return CLR2LAST;

			case key_ent:
				ENC = SCRAM_LEVEL_temp;	
				sendTo152(_SETENC);
				return ENT2LAST;

			case key_0:
				SCRAM_LEVEL_temp = 0;
				LCD_ShowAscii0608(54, row, SCRAM_LEVEL_temp + '0', 0);
				break;

			case key_a:
			case key_c:
			case key_pre_add:
				if (SCRAM_LEVEL_temp < 8)
				{
					SCRAM_LEVEL_temp++;
					LCD_ShowAscii0608(54, row, SCRAM_LEVEL_temp + '0', 0);
				}
				break;

			case key_b:
			case key_d:
			case key_pre_sub:	
				if (SCRAM_LEVEL_temp > 0)
				{
					SCRAM_LEVEL_temp--;
					LCD_ShowAscii0608(54, row, SCRAM_LEVEL_temp + '0', 0);
				}
				break;
		}
	}
}
//
//Button '8(PGM)' parameter settings menu
//Related setting 5: Launch time limit
int  PGM_TOT_Set(u8 row)
{
	u8 tot_temp = TOT;
	LCD_ShowString0608(30, row, ": MINUTE", 0, 128);
	LCD_ShowAscii0608(36, row, tot_temp + '0', 0);
	if (tot_temp == 0)
		LCD_ShowString0608(30, row, ":NO TOT ", 0, 128);
	
	LCD_ShowPIC0608(116, row, 0, 0);
	while (1)
	{
		MY_GLOBAL_FUN(0);
		switch (KEY_SCAN(0))
		{
			case key_8:
				return BACK2MAIN;
			
			case key_clr:
				return CLR2LAST;
			
			case key_ent:
				TOT = tot_temp;
				sendTo152(_SETTOT);
				return ENT2LAST;

			case key_0:
				tot_temp = 0;
				LCD_ShowString0608(30, row, ":NO TOT ", 0, 128);
				break;

			case key_a:
			case key_c:
			case key_pre_add:
				if (tot_temp < 9)
				{
					tot_temp++;
					LCD_ShowString0608(30, row, ": MINUTE", 0, 128);
					LCD_ShowAscii0608(36, row, tot_temp + '0', 0);
				}
				break;

			case key_b:
			case key_d:
			case key_pre_sub:	
				if (tot_temp > 0)
				{
					tot_temp--;
					LCD_ShowString0608(30, row, ": MINUTE", 0, 128);
					LCD_ShowAscii0608(36, row, tot_temp + '0', 0);
					if (tot_temp == 0)
						LCD_ShowString0608(30, row, ":NO TOT ", 0, 128);
				}
				break;
		}
		//
	}
}

//Button '8(PGM)' parameter settings menu
//Related setting 6: Lamptime
int  PGM_LAMP_TIME_Set(u8 row)
{
	u8 LT = LAMP_TIME/10000;		
	
	LCD_ShowAscii0608(60,  row, ':', 0);				//
    	LCD_ShowString0608(72, row, LAMP_SHOW[LT], 0, 128);		//
	LCD_ShowPIC0608(116,   row, 0, 0);
	
	while (1)
	{
		MY_GLOBAL_FUN(0);
		switch (KEY_SCAN(0))
		{
			case key_8:
				return BACK2MAIN;
			
			case key_clr:
				return CLR2LAST;
			
			case key_ent:
				LAMP_TIME = LT*10000;
				return ENT2LAST;

			case key_0:
				LT = 0;
                LCD_ShowString0608(72, row, LAMP_SHOW[LT], 0, 128);
				break;

			case key_a:
			case key_c:
			case key_pre_add:
			case key_b:
			case key_d:
			case key_pre_sub:	
				LT = !LT;
                LCD_ShowString0608(72, row, LAMP_SHOW[LT], 0, 128);
				break;
			}
		//
	}


}


//
//Button '8(PGM)' parameter settings menu
//Related setting 7: Six-pin power output
int  PGM_POWEROUT_Set(u8 row)
{
	char power = OP;
	LCD_ShowAscii0608(60, row, ':', 0);
	LCD_ShowString0608(66, row, opt_state[power], 0, 128);
	LCD_ShowPIC0608(116, row, 0, 0);

	while (1)
	{
		MY_GLOBAL_FUN(0);
		switch (KEY_SCAN(0))
		{
			case key_8:
				return BACK2MAIN;
			
			case key_clr:
				return CLR2LAST;
			
			case key_a:
			case key_b:
			case key_c:
			case key_d:
			case key_pre_add:
			case key_pre_sub:
				power = !power;
				LCD_ShowString0608(66, row, opt_state[power], 0, 128);
				break;

			case key_ent:
				OP = power;
				sendTo152(_SETOP);
				return ENT2LAST;
		}
	}
}

//
//Button '8(PGM)' parameter settings menu
//Related settings 8: PTT button tone setting
int  TONE_SET(u8 _tone) //_tone: The tone to be set;
{
	char t_sta[2] = {PRE_TONE, END_TONE}; 			//The status of the pre-confidence signal and the post-confidence signal
		

	LCD_ShowAscii0608(84, _tone + 3, ':', 0);
	LCD_ShowString0608(90, _tone + 3, opt_state[t_sta[_tone]], 0, 128); 		//Display switch status
	while (1)
	{
		MY_GLOBAL_FUN(0);
		switch (KEY_SCAN(0))
		{
			case key_8:
				return BACK2MAIN;
			case key_clr:
				return CLR2LAST;
			
			case key_a:
			case key_b:
			case key_c:
			case key_d:
			case key_pre_add:
			case key_pre_sub:
				t_sta[_tone] = !t_sta[_tone];
				LCD_ShowString0608(90, _tone + 3, opt_state[t_sta[_tone]], 0, 128);
				break;

			case key_ent:			
				if (_tone)
					END_TONE = t_sta[_tone];			
				else
					PRE_TONE = t_sta[_tone];
				sendTo152(_SETTONE);
				return ENT2LAST;
		
		}
	}
}
int  PGM_TONE_Select(u8 row)
{
	u8 index = 0;
	LCD_ShowMenu31(menu_tone, 2, index);
	while (1)
	{
		MY_GLOBAL_FUN(0);
		if(FLAG_LINE == 0)
			return BACK2MAIN;
		
		switch (KEY_SCAN(0))
		{
			case key_8:
				return BACK2MAIN;
			case key_clr:
				return CLR2LAST;
			
			case key_a:
			case key_b:
			case key_c:
			case key_d:
			case key_pre_add:
			case key_pre_sub:
				index = !index;
				LCD_ShowMenu31(menu_tone, 2, index);
				break;

			case key_ent:
				if (TONE_SET(index)==BACK2MAIN)
					return BACK2MAIN;
				
				LCD_ShowMenu31(menu_tone, 2, index);
				break;
		}
	}
}
//	

//
//////////////////////radio////////////////////////////////
int fm_locate=0, 	    	//Cursor position
	fm_bit=0, 		//Number of input frequency bits
	fm_freq_static;	 	//Frequency calculated after input

char ff_buf[5]={0}; 		//The number on the 100,000 digits of the loading frequency
u8	flag_clear=0, 		//After clr is pressed, is it cleared or exited?
	fm_finish=0, 		//Is it complete to press the digital input frequency?
	val_in=0, 		//Press the number to process, if you enter '1' at the beginning, wait for 1, if you enter '8' directly at the beginning, wait for 2
	key_press=17, 		//Key value return
	first_press=1;		

int RDA5807_Switch(void)
{
	char FM_NOW = WFM;
	LCD_ShowPIC0608   (104, 3, 1, 1);
	LCD_ShowString0608(110, 3, opt_state[WFM], 0, 128);
	
	while(1)
	{
		MY_GLOBAL_FUN(0);
		switch(KEY_SCAN(0))
		{
			case key_a:
			case key_b:
			case key_c:
			case key_d:
			case key_pre_add:
			case key_pre_sub:
				FM_NOW = !FM_NOW;
				LCD_ShowString0608(110, 3, opt_state[FM_NOW], 0, 128);
			break;
				
			case key_n:
			case key_ent:
				LCD_ShowAscii0608 (104, 3, ' ', 1);
				LCD_ShowString0608(110, 3, opt_state[FM_NOW], 1, 128);
				if(FM_NOW == WFM)
					return 0;
				
				WFM = FM_NOW;
				sendTo152(_SETFM);
				if(FM_CHAN)
					LCD_ShowString0608(0, 3, "TRUE      ", 1, 120);
				else
					LCD_ShowString0608(0, 3, "FALSE     ", 1, 120);
				return 1;
							
			case key_clr:
				LCD_ShowAscii0608 (104, 3, ' ', 1);
				LCD_ShowString0608(110, 3, opt_state[FM_NOW], 1, 128);
				return 1;
			
			case key_3:	
				return 3;
		}
	}
	
	
}


void Radio_Freq_Show(int fm_freq, int mode)		//Display frequency
{
//	char buf[6]={0};
//	double fm = fm_freq/10;
//	memset(buf, 0, 6);
//	sprintf(buf, "%03.1f", (double)fm_freq/10);
//	LCD_ShowString1016(0, 4, buf, mode, 128);

	LCD_ShowAscii1016(0,  4, fm_freq/1000%10 + '0', 1);
	LCD_ShowAscii1016(10, 4, fm_freq/100%10  + '0', 1);
	LCD_ShowAscii1016(20, 4, fm_freq/10%10   + '0', 1);
	LCD_ShowAscii1016(30, 4, '.'                  , 1);
	LCD_ShowAscii1016(40, 4, fm_freq%10      + '0', 1);

}


//Operations such as display and input frequency, excluding settings
int FM_Freq_Set_Show(int x,int y,int* result)
{
	if(fm_locate==30+x)
	{
		LCD_ShowAscii1016(fm_locate, y, '.', 1);
		fm_locate+=10;
	}
	if(fm_locate==50+x||fm_bit==4)
		fm_finish=1;
	if(fm_locate>x)
		LCD_ShowAscii1016(fm_locate, y, '_', 1);
	
	key_press=KEY_SCAN(0);
	
	switch(key_press)
	{
		case key_0:
			D_printf("Press{0}");
			if(fm_bit!=0 
				|| (ff_buf[0]==0 && ((ff_buf[1]==8&&ff_buf[2]>=7) || ff_buf[1]==9))
				|| ff_buf[0]==1)
			val_in=1;
			break;
			
		case key_1:
			D_printf("Press{1}");
			if(fm_bit==0
				|| (ff_buf[0]==0 && ((ff_buf[1]==8&&ff_buf[2]>=7) || ff_buf[1]==9))
				|| (ff_buf[0]==1 && ff_buf[1]==0 && ff_buf[2]<8 && fm_bit>1))		//ff_buf has been cleared to zero
			val_in=1;
			break;
			
		case key_2:
		case key_4:
		case key_5:
		case key_6:
			D_printf("Press{%d}", key_press);
			if((ff_buf[0]==0 && ((ff_buf[1]==8&&ff_buf[2]>=7) || ff_buf[1]==9))
			 ||(ff_buf[0]==1 && ff_buf[1]==0 && ff_buf[2]<8 && fm_bit>1))
			val_in=1;
			break;
			
		case key_3:	
			if(fm_bit == 0)
				return 8;
			if((ff_buf[0]==0 && ((ff_buf[1]==8&&ff_buf[2]>=7) || ff_buf[1]==9))
			 ||(ff_buf[0]==1 && ff_buf[1]==0 && ff_buf[2]<8 && fm_bit>1))
			val_in=1;
			break;
			
			
		case key_7:
			D_printf("Press{7}");
			if((ff_buf[0]==0 && (ff_buf[1]==8 || ff_buf[1]==9))
			 ||(ff_buf[0]==1 && ff_buf[1]==0 && (fm_bit==2 || (ff_buf[2]<8&&fm_bit>1))))
			val_in=1;
			break;
			
		case key_8:
			D_printf("Press{8}");
			if(fm_bit==0)val_in=2;
			else if((ff_buf[0]==0 && (ff_buf[1]==8 || ff_buf[1]==9))
						||(ff_buf[0]==1 && ff_buf[1]==0 && (fm_bit==2 || (ff_buf[2]<8&&fm_bit>1))))
			val_in=1;
			break;
			
		case key_9:
			D_printf("Press{9}");
			if(fm_bit==0)val_in=2;
			else if((ff_buf[0]==0 && (ff_buf[1]==8 || ff_buf[1]==9))
						||(ff_buf[0]==1 && ff_buf[1]==0 && (ff_buf[2]<8&&fm_bit>2)))
			val_in=1;
			break;
			
		case key_clr:
			if(ff_buf[0]==0&&ff_buf[1]==0&&ff_buf[2]==0&&ff_buf[3]==0 && flag_clear)
			{
				D_printf("Press{CLR} : EXIT\n");
				flag_clear=0;
				return 8;//退出
			}
			D_printf("Press{CLR} : CLEAR\n");
			LCD_ShowAscii1016(x+0,  y, ' ',  1);
			LCD_ShowAscii1016(x+10, y, ' ',  1);
			LCD_ShowAscii1016(x+20, y, ' ',  1);
			LCD_ShowAscii1016(x+30, y, ' ',  1);
			LCD_ShowAscii1016(x+40, y, ' ',  1);
			LCD_ShowAscii1016(x+50, y, '}'+1,1);		//Arrow
			ff_buf[0]=0,ff_buf[1]=0,ff_buf[2]=0,ff_buf[3]=0;
			fm_locate=x;
			fm_bit=0;
			flag_clear=1;		//EXIT required
			first_press=1;		//Empty the need
		  break;
			
		case key_ent:
		case key_n:
			D_printf("Press{ENT} : Enter\n");
			if(fm_bit>0||(fm_bit==0&&flag_clear==1))
			{
				if(fm_bit<3 || fm_locate<50+x)
					for(;fm_bit<4;fm_bit++)		//Make up zero
						ff_buf[fm_bit]=0;
				fm_finish=1;
				flag_clear=0;
			}
			else if(fm_bit==0)
			{
				if(RDA5807_Switch()==3)return 8;
			}
			break;
		
		case key_a:
		case key_pre_add:
			D_printf("Press{+} : increase the FM_Freq by 100K\n");
			ff_buf[0]=0,ff_buf[1]=0,ff_buf[2]=0,ff_buf[3]=0;
			first_press=1;
			fm_locate=x;
			fm_bit=0;
			return 2;
		
		case key_d:
		case key_pre_sub:
			D_printf("Press{-} : decrease the FM_Freq by 100K\n");
			ff_buf[0]=0,ff_buf[1]=0,ff_buf[2]=0,ff_buf[3]=0;	
			first_press=1;
			fm_locate=x;
			fm_bit=0;
			return 3;
		
		case key_b:
			if(fm_bit!=0)
			{
				for(; fm_bit<4; fm_bit++)
				{
					ff_buf[fm_bit]=0;
					LCD_ShowAscii1016(fm_locate, y, '0', 1);
					if(fm_bit==2)fm_locate+=20;
					else fm_locate+=10;
				}
				first_press=1;
				fm_locate=x;
				fm_bit=0;
			}
			D_printf("Press{*} : Search Forwards\n");
			return 4;
			
		case key_c:
			if(fm_bit!=0)
			{
				for(; fm_bit<4; fm_bit++)
				{
					ff_buf[fm_bit]=0;
					LCD_ShowAscii1016(fm_locate, y, '0', 1);
					if(fm_bit==2)fm_locate+=20;
					else fm_locate+=10;
				}
				first_press=1;
				fm_locate=x;
				fm_bit=0;
			}
			D_printf("Press{#} : Search Backwards\n");
			return 5;			
	}
	//

	//
	if(val_in==1)
	{
		flag_clear=0;val_in=0;
		if(first_press)
		{
			first_press=0;
			LCD_ShowAscii1016(x+0,  y,  ' ',  1);
			LCD_ShowAscii1016(x+10, y,  ' ',  1);
			LCD_ShowAscii1016(x+20, y,  ' ',  1);
			LCD_ShowAscii1016(x+30, y,  ' ',  1);
			LCD_ShowAscii1016(x+40, y,  ' ',  1);
			LCD_ShowAscii1016(x+50, y, '}'+1, 1);		//Arrow
		}
		D_printf("FM_Freq[%d]:%d\n", fm_bit, key_press);
		ff_buf[fm_bit]=key_press;
		fm_bit++;
		LCD_ShowAscii1016(fm_locate, y, key_press + '0', 1);
		fm_locate+=10;
	}
	else if(val_in==2)	//8x, 9x
	{
		flag_clear=0;val_in=0;
		if(first_press)
		{
			first_press=0;
			LCD_ShowAscii1016(x+0,  y,   ' ', 1);
			LCD_ShowAscii1016(x+10, y,   ' ', 1);
			LCD_ShowAscii1016(x+20, y,   ' ', 1);
			LCD_ShowAscii1016(x+30, y,   ' ', 1);
			LCD_ShowAscii1016(x+40, y,   ' ', 1);
			LCD_ShowAscii1016(x+50, y, '}'+1, 1);		//Arrow
		}
		D_printf("FM_Freq[2]:%d", key_press);
		ff_buf[0]=0,ff_buf[1]=key_press;
		fm_bit+=2;
		LCD_ShowAscii1016(fm_locate,   y, ' ', 1);
		LCD_ShowAscii1016(fm_locate+10,y, key_press + '0',1);
		fm_locate+=20;
	}
	//
	
	//
	if(fm_finish)
	{
		D_printf("Submit the edit\n");
		LCD_ShowAscii1016(x+50, y, ' ', 1);//' '

		fm_freq_static=ff_buf[0]*1000+ff_buf[1]*100+ff_buf[2]*10+ff_buf[3];
		
		if(fm_freq_static<870 || fm_freq_static>1090) 
			fm_freq_static=870;
		
		D_printf("NEW FM_Freq:  %d*100K\n", fm_freq_static);
		*result=fm_freq_static;
		Radio_Freq_Show(fm_freq_static, 1);
		
		memset(ff_buf, 0, 4);
		fm_freq_static=0;
		fm_locate=x;
		fm_bit=0;
		first_press=1;
		fm_finish=0;
		return 1;
	}
	//
	
	return 0;
}






//
void Enter_Radio()
{
	unsigned char result, fm_change=1;
	
	LCD_Clear(EDITZONE64);
	Radio_Freq_Show(FM_FREQ, 1);
	LCD_ShowString0608(110, 3, opt_state[WFM], 1, 128);
	LCD_ShowPIC1616(60, 4,11,1);
	LCD_ShowPIC1616(86, 4,12,1);
	LCD_ShowPIC1616(112,4,13,1);
	
	while(1)
	{
		MY_GLOBAL_FUN(1);
		if(!A002_SQ_READ)
		{
			LCD_ShowString0608(0, 3, "R:", 1, 12);
			if(Home_Mode == DUAL_MODE)
				LCD_ShowFreq(12, 3, rcv_chan?chan_arv[CHANB].RX_FREQ:chan_arv[CHANA].RX_FREQ, 1);
			else
				LCD_ShowFreq(12, 3, chan_arv[NOW].RX_FREQ, 1);
		}
		//
		
		result=FM_Freq_Set_Show(0, 4, (int*)&FM_FREQ);
		if(result==1)				//Finished
		{
			fm_change=1;
			sendTo152(_SETFM);
		}
		else if(result==2)			//Up plus P key
		{
			fm_change=1;
			if(FM_FREQ<1080)
				FM_FREQ++;

			sendTo152(_SETFM);
			Radio_Freq_Show(FM_FREQ, 1);
		}
		else if(result==3)			//Down minus N key
		{
			fm_change=1;
			if(FM_FREQ>870)
				FM_FREQ--;
			
			sendTo152(_SETFM);
			Radio_Freq_Show(FM_FREQ, 1);
		}
		else if(result==4)			//Swipe down the left button
		{
			if(WFM==0)
				continue;
			fm_change=1;
			LCD_ShowPIC1616(60,4,11,0);
			LCD_ShowString0608(0, 3, "SEEKING...   ", 1, 120);
			while(1)
			{
				MY_GLOBAL_FUN(0);
				FM_FREQ--;
				if(FM_FREQ<870)
					FM_FREQ=1080;
				sendTo152(_SETFM);
				Radio_Freq_Show(FM_FREQ, 1);
				
				if (FM_CHAN)
				{
					LCD_ShowPIC1616(60,4,11,1);
					break;
				}
				if(KEY_SCAN(0)==key_clr)
					break;
			}
			//
			LCD_ShowPIC1616(60,4,11,1);
		}
		else if(result==5)			//Swipe up right button
		{
			if(WFM==0)
				continue;
			fm_change=1;
			LCD_ShowPIC1616(86,4,12,0);
			LCD_ShowString0608(0, 3, "SEEKING...   ", 1, 120);
			while(1)
			{
				MY_GLOBAL_FUN(0);
				FM_FREQ++;
				if(FM_FREQ>1080)
					FM_FREQ=870;
				
				sendTo152(_SETFM);
				Radio_Freq_Show(FM_FREQ, 1);
				
				if (FM_CHAN)
				{
					LCD_ShowPIC1616(86,4,12,1);
					break;
				}
				if(KEY_SCAN(0)==key_clr)
					break;
			}
			LCD_ShowPIC1616(86,4,12,1);
		}
		else if(result==8)			//Exit CLR key
			return;

							//Judge whether the real station is or not
		if(fm_change)
		{
			fm_change=0;
			Radio_Freq_Show(FM_FREQ, 1);
			if(FM_CHAN)
				LCD_ShowString0608(0, 3, "TRUE      ", 1, 120);
			else
				LCD_ShowString0608(0, 3, "FALSE     ", 1, 120);
			
		}
	}
}
//



///////////////////////////////////////////////////////////

void TOOLS_Menu(void)					//'Tool' function button
{
	LCD_Clear(GLOBAL64);
	u8 num = 0;
	switch(MACHINE_OPTION)
	{
	case 117:
		num = tools_prc117g;
		break;
	case 148:
		num = tools_prc148;
		break;
	case 152:
		num = tools_prc152;
		break;
	}
	LCD_ShowMenu41(MENU_TOOLS, MENU_TOOLS_NUM, num);
	while(1)
	{
        MY_GLOBAL_FUN(0);
		switch (KEY_SCAN(0))
		{
		case key_tools:
		case key_clr:
			return;

		case key_n:
		case key_ent:
			if(num == tools_lcd_inverted)
			{
				LCD_INVERTED_Set(2);
/*				//directly set the lcd showing mode about INVERTION
				LCD_INVERTED = (LCD_INVERTED+1)%2;
				LCD_Write(0xE2,0); delay_ms(10);
				if(LCD_INVERTED)
				{
					LCD_Write(0xC0,0); 	//com1 --> com64				//C8: Normal sequence; C0: Reverse scan
					LCD_Write(0xA1,0); 	//ADC select S0->S131(from S1-S128)  	//A0: Left->right; A1: Reverse, right to left
				}
				else
				{
					LCD_Write(0xC8,0); 	//com1 --> com64				//C8: Normal sequence; C0: Reverse scan
					LCD_Write(0xA0,0); 	//ADC select S0->S131(from S1-S128)  	//A0: Left->right; A1: Reverse, right to left
				}
				LCD_Write(0x20+(5&0x07), 0);
				LCD_Write(0x81,0); 
				LCD_Write(36 & 0x3F, 0);
				LCD_Clear(GLOBAL64);
*/				
			}
			if(num == tools_prc117g)
			{
				MACHINE_OPTION = 117;
				return;
			}
			if(num == tools_prc148)
			{
				MACHINE_OPTION = 148;
				return;
			}
			if(num == tools_prc152)
			{
				MACHINE_OPTION = 152;
				return;
			}
			if(num == tools_update)
			{
				waitToUpdate();
			}
            if(num == tools_about)
            {
                if(checkAbout() == BACK2MAIN)
                return;
            }
			LCD_ShowMenu41(MENU_TOOLS, MENU_TOOLS_NUM, num);
			break;

		case key_c:
		case key_d:
		case key_pre_sub:
			num = (num + 1) % MENU_TOOLS_NUM;
			LCD_ShowMenu41(MENU_TOOLS, MENU_TOOLS_NUM, num);
			break;

		case key_a:
		case key_b:
		case key_pre_add:
			num = (num -1 + MENU_TOOLS_NUM) % MENU_TOOLS_NUM;
			LCD_ShowMenu41(MENU_TOOLS, MENU_TOOLS_NUM, num);
			break;
		
		case key_0:
			num = 0;
			LCD_ShowMenu41(MENU_TOOLS, MENU_TOOLS_NUM, num);
		}
	
	}

}
void waitToUpdate()
{
	D_printf("WAIT FOR THE UPGRADE!\n");
	Iap_Write();
	for (u8 i = 0; i < 5; i++)
	{
		LCD_ShowPIC2516(26 * i, 0, i);
		LCD_ShowPIC2516(26 * i, 6, i + 5);
	}
}
int LCD_INVERTED_Set(u8 row)
{
	u8 inv_temp = LCD_INVERTED;
	LCD_ShowAscii0608(84, row, ':', 0);
	LCD_ShowString0608(90, row, opt_state[inv_temp], 0, 128);
	LCD_ShowPIC0608(116, row, 0, 0);
	
	while (1)
	{
		MY_GLOBAL_FUN(0);
		switch (KEY_SCAN(0))
		{
			case key_8:
				return BACK2MAIN;
			
			case key_clr:
				return CLR2LAST;
			
			case key_ent:
			case key_n:
				LCD_INVERTED = inv_temp;
				LCD_Write(0xE2,0); delay_ms(10);
				if(LCD_INVERTED)
				{
					LCD_Write(0xC0,0); 	//com1 --> com64		   	//C8: Normal sequence; C0: Reverse scan
					LCD_Write(0xA1,0); 	//ADC select S0->S131(from S1-S128)     //A0: Left->right; A1: Reverse, right to left
				}
				else
				{
					LCD_Write(0xC8,0); 	//com1 --> com64			//C8: Normal sequence; C0: Reverse scan
					LCD_Write(0xA0,0); 	//ADC select S0->S131(from S1-S128)     //A0: Left->right; A1: Reverse, right to left
				}
				LCD_Write(0x20+(5&0x07), 0);
				LCD_Write(0x81,0); 
				LCD_Write(36 & 0x3F, 0);
				LCD_Clear(GLOBAL64);
				return ENT2LAST;

			case key_0:
				inv_temp = OFF;
				LCD_ShowString0608(90, row, opt_state[inv_temp], 0, 128);
				break;

			case key_a:
			case key_c:
			case key_b:
			case key_d:
			case key_pre_add:
			case key_pre_sub:	
				inv_temp = (inv_temp + 1)%2;
				LCD_ShowString0608(90, row, opt_state[inv_temp], 0, 128);
				break;
		}
		//
	}
}
int  checkAbout(void)		//Device Information Query
{
    LCD_Clear(GLOBAL64);
    LCD_ShowString0608(31, 2, "INFORMATION",     1, 128);
    LCD_ShowString0608(0,  4, "Device :",        0, 128);
    LCD_ShowString0608(0,  5, "Version:",        0, 128);
    LCD_ShowString0608(49, 4, STR_KDU,        	 1, 128);
    LCD_ShowString0608(49, 5, VERSION_KDU,       1, 128);
    while(1)
    {
        MY_GLOBAL_FUN(0);
        switch (KEY_SCAN(0))
        {
            case key_tools:
                return BACK2MAIN;
            case key_clr:
                return CLR2LAST;
            case key_ent:
                return ENT2LAST;
            default:
                break;
        }
    }
}
//


void LoadCHAN2KDU(char *Lbuf, CHAN_ARV_P B)
{
	B->CHAN	              = (Lbuf[chan_rank + 0] - '0') * 100  + (Lbuf[chan_rank + 1] - '0') * 10    + (Lbuf[chan_rank + 2] - '0');
	B->RX_FREQ 			  = (Lbuf[rx_rank   + 0] - '0') * 100  + (Lbuf[rx_rank   + 1] - '0') * 10    + (Lbuf[rx_rank   + 2] - '0') + (Lbuf[rx_rank + 4] - '0') * 0.1 +
							(Lbuf[rx_rank   + 5] - '0') * 0.01 + (Lbuf[rx_rank   + 6] - '0') * 0.001 + (Lbuf[rx_rank   + 7] - '0') * 0.0001;
	B->TX_FREQ            = (Lbuf[tx_rank   + 0] - '0') * 100  + (Lbuf[tx_rank   + 1] - '0') * 10    + (Lbuf[tx_rank   + 2] - '0') + (Lbuf[tx_rank + 4] - '0') * 0.1 +
							(Lbuf[tx_rank   + 5] - '0') * 0.01 + (Lbuf[tx_rank   + 6] - '0') * 0.001 + (Lbuf[tx_rank   + 7] - '0') * 0.0001;

	B->RS 	              = (Lbuf[rs_rank   + 0] - '0') * 100  + (Lbuf[rs_rank   + 1] - '0') * 10    + (Lbuf[rs_rank   + 2] - '0');
	B->TS 	              = (Lbuf[ts_rank   + 0] - '0') * 100  + (Lbuf[ts_rank   + 1] - '0') * 10    + (Lbuf[ts_rank   + 2] - '0');
	
	B->POWER              = kdu_recv_data(Lbuf[pw_rank]);
	B->GBW 	              = kdu_recv_data(Lbuf[bw_rank]);
	
	for (u8 i = 0; i < 7; i++)
		B->NN[i] = Lbuf[nn_rank + i];
	
//	printf("*****cf:%d\r vu:%d\r chan:%d\r  rx:%.4lf\r tx:%.4lf\r rs:%d\r ts: %d\r power:%d\r gbw:%d\r nn:%s\n",
//			   CF, VU, CHAN, chan_arv[NOW].RX_FREQ, chan_arv[NOW].TX_FREQ, chan_arv[NOW].RS, chan_arv[NOW].TS, chan_arv[NOW].POWER, chan_arv[NOW].GBW, chan_arv[NOW].NN);
}
void sendTo152(u8 cmd)		//Transmit and receive processing
{
	sendCommand(cmd);
	waitToReceive(2000);
}

void sendCommand(u8 cmd)	//Send command handler
{
	memset(send_buf, '0', 128);
 
	for(int i = 0; i<15; i++)
		send_buf[i] = prefix_buf[cmd][i];
	
	switch(cmd)
	{
		case _RELA:
			sprintf(send_buf+chan_rank, "%03d", chan_arv[CHANA].CHAN);
			break;
		
		case _RELB:
			sprintf(send_buf+chan_rank, "%03d", chan_arv[CHANB].CHAN);
			break;
		
		case _RELOAD:
		case _ASKCHAN:
			sprintf(send_buf+chan_rank, "%03d", CHAN);
		
			send_buf[cf_rank]	= kdu_send_data(CF);
			send_buf[vu_rank]	= kdu_send_data(VU);
		
		break;
		
		case _SETCHAN:
			send_buf[cf_rank]	= kdu_send_data(CF);
			send_buf[vu_rank]	= kdu_send_data(VU);
		
			sprintf(send_buf+chan_rank, "%03d", CHAN);
			sprintf(send_buf+rx_rank,   "%.4f", chan_arv[NOW].RX_FREQ);
			sprintf(send_buf+tx_rank,   "%.4f", chan_arv[NOW].TX_FREQ);
			sprintf(send_buf+rs_rank,   "%03d", chan_arv[NOW].RS);
			sprintf(send_buf+ts_rank,   "%03d", chan_arv[NOW].TS);
			sprintf(send_buf+nn_rank,     "%s", chan_arv[NOW].NN);

			send_buf[pw_rank]	= kdu_send_data(chan_arv[NOW].POWER);
			send_buf[bw_rank]	= kdu_send_data(chan_arv[NOW].GBW);
		break;
		
		case _SETHOMEMODE:
			send_buf[homemode_rank] = kdu_send_data((Home_Mode+1)%3);
		break;
		
		case _SETVOLU:
			send_buf[volume_rank] = kdu_send_data(VOLUME);
		break;
		
		case _SETAUD:
			send_buf[aud_rank]	= kdu_send_data(AUD);
			send_buf[mic_rank]	= kdu_send_data(MIC);
		break;
		
		case _SETSQL:
			send_buf[sql_rank]	= kdu_send_data(SQL);
		break;
		
		case _SETSTEP:
			 send_buf[step_rank]= kdu_send_data(STEP_LEVEL);
		break;
		
		case _SETENC:
			send_buf[enc_rank]	= kdu_send_data(ENC);
		break;
		
		case _SETTOT:
			send_buf[tot_rank]	= kdu_send_data(TOT);
		break;
		
		
		case _SETOP:
			send_buf[op_rank]	= kdu_send_data(OP);
		break;
		
		case _SETTONE:
			send_buf[pre_rank]	= kdu_send_data(PRE_TONE);
			send_buf[end_rank]	= kdu_send_data(END_TONE);
		break;
		
		case _SETFM:
			sprintf(send_buf+ffreq_rank,  "%04d", FM_FREQ);
			send_buf[wfm_rank]	= kdu_send_data(WFM);		//Turn FM on/off
		break;
        
        case _SETDUALPOS:
            send_buf[nowselchan_rank] = kdu_send_data(sele_pos);
        break;
			
		default:
			break;
	}
	UART1_Send_Message(send_buf, BUF_SIZE);
}
u8 waitToReceive(int i) 	//Receiving, waiting and processing the return value is meaningless
{
	if(FLAG_LINE == OFF)
        return NO_OPERATE;
	while (!usart1_recv_end_flag && i--) delay_ms(1);
	if (i==0)
	{
        D_printf("NO ANSWER!\n");
#if TEST
		FLAG_LINE = ON;
#else		
		FLAG_LINE = OFF;
#endif
		LCD_ShowString0608(0, 0, "LINK FAILED ", 1, 128);
	}
	receiveProcess();
	return NO_OPERATE;
}

int exitCalVal = 0;
int receiveProcess()		//Receiving data analysis and processing
{
	if (usart1_recv_end_flag)
	{
		bsp_StopTimer(TMR_REPLY);
        FLAG_LINE = ON;
		//Receive all parameters of 152
		if (strstr((const char *)rx1_buf, prefix_buf[ASKALL]))
		{
			exitCalVal = 0;
			LoadCHAN2KDU(rx1_buf, &chan_arv[NOW]);
			CHAN = chan_arv[NOW].CHAN;

			CF 		= kdu_recv_data(rx1_buf[cf_rank]);
			VU 		= kdu_recv_data(rx1_buf[vu_rank]);
			chan_arv[CHANA].CHAN = (rx1_buf[chana_rank + 0] - '0') * 100  + (rx1_buf[chana_rank + 1] - '0') * 10    + (rx1_buf[chana_rank + 2] - '0');
			chan_arv[CHANB].CHAN = (rx1_buf[chanb_rank + 0] - '0') * 100  + (rx1_buf[chanb_rank + 1] - '0') * 10    + (rx1_buf[chanb_rank + 2] - '0');
			
			VOLUME 	= kdu_recv_data(rx1_buf[volume_rank]);
			STEP_LEVEL = kdu_recv_data(rx1_buf[step_rank]);
			SQL 	= kdu_recv_data(rx1_buf[sql_rank]);
			AUD 	= kdu_recv_data(rx1_buf[aud_rank]);
			MIC 	= kdu_recv_data(rx1_buf[mic_rank]);
			ENC 	= kdu_recv_data(rx1_buf[enc_rank]);
			TOT		= kdu_recv_data(rx1_buf[tot_rank]);
			OP 		= kdu_recv_data(rx1_buf[op_rank]);
			
			PRE_TONE= kdu_recv_data(rx1_buf[pre_rank]);
			END_TONE= kdu_recv_data(rx1_buf[end_rank]);
			
			FM_FREQ = (rx1_buf[ffreq_rank + 0] - '0') * 1000 + (rx1_buf[ffreq_rank + 1] - '0') * 100 + (rx1_buf[ffreq_rank + 2] - '0') * 10 + (rx1_buf[ffreq_rank + 3] - '0');	
			WFM 	= kdu_recv_data(rx1_buf[wfm_rank]);
			FM_CHAN = kdu_recv_data(rx1_buf[fmchan_rank]);
			
			VOLTAGE	= (rx1_buf[volt_rank + 0] - '0') * 100 + (rx1_buf[volt_rank + 1] - '0') * 10 + (rx1_buf[volt_rank + 2] - '0');
			RSSI 	= (rx1_buf[rssi_rank + 0] - '0') * 100 + (rx1_buf[rssi_rank + 1] - '0') * 10 + (rx1_buf[rssi_rank + 2] - '0');
			
			A002_SQ_READ = kdu_recv_data(rx1_buf[sq_rank]);
			SQUELCH_STATE= kdu_recv_data(rx1_buf[squ_rank]);
			PTT_READ     = kdu_recv_data(rx1_buf[ptt_rank]);
			if(PTT_READ==0 || A002_SQ_READ==0)
				LightBacklight();		//TMR_PERIOD_500MS    TMR_PERIOD_1Sbsp_StartAutoTimer
			//
			Home_Mode	 = kdu_recv_data(rx1_buf[homemode_rank]);
			rcv_chan     = kdu_recv_data(rx1_buf[nowrcvchan_rank]);
		}
		//Receive 152 channel parameters
		else if(strstr((const char *)rx1_buf, prefix_buf[ASKCHAN]) || strstr((const char *)rx1_buf, prefix_buf[RELOAD]))
		{
			LoadCHAN2KDU(rx1_buf, &chan_arv[NOW]);
			CHAN    = chan_arv[NOW].CHAN;
			CF 		= kdu_recv_data(rx1_buf[cf_rank]);
			VU 		= kdu_recv_data(rx1_buf[vu_rank]);
		}
		//Receive the confirmation data of the reply and set the data in KDU
		else if(strstr((const char *)rx1_buf, prefix_buf[SETCHAN]))
		{
			LoadCHAN2KDU(rx1_buf, &chan_arv[NOW]);
			CHAN = chan_arv[NOW].CHAN;
		}
		////////////////////////////////////////////////////////////////
		
		//Load the parameters of channel A in dual-guard mode
		else if(strstr((const char *)rx1_buf, prefix_buf[ASKA])    || strstr((const char *)rx1_buf, prefix_buf[RELA]))
		{
			exitCalVal = 0;
			LoadCHAN2KDU(rx1_buf, &chan_arv[CHANA]);
		}
		//Load the parameters of channel B in dual-guard mode
		else if(strstr((const char *)rx1_buf, prefix_buf[ASKB])    || strstr((const char *)rx1_buf, prefix_buf[RELB]))
		{
			exitCalVal = 0;
			LoadCHAN2KDU(rx1_buf, &chan_arv[CHANB]);
		}
        //Select channel in dual watch mode
        else if(strstr((const char *)rx1_buf, prefix_buf[SETDUALPOS]))
		{
			sele_pos = kdu_recv_data(rx1_buf[nowselchan_rank]);	
		}
		else if(strstr((const char *)rx1_buf, prefix_buf[SETHOMEMODE]))
		{
			Home_Mode = kdu_recv_data(rx1_buf[homemode_rank]);	
		}
		//
		else if(strstr((const char *)rx1_buf, prefix_buf[SETSTEP]))
		{
			STEP_LEVEL = kdu_recv_data(rx1_buf[step_rank]);
		}
		else if(strstr((const char *)rx1_buf, prefix_buf[SETSQL]))
		{
			SQL = kdu_recv_data(rx1_buf[sql_rank]);
		}
		else if(strstr((const char *)rx1_buf, prefix_buf[SETAUD]))
		{
			AUD = kdu_recv_data(rx1_buf[aud_rank]);
			if(AUD == 0)
				MIC = 4;
			else
				MIC = kdu_recv_data(rx1_buf[mic_rank]);
		}
		else if(strstr((const char *)rx1_buf, prefix_buf[SETENC]))
		{
			ENC = kdu_recv_data(rx1_buf[enc_rank]);
		}
		else if(strstr((const char *)rx1_buf, prefix_buf[SETTOT]))
		{
			TOT = kdu_recv_data(rx1_buf[tot_rank]);
		}
		else if(strstr((const char *)rx1_buf, prefix_buf[SETOP]))
		{
			OP = kdu_recv_data(rx1_buf[op_rank]);
		}
		else if(strstr((const char *)rx1_buf, prefix_buf[SETVOLU]))
		{
			VOLUME = kdu_recv_data(rx1_buf[volume_rank]);
		}
		else if(strstr((const char *)rx1_buf, prefix_buf[SETTONE]))
		{
			PRE_TONE = kdu_recv_data(rx1_buf[pre_rank]);
			END_TONE = kdu_recv_data(rx1_buf[end_rank]);
		}
		else if(strstr((const char *)rx1_buf, prefix_buf[SETFM]))
		{
			FM_FREQ = (rx1_buf[ffreq_rank + 0] - '0') * 1000 + (rx1_buf[ffreq_rank + 1] - '0') * 100 + (rx1_buf[ffreq_rank + 2] - '0') * 10 + (rx1_buf[ffreq_rank + 3] - '0');	
			WFM 	= kdu_recv_data(rx1_buf[wfm_rank]);
			FM_CHAN = kdu_recv_data(rx1_buf[fmchan_rank]); 
		}
		else
		{
			if(exitCalVal ++ >3)
                FLAG_LINE = OFF;
			D_printf("FM_CHAN:%d,%d,%s %s, %s      \n",FM_CHAN, kdu_recv_data(rx1_buf[fmchan_rank]), 
				strstr((const char *)rx1_buf, prefix_buf[SETFM]), rx1_buf, rx1_buf+ffreq_rank);
		}
		//
//		printf("%d, %d\n", FM_FREQ, FM_CHAN);
//
//		printf("cf:%d\r vu:%d\r chan:%d\r  rx:%.4lf\r tx:%.4lf\r rs:%d\r ts: %d\r power:%d\r gbw:%d\r nn:%s\n",
//			   CF, VU, CHAN, chan_arv[NOW].RX_FREQ, chan_arv[NOW].TX_FREQ, chan_arv[NOW].RS, chan_arv[NOW].TS, chan_arv[NOW].POWER, chan_arv[NOW].GBW, chan_arv[NOW].NN);
		EN_Recv();
		return 1;
	}
	//
	return 0;

}
//

void wait152StartUp(void)
{
	int i = 10;
	while(!receiveProcess() && i--)
	{
		sendCommand(_ASKALL);			//Get all parameters
        delay_ms(1000);					//Wait to receive real-time change data to determine that 152 has been turned on
	}
	if(i<1)
		FLAG_LINE = OFF;
	else
		FLAG_LINE = ON;
	
}
//











