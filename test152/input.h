#ifndef MM_INPUT_H
#define MM_INPUT_H

#define click_max_ms 20
#define double_click_window_ms 200
#define hold_short_ms 1000
#define hold_long_ms 5000
#define input_state_fade 1000 //how long after release to wait before resetting state

typedef enum press_t {
	PRESS_RAW = 0, //don't know what i mean by this yet
	PRESS_DOWN,
	PRESS_RELEASE,
	PRESS_CLICK,
	PRESS_DOUBLE,
	PRESS_HOLD_SHORT,
	PRESS_HOLD_LONG,
} press_t;

typedef enum key_t {
	ENCODER_BTN,
	ENCODER_LEFT,
	ENCODER_RIGHT,
	VOL_UP_BTN,
	VOL_DN_BTN,
	PTT_BTN,
	SQL_BTN,
	KEY_0,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_PLUS,
	KEY_MINUS,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_ENT,
	KEY_CLR
} key_t;

typedef struct input_t {
	press_t  type;
	key_t    key;
	uint64_t first_down_time; //esp_timer_get_time()
} input_t;

#define _in_q_max 20
extern input_t _in_q[_in_q_max] = {0};
#define _in_state_max 8
extern input_t _in_state[_in_state_max] = {0};

#endif
