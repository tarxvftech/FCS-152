/*
    bsp_MatrixKeyBoard.h    
*/
#ifndef __BSP_MATRIXKEYBOARD_H__
#define __BSP_MATRIXKEYBOARD_H__

#include "FCS152_KDU.h" 

#define MATRIX_ROWS 4 
#define MATRIX_COLS 4 

#define KEY_ROW1_PIN            GPIO_NUM_2 
#define KEY_ROW2_PIN            GPIO_NUM_3
#define KEY_ROW3_PIN            GPIO_NUM_0 
#define KEY_ROW4_PIN            GPIO_NUM_1 
#define KEY_ROW_ALL_PIN         (BIT(KEY_ROW1_PIN) |BIT(KEY_ROW2_PIN) |BIT(KEY_ROW3_PIN) |BIT(KEY_ROW4_PIN) ) 

#define KEY_COL1_PIN            GPIO_NUM_41 
#define KEY_COL2_PIN            GPIO_NUM_42 
#define KEY_COL3_PIN            GPIO_NUM_45 
#define KEY_COL4_PIN            GPIO_NUM_46 
#define KEY_COL_ALL_PIN         (BIT(KEY_COL1_PIN) |BIT(KEY_COL2_PIN) |BIT(KEY_COL3_PIN) |BIT(KEY_COL4_PIN) ) 

#define KEY_ALL_ROW_UNSELECT()  {digitalWrite(KEY_ROW1_PIN, 0); \
                                 digitalWrite(KEY_ROW2_PIN, 0); \
                                 digitalWrite(KEY_ROW3_PIN, 0); \
                                 digitalWrite(KEY_ROW4_PIN, 0); }  

#define KEY_ALL_ROW_SELECT()	{digitalWrite(KEY_ROW1_PIN, 1); \
                                 digitalWrite(KEY_ROW2_PIN, 1); \
                                 digitalWrite(KEY_ROW3_PIN, 1); \
                                 digitalWrite(KEY_ROW4_PIN, 1); }  

#define KEY_ROW1_SELECT()       {KEY_ALL_ROW_UNSELECT();digitalWrite(KEY_ROW1_PIN, 1);} 
#define KEY_ROW2_SELECT()       {KEY_ALL_ROW_UNSELECT();digitalWrite(KEY_ROW2_PIN, 1);} 
#define KEY_ROW3_SELECT()       {KEY_ALL_ROW_UNSELECT();digitalWrite(KEY_ROW3_PIN, 1);} 
#define KEY_ROW4_SELECT()       {KEY_ALL_ROW_UNSELECT();digitalWrite(KEY_ROW4_PIN, 1);} 

#define KEY_COL1_READ           (digitalRead(KEY_COL1_PIN)?0x0001:0) 
#define KEY_COL2_READ           (digitalRead(KEY_COL2_PIN)?0x0002:0) 
#define KEY_COL3_READ           (digitalRead(KEY_COL3_PIN)?0x0004:0) 
#define KEY_COL4_READ           (digitalRead(KEY_COL4_PIN)?0x0008:0) 









///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define MATRIX_RESULT_0         0 
#define MATRIX_RESULT_1         1 
#define MATRIX_RESULT_2         2 
#define MATRIX_RESULT_3         3 
#define MATRIX_RESULT_4         4 
#define MATRIX_RESULT_5         5 
#define MATRIX_RESULT_6         6 
#define MATRIX_RESULT_7         7 
#define MATRIX_RESULT_8         8 
#define MATRIX_RESULT_9         9 
#define MATRIX_RESULT_CLR       10 
#define MATRIX_RESULT_ENT       11 
#define MATRIX_RESULT_P         12  //shang
#define MATRIX_RESULT_N         13  //xia
#define MATRIX_RESULT_LEFT      14 
#define MATRIX_RESULT_RIGHT     15 
#define MATRIX_RESULT_ERROR     17 
/*
	*	,------------.
	*	| 1| 2| 3|CLR|
	*	|------------|
	*	| 4| 5| 6|ENT|
	*	|------------|
	*	| 7| 8| 9| U |
	*	|------------|
	*	| 0| L| R| D |
	*	`------------'
*/
typedef struct KeyPos_Type
{
    uint8_t row;
    uint8_t col;
}KeyPos;
typedef struct KeyEvent_Type
{
    KeyPos key;
    bool pressed;
}KeyEvent;

void bsp_Matrix_Init(void);
unsigned char Matrix_KEY_Scan(unsigned char mode);






#endif



