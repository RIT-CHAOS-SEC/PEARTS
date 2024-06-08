#include "stm32l5xx_hal.h"
#include "application.h"
#include "math.h"
#include "stm32l5xx_hal_tim.h"
#include "core_cm33.h"

#if APP_SEL == PRIME
typedef  unsigned char  bool;
typedef  unsigned long  ulong;

bool divides (ulong n, ulong m);
bool even (ulong n);
bool prime (ulong n);
void swap (ulong* a, ulong* b);

bool divides (ulong n, ulong m) {
  return (m % n == 0);
}

bool even (ulong n) {
  return (divides (2, n));
}

bool prime (ulong n) {
  ulong i;
  if (even (n))
      return (n == 2);
  for (i = 3; i * i <= n; i += 2) {
      if (divides (i, n)) /* ai: loop here min 0 max 357 end; */
          return 0;
  }
  return (n > 1);
}

void swap (ulong* a, ulong* b) {
  ulong tmp = *a;
  *a = *b;
  *b = tmp;
}

volatile int result = 0;
ulong x;
ulong y;

void application (void)
{
  x =  21649L;
  y = 513239L;
  swap (&x, &y);
  result = (!(prime(x) && prime(y)));
  SECURE_record_output_data(0);
}
#endif

#if APP_SEL == DUFF
/* $Id: duff.c,v 1.2 2005/04/04 11:34:58 csg Exp $ */



/*----------------------------------------------------------------------

 *  WCET Benchmark created by Jakob Engblom, Uppsala university,

 *  February 2000.

 *

 *  The purpose of this benchmark is to force the compiler to emit an

 *  unstructured loop, which is usually problematic for WCET tools to

 *  handle.

 *

 *  The execution time should be constant.

 *

 *  The original code is "Duff's Device", see the Jargon File, e.g. at

 *  http://www.tf.hut.fi/cgi-bin/jargon.  Created in the early 1980s

 *  as a way to express loop unrolling in C.

 *

 *----------------------------------------------------------------------*/



#define ARRAYSIZE  100

#define INVOCATION_COUNT 43 /* exec time depends on this one! */





void duffcopy( char *to, char *from, int count)

{

  int n=(count+7)/8;

  switch(count%8){

  case 0: do{     *to++ = *from++;

  case 7:         *to++ = *from++;

  case 6:         *to++ = *from++;

  case 5:         *to++ = *from++;

  case 4:         *to++ = *from++;

  case 3:         *to++ = *from++;

  case 2:         *to++ = *from++;

  case 1:         *to++ = *from++;

  } while(--n>0);

  }

}

char source[ARRAYSIZE];

char target[ARRAYSIZE];

void application(void)
{
  duffcopy( source, target, INVOCATION_COUNT );
  SECURE_record_output_data(0);
}
#endif

#if APP_SEL == CRC32
// between 8-9kb

/* This scale factor will be changed to equalise the runtime of the
   benchmarks. */
#define SCALE_FACTOR    (REPEAT_FACTOR >> 5)

#include <stdlib.h>

#ifdef __TURBOC__
#pragma warn -cln
#endif

/**********************************************************************\
  |* Demonstration program to compute the 32-bit CRC used as the frame  *|
  |* check sequence in ADCCP (ANSI X3.66, also known as FIPS PUB 71     *|
  |* and FED-STD-1003, the U.S. versions of CCITT's X.25 link-level     *|
  |* protocol).  The 32-bit FCS was added via the Federal Register,     *|
  |* 1 June 1982, p.23798.  I presume but don't know for certain that   *|
  |* this polynomial is or will be included in CCITT V.41, which        *|
  |* defines the 16-bit CRC (often called CRC-CCITT) polynomial.  FIPS  *|
  |* PUB 78 says that the 32-bit FCS reduces otherwise undetected       *|
  |* errors by a factor of 10^-5 over 16-bit FCS.                       *|
  \**********************************************************************/

/* Some basic types.  */
typedef unsigned char  BYTE;
typedef unsigned long  DWORD;
typedef unsigned short WORD;

#define UPDC32(octet,crc) (crc_32_tab[((crc)^((BYTE)octet)) & 0xff] ^ ((crc) >> 8))

/* Need an unsigned type capable of holding 32 bits; */

typedef DWORD UNS_32_BITS;

/* Copyright (C) 1986 Gary S. Brown.  You may use this program, or
   code or tables extracted from it, as desired without restriction.*/

/* First, the polynomial itself and its table of feedback terms.  The  */
/* polynomial is                                                       */
/* X^32+X^26+X^23+X^22+X^16+X^12+X^11+X^10+X^8+X^7+X^5+X^4+X^2+X^1+X^0 */
/* Note that we take it "backwards" and put the highest-order term in  */
/* the lowest-order bit.  The X^32 term is "implied"; the LSB is the   */
/* X^31 term, etc.  The X^0 term (usually shown as "+1") results in    */
/* the MSB being 1.                                                    */

/* Note that the usual hardware shift register implementation, which   */
/* is what we're using (we're merely optimizing it by doing eight-bit  */
/* chunks at a time) shifts bits into the lowest-order term.  In our   */
/* implementation, that means shifting towards the right.  Why do we   */
/* do it this way?  Because the calculated CRC must be transmitted in  */
/* order from highest-order term to lowest-order term.  UARTs transmit */
/* characters in order from LSB to MSB.  By storing the CRC this way,  */
/* we hand it to the UART in the order low-byte to high-byte; the UART */
/* sends each low-bit to hight-bit; and the result is transmission bit */
/* by bit from highest- to lowest-order term without requiring any bit */
/* shuffling on our part.  Reception works similarly.                  */

/* The feedback terms table consists of 256, 32-bit entries.  Notes:   */
/*                                                                     */
/*  1. The table can be generated at runtime if desired; code to do so */
/*     is shown later.  It might not be obvious, but the feedback      */
/*     terms simply represent the results of eight shift/xor opera-    */
/*     tions for all combinations of data and CRC register values.     */
/*                                                                     */
/*  2. The CRC accumulation logic is the same for all CRC polynomials, */
/*     be they sixteen or thirty-two bits wide.  You simply choose the */
/*     appropriate table.  Alternatively, because the table can be     */
/*     generated at runtime, you can start by generating the table for */
/*     the polynomial in question and use exactly the same "updcrc",   */
/*     if your application needn't simultaneously handle two CRC       */
/*     polynomials.  (Note, however, that XMODEM is strange.)          */
/*                                                                     */
/*  3. For 16-bit CRCs, the table entries need be only 16 bits wide;   */
/*     of course, 32-bit entries work OK if the high 16 bits are zero. */
/*                                                                     */
/*  4. The values must be right-shifted by eight bits by the "updcrc"  */
/*     logic; the shift must be unsigned (bring in zeroes).  On some   */
/*     hardware you could probably optimize the shift in assembler by  */
/*     using byte-swap instructions.                                   */

