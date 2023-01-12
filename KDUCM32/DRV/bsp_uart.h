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

/**
 * @file log.h		#include "myuart.h"
 * @author CMIOT Solution Team
 * @version v1.0.0
 *
 * @COPYRIGHT(c) 2020, China Mobile IOT. All rights reserved.
 */
#ifndef __LOG_H__
#define __LOG_H__

#ifndef LOG_ENABLE
#define LOG_ENABLE 1
#endif

#if LOG_ENABLE

#include <stdio.h>

#define LOG_NONE    0
#define LOG_ERROR   10
#define LOG_WARNING 20
#define LOG_INFO    30
#define LOG_DEBUG   40

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_DEBUG
#endif

#if LOG_LEVEL >= LOG_INFO
#define log_info(...) printf(__VA_ARGS__)
#else
#define log_info(...)
#endif

#if LOG_LEVEL >= LOG_ERROR
#define log_error(...) printf(__VA_ARGS__)
#else
#define log_error(...)
#endif

#if LOG_LEVEL >= LOG_WARNING
#define log_warning(...) printf(__VA_ARGS__)
#else
#define log_warning(...)
#endif

#if LOG_LEVEL >= LOG_DEBUG
#define log_debug(...) printf(__VA_ARGS__)
#else
#define log_debug(...)
#endif

void log_init(void);

#else /* !LOG_ENABLE */

#define log_info(...)
#define log_warning(...)
#define log_error(...)
#define log_debug(...)
#define log_init()

#endif

#define log_func() log_debug("call %s\r\n", __FUNCTION__)

void TIM6_Init(void);
void UART1_Put_String(unsigned char s[]);  
void UART1_Send_Message(char s[], int size); 

//test
void UART1_PrintBack(void);


#endif /* __LOG_H__ */
