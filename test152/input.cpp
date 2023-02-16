#include "main.h"
#include "input.h"

//input_t _in_q[_in_q_max];
//input_t _in_state[_in_state_max];


input_t _inpstorage[max_inp];
inp_q_t input_q;
void input_init(){
    inp_q_init(&input_q, _inpstorage, max_inp);
}
void input_scan_task(){
    Encoder_Switch_Scan(0);
    if(PTT_READ){
    }
    if(SQUELCH_READ){
    }
    Event_Matrix(Matrix_KEY_Scan(0));
}


