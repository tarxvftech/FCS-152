#include "main.h"
#include "input.h"

input_t _in_q[_in_q_max];
input_t _in_state[_in_state_max];
void input_scan_task(){
    Encoder_Switch_Scan(0);
    if(PTT_READ){
    }
    if(SQUELCH_READ){
    }
    Event_Matrix(Matrix_KEY_Scan(0));
}


