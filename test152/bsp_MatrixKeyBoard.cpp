#include "bsp_MatrixKeyBoard.h"
#include "bsp_device.h"
#include "bsp_timer.h"
#include "tim_int.h"

const  uint8_t keymaps[MATRIX_ROWS][MATRIX_COLS]= {
    {MATRIX_RESULT_1, MATRIX_RESULT_2,    MATRIX_RESULT_3,      MATRIX_RESULT_CLR},
    {MATRIX_RESULT_4, MATRIX_RESULT_5,    MATRIX_RESULT_6,      MATRIX_RESULT_ENT},
    {MATRIX_RESULT_7, MATRIX_RESULT_8,    MATRIX_RESULT_9,      MATRIX_RESULT_P},
    {MATRIX_RESULT_0, MATRIX_RESULT_LEFT, MATRIX_RESULT_RIGHT,  MATRIX_RESULT_N}
};

void bsp_Matrix_Init(void) {
    // gpio_config_t io_conf;

    // //disable interrupt
    // io_conf.intr_type = GPIO_INTR_DISABLE;
    // //set as output mode
    // io_conf.mode = GPIO_MODE_OUTPUT;
    // //bit mask of the pins that you want to set
    // io_conf.pin_bit_mask = KEY_ROW_ALL_PIN;
    // //disable pull-down mode
    // io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    // //disable pull-up mode
    // io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    // //configure GPIO with the given settings
    // gpio_config(&io_conf);


    // io_conf.mode         = GPIO_MODE_INPUT;
    // io_conf.pin_bit_mask = KEY_COL_ALL_PIN;
    // io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    // gpio_config(&io_conf);

    pinMode(KEY_ROW1_PIN, OUTPUT);
    pinMode(KEY_ROW2_PIN, OUTPUT);
    pinMode(KEY_ROW3_PIN, OUTPUT);
    pinMode(KEY_ROW4_PIN, OUTPUT);

    pinMode(KEY_COL1_PIN, INPUT_PULLDOWN);
    pinMode(KEY_COL2_PIN, INPUT_PULLDOWN);
    pinMode(KEY_COL3_PIN, INPUT_PULLDOWN);
    pinMode(KEY_COL4_PIN, INPUT_PULLDOWN);


    KEY_ALL_ROW_UNSELECT();     //释放行

}

static void Key_Select_Row(uint8_t row) {
    switch (row) {
    case 0:
        KEY_ROW1_SELECT();
        break;

    case 1:
        KEY_ROW2_SELECT();
        break;

    case 2:
        KEY_ROW3_SELECT();
        break;

    case 3:
        KEY_ROW4_SELECT();
        break;

    default:
        break;
    }
}
//
uint16_t Key_Read_Col(void) {
    return (uint16_t)(KEY_COL1_READ | KEY_COL2_READ | KEY_COL3_READ | KEY_COL4_READ);
}
//
unsigned char Matrix_KEY_Scan(unsigned char mode) {
    if (bsp_CheckTimer(TMR_KEY_SCAN)==0) {
        return MATRIX_RESULT_ERROR;
    }
    static volatile int key_val=MATRIX_RESULT_ERROR;

    uint16_t col_Value = 0;
    //  KeyEvent key_e;
    for (unsigned char r = 0; r < MATRIX_ROWS; r++) {
        Key_Select_Row(r);
        col_Value = Key_Read_Col();
        // D_printf("ROW_ON:%d, COL:%x\n", r, col_Value);
        for (uint8_t c = 0; c < MATRIX_COLS; c++) {
            if (col_Value & (uint16_t)1<<c) {
                ClearShut();
                if (keymaps[r][c] ^ key_val) {
                    bsp_StartAutoTimer(TMR_KEY_SAME, 400);//TMR_PERIOD_500MS    TMR_PERIOD_1Sbsp_StartAutoTimer
                    key_val = keymaps[r][c];
                    return key_val;
                } else {
                    if (bsp_CheckTimer(TMR_KEY_SAME)) {     //400ms
                        return key_val;
                    }
                    return MATRIX_RESULT_ERROR;
                }
            }
        }
    }
    key_val = MATRIX_RESULT_ERROR;
    return key_val;
}


