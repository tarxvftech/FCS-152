/*
    file@userinclude.h
*/
#ifndef __USERINCLUDE_H__
#define __USERINCLUDE_H__

#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define THISCHIP_STM32F103RET6   0
#define THISCHIP_CM32M101A       1
#define THISCHIP_ESP32S2         2

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;

#define ON          1
#define OFF         0

typedef enum {
    key_state_0 = 0,
    key_state_1,
    key_state_2,
    key_state_3
} KeyState;

typedef enum {
    key_idle   = 0, //
    key_click,      //press, click (?)
    key_double,
    key_long
} KeyReturn;

#define KDU_DATA_DEBUG 1    //Data interaction processing (using strings, data transmission is stable, and it is not easy to be disturbed by character escape)
#if KDU_DATA_DEBUG
#define kdu_recv_data(a) (a-'0')
#define kdu_send_data(a) (a+'0')
#else
#define kdu_recv_data(a) a
#define kdu_send_data(a) a
#endif

/////////////////////////////////////////////////////////////////////////////////////////
//get a byte or word at the specified address
#define MEM_B( x ) ( *( (byte *) (x) ) )
#define MEM_W( x ) ( *( (word *) (x) ) )

//get the maximun and minimum values
#define MAX( x, y ) ( ((x) > (y)) ? (x) : (y) )
#define MIN( x, y ) ( ((x) < (y)) ? (x) : (y) )

//get the offset of a 'field' in a struct
#define FPOS( type, field ) \
/*lint -e545 */ ( (dword) &(( type *) 0)-> field ) /*lint +e545 */

//convert two bytes to a word in LSB format
#define FLOPW( ray, val)\
    (ray)[0] = ((val)/256);\
    (ray)[1] = ((val)&0xFF)

//get the address of a variable (word width)
#define B_PTR( var ) ( (byte*) (void*) &(var) )
#define W_PTR( var ) ( (word*) (void*) &(var) )

//get the high and low bytes of a word
#define WORD_LO(XXX) ((byte) ((word)(xxx) & 0xFF))
#define WORD_HI(XXX) ((byte) ((word)(xxx) >> 8))

//returns the nearest multiple of n greater than x
//Returns a value, the nearest multiple of n larger than x - Double?
#define RND8(x) ( ( ((x)+7) /8) *8)
#define RNDN(x, n) ((((x)+(n-1)) /(n) ) *(n))

//convert a letter to uppercase
#define UPCASE( c ) ( ( (c)>='a' && (c)<='z') ? ((c)-0x20):(c) )
#define DOCASE( C ) ( ( (C)>='A' && (C)<='Z') ? ((C)+0x20):(C) )

//check whether the charater is a decimal number
//check whether the charater is a hexadecimal number
#define DECCHK( c ) ((c)>='0' && (c)<='9')
#define HEXCHK( c ) ( ((c)>='0'&&(c)<=9) || \
    ( (c)>='A'&& (c)<='F') || \
    ( (c)>='a'&& (c)<='f') )

//check for growth overflow
#define INC_STA( val ) (val = (val)+1 > (val) ? (val+1):(val))

//return the number of elements in an array
#define ARR_SIZE(a) (sizeof((a))/sizeof((a[0])))

//X%(2^n)       mod_by = 2^n
#define MOD_BY_POWER_OF_TWO(val, mod_by) ( (dword)(val) & (dword)((mod_by)-1) )
#endif
