#ifndef _PLCVAR_H
#define _PLCVAR_H

#ifdef _MAIN_C
#define _VAR_PLCVM 
#else
#define _VAR_PLCVM extern 
#endif

#include "fx1ntype.h"


/***************************************************************************************/ 

#define   _X_num      48    // 48个输入端口，    编号：X0-X57                //  控制在 256 以内  MJ 2010.04.06
#define   _Y_num      48    // 48个输出端口，    编号：Y0-Y57                //  控制在 256 以内  MJ 2010.04.06
#define   _M_num      256   // 256个中间M继电器，编号：M0-M255                //  控制在 256 以内  MJ 2010.04.06
#define   _S_num      256   // 256个中间S继电器，编号：S0-S255                //  控制在 256 以内  MJ 2010.04.06
#define   _T_num      32    // 32个定时器，      编号：T0-T31               //  控制在 256 以内  MJ 2010.04.06
#define   _C_num      128   // 128个计数器，      编号：C0-C127               //  控制在 256 以内  MJ 2010.04.06
#define   _M8xxx_num  24    // 24个中间M8xxx继电器， 编号：M8000-M8023        //  控制在 256 以内  MJ 2010.04.06
#define   _D_num      128    // 128个位元件 D，    编号：D0-D127              //  控制在 256 以内  MJ 2010.04.06

#define   _X_BYTE     (_X_num + 7) / 8    // 48个输入端口，所占内存字节数
#define   _Y_BYTE     (_Y_num + 7) / 8    // 48个输出端口，所占内存字节数
#define   _M_BYTE     (_M_num + 7) / 8    // 256个中间M继电器，所占内存字节数  
#define   _S_BYTE     (_S_num + 7) / 8    // 256个中间S继电器，所占内存字节数
#define   _T_BYTE     (_T_num + 7) / 8    // 32个定时器，所占内存字节数 
#define   _C_BYTE     (_C_num + 7) / 8    // 128个计数器，所占内存字节数
#define   _M8xxx_BYTE (_M8xxx_num+7)/8    // 24个M8xxx继电器，所占内存字节数  

//-------------------------------------------------------------------------------------//
//  位元件 M8xxx 存储位                                                                //
//-------------------------------------------------------------------------------------//
#define    _M8000_    rM8xxx[0].BIT.BIT0
#define    _M8001_    rM8xxx[0].BIT.BIT1
#define    _M8002_    rM8xxx[0].BIT.BIT2
#define    _M8003_    rM8xxx[0].BIT.BIT3
#define    _M8004_    rM8xxx[0].BIT.BIT4
//.......
#define    _M8011_    rM8xxx[1].BIT.BIT3
#define    _M8012_    rM8xxx[1].BIT.BIT4
#define    _M8013_    rM8xxx[1].BIT.BIT5
#define    _M8014_    rM8xxx[1].BIT.BIT6
//.......
#define    _M8020_    rM8xxx[2].BIT.BIT4
#define    _M8021_    rM8xxx[2].BIT.BIT5
#define    _M8022_    rM8xxx[2].BIT.BIT6

//-----------------------------------------

#define    _M8000_old    rM8xxx1[0].BIT.BIT0
#define    _M8001_old    rM8xxx1[0].BIT.BIT1
#define    _M8002_old    rM8xxx1[0].BIT.BIT2
#define    _M8003_old    rM8xxx1[0].BIT.BIT3
#define    _M8004_old    rM8xxx1[0].BIT.BIT4
//.......
#define    _M8011_old    rM8xxx1[1].BIT.BIT3
#define    _M8012_old    rM8xxx1[1].BIT.BIT4
#define    _M8013_old    rM8xxx1[1].BIT.BIT5
#define    _M8014_old    rM8xxx1[1].BIT.BIT6
//.......
#define    _M8020_old    rM8xxx1[2].BIT.BIT4
#define    _M8021_old    rM8xxx1[2].BIT.BIT5
#define    _M8022_old    rM8xxx1[2].BIT.BIT6

//-------------------------------------------------------------------------------------//
//  位元件 S 存储位                                                                    //
//-------------------------------------------------------------------------------------//
#define    _S243_     rS[30].BIT.BIT3   
#define    _S244_     rS[30].BIT.BIT4   
#define    _S245_     rS[30].BIT.BIT5   
#define    _S246_     rS[30].BIT.BIT6   
#define    _S247_     rS[30].BIT.BIT7    
#define    _S248_     rS[31].BIT.BIT0   
#define    _S249_     rS[31].BIT.BIT1
#define    _S250_     rS[31].BIT.BIT2   
#define    _S251_     rS[31].BIT.BIT3   
#define    _S252_     rS[31].BIT.BIT4   
#define    _S253_     rS[31].BIT.BIT5   
#define    _S254_     rS[31].BIT.BIT6   
#define    _S255_     rS[31].BIT.BIT7   

//-------------------------------------------------------------------------------------//

//-------------------------------------------------------------------------------------//
//  位元件 D 内存分配                                                                  //
//-------------------------------------------------------------------------------------//
 
