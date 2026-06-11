#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__

#include "main.h"
#include <stdint.h>
#include <sys/types.h>

#define RING_BUFFER_SIZE 128

// success status
typedef enum { RB_OK = 0, RB_ERROR } RB_Status;

// object ringBuffer

typedef struct {
  uint16_t Head;
  uint16_t Tail;
  u_int8_t Buffer[RING_BUFFER_SIZE];
} RingBuffer_t;

// Functions:

// Read
RB_Status RB_Read(RingBuffer_t *Buf, uint8_t *Value);
// Write
RB_Status RB_Write(RingBuffer_t *Buf, uint8_t Value);
// flush
void RB_Flush(RingBuffer_t *Buf);

#endif