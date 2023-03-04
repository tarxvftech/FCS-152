#include "main.h"
#include "input.h"

//input_t _in_q[_in_q_max];
//input_t _in_state[_in_state_max];


input_t _inpstorage[max_inp];
inp_q_t input_q;
void input_init(){
    inp_q_init(&input_q, _inpstorage, max_inp);
}
void input_scan(){
    input_t in;
    u8 encoder_in = Encoder_Switch_Scan(0);
    if( encoder_in == key_click ){
	in.key = ENCODER_BTN;
	in.type = PRESS_CLICK;
	inp_q_put(&input_q, in);
    } else if( encoder_in == key_long ){
        SHUT();
    }
    //if(PTT_READ){
    //}
    //if(SQUELCH_READ){
    //}
    u8 matrix_key = Matrix_KEY_Scan(0); //previously handled by event_matrix
    //adapting existing system by inserting here - might replace lower-level input handling later, might not
    //in the meantime, this is a perfectly fine place to start replacing the UI input handling
    if( matrix_key == MATRIX_RESULT_ERROR ){
	//? should do nothing, as far as I can tell. 
	//"ERROR" might be too strong a word. 
    } else {
	in.key = (rkey_t) ((int)matrix_key + (int)KEY_0); 
	//requires the KEY_ enums to be in same order as the matrix_key
	//defines (specifically done to allow this rather than a long
	//switch case)
	in.type = PRESS_CLICK; //keyboard is always just a click in the original firmware. We could fix that if we wanted, later.
	inp_q_put(&input_q, in);
    }
}

//typedef struct input_et {
	//press_t  type;
	//rkey_t    key;
	//uint64_t first_down_time; //esp_timer_get_time()
//} input_t;