/* The BEEBS version of this code uses its own version of rand, to
   avoid library/architecture variation. */

const static UNS_32_BITS crc_32_tab[] = { /* CRC polynomial 0xedb88320 */
   0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
   0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
   0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
   0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
   0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
   0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
   0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
   0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
   0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
   0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
   0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
   0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
   0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
   0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
   0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
   0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
   0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
   0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
   0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
   0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
   0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
   0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
   0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
   0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
   0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
   0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
   0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
   0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
   0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
   0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
   0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
   0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
   0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
   0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
   0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
   0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
   0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
   0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
   0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
   0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
   0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
   0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
   0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};


/* Yield a sequence of random numbers in the range [0, 2^15-1].

   The seed is always initialized to zero.  long int is guaranteed to be at
   least 32 bits. The seed only ever uses 31 bits (so is positive).

   For BEEBS this gets round different operating systems using different
   multipliers and offsets and RAND_MAX variations. */

static long int seed = 0;

static int
rand_beebs ()
{

  seed = (seed * 1103515245L + 12345) & ((1UL << 31) - 1);
  return (int) (seed >> 16);

}


DWORD crc32pseudo()
{
   int i;
   register DWORD oldcrc32;

   oldcrc32 = 0xFFFFFFFF;

   for (i = 0 ; i < 1024; ++i)
   {
      oldcrc32 = UPDC32(rand_beebs(), oldcrc32);
   }

   return ~oldcrc32;
}

void application()
{
  DWORD r;
  r = crc32pseudo();
  SECURE_record_output_data((int)r);
}
#endif

#if APP_SEL == SEARCH
//16 KB
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sglib.h"

#define MAX_ELEMS 1000

int array[100] = {14, 66, 12, 41, 86, 69, 19, 77, 68, 38, 26, 42, 37, 23, 17, 29, 55, 13,
  90, 92, 76, 99, 10, 54, 57, 83, 40, 44, 75, 33, 24, 28, 80, 18, 78, 32, 93, 89, 52, 11,
  21, 96, 50, 15, 48, 63, 87, 20, 8, 85, 43, 16, 94, 88, 53, 84, 74, 91, 67, 36, 95, 61,
  64, 5, 30, 82, 72, 46, 59, 9, 7, 3, 39, 31, 4, 73, 70, 60, 58, 81, 56, 51, 45, 1, 6, 49,
  27, 47, 34, 35, 62, 97, 2, 79, 98, 25, 22, 65, 71, 0};

/* Use within BENCHMARK to avoid calls being optimised out.  */
volatile int found = 0;

void application()
{
  volatile int cnt=0;
  int tmp, index, i;

  index = 0;
  for(i=0; i< 100; i++) {
    tmp = array[i];
    SGLIB_ARRAY_BINARY_SEARCH(int, array, 0, i, tmp, SGLIB_NUMERIC_COMPARATOR, found, index);
    cnt += index;
  }

  SECURE_record_output_data(cnt);
}

#endif

#if APP_SEL == MOUSE
// Mouse buttons
#define MOUSE_SWITCH 2      // switch to turn on and off mouse control
#define MOUSE_LEFT_BTN 5
#define MOUSE_RIGHT_BTN 3

#define MOUSE_X_AXIS_INPT 'x'
#define MOUSE_Y_AXIS_INPT 'y'

#define MOUSE_SENSITIVITY 10
#define JOYSTICK_DEADZONE 1

#define LOOP_DELAY 5

// Operation modes for readMouseButton function
#define INPUT_LOW_MODE 1
#define INPUT_HIGH_MODE 2
#define INPUT_CHANGED_MODE 3


// Ported mouse.cpp
#define MOUSE_LEFT 1
#define MOUSE_RIGHT 2
#define MOUSE_MIDDLE 4
#define MOUSE_ALL (MOUSE_LEFT | MOUSE_RIGHT | MOUSE_MIDDLE)

#define bool    uint8_t
#define true    1
#define false   0

uint8_t _buttons;
//================================================================================
//================================================================================
//  Mouse

/* This function is for limiting the input value for x and y
 * axis to -127 <= x/y <= 127 since this is the allowed value
 * range for a USB HID device.
 */
signed char limit_xy(int const xy)
{
  if     (xy < -127) return -127;
  else if(xy >  127) return 127;
  else               return xy;
}

void mouseBegin(void) 
{
   
}

void mouseEnd(void) 
{
   
}

void mouseMove(int x, int y, signed char wheel)
{
   
    uint8_t m[4];
    m[0] = _buttons;
    m[1] = limit_xy(x);
    m[2] = limit_xy(y);
    m[3] = wheel;
    //HID().SendReport(1,m,4);
}

void mouseClick(uint8_t b)
{
   
    _buttons = b;
    mouseMove(0,0,0);
    _buttons = 0;
    mouseMove(0,0,0);
}
void mouseButtons(uint8_t b)
{
   
    if (b != _buttons)
    {
        _buttons = b;
        mouseMove(0,0,0);
    }
}

void mousePress(uint8_t b) 
{
   
    mouseButtons(_buttons | b);
}

void mouseRelease(uint8_t b)
{
   
    mouseButtons(_buttons & ~b);
}

bool mouseIsPressed(uint8_t b)
{
   
    if ((b & _buttons) > 0) 
        return true;
    return false;
}

bool mouseActive = false;    // whether or not to control the mouse
bool lastSwitchState = true; // previous switch state
bool lastMouseRightState = true;
bool lastMouseLeftState = true;

int mockReads[200] = {95, -39, -39, -10, -100, 119, 43, 15, 67, 129, 98, -30, 26, 90, -154, -109, -182, -103, -114, -111, 14, -119, -148, -46, -67, -162, -1, -198, 116, -82, 4, -158, 60, 73, 77, -74, 180, -128, -134, -3, 195, -31, 123, -33, 0, 150, 42, 125, 27, -164, -38, -99, -162, 112, 70, -21, 134, -123, 72, 184, 81, -169, -147, 146, -3, -143, 173, 13, 90, 189, -99, -40, -69, -65, 80, -34, -82, -196, -68, -75, 200, -144, 163, 96, 14, 38, -140, 31, 94, 54, 87, 173, -75, -199, -74, -8, 95, -185, -34, 62, -1, -120, -124, -164, -2, 51, 116, -86, -135, 114, 179, 21, 173, -39, -40, -28, -1, -73, -1, 192, -76, -141, -91, -160, 139, 43, -132, 38, -68, -168, 87, 9, 66, 50, -84, -146, -56, -104, -114, -45, -7, 43, 161, 58, -65, -7, -66, -54, 129, 55, 101, 28, -103, 46, 121, 188, -83, 147, 174, -195, -54, -169, 76, 31, 194, -104, -112, 53, 159, 35, -172, 184, -175, -56, -148, 192, 132, -87, 124, -160, -22, -163, -172, -139, 36, -125, -71, -99, -139, -81, 167, 40, 50, 45, -94, -20, -111, 21, 11, -55};

