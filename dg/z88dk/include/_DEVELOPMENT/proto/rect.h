include(__link__.m4)

#ifndef _RECT_H
#define _RECT_H

#include <stdint.h>

// STRUCTURES

struct r_Ival8
{
   uint8_t coord;              // +0
   uint8_t width;              // +1
};

struct r_Ival16
{
   uint16_t coord;             // +0
   uint16_t width;             // +2  
};

struct r_Rect8
{
   uint8_t x;                  // +0  struct r_Ival8
   uint8_t width;              // +1
   uint8_t y;                  // +2  struct r_Ival8
   uint8_t height;             // +3
};

struct r_Rect16
{
   uint16_t x;                 // +0  struct r_Ival16
   uint16_t width;             // +2
   uint16_t y;                 // +4  struct r_Ival16
   uint16_t height;            // +6
};

#endif
