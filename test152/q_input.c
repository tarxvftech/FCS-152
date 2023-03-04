
#include <assert.h>
#include <stdio.h>
#ifdef __cplusplus      
extern "C" {  
#endif  

static const char t9keyboard[10][8] = {" 0", ".?!,/_-1", "ABC2", "DEF3", "GHI4", "JKL5", "MNO6", "PQRS7", "TUV8", "WXYZ9"};

int _t9_lookup( int digit, int count ){
    if( digit > 10 ){
        digit -= '0';
    }
    assert(count < 8);
    assert(count > 0);
    assert(digit >= 0 );
    assert(digit < 10 );
    int ret= t9keyboard[digit][count-1];
    /*printf("t9: %d %d -> %c\n", digit,count, ret);*/
    return ret;
}
void convert_t9simple( char * input, char * output, int maxlength ){
    for( int i =0; input[i] != 0; i++ ){
        if( !( (input[i] >= '0' && input[i] <= '9' )
                    && input[i] != '#' && input[i] != '*'
             ) ){
            return;
        }
    }
    int i =0;
    int o =0;
    int lastinchar = 0;
    int currentinchar = 0;
    int count = 0;
    for( ; input[i] != 0 && i < maxlength-1; i++ ){
        currentinchar = input[i];
        if( currentinchar == '*' ){
            o--;
            continue;
        }
        if( lastinchar != 0 && lastinchar != currentinchar && count > 0 ){
            output[o] = _t9_lookup(lastinchar, count);
            count = 0; 
            o++;
        }
        count++;
        if( currentinchar != '#' ){
            lastinchar = currentinchar;
        } else {
            lastinchar = 0;
            count=0;
        }
    }
    if( count > 0 ){
        output[o++] = _t9_lookup(lastinchar, count);
    }
    output[maxlength-1] = 0;
}
#ifdef __cplusplus      
}
#endif  