/*void setup() {
  pinMode(MOUSE_SWITCH, INPUT);       // the switch pin
  pinMode(MOUSE_LEFT_BTN, INPUT);
  pinMode(MOUSE_RIGHT_BTN, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
}*/
int indx = 0;
int analogRead(char axis){
   
    int readings;
    if(axis == 'x'){
        readings = mockReads[indx];
    }else if(axis == 'y'){
        readings = mockReads[indx];
    }
    indx += 1;
    return readings;
}

int map(int reading, int min, int max, int min_sen, int max_sen){
   
    return (reading - min) * (max_sen - min_sen) / (max - min) + min_sen;
}

void handleMouse() {
   
  int xReading = analogRead(MOUSE_X_AXIS_INPT);
  int yReading = analogRead(MOUSE_Y_AXIS_INPT);

  // map the reading from the analog input range to the output range:
  int mouseX = map(xReading, 0, 1024, -MOUSE_SENSITIVITY, MOUSE_SENSITIVITY);
  int mouseY = map(yReading, 0, 1024, MOUSE_SENSITIVITY, -MOUSE_SENSITIVITY);

  // if joystick moved out of the deadzone, move the mouse
  if (mouseX > JOYSTICK_DEADZONE || mouseX < -JOYSTICK_DEADZONE || mouseY < -JOYSTICK_DEADZONE || mouseY > JOYSTICK_DEADZONE)
  {
    mouseMove(mouseX, mouseY, 0);
  }
}

bool digitalRead(int button){
   
    return !lastSwitchState;
}

/**
 * Has folloving operation modes:
 * 1. INPUT_LOW_MODE - returns true when btn state changes from HIGH to LOW
 * 2. INPUT_HIGH_MODE - returns true when btn state changes from LOW to HIGH
 * 3. INPUT_CHANGED_MODE - return true when btn state changes from HIGH to LOW or from LOW to HIGH
 */
bool readMouseButton(int button, bool lastState, unsigned char mode) {
   
  bool ret = false;
  bool state = digitalRead(button);
  if (state != lastState) {
    if ((mode == INPUT_LOW_MODE && state == false) || (mode == INPUT_HIGH_MODE && state == true) || mode == INPUT_CHANGED_MODE) {
      ret = true;
    }
  }
  lastState = state;
  return ret;
}

void application() {
   
  for(int i=0; i<100; i++){
    // check if mouse in ON/OFF
    if (readMouseButton(MOUSE_SWITCH, lastSwitchState, INPUT_LOW_MODE)) {
      if (mouseActive) {
        mouseEnd();
        mouseActive = false;
      } else {
        mouseBegin();
        mouseActive = true;
      }
    }
    //digitalWrite(LED_BUILTIN, mouseActive);

    if (mouseActive) {
      handleMouse();

      // check left mouse btn
      if (readMouseButton(MOUSE_LEFT_BTN, lastMouseLeftState, INPUT_CHANGED_MODE)) {
        if (lastMouseLeftState == false && !mouseIsPressed(MOUSE_LEFT_BTN)) {
          mousePress(MOUSE_LEFT);
        } else if (lastMouseLeftState == true && mouseIsPressed(MOUSE_LEFT_BTN))
          mouseRelease(MOUSE_LEFT);
        }
      }

      // check right mouse btn
      if (readMouseButton(MOUSE_RIGHT_BTN, lastMouseRightState, INPUT_HIGH_MODE)) {
        mousePress(MOUSE_RIGHT);
        mouseRelease(MOUSE_RIGHT);
    } 
  
    for(int j=0; j < LOOP_DELAY; j++){}
  }

}
#endif

// #include "stdio.h"
#if APP_SEL == GPS
// #include <ctype.h>
#include <stdlib.h>
// #include <string.h>

//Lets fake another library

// get_position, f_get_position, get_datetime, crack_datetime stats
#define _GPS_MAX_FIELD_SIZE 15
#define _GPRMCterm "GPRMC"
#define _GPGGAterm "GPGGA"
#define _GNRMCterm "GNRMC"
#define _GNGGAterm "GNGGA"
#define GPS_INVALID_ANGLE 999999999

enum {GPS_SENTENCE_GPGGA,  GPS_SENTENCE_GPRMC, GPS_SENTENCE_OTHER};
static const float GPS_INVALID_F_ANGLE, GPS_INVALID_F_ALTITUDE, GPS_INVALID_F_SPEED;
int encodedCharCount = 0; 
uint8_t parity =0;
int isChecksumTerm = 0;
uint8_t curSentenceType = GPS_SENTENCE_OTHER;
uint8_t curTermNumber = 0;
uint8_t curTermOffset = 0;
char term[_GPS_MAX_FIELD_SIZE] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
int sentenceHasFix = 0;
int passedChecksumCount = 0;
int sentencesWithFixCount = 0;
int failedChecksumCount = 0;

int mystrcmp(char * s1, char * s2) {
 
    int res = 0;
    int first = 1;
    for(int i = 0; i < _GPS_MAX_FIELD_SIZE; i++) {
      if (first == 1 && s1[i] > s2[i]) {
        res = 1;
        first = 0;
      }
      else if (first == 1 && s1[i] < s2[i]) {
        res = 1;
        first = 0;
      }
    }
    return res;
}

int isdigit(int c)
{
 
    return (unsigned)c - '0' < 10;
}

int fromHex(char a){
 
    if (a >= 'A' && a <= 'F'){
        return a - 'A' + 10;
    }else if (a >= 'a' && a <= 'f'){
        return a - 'a' + 10;
    }else{
        return a - '0';
    }
}

// mock functions
int validDate = 0;
int upDate = 0;
uint32_t dateValue = 0;
void date_commit(){
 
    validDate = 1;
    upDate = 1;
}

int32_t timeVal = 0;
int validTime = 0;
int updateTime = 0;
void time_commit(){
 
    validTime = 1;
    updateTime = 1;
}


float lat = 0;
float lng = 0;
int rawNewLatDataNegative = 0;
int rawNewLongDataNegative = 0;
int validLoc = 0;
int updateLoc = 0;
void location_commit(){
 
    validLoc = 0;
    updateLoc = 0;
}

