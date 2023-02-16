#include <assert.h>
#include <stdio.h>
#include "int_q_t.h"
#ifdef __cplusplus      
extern "C" {  
#endif

bool int_q_empty(int_q_t * q) {
    assert(q!=NULL);
    bool r = int_q_size(q) == 0;
    if( r ){
        assert(q->head == q->tail);
        assert(  int_q_space(q) == q->max);
        assert( !int_q_full(q));
    }
    return r;
}
bool int_q_full(int_q_t * q) {
    return q->max == q->sz;
}
uint8_t int_q_space(int_q_t * q) {
    return q->max - q->sz;
}
uint8_t int_q_size(int_q_t * q) {
    return q->sz;
}
//maybe a way to zero unused items?
void int_q_init(int_q_t * q, int * storage, uint8_t _max){
    q->element_size = sizeof(int);
    q->storage = storage;
    q->max = _max;
    /*q_zerounused(q);*/
}
//invariants:
//in a nonzero list, head points to an occupied cell
//tail points to an unoccupied cell? Maybe I should change that.

//standard is dequeue style
//some aliases for stack operation and queue ops
int int_q_get(int_q_t * q) {
    return int_q_lpop(q);
}
int int_q_peek(int_q_t * q){
    return int_q_lpeek(q);
}
int int_q_put(int_q_t * q, int val) { 
    return int_q_rpush(q, val);
}

int int_q_lpeek(int_q_t * q){
    assert(q->sz > 0);
    return int_q_idx_l(q,0);
}
int int_q_rpeek(int_q_t * q){
    assert(q->sz > 0);
    return int_q_idx_r(q,0);
}

int int_q_idx_l(int_q_t * q, uint8_t idx){
    assert(q->sz > 0 && q->sz > idx); // > idx and not >= idx because idx can be zero and sz would be 1
    return q->storage[ (q->head + idx)%q->max ];
}
int int_q_idx_r(int_q_t * q, uint8_t idx){
    assert(q->sz > 0 && q->sz > idx); // > idx and not >= idx because idx can be zero and sz would be 1
    return q->storage[ (q->tail - idx- 1)%q->max ];
}

int int_q_rpush(int_q_t * q, int val) {
    /*printf("head: %d tail %d sz %d\n",q->head, q->tail, q->sz);*/
    if( ! int_q_full(q) ){
        q->storage[ q->tail ] = val;
        q->sz++;
        q->tail = (q->tail + 1) % q->max;
        return 1;
    }
    return 0;
}
int int_q_lpush(int_q_t * q, int val) {
    /*printf("head: %d tail %d sz %d\n",q->head, q->tail, q->sz);*/
    if( ! int_q_full(q) ){
        q->head = (q->head - 1) % q->max;
        q->storage[ q->head ] = val;
        q->sz++;
        return 1;
    }
    return 0;
}
int int_q_rpop(int_q_t * q) {
    /*printf("head: %d tail %d sz %d\n",q->head, q->tail, q->sz);*/
    assert(q->sz > 0);
    q->tail = (q->tail - 1) % q->max; //tail normally points at the next cell
    int h = q->tail;
    int v = q->storage[h];
    q->sz--;
    memset( q->storage+h, 0, q->element_size);
    return v;
}
int int_q_lpop(int_q_t * q) {
    /*printf("head: %d tail %d sz %d\n",q->head, q->tail, q->sz);*/
    assert(q->sz > 0);
    int h = q->head;
    int v = q->storage[h];
    q->head = (q->head + 1) % q->max;
    q->sz--;
    memset( q->storage+h, 0, q->element_size);
    return v;
}
#ifdef __cplusplus      
}
#endif
