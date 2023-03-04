
#ifndef MM_INPUTTYPES_H
#define MM_INPUTTYPES_H
typedef enum press_t {
	PRESS_RAW = 0, //don't know what i mean by this yet
	PRESS_DOWN,
	PRESS_RELEASE,
	PRESS_CLICK,
	PRESS_DOUBLE,
	PRESS_HOLD_SHORT,
	PRESS_HOLD_LONG,
} press_t;

typedef enum rkey_t {
	NO_KEY = 0,
	ENCODER_BTN,
	ENCODER_LEFT,
	ENCODER_RIGHT,
	VOL_UP_BTN,
	VOL_DN_BTN,
	PTT_BTN,
	SQL_BTN,
	KEY_0, //==8, KEY_0 and onwards must match order in bsp_matrixkeyboard.h because I just add an offset to transform
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_CLR, //18
	KEY_ENT,
	KEY_PLUS,
	KEY_MINUS,
	KEY_LEFT,
	KEY_RIGHT,
} rkey_t;

typedef struct input_et {
	press_t  type;
	rkey_t    key;
	uint64_t first_down_time; //esp_timer_get_time()
} input_t;
#endif