float speedVal = 0;
int validSpeed = 0;
int updateSpeed = 0;
void speed_commit(){
 
    validSpeed = 1;
    updateSpeed = 1;
}

float degrees = 0.0;
int validDeg = 0;
int updateDeg = 0;
void course_commit(){
 
    validDeg = 1;
    updateDeg = 1;
}

float height = 0.0;
int validAlt = 0;
int updateAlt = 0;
void altitude_commit(){
 
    validAlt = 1;
    updateAlt = 1;
}

int validSat = 0;
int updateSat = 0;
int satCount = 0;
void satellites_commit(){
 
    validSat = 1;
    updateSat = 1;
}

float hdopVal = 0.0;
int validHDop = 0;
int updateHDop = 0;
void hdop_commit(){
 
    validHDop = 1;
    updateHDop = 1;
}

float parseDegrees(char *term)
{
  uint32_t leftOfDecimal = (uint32_t)atol(term);
  uint16_t minutes = (uint16_t)(leftOfDecimal % 100);
  uint32_t multiplier = 10000000UL;
  uint32_t tenMillionthsOfMinutes = minutes * multiplier;

  int16_t deg = (int16_t)(leftOfDecimal / 100);

  while (isdigit(*term))
    ++term;

  if (*term == '.')
    while (isdigit(*++term))
    {
      multiplier /= 10;
      tenMillionthsOfMinutes += (*term - '0') * multiplier;
    }

  deg += ((5 * tenMillionthsOfMinutes + 1) / 3)/1000000000.0f;
  return (float) deg;
}

int32_t parseDecimal(char *term){
 
    int negative = *term == '-';
    if (negative) ++term;
    int32_t ret = 100 * (int32_t)atol(term);
    while (isdigit(*term)) ++term;
    if (*term == '.' && isdigit(term[1])){
        ret += 10 * (term[1] - '0');
        if (isdigit(term[2]))
            ret += term[2] - '0';
    }
    return negative ? -ret : ret;
}

void time_setTime(char *term){
 
    timeVal = parseDecimal(term);
}

void location_setLatitude(char *term)
{
 
   lat = parseDegrees(term);
}

void location_setLongitude(char *term)
{
 
   lng = parseDegrees(term);
}

void speed_set(char *term){
 
    speedVal = parseDecimal(term);
}

void course_set(char *term){
 
    degrees = parseDecimal(term);
}

void satellites_set(char *term){
 
    satCount ++;
}

void date_setDate(char *term){
 
    dateValue = atol(term);
}

void hdop_set(char *term){
 
    hdopVal = parseDecimal(term);
}

void altitude_set(char *term){
 
    height = parseDecimal(term);
}

int endOfTermHandler(){
 
    if(isChecksumTerm){
        unsigned char checksum = 16 * fromHex(term[0]) + fromHex(term[1]);
        if (checksum == parity){
            passedChecksumCount++;
            if (sentenceHasFix){
                ++sentencesWithFixCount;
            }
            switch(curSentenceType){
                case GPS_SENTENCE_GPRMC:
                    date_commit();
                    time_commit();
                    if(sentenceHasFix){
                        location_commit();
                        speed_commit();
                        course_commit();
                    }
                    break;
                case GPS_SENTENCE_GPGGA:
                    time_commit();
                    if(sentenceHasFix){
                        location_commit();
                        altitude_commit();
                    }
                    satellites_commit();
                    hdop_commit();
                    break;
               }
            return 1;
        }else{
            ++failedChecksumCount;  
        }
        // return 0;
    }
    if (curTermNumber == 0){
        if(!mystrcmp(term, _GPRMCterm) || !mystrcmp(term, _GNRMCterm)){
            curSentenceType = GPS_SENTENCE_GPRMC;
        }else if (!mystrcmp(term, _GPGGAterm) || !mystrcmp(term, _GNGGAterm)){
            curSentenceType = GPS_SENTENCE_GPGGA;
        }else{
            curSentenceType = GPS_SENTENCE_OTHER;
        }
        // return 0;
    }
    if (curSentenceType != GPS_SENTENCE_OTHER && term[0]){
        switch(curSentenceType){
            case GPS_SENTENCE_GPRMC:
                switch(curTermNumber){
                    case 1:
                        time_setTime(term);
                        break;
                    case 2:
                        sentenceHasFix = term[0] == 'A';
                        break;
                    case 3:
                        location_setLatitude(term);
                        break;
                    case 4:
                        rawNewLatDataNegative = term[0] == 'S';
                        break;
                    case 5:
                        location_setLongitude(term);
                        break;
                    case 6:
                        rawNewLongDataNegative = term[0] == 'W';
                        break;
                    case 7:
                        speed_set(term);
                        break;
                    case 8:
                        course_set(term);
                        break;
                    case 9:
                        date_setDate(term);
                        break;
                }
                break;
            case GPS_SENTENCE_GPGGA:
                switch(curTermNumber){
                    case 1:
                        time_setTime(term);
                        break;
                    case 2:
                        location_setLatitude(term);
                        break;
                    case 3:
                        rawNewLatDataNegative = term[0] == 'S';
                        break;
                    case 4:
                        location_setLongitude(term);
                        break;
                    case 5:
                        rawNewLongDataNegative = term[0] == 'W';
                        break;
                    case 6:
                        sentenceHasFix = term[0] > 0;
                        break;
                    case 7:
                        satellites_set(term);
                        break;
                    case 8:
                        hdop_set(term);
                        break;
                    case 9:
                        altitude_set(term);
                        break;
                }
                break;
            }
        }
    return 0;
}


int gps_encode(char c) {
 
    SECURE_record_output_data(c);
    ++encodedCharCount;

    switch(c){
        case ',':
            parity ^= (uint8_t)c;
            SECURE_record_output_data('1');
        case '\r':
            SECURE_record_output_data('2');
        case '\n':
            SECURE_record_output_data('3');
        case '*':
            {
            SECURE_record_output_data('4');
            int isValidSentence = 0;
            if (curTermOffset < 15){
                term[curTermOffset] = 0;
                isValidSentence = endOfTermHandler();
            }
            ++curTermNumber;
            curTermOffset = 0;
            isChecksumTerm = (int)c == '*';
            return isValidSentence;
            break;
            }
        case '$':
            SECURE_record_output_data('5');
            curTermNumber = curTermOffset = 0;
            parity = 0;
            curSentenceType = GPS_SENTENCE_OTHER;
            isChecksumTerm = 0;
            sentenceHasFix = 0;
            return 0;
        default:
            SECURE_record_output_data('6');
            if (curTermOffset < _GPS_MAX_FIELD_SIZE-1){
                term[curTermOffset++] = c;
            }
            if (!isChecksumTerm){
                parity ^= c;
            }
            return 0;
    }
    return 0;
}

