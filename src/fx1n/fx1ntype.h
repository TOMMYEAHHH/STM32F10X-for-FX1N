#ifndef _SYSTYPE_H
#define _SYSTYPE_H
#include "Port.h"
/*
#define  u8t   unsigned char
#define  u16t  unsigned short
#define  u32t  unsigned int
#define  s32t  int
*/
#define  bit  unsigned char

//#define   CODE_START   0x805c         // PLC执行代码缓冲区首地址
//#define CODE_START  0x10000
//#define CODE_END    0x20000
//#define     CODE_END     CODE_START + PLCSTEP*2               // PLC执行代码缓冲区结束地址
typedef struct { 
  unsigned char BIT0: 1; 
  unsigned char BIT1: 1; 
  unsigned char BIT2: 1; 
  unsigned char BIT3: 1; 
  unsigned char BIT4: 1; 
  unsigned char BIT5: 1; 
  unsigned char BIT6: 1; 
  unsigned char BIT7: 1; 
}TYPE_BIT;   //定义一个只能按位域寻址的新变量类型 
typedef union { 
  TYPE_BIT       BIT;      //可以按位域寻址 
  unsigned char  BYTE;     //可以按字节寻址 
} TYPE_BIT_BYTE;  //定义一个既能按位域寻址也可按字节寻址的新变量类型 

typedef union {     //重点注意C编辑器的多字节变量类型的高低字节前后排列次序
  struct { //unsigned char BYTEH;   //可以按字节寻址
           unsigned char BYTEL;   //可以按字节寻址 
           unsigned char BYTEH;   //可以按字节寻址
         } BYTES;
  unsigned int  WORD;       //可以按字寻址 
}TYPE_BYTES_WORD;  //定义一个既能按字节寻址也可按字寻址的新变量类型


#endif 
