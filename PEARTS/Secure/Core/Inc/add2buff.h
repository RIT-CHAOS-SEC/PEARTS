//#if !defined(ADD2BUFF_H)
//#define ADD2BUFF_H

#ifndef __APP_ADD2BUFF__H__
#define __APP_ADD2BUFF__H__

#include "stm32l5xx.h"

typedef struct MBuff{
    uint32_t* buffer;
    int pos;
    int offset;
} MBuff;

void setBuffer(uint32_t* buff,int pos, int offset);

void addBitToBuffer(uint32_t bit );

void add32BitToBuffer(uint32_t bit);

void add16BitsToBuffer(uint32_t bit);

void add8BitsToBuffer(uint32_t bit);

#endif // ADD2BUFF_H