void get_position(long *latitude, long *longitude){
 
    if (latitude) *latitude = lat;
    if (longitude) *longitude = lng;
}

void f_get_position(float *flat, float *flng){
 
    long tempLat, tempLong;
    get_position(&tempLat, &tempLong);
    *flat = tempLat == GPS_INVALID_ANGLE ? GPS_INVALID_F_ANGLE : (tempLat / 1000000.0f);
    *flng = tempLong == GPS_INVALID_ANGLE ? GPS_INVALID_F_ANGLE : (tempLong / 1000000.0f);
}

void get_datetime(unsigned long *date, unsigned long *time){
 
    if(date) *date = dateValue;
    if(time) *time = timeVal;
}

void crack_datetime(int* year, unsigned char* month, unsigned char* day, unsigned char* hour, unsigned char* minute, unsigned char* second, unsigned char* hundredths){
 
    unsigned long tempDate, tempTime;
    get_datetime(&tempDate, &tempTime);
    if (year) {
        *year = tempDate % 100;
        *year += *year > 80 ? 1900 : 2000;
    } 
    if (month) *month = (tempDate / 100) % 100;
    if (day) *day = tempDate / 10000;
    if (hour) *hour = tempTime / 1000000;
    if (minute) *minute = (tempTime / 10000) % 100;
    if (second) *second = (tempTime / 100) % 100;
    if (hundredths) *hundredths = tempTime % 100;
}

void stats(unsigned long* chars, unsigned short* sentences, unsigned short* failed){
 
    if (chars) *chars = encodedCharCount;
    if (sentences) *sentences = passedChecksumCount;
    if (failed) *failed = failedChecksumCount;
} 

// __attribute ((naked)) void my_aeabi_i2d()
//     __asm__ volatile("teq   r0, #0");
//     __asm__ volatile("itteq");
//     __asm__ volatile("moveq r1, #0");
//     __asm__ volatile("bxeq  lr");
//     __asm__ volatile("bx    lr");
// }

void gpsdump()
{

  long lat, lon;
  float flat, flon;
  unsigned long date, time, chars;
  int year;
  unsigned char month, day, hour, minute, second, hundredths;
  unsigned short sentences, failed;

  get_position(&lat, &lon);
  f_get_position(&flat, &flon);
  get_datetime(&date, &time);
  crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths);
  stats(&chars, &sentences, &failed);
}


#define BUFFER_LEN    46
const char input_buffer[BUFFER_LEN] = 
{'$', 'G', 'P', 'R', 'M', 'C', '\n', '1', '0', '.', '2', '3', ',', 'A',',', '-', '2', '4', ',', 'N', ',',
'5', '4', ',', 'W', ',', '1', '5', '.', '4', '3',',', '9', '9', '.', '9', ',', '1', '2', '3', '4', '*', '3',
'4', '\n'}; // $GPRMC\n10.23,A,-24,N,54,W,15.43,99.9,1234*34\n$GPGGA\n10.23,-9,S,13,E,1,Doesnt matter,12.34,56.78*12\n\0};

volatile uint32_t lt, ln;
volatile uint32_t d, t, c;
volatile int y;
volatile char m, da, h, mi, s, hu;
volatile unsigned short se, f;

void application(){
 
    for (int buffer_index = 0; buffer_index < BUFFER_LEN; buffer_index++){
        gps_encode(input_buffer[buffer_index]);
    }

    gpsdump();
}

#endif

#if APP_SEL == GEIGER
uint32_t bouncer_state;
#define sig_len 14
uint8_t signals[sig_len] = {0b00000001, 0b00000001, 0b00000000, 0b00000010, 0b00000011, 0b00000000, 0b00000000, 0b00000001, 0b00000011, 0b00000011, 0b00000001, 0b00000010, 0b00000010, 0b00000000};
int sig_idx = 0;

// Lets emulate the bouncer
const uint8_t DEBOUNCED_STATE = 0b00000001;
const uint8_t UNSTABLE_STATE = 0b00000010;
const uint8_t CHANGED_STATE = 0b00000100;

void setStateFlag(const uint8_t flag) {
 
    bouncer_state |= flag;
}

void unsetStateFlag(const uint8_t flag) {
 
    bouncer_state &= ~flag;
}

void toggleStateFlag(const uint8_t flag) {
 
    bouncer_state ^= flag;
}

uint32_t getStateFlag(const uint8_t flag) {
 
    return ((bouncer_state & flag) != 0);
}  
void changeState(){
    toggleStateFlag(DEBOUNCED_STATE);
    setStateFlag(CHANGED_STATE);
}

uint32_t digitalRead(){
 
    uint32_t val = signals[sig_idx];
    sig_idx++;
    return val;
}

uint32_t changed() {
 
    return getStateFlag(CHANGED_STATE);
}

uint32_t readCurrentState() {
 
    return digitalRead();
}

void bouncer_begin(){
 
   bouncer_state = 0;
   if (readCurrentState()){
      setStateFlag(DEBOUNCED_STATE | UNSTABLE_STATE);
   } 
}

uint32_t bouncer_update(){
 
   unsetStateFlag(CHANGED_STATE);
   
   uint32_t currentState = readCurrentState();

   if (((currentState & UNSTABLE_STATE) !=0) != getStateFlag(UNSTABLE_STATE)){
      toggleStateFlag(UNSTABLE_STATE);
   }else{
      if (((currentState & DEBOUNCED_STATE) !=0) != getStateFlag(DEBOUNCED_STATE)){
          changeState();
      }
   }
   return changed();
}

int bouncer_read() {
    return getStateFlag(DEBOUNCED_STATE);
}

uint8_t datastring[10] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
void application() {
  int index = 0;

  bouncer_begin();
  while(sig_idx < sig_len){
      if(bouncer_update()){
          if(bouncer_read() == 0){
              datastring[index] = '1';
              datastring[index+1] = ',';
              index = index + 2;   
          }
      }
  }
}
#endif

#if APP_SEL == BASIC


// int test = 0;
int data = 0;
void (*fun)();

// void fun1() { test = 1; } 
// void fun2() { test = 2; } 

void reset_then_evaluate(){
  data = 0;
  evaluate(data);
}

void evaluate(int value){
  if ((value & 1) == 1){
      if (value == 1){
        data++;
      }
      data++;
  } else{
      data = data + 2;
  }
}

int init = 1;

void application(){
    if (init){
      fun = &reset_then_evaluate;
    } else {
      fun = &evaluate;
    }

    fun();
    
    for(int i=1; i<32; i++){
      evaluate(i);
    }
}

#endif

/******************* SYRINGE APP *********************/

#if APP_SEL == SYRINGE
enum{PUSH,PULL};
int steps;

