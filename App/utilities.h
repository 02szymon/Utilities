/*
*   Author:  Szymon
*   Date:    2024-06-01
*   Description: This file contains utility functions for the project, including logging and parsing functions for UART communication.
*/
#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#include "ring_buffer.h"

extern void LOG_UART(UART_HandleTypeDef *huart, const char *str);
extern void PARSER_TAKE_LINE(RingBuffer_t *Buf);
extern void PARSER_PARSE(RingBuffer_t *Buf);
char* strtoke(char *str, const char *delim);

#endif