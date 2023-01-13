#ifndef __FONT_H__
#define __FONT_H__

#define MENU_ZERO_NUM       1
#define MENU_PGM_NUM        7
#define MENU_OPT_NUM        2
#define MENU_TOOLS_NUM      6

#define MATRIX_MENU1_ROW    4
#define MATRIX_MENU1_TOTALN1 MATRIX_MENU1_ROW*2-1       //RT_Menu菜单总项数-1

extern char matrix_menu1[4][2][12];
extern const char
* CONFIRM_OPT[],
*opt_state[],
*FM_Show[],
*POWER_SHOW[2],
*LIGHT_SHOW[],
*LAMP_SHOW[],
*TRF_Show[],
*STEP_SHOW[],
*menu1[6],
*MENU_ZERO[MENU_ZERO_NUM],
*MENU_PGM[MENU_PGM_NUM],
*MENU_OPT[MENU_OPT_NUM],
*MENU_TOOLS[MENU_TOOLS_NUM],
*matrix_menu_subvoice[41][3],
*menu_tone[2],
*menu_audio[3],
pic_0608[][6],
pic_0808[][8],
pic_0816[],
pic_2424[],
pic_1616[],
pic2516[][50],
*menu_subvoice[122];

extern const unsigned char

ascii_0408[][4], ascii_0608[][6], ascii_1016[][20],
           pic_0408[][4],   pic_KDUClear[],  pic_HARRIS[],     pic_BaoTong[];

extern const unsigned char square_9[10][9];
extern const char prefix_buf[][16];

#endif





