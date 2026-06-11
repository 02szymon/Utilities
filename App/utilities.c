/*
* Author: Szymon
* Date: 2024-06-01
* Description: This file contains utility functions for the project, including logging and parsing functions for UART communication.
*/

#include "main.h"
#include "ring_buffer.h"
#include "utilities.h"
#include "stm32f0xx_hal_rtc.h"
#include "string.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "usart.h"
#include "rtc.h"
#include "stdlib.h"


/* 
*
*   Structures and variables for utilities functions
*
*/

RTC_TimeTypeDef RtcTime;
RTC_DateTypeDef RtcDate;

uint8_t ReceivedData[128];
uint8_t ReceivedOneData;

char Message[100];
char LogMessage[150];
uint8_t MessageLength;

/*
*   End of structures and variables for utilities functions
*/


/*
*
*   Logging fucntions
*
*/

void LOG_UART(UART_HandleTypeDef *huart, const char *str) {
  HAL_RTC_GetTime(&hrtc, &RtcTime, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, &RtcDate, RTC_FORMAT_BIN);
  MessageLength =
      sprintf(LogMessage, "LOG: %02d/%02d/%02d %02d:%02d:%02d -> %s \r\n",
              RtcDate.Date, RtcDate.Month, RtcDate.Year, RtcTime.Hours,
              RtcTime.Minutes, RtcTime.Seconds, str);
  HAL_UART_Transmit(huart, (uint8_t *)LogMessage, MessageLength, HAL_MAX_DELAY);
}

/*
*   End of logging functions
*/


/*
*
* Additional functions for parsing and taking lines from the ring buffer
*
*/

char* strtoke(char *str, const char *delim)
{
  static char *start = NULL; /* stores string str for consecutive calls */
  char *token = NULL; /* found token */
  /* assign new start in case */
  if (str) start = str;
  /* check whether text to parse left */
  if (!start) return NULL;
  /* remember current start as found token */
  token = start;
  /* find next occurrence of delim */
  start = strpbrk(start, delim);
  /* replace delim with terminator and move start to follower */
  if (start) *start++ = '\0';
  /* done */
  return token;
}


/*
*   End of additional functions for parsing and taking lines from the ring buffer
*/



/*
*
*  Parsing functions
*
*/

// This function takes a line from the ring buffer and stores it in the ReceivedData array
void PARSER_TAKE_LINE(RingBuffer_t *Buf) { 

  static uint8_t i = 0;
  do {
    RB_Read(Buf, &ReceivedOneData);
    if (ReceivedOneData == '\n') {
      ReceivedData[i] = 0;
    } else {
      ReceivedData[i] = ReceivedOneData;
    }
    i++;
  } while (ReceivedOneData != '\n');
  i = 0;
}

// This function parses the received line and executes the corresponding command
static void PARSE_LED(void) {
  char *ParsePointer = strtok(NULL, ",");

  if (strlen(ParsePointer) > 0) {
    if ((ParsePointer[0] < '0' || ParsePointer[0] > '1') &&
        ParsePointer[0] != '3') {
      LOG_UART(&huart2, "Invalid LED State");
      return;
    }

    if (ParsePointer[0] == '3') {
      HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
      LOG_UART(&huart2, "Led Toggled");
      return;
    }

    if (ParsePointer[0] == '1') {
      HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
      LOG_UART(&huart2, "Led Turned On");
      return;
    } 

    if (ParsePointer[0] == '0') {
      HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
      LOG_UART(&huart2, "Led Turned Off");
      return;
    }
  }
}

// This function parses the received line and executes the corresponding command for ENV data
static void PARSE_ENV(void) {

  uint8_t i, j;
  float EnvData[3];

  for (i = 0; i < 3; i++) {
    char *ParsePointer = strtok(NULL, ",");
    if (ParsePointer != NULL && strlen(ParsePointer) > 0) {
      for (j = 0; ParsePointer[j] != 0; j++) {
        if ((ParsePointer[j] < '0' || ParsePointer[j] > '9') &&
            ParsePointer[j] != '.') {
          LOG_UART(&huart2, "Invalid ENV Data -> Needed only numbers and dot");
          return;
        }
        EnvData[i] = atof(ParsePointer);
      }
    } else {
      LOG_UART(&huart2, "Invalid ENV Data -> Not enough data");
      return;
    }
  }
  sprintf(Message, "Temp: %.2f C", EnvData[0]);
  LOG_UART(&huart2, Message);
  sprintf(Message, "Hum: %.2f %%", EnvData[1]);
  LOG_UART(&huart2, Message);
  sprintf(Message, "Pres: %.2f hPa", EnvData[2]);
  LOG_UART(&huart2, Message);
}
 

// This function parses the received line and executes the corresponding command based on the first word in the line
void PARSER_PARSE(RingBuffer_t *Buf) {

  PARSER_TAKE_LINE(Buf);

  char *ParsePointer = strtok((char *)ReceivedData, "=");

  if (strcmp("LED", ParsePointer) == 0) {
    PARSE_LED();
  }

  if (strcmp("ENV", ParsePointer) == 0) {
    PARSE_ENV();
  }
}

/*
*  End of parsing functions
*/