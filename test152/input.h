#ifndef MM_INPUT_H
#define MM_INPUT_H
#include "inputtypes.h"
#define click_max_ms 20
#define double_click_window_ms 200
#define hold_short_ms 1000
#define hold_long_ms 5000
#define input_state_fade 1000 //how long after release to wait before resetting state


//#define _in_q_max 20
//extern input_t _in_q[_in_q_max];
//#define _in_state_max 8
//extern input_t _in_state[_in_state_max];

#include "inp_q_t.h"
#define max_inp 10
extern input_t _inpstorage[max_inp];
extern inp_q_t input_q;
void input_init();

#endif
