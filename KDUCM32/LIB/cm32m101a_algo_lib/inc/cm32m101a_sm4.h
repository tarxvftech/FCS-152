/*
  ******************************************************************************
  *
  * COPYRIGHT(c) 2020, China Mobile IOT
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of China Mobile IOT nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */ 
/*****************************************************************************
* File Name: cm32m101a_sm4.h
* Function: Declaring SM4 algorithm library API
* version: V1.1
* date: 2020-4-8
* ****************************************************************************/

#ifndef _CM32M101A_SM4_H_
#define _CM32M101A_SM4_H_

#include <stdint.h>

#define SM4_ECB (0x11111111)
#define SM4_CBC (0x22222222)
#define SM4_ENC  (0x33333333)
#define SM4_DEC  (0x44444444)
enum{
     SM4_Crypto_OK=0, //SM4 opreation success
	   SM4_Init_OK=0, //SM4 Init opreation success
		 SM4_ADRNULL =0x27A90E35, //the address is NULL
		 SM4_ModeErr, //working mode error(Neither ECB nor CBC)
		 SM4_EnDeErr,  // En&De error(Neither encryption nor decryption)
     SM4_LengthErr,//the word length of input error(the word length is 0 or is not as times as 4)
	   SM4_UnInitError,   //SM4 uninitialized
};

typedef struct{
	uint32_t *in;   // the first part of input to be encrypted or decrypted
	uint32_t *iv;   // the first part of initial vector
	uint32_t *out;  // the first part of out
	uint32_t *key;  // the first part of key
	uint32_t inWordLen; //the word length of input or output 
	uint32_t EnDeMode; //encrypt/decrypt
	uint32_t workingMode; //  ECB/CBC 
}SM4_PARM;

/**
 * @brief SM4_Init
 * @return SM4_Init_OK, SM4 Init success; othets: SM4 Init fail
 * @note    
 */
uint32_t SM4_Init(SM4_PARM *parm);
/**
 * @brief SM4 crypto
 * @param[in] parm pointer to SM4 context and the detail please refer to struct SM4_PARM in SM4.h
 * @return SM4_Crypto_OK, SM4 crypto success; othets: SM4 crypto fail(reference to the definition by enum variation) 
 * @note  1.Please refer to the demo in user guidance before using this function  
 *        2.Input and output can be the same buffer
 *        3. IV can be NULL when ECB mode
 *        4. The word lengrh of message must be as times as 4.
 *        5. If the input is in byte, make sure align by word.
 */
uint32_t SM4_Crypto(SM4_PARM *parm);


/**
 * @brief Close SM4 algorithm
 * @return none
 * @note if you want to close SM4 algorithm, this function can be recalled.  
 */
void SM4_Close(void);


/**
 * @brief Get SM4 lib version
 * @param[out] type pointer one byte type information represents the type of the lib, like Commercial version.\
 * @Bits 0~4 stands for Commercial (C), Security (S), Normal (N), Evaluation (E), Test (T), Bits 5~7 are reserved. e.g. 0x09 stands for CE version.
 * @param[out] customer pointer one byte customer information represents customer ID. for example, 0x00 stands for standard version, 0x01 is for Tianyu customized version...
 * @param[out] date pointer array which include three bytes date information. If the returned bytes are 18,9,13,this denotes September 13,2018 
 * @param[out] version pointer one byte version information represents develop version of the lib. e.g. 0x12 denotes version 1.2.
 * @return none
 * @1.You can recall this function to get SM4 lib information
 */
void SM4_Version(uint8_t *type, uint8_t *customer, uint8_t date[3], uint8_t *version);


#endif