extern TIM_HandleTypeDef htim1;
void delay(uint32_t us){
    // htim1.Instance->CR1 |= TIM_CR1_CEN;

    // while (htim1.Instance->CNT < us);

    // htim1.Instance->CR1 &= ~TIM_CR1_CEN;
 
    for(int i=0; i<us; i++);
}

uint8_t maxinputpointer = 2;
char input[2] = "+\n";

char getserialinput(uint8_t inputserialpointer)
{
 
    if (inputserialpointer < maxinputpointer)
    {
        return input[inputserialpointer];
    }
    return 0;
}


// Bolus size
uint16_t mLBolus =  5;
void run_syringe()
{
 
    /* -- Global variables -- */
    // Input related variables
    volatile uint8_t inputserialpointer = -1;
    uint16_t inputStrLen = 0;
    char inputStr[10]; //input string storage

    // Steps per ml
    int ustepsPerML = (MICROSTEPS_PER_STEP * STEPS_PER_REVOLUTION * SYRINGE_BARREL_LENGTH_MM) / (SYRINGE_VOLUME_ML * THREADED_ROD_PITCH);

    //int ustepsPerML = 10;
    int inner = 0;
    int outer = 0;
    steps = 0;

    while(outer < 1)
    {
       char c = getserialinput(inputserialpointer);
       inputserialpointer++;
       // hex to char reader
       while (inner < 10)
       {
   
           if(c == '\n') // Custom EOF
           {
       
               break;
           }
   
           if(c == 0)
           {
       
               outer = 10;
               break;
           }
   
           inputStr[inputStrLen++] = c;
           c = getserialinput(inputserialpointer);
           inputserialpointer++;
   
           inner += 1;
       }
       inputStr[inputStrLen++] = '\0';
       steps = mLBolus * ustepsPerML;
       // SECURE_record_output_data(mLBolus);
       // SECURE_record_output_data(ustepsPerML);
       // SECURE_record_output_data(steps);
    
        for(int i=0; i < steps; i++)
        {
            if(inputStr[0] == '+' || inputStr[0] == '-')    
            {   
                // write 0xff to port
                sensor = 0xff;
                delay(SPEED_MICROSECONDS_DELAY);
            }
            // write 0x00 to port
            sensor = 0x00;
            delay(SPEED_MICROSECONDS_DELAY);
        }
        // delay(SPEED_MICROSECONDS_DELAY);
        inputStrLen = 0;
        outer += 1;
    }
}

void application()
{
 
    // for(int i=0; i<TOTAL_INJECTIONS; i++){
    run_syringe();
    // }
}
#endif

#if APP_SEL == TEMP
int temp;
int humidity;
uint8_t data[5] = {0,0,0,0,0};
uint8_t valid_reading = 0;

extern __IO uint32_t uwTick;
void delay(uint32_t us){
 
    // uint32_t start = uwTick;
    // while(uwTick - start < us);
    for(int i=0; i<us; i++);
}

// uint8_t sim_sensor = 1;
// counter += (sim_sensor) >> 8;
// sim_sensor++;
// sim_sensor = (sim_sensor >> 1);
uint8_t counter = 0;

void read_data(){

    // pull signal high & delay
    GPIOA->BRR = (uint32_t)GPIO_PIN_8;
    delay(250);

    /// pull signal low for 20us
    GPIOA->BSRR = (uint32_t)GPIO_PIN_8;
    delay(20);

    // pull signal high for 40us
    GPIOA->BRR = (uint32_t)GPIO_PIN_8;
    delay(40);

    //read timings
    int j = 0;
    int i;
    for(i=0; i<MAX_READINGS; i++){

        counter += (GPIOA->IDR & GPIO_PIN_8) >> 8;

        // ignore first 3 transitions
        if ((i >= 4) && ( (i & 0x01) == 0x00)) {
    
            // shove each bit into the storage bytes
            data[j >> 3] <<= 1;
            if (counter > 6){
        
                data[j >> 3] |= 1;
            }
    
            j++;
        }

    }

    // SECURE_record_output_data(i);

    // check we read 40 bits and that the checksum matches
    if ((j >= 40) && (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) ) {

        valid_reading = 1;
    } else {

        valid_reading = 0;
    }
}

uint16_t get_temperature(){
    read_data();

    uint16_t t = data[2];
    t |= (data[3] << 8);
    return t;
}

uint16_t get_humidity(){
    read_data();

    uint16_t h = data[0];
    h |= (data[1] << 8);
    return h;
}


void application(){
    // Get sensor readings
    temp = get_temperature();
    // temp = (get_temperature() >> 1);
    // temp += (get_temperature() >> 1);

    // humidity = (get_humidity() >> 1);
    // humidity += (get_humidity() >> 1);

    // uint32_t output = (temp << 32) | humidity;
    // SECURE_record_output_data(output);
}
#endif // TEMP

#if APP_SEL == ULT

#define MAX_DURATION    1000
extern __IO uint32_t uwTick;

void delay(uint32_t us){
 
    // uint32_t start = uwTick;
    // while(uwTick - start < us);
    for(int i=0; i<us; i++);
}

uint32_t pulseIn(void){
 
    uint32_t duration = 0;

    for(int i=0; i < MAX_DURATION; i++){
        duration += (GPIOA->IDR & GPIO_PIN_8) >> 8;
    } 

    return duration;
 }

uint32_t getUltrasonicReading(void){
 
    // Set as output and Set signal low for 2us
    GPIOA->BSRR = (uint32_t)GPIO_PIN_8;
    
    delay(2);

    // Set signal high for 5 us
    GPIOA->BRR = (uint32_t)GPIO_PIN_8;

    delay(5);

    // Set signal low
    GPIOA->BSRR = (uint32_t)GPIO_PIN_8;

    // Set as input and read for duration
    uint32_t duration = pulseIn();

    return duration;
}

void application(){
 
    uint32_t ult_vec = 0;
    // for(int i=0; i<MAX_READINGS; i++){
    //     ult_vec += getUltrasonicReading()/MAX_READINGS;
    // }

    ult_vec = getUltrasonicReading();

    SECURE_record_output_data(ult_vec);
    // SECURE_record_output_data(read_val);

}
#endif

#if APP_SEL == ATTACK

// --------------------- Main ------------------//
#define MAX_READINGS        83
#define MAX_DURATION        1000
#define cmd_ult             'u'
#define cmd_temp_hum        't'
#define cmd_all             'a'

