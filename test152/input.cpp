#include "main.h"
#include "input.h"

typedef struct q {
    uint8_t sz;
    uint8_t max;
    uint8_t tail;
    uint8_t head;
    uint8_t element_size;
    void * storage;
} q_t;

q_t input_q = {0};
input_t _input_q_storage[_in_q_max];


q_init(input_q, sizeof(input_t), (void*)_input_q_storage);

input_t _in_q[_in_q_max] = {0};
input_t _in_state[_in_state_max] = {0};


void q_init(q_t * q, size_t each_sz, void * storage){
    q->element_size = each_sz;
    q->storage = storage;
}
void q_space(q_t * q) {
    return q->max - q->sz;
}
void q_size(input_t * arr) {
}
void q_get(input_t * arr) {
}
void q_put(input_t * arr, input_t val) {
    arr[ _in_q_tail ] = val;
    _in_q_sz++
    _in_q_tail = (_in_q_tail + 1) % _in_q_max;
}

void stk_push() {
}
void stk_pop() {
}
void stk_peek() {
}
int stk_size() {
}

void input_scan_task(){
    Encoder_Switch_Scan(0);
    if(PTT_READ){
    }
    if(SQUELCH_READ){
    }
    Event_Matrix(Matrix_KEY_Scan(0));
}


