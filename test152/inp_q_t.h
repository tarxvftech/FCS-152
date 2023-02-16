#ifndef MM_inp_Q_T_H
#define MM_inp_Q_T_H
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define Q_T_V 0.0.1

//optional
#include "inputtypes.h"
//#include "inc_inp_q_t.h" 
//e.g. for int inc_int_q_t.h has to be available and would just include whatever files you need to include in here for the types to work.
 
/*
Q_INCLUDES
inp, input_t, uint8_t, and Q_INCLUDES and such are _not_ preprocessor defines here.
They are used to template this and q_t.c out to allow using this with multiple types.
This comment above will make no sense after replacement. Lol.
*/

#ifdef __cplusplus      
extern "C" {  
#endif

typedef struct inp_q_t {
    uint8_t sz;
    uint8_t max;
    uint8_t tail;
    uint8_t head;
    uint8_t element_size;
    input_t * storage;
} inp_q_t;
void inp_q_init(inp_q_t * q, input_t * storage, uint8_t _max); //for int, int_q_init
int inp_q_put( inp_q_t * q, input_t val);
input_t inp_q_get( inp_q_t * q);
input_t inp_q_peek(inp_q_t * q);
bool inp_q_empty( inp_q_t * q);
bool inp_q_full( inp_q_t * q);
uint8_t inp_q_space( inp_q_t * q);
uint8_t inp_q_size( inp_q_t * q); //for int, int_q_size
input_t inp_q_lpeek(inp_q_t * q);
input_t inp_q_rpeek(inp_q_t * q);
input_t inp_q_idx_l(inp_q_t * q, uint8_t idx);
input_t inp_q_idx_r(inp_q_t * q, uint8_t idx);
int inp_q_rpush(inp_q_t * q, input_t val);
int inp_q_lpush(inp_q_t * q, input_t val);
input_t inp_q_rpop(inp_q_t * q);
input_t inp_q_lpop(inp_q_t * q);

#ifdef __cplusplus        
}  
#endif

#endif