/** Benign: calls Ultrasonic only **/
char input[4] = {'u', 0, 16, ':'};
//
/** Attack: overwrites return address and stack pointer to cause infinite loop in process_command function **/ 
//
// char input[33] = {'b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b',0xb4,0x06,0x04,0x08, 0xf0, 0xff, 0x03, 0x20, 0xcd, 0x05, 0x04, 0x08, ':'};
//
uint32_t data[4] = {0,0,0,0};
uint8_t temp_data[5] = {0,0,0,0,0};
uint8_t valid_reading = 0;

int ult_runs;
int temp_runs;
int hum_runs;
int seq_runs;
int runs;

extern __IO uint32_t uwTick;

void delay(unsigned int us){
 
    // uint32_t start = uwTick;
    // while(uwTick - start < us);
    for(int i=0; i<us; i++);
}

void read_data(void){
 
    uint8_t counter = 0;
    uint16_t j = 0, i;

    // pull signal high & delay
    GPIOA->BRR = (uint32_t)GPIO_PIN_8;
    delay(250);

    /// pull signal low for 20us
    GPIOA->BSRR = (uint32_t)GPIO_PIN_8;
    delay(20);

    // pull signal high for 40us
    GPIOA->BRR = (uint32_t)GPIO_PIN_8;
    delay(40);

    //read timings
    for(i=0; i<MAX_READINGS; i++){

        counter += (GPIOA->IDR & GPIO_PIN_8) >> 8;

        // ignore first 3 transitions
        if ((i >= 4) && ( (i & 0x01) == 0x00)) {

            // shove each bit into the storage bytes
            temp_data[j >> 3] <<= 1;
            if (counter > 6){

                data[j >> 3] |= 1;
            }

            j++;
        }

    }
    // check we read 40 bits and that the checksum matches
    if ((j >= 40) && (temp_data[4] == ((temp_data[0] + temp_data[1] + temp_data[2] + temp_data[3]) & 0xFF)) ) {

        valid_reading = 1;
    } else {

        valid_reading = 0;
    }
}

uint16_t get_temperature(void){
 
    read_data();

    uint16_t t = temp_data[2];
    t |= (temp_data[3] << 8);
    return t;
}

uint16_t get_humidity(void){
 
    read_data();

    uint16_t h = temp_data[0];
    h |= (temp_data[1] << 8);
    return h;
}

uint32_t pulseIn(void){
 
    uint32_t duration = 0;

    for(int i=0; i < MAX_DURATION; i++){
        duration += (GPIOA->IDR & GPIO_PIN_9) >> 8;
    } 

    return duration;
 }

long getUltrasonicReading(){
 
    // Set as output and Set signal low for 2us
    GPIOA->BSRR = (uint32_t)GPIO_PIN_9;

    delay(2);

    // Set signal high for 5 us
    GPIOA->BRR = (uint32_t)GPIO_PIN_9;

    delay(5);

    // Set signal low
    GPIOA->BSRR = (uint32_t)GPIO_PIN_9;

    // Set as input and read for duration
    unsigned long duration = pulseIn();

    return duration;
}

long run_ultrasonic(int runs){
 
    int i;
    long total = 0;
    for(i=0; i<runs; i++){
        total += getUltrasonicReading()/runs;
    }
    return total;
}

long run_temperature(int runs){
 
    int i;
    long total = 0;
    for(i=0; i<runs; i++){
        total += get_temperature()/runs;
    }
    return total;
}

long run_humidity(int runs){
 
    int i;
    long total = 0;
    for(i=0; i<runs; i++){
        total += get_humidity()/runs;
    }
    return total;
}

uint32_t send_data; ///2003ffc0
void read_command(char * msg, char * input){
    // int i=0; 
    while(*input != ':'){
        // if (i<31)
          *msg = *input;
        // else
          // break;
        // i++;
        // SECURE_record_output_data(*input);
        // SECURE_record_output_data(*msg);/
        msg++;
        input++;
    }
}

char cmd;
void process_command(){
    char msg[16] = {'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a'};
    read_command(msg, input);

    switch(msg[0]){
        case cmd_ult:
            ult_runs = (msg[1] << 8) | msg[2];
            break;
        case cmd_temp_hum:
            temp_runs = (msg[1] << 8) | msg[2];
            SECURE_record_output_data(temp_runs);
            hum_runs = (msg[4] << 8) | msg[5];
            SECURE_record_output_data(hum_runs);
            break;
        case cmd_all:
            seq_runs  = (msg[1]  << 8) | msg[2];
            ult_runs  = (msg[4]  << 8) | msg[5];
            temp_runs = (msg[7]  << 8) | msg[8];
            hum_runs  = (msg[10] << 8) | msg[11];
        default:
            break;
    }
    cmd = msg[0];
    // SECURE_record_output_data(cmd);
    // return msg[0];
}

int (*sensor_action)(char);

int record_output_data(char data){
 
    return data^0xff;
}

void application(void)
{

    process_command();

    switch(cmd){
        case cmd_ult:
            runs = record_output_data(cmd_ult);
            // data[0] = run_ultrasonic(ult_runs);
            sensor_action = &run_ultrasonic;
            runs = ult_runs;
            break;
        case cmd_temp_hum:
            runs = record_output_data(cmd_temp_hum);
            // data[1] = run_temperature(temp_runs);
            // data[2] = run_humidity(hum_runs);
            sensor_action = &run_temperature;
            runs = temp_runs;
            break;
        case cmd_all:
            runs = record_output_data(cmd_all);
            // data[0] = run_ultrasonic(ult_runs);
            // data[1] = run_temperature(temp_runs);
            // data[2] = run_humidity(hum_runs);
            sensor_action = &run_humidity;
            runs = hum_runs;
            break;
        default:
            break;
    }

    sensor_action(runs);
}
#endif

#if APP_SEL == DIJKSTRA
// from https://github.com/mageec/beebs/blob/master/src/dijkstra/dijkstra_small.c
#include <stdio.h>
#include <stdlib.h>

/* This scale factor will be changed to equalise the runtime of the
   benchmarks. */
#define SCALE_FACTOR    (REPEAT_FACTOR >> 9)

#define NUM_NODES                          10
#define NONE                               9999

struct _NODE
{
   int iDist;
   int iPrev;
};
typedef struct _NODE NODE;

struct _QITEM
{
   int iNode;
   int iDist;
   int iPrev;
   struct _QITEM *qNext;
};
typedef struct _QITEM QITEM;

QITEM *qHead = NULL;

int AdjMatrix[NUM_NODES][NUM_NODES] = {{32,  32,  54,  12,  52,  56,  8,   30,  44,  94},
   {76,  54,  65,  14,  89,  69,  4,   16,  24,  47},
   {38,  31,  75,  40,  61,  21,  84,  51,  86,  41},
   {80,  16,  53,  14,  94,  29,  77,  99,  16,  29},
   {59,  7,   14,  78,  79,  45,  54,  83,  8,   94},
   {94,  41,  3,   61,  27,  19,  33,  35,  78,  38},
   {3,   55,  41,  76,  49,  68,  83,  23,  67,  15},
   {68,  28,  47,  12,  82,  6,   26,  96,  98,  75},
   {7,   1,   46,  39,  12,  68,  41,  28,  31,  0},
   {82,  97,  72,  61,  39,  48,  11,  99,  38,  49}};

