#include <assert.h>
#include <stdio.h>
#include "inp_q_t.h"
#ifdef __cplusplus      
extern "C" {  
#endif

bool inp_q_empty(inp_q_t * q) {
    assert(q!=NULL);
    bool r = inp_q_size(q) == 0;
    if( r ){
        assert(q->head == q->tail);
        assert(  inp_q_space(q) == q->max);
        assert( !inp_q_full(q));
    }
    return r;
}
bool inp_q_full(inp_q_t * q) {
    return q->max == q->sz;
}
uint8_t inp_q_space(inp_q_t * q) {
    return q->max - q->sz;
}
uint8_t inp_q_size(inp_q_t * q) {
    return q->sz;
}
//maybe a way to zero unused items?
void inp_q_init(inp_q_t * q, input_t * storage, uint8_t _max){
    q->element_size = sizeof(input_t);
    q->storage = storage;
    q->max = _max;
    /*q_zerounused(q);*/
}
//invariants:
//in a nonzero list, head points to an occupied cell
//tail points to an unoccupied cell? Maybe I should change that.

//standard is dequeue style
//some aliases for stack operation and queue ops
input_t inp_q_get(inp_q_t * q) {
    return inp_q_lpop(q);
}
input_t inp_q_peek(inp_q_t * q){
    return inp_q_lpeek(q);
}
int inp_q_put(inp_q_t * q, input_t val) { 
    return inp_q_rpush(q, val);
}

input_t inp_q_lpeek(inp_q_t * q){
    assert(q->sz > 0);
    return inp_q_idx_l(q,0);
}
input_t inp_q_rpeek(inp_q_t * q){
    assert(q->sz > 0);
    return inp_q_idx_r(q,0);
}

input_t inp_q_idx_l(inp_q_t * q, uint8_t idx){
    assert(q->sz > 0 && q->sz > idx); // > idx and not >= idx because idx can be zero and sz would be 1
    return q->storage[ (q->head + idx)%q->max ];
}
input_t inp_q_idx_r(inp_q_t * q, uint8_t idx){
    assert(q->sz > 0 && q->sz > idx); // > idx and not >= idx because idx can be zero and sz would be 1
    return q->storage[ (q->tail - idx- 1)%q->max ];
}

int inp_q_rpush(inp_q_t * q, input_t val) {
    /*printf("head: %d tail %d sz %d\n",q->head, q->tail, q->sz);*/
    if( ! inp_q_full(q) ){
        q->storage[ q->tail ] = val;
        q->sz++;
        q->tail = (q->tail + 1) % q->max;
        return 1;
    }
    return 0;
}
int inp_q_lpush(inp_q_t * q, input_t val) {
    /*printf("head: %d tail %d sz %d\n",q->head, q->tail, q->sz);*/
    if( ! inp_q_full(q) ){
        q->head = (q->head - 1) % q->max;
        q->storage[ q->head ] = val;
        q->sz++;
        return 1;
    }
    return 0;
}
input_t inp_q_rpop(inp_q_t * q) {
    /*printf("head: %d tail %d sz %d\n",q->head, q->tail, q->sz);*/
    assert(q->sz > 0);
    q->tail = (q->tail - 1) % q->max; //tail normally points at the next cell
    int h = q->tail;
    input_t v = q->storage[h];
    q->sz--;
    memset( q->storage+h, 0, q->element_size);
    return v;
}
input_t inp_q_lpop(inp_q_t * q) {
    /*printf("head: %d tail %d sz %d\n",q->head, q->tail, q->sz);*/
    assert(q->sz > 0);
    int h = q->head;
    input_t v = q->storage[h];
    q->head = (q->head + 1) % q->max;
    q->sz--;
    memset( q->storage+h, 0, q->element_size);
    return v;
}
#ifdef __cplusplus      
}
#endif
