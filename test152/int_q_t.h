#ifndef MM_int_Q_T_H
#define MM_int_Q_T_H
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define Q_T_V 0.0.1

//optional
//#include "inc_int_q_t.h" 
//e.g. for int inc_int_q_t.h has to be available and would just include whatever files you need to include in here for the types to work.
 
/*
Q_INCLUDES
int, int, uint8_t, and Q_INCLUDES and such are _not_ preprocessor defines here.
They are used to template this and q_t.c out to allow using this with multiple types.
This comment above will make no sense after replacement. Lol.
*/

#ifdef __cplusplus      
extern "C" {  
#endif

typedef struct int_q_t {
    uint8_t sz;
    uint8_t max;
    uint8_t tail;
    uint8_t head;
    uint8_t element_size;
    int * storage;
} int_q_t;
void int_q_init(int_q_t * q, int * storage, uint8_t _max); //for int, int_q_init
int int_q_put( int_q_t * q, int val);
int int_q_get( int_q_t * q);
int int_q_peek(int_q_t * q);
bool int_q_empty( int_q_t * q);
bool int_q_full( int_q_t * q);
uint8_t int_q_space( int_q_t * q);
uint8_t int_q_size( int_q_t * q); //for int, int_q_size
int int_q_lpeek(int_q_t * q);
int int_q_rpeek(int_q_t * q);
int int_q_idx_l(int_q_t * q, uint8_t idx);
int int_q_idx_r(int_q_t * q, uint8_t idx);
int int_q_rpush(int_q_t * q, int val);
int int_q_lpush(int_q_t * q, int val);
int int_q_rpop(int_q_t * q);
int int_q_lpop(int_q_t * q);

#ifdef __cplusplus        
}  
#endif

#endif