_VAR_PLCVM TYPE_BIT_BYTE    rX[_X_BYTE] , rY[_Y_BYTE];       //  位元件 X，Y 存储位
_VAR_PLCVM TYPE_BIT_BYTE    rX1[_X_BYTE], rY1[_Y_BYTE];      //  位元件 X，Y 存储位上一步备份
_VAR_PLCVM TYPE_BIT_BYTE    rM8xxx[3];                       //  位元件 M8xxx 存储位 
_VAR_PLCVM TYPE_BIT_BYTE    rM8xxx1[3];                      //  位元件 M8xxx 存储位上一步备份 
_VAR_PLCVM TYPE_BIT_BYTE    rTF[_T_BYTE];                    //  T 得电失电标志位 
_VAR_PLCVM TYPE_BIT_BYTE    rCF[_C_BYTE];                    //  C 得电失电标志位 
_VAR_PLCVM TYPE_BIT_BYTE    rT[_T_BYTE] , rC[_C_BYTE];       //  位元件 T，C 输出位    
_VAR_PLCVM TYPE_BIT_BYTE    rT1[_T_BYTE], rC1[_C_BYTE];      //  位元件 T，C 输出位上一步备份
_VAR_PLCVM TYPE_BIT_BYTE    rM[_M_BYTE];                     //  位元件 M 存储位
_VAR_PLCVM TYPE_BIT_BYTE    rS[_S_BYTE];                     //  位元件 S 存储位
_VAR_PLCVM TYPE_BIT_BYTE    rM1[_M_BYTE];                    //  位元件 M 存储位上一步备份
_VAR_PLCVM TYPE_BIT_BYTE    rS1[_S_BYTE];                    //  位元件 S 存储位上一步备份
_VAR_PLCVM unsigned   int     _T[_T_num][2];                   //  位元件 T 内存分配    
_VAR_PLCVM unsigned   int     _C[_C_num];                      //  位元件 C 内存分配    
_VAR_PLCVM unsigned   int     _D[_D_num];                      //  位元件 D 内存分配 

_VAR_PLCVM u8t    rM8xxxS[3];                                 //正式特殊继电器

_VAR_PLCVM u8t plcnullvar;


//-------------------------------------------------------------------------------------//
//  基本指令，扩展指令 子函数                                                          //
//-------------------------------------------------------------------------------------//
_VAR_PLCVM u8t  addr0T,addr1T,addr2T,addr3T,addr4T;  // 数据位第一至第五位字地址/数据类型

_VAR_PLCVM TYPE_BYTES_WORD addr0x,addr1x,addr2x,addr3x,addr4x; // 数据位第一至第五位字地址/数据缓冲区

_VAR_PLCVM TYPE_BYTES_WORD  order0;                            // 命令位地址缓冲区

_VAR_PLCVM u32t ppp;
_VAR_PLCVM const u8t *CODE_p;

#ifdef _MAIN_C
const u8t BYTE_BIT_s[8]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
const u8t BYTE_BIT_w[8]={0xff,0xfe,0xfc,0xf8,0xf0,0xe0,0xc0,0x80};
const u8t BYTE_BIT_e[8]={0x01,0x03,0x07,0x0f,0x1f,0x3f,0x7f,0xff};
#else
extern const u8t BYTE_BIT_s[8];
extern const u8t BYTE_BIT_w[8];
extern const u8t BYTE_BIT_e[8];
#endif


#define   addr0     addr0x.WORD          // 数据位第一字地址/数据缓冲区
#define   addr0L    addr0x.BYTES.BYTEL   // 数据位第一字地址/数据低位
#define   addr0H    addr0x.BYTES.BYTEH   // 数据位第一字地址/数据高位
#define   addr1     addr1x.WORD          // 数据位第二字地址缓冲区
#define   addr1L    addr1x.BYTES.BYTEL   // 数据位第二字地址低位
#define   addr1H    addr1x.BYTES.BYTEH   // 数据位第二字地址高位
#define   addr2     addr2x.WORD          // 数据位第三字地址缓冲区
#define   addr2L    addr2x.BYTES.BYTEL   // 数据位第三字地址低位
#define   addr2H    addr2x.BYTES.BYTEH   // 数据位第三字地址高位
#define   addr3     addr3x.WORD          // 数据位第四字地址缓冲区
#define   addr3L    addr3x.BYTES.BYTEL   // 数据位第四字地址低位
#define   addr3H    addr3x.BYTES.BYTEH   // 数据位第四字地址高位
#define   addr4     addr4x.WORD          // 数据位第五字地址缓冲区
#define   addr4L    addr4x.BYTES.BYTEL   // 数据位第五字地址低位
#define   addr4H    addr4x.BYTES.BYTEH   // 数据位第五字地址高位

#define   order     order0.WORD           // 命令位地址缓冲区
#define   orderL    order0.BYTES.BYTEL    // 命令位地址缓冲区低位
#define   orderH    order0.BYTES.BYTEH    // 命令位地址缓冲区高位


_VAR_PLCVM u8t P0;
_VAR_PLCVM u8t P1;
_VAR_PLCVM u8t P2;
_VAR_PLCVM u8t P3;

//_VAR_PLCVM u8t CY;

/*
// (bits in PSW)
sbit CY       = PSW^7;
sbit AC       = PSW^6;
sbit F0       = PSW^5;
sbit RS1      = PSW^4;
sbit RS0      = PSW^3;
sbit OV       = PSW^2;
sbit P        = PSW^0;
*/
// (bits in PSW)
//_VAR_PLCVM u8t CY ;
_VAR_PLCVM u8t AC ;
_VAR_PLCVM u8t F0 ;
_VAR_PLCVM u8t RS1;
_VAR_PLCVM u8t RS0;
_VAR_PLCVM u8t OV ;
_VAR_PLCVM u8t P  ;

//by ykb
_VAR_PLCVM u8t PLCRunFlag;                                          // PLC 停止[0]\运行[1] 标志位  MJ.2009.12.01 


#endif