int g_qCount = 0;
NODE rgnNodes[NUM_NODES];
int ch;
int iPrev, iNode;
int i, iCost, iDist;


/* BEEBS heap is just an array */

#include <stddef.h>

#define QHEAP_SIZE (8192 / sizeof (QITEM))
static QITEM qheap[QHEAP_SIZE];
static QITEM *qfree_list;

/* Initialize the BEEBS heap.

   Because we know the size of objects is always the same, we have a
   specially hacked version of malloc */

static void
qinit_heap (void)
{
  qfree_list = NULL;
  int  i;

  for (i = 0; i < QHEAP_SIZE; i++)
    {
      qheap[i].qNext = qfree_list;
      qfree_list = &(qheap[i]);
    }
}

/* BEEBS version of malloc.

   This is primarily to reduce library and OS dependencies. This is unusual
   for an embedded program in its dynamic use of malloc and free.  However we
   know that malloc is always used with the same sized object, so we cheat. */

static void *
qmalloc_beebs (size_t size)
{
  if ((size != sizeof(QITEM)) || (0 == size))
    return NULL;

  QITEM *new_ptr = qfree_list;

  if (new_ptr != NULL)
    {
      qfree_list = new_ptr->qNext;
    }

  return (void *) new_ptr;
}

/* BEEBS version of free.

   Even our simplified version has to work correctly, or we'll run out of
   memory. Assume we are given a valid pointer to a QITEM - there is no way
   to check. */

static void
qfree_beebs (void *ptr)
{
  QITEM *qptr = (QITEM *) ptr;

  qptr->qNext = qfree_list;
  qfree_list = qptr;
}


void enqueue (int iNode, int iDist, int iPrev)
{
   QITEM *qNew = (QITEM *) qmalloc_beebs(sizeof(QITEM));
   QITEM *qLast = qHead;

   qNew->iNode = iNode;
   qNew->iDist = iDist;
   qNew->iPrev = iPrev;
   qNew->qNext = NULL;

   if (!qLast)
   {
      qHead = qNew;
   }
   else
   {
      while (qLast->qNext) qLast = qLast->qNext;
      qLast->qNext = qNew;
   }
   g_qCount++;
}


QITEM* dequeue (int *piNode, int *piDist, int *piPrev)
{
   QITEM *qKill = qHead;

   if (qHead != 0)
   {
      *piNode = qHead->iNode;
      *piDist = qHead->iDist;
      *piPrev = qHead->iPrev;
      qHead = qHead->qNext;
      g_qCount--;
      return qKill;
   }
   return 0;
}


int qcount (void)
{
   return(g_qCount);
}

int dijkstra(int chStart, int chEnd)
{
   for (ch = 0; ch < NUM_NODES; ch++)
   {
      rgnNodes[ch].iDist = NONE;
      rgnNodes[ch].iPrev = NONE;
   }

   if (chStart == chEnd)
   {
      return 0;
   }
   else
   {
      rgnNodes[chStart].iDist = 0;
      rgnNodes[chStart].iPrev = NONE;

      enqueue (chStart, 0, NONE);

      while (qcount() > 0)
      {
         QITEM *tmp = dequeue (&iNode, &iDist, &iPrev);
         if(tmp != 0)
            qfree_beebs(tmp);
         for (i = 0; i < NUM_NODES; i++)
         {
            iCost = AdjMatrix[iNode][i];
            if (iCost != NONE)
            {
               if ((NONE == rgnNodes[i].iDist) ||
                     (rgnNodes[i].iDist > (iCost + iDist)))
               {
                  rgnNodes[i].iDist = iDist + iCost;
                  rgnNodes[i].iPrev = iNode;
                  enqueue (i, iDist + iCost, iNode);
               }
            }
         }
      }
   }

   return rgnNodes[chEnd].iDist;
}

int output[NUM_NODES * NUM_NODES];
int output_count = 0;
// int output = 0;
// int i = 5;
// int j = 5;
void application() {
   int i, j;
   qinit_heap();
   output_count = 0;
   // output =  dijkstra(i,j);
   /* finds 10 shortest paths between nodes */
   for(j = 0; j < 5; j++) {
      for (i=0; i < 5; i++) {
         output[output_count] = dijkstra(i,j);
         output_count++;
      }
   }

   // // TRACES -- Send results
   // for(i=0; i<NUM_NODES*NUM_NODES; i++){
   //    SECURE_record_output_data(output[output_count]);
   // }
}

#endif

#if APP_SEL == HAMMING
// Source: https://www.geeksforgeeks.org/hamming-code-implementation-in-c-cpp/
// Modified to use faster ops
 
#define N    128

// Store input bits
int input[N] = {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0,
                0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1,
                0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0,
                0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0, 0};
 
// Store hamming code
int code[N];
 
int ham_calc(int, int);
void solve(int input[], int);
 
// Function to calculate bit for
// ith position
int ham_calc(int position, int c_l)
{
 
    int count = 0, i, j;
    i = position - 1;
 
    // Traverse to store Hamming Code
    while (i < c_l) {
 
        for (j = i; j < i + position; j++) {
 
            // If current bit is 1
            if (code[j] == 1)
                count++;
        }
 
        // Update i
        i = i + 2 * position;
    }
 
    return (count & 0x1);
}
 
// Function to calculate hamming code
void solve(int input[], int n)
{
 
    int i, p_n = 0, c_l, j, k;
    i = 0;
 
    // Find msg bits having set bit
    // at x'th position of number
    while (n > (1 << i) - (i + 1)) {
        p_n++;
        i++;
    }
 
    c_l = p_n + n;
 
    j = k = 0;
 
    // Traverse the msgBits
    for (i = 0; i < c_l; i++) {
 
        // Update the code
        if (i == ((1 << k) - 1)) {
            code[i] = 0;
            k++;
        }
        // Update the code[i] to the
        // input character at index j
        else {
            code[i] = input[j];
            j++;
        }
    }
 
    // Traverse and update the
    // hamming code
    for (i = 0; i < p_n; i++) {
 
        // Find current position
        int position = (1 << i);
 
        // Find value at current position
        int value = ham_calc(position, c_l);
 
        // Update the code
        code[position - 1] = value;
    }
}
 
// Driver Code
void application()
{
 
 
    // Function Call
    solve(input, N);
}

#endif