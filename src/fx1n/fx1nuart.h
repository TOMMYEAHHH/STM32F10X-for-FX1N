#ifndef __FX1N__H__
#define __FX1N__H__

//======================================================================================================================
//
//                                          Please Read Me First 
//
//  45 30 30 Read Data Register
//  45 31 30 Write Data Register
//      General Data Register       Start Address:0x4000    End Address:0x7e80  Data Total:0x3e80(16000)bytes. D0000 ~ D7999
//      Special Data Register       Start Address:0x0e00    End Address:0x1000  Data Total:0x0200(512)  bytes. D8000 ~ D8255
//  45 30 31 Read Code
//  45 31 31 Write Code
//      General Code                Start Address:0x805c    End Address:0xbedc  Code Total:0x3e80(16000)bytes.
//      Special Code                Start Address:0x8000    End Address:0x805c  Code Total:0x005c(92)   bytes.
//  注:  1.从上述 Data Total & Code Total 所示,占用 IAP 空间大小为 16000+512+16000+92=32604 bytes.
//      2.需要 IAP 最后一页,存储 PLC 类型; IAP 倒数第二页,当作缓存空间,以上两页不许被其他数据占用.
//      3.0x4000单元以下的地址(即0x0000 ~ 0x4000),都用做了什么用,还有待于进一步研究,
//      因以上几点,故而 总占用的 IAP 空间应为 32604 + 2 * 512 = 33628 bytes.实际上可用 IAP 大小为 31K(共 62 页,每页 512 bytes).
//  故而,作如下处理:
//      1.从 IAP 地址 0x8000 ~ 0xbfff  即 IAP 第一页       至 第三十一页 存储PLC程序         -- 对应PLC 的 0x8000 ~ 0xbfff 地址 -- Special Code & General Code Area
//      2.从 IAP 地址 0xc000 ~ 0xc1ff  即 IAP 第三十二页 至 第三十三页 存储PLC数据寄存器  -- 对应PLC 的 0x0e00 ~ 0x1000 地址 -- Special Data Register Area
//      3.从 IAP 地址 0xc200 ~ 0xf7ff  即 IAP 第三十四页 至 第六十页      存储PLC数据寄存器  -- 对应PLC 的 0x4000 ~ 0x7600 地址 -- General Data Register Area
//      4.从 IAP 地址 0xf800 ~ 0xf9ff  即 IAP 第六十一页                 缓存空间.
//      5.从 IAP 地址 0xfa00 ~ 0xfbff  即 IAP 第六十二页                 存储 PLC 类型标识, 共 PLCTypeArrayLen 个字节,当页其他剩余字节保留.
//      6.其中  IAP 第三十四页 至 第六十页 存储PLC数据寄存器   ,共有 27 页 即 27 * 512 = 0x3600(13824) bytes. < Data Total:0x3e80(16000)bytes.
//        故而, 大于等于 0x4000 + 0x3600 = 0x7600 地址的 2176(0x7e80-0x7600) bytes 无法保存.当读取时,返回 0;
//======================================================================================================================

//ykb begin
/*
+----------------------------------------------- -0x00
+PLCSoftwareVerAddr    0x00-0x08
+----------------------------------------------- -0x08
+PLCSoftwareVerAddr    0x08-0x09
+-------------------------------------------------0x09
+
+PLCStateRealAddr          PLCSoftwareVerAddr + 1 0x08
*/

#ifdef _FX1N_C
#define _VAR_FX1N_C 
#else
#define _VAR_FX1N_C extern 
#endif

#define  FX1N_FLASH_SIZE      (1024 * 9)

_VAR_FX1N_C u16t flashbufs[FX1N_FLASH_SIZE / 2];

//ykb end
//软件版本地址
#define PLCSoftwareVerAddr      (u32t)flashbufs
//#define PLCSoftwareVerAddr      (u32t)(0x8010000 - FX1N_FLASH_SIZE)
//PLC状态地址
#define PLCStateAddr            0x01c0
#define PLCStateRealAddr        (PLCSoftwareVerAddr+8)
#define PLCStateRun             0x09
#define PLCStateStop            0x0a
//PLC监控
#define PLCMonitorPollAddr      0x1400                  //  监控时.轮询 PLC 寄存器元件 及 位元件。
#define PLCS0x1400RealAddr      (PLCStateRealAddr+1)
#define PLCMonitorAckAddr       0x1790                  //  监控时.回复 轮询的 器件内容。
//#define PLCXYMonitorRealAddr    (PLCS0x1400RealAddr+4)
//SData数据
#define SDataRegStartAddr       0x0e00
#define SDataRegEndAddr         0x0fff
//#define SDataRegIAPStartAddr    CodeEndAddr
#define SDataRegIAPStartAddr    (PLCS0x1400RealAddr + 8)
#define SDataRegIAPEndAddr      (SDataRegIAPStartAddr + (SDataRegEndAddr - SDataRegStartAddr))
//#define SDataRegOffset          (SDataRegIAPStartAddr - SDataRegStartAddr)
//#define SDataRegOffset          0xb200
//#define SDataRegPageNum         2

#define GDataRegStartAddr       0x4000
#define GDataRegEndAddr         0x75ff
#define GDataRegRealEndAddr     0x7e7f
#define GDataRegIAPStartAddr    SDataRegIAPEndAddr
#define GDataRegIAPEndAddr      (GDataRegIAPStartAddr + (GDataRegRealEndAddr - GDataRegStartAddr))
#define GDataRegOffset          (GDataRegIAPStartAddr - GDataRegStartAddr)
//#define GDataRegOffset          0x8200
//#define GDataRegPageNum         27

#define GDataReg_D_StartAddr    (GDataRegStartAddr)
#define GDataReg_D_EndAddr      (GDataReg_D_StartAddr+_D_num-1)
#define GDataReg_T_StartAddr    
#define GDataReg_T_EndAddr      
#define GDataReg_C_StartAddr    
#define GDataReg_C_EndAddr      


//#define CodeStartAddr           (PLCSoftwareVerAddr + 512)
//#define CodeEndAddr             (CodeStartAddr + 0x3fff)
//#define PLCSTEP                 8000

//#define   CODE_START            (CodeStartAddr + 0x5c)
//#define   CODE_END              (CODE_START + PLCSTEP*2)

#define CodeStartAddr           (PLCSoftwareVerAddr + 512)
#define CodeEndAddr             (PLCSoftwareVerAddr + FX1N_FLASH_SIZE)
#define   CODE_START            (CodeStartAddr + 0x5c)
#define   CODE_END              CodeEndAddr

//#define MCUFLASHSIZE            64*1024                             //  64K
//#define MCUIAPFLASHSIZE         31*1024                             //  31K
//#define MCUISPFLASHSIZE         1*1024                              //  1K

//
#define uENQ                0x05    //  请求  
#define uACK                0x06    //  PLC 接收正确响应  
#define uNACK               0x15    //  PLC 接收错误响应 
#define uSTX                0x02    //  报文开始
#define uETX                0x03    //  报文结束

#define uCMDFNC             0x45    //  命令: 功能
#define uCMDRead            0x30    //  命令: 读
#define uCMDWrite           0x31    //  命令: 写
#define uCMDunKnow34        0x34    //  命令: 未知
#define uCMDErasure         0x36    //  命令: 清除
#define uCMDEnforceOn       0x37    //  命令: 强制输出 ON
#define uCMDEnforceOff      0x38    //  命令: 强制输出 OFF
#define uCMDDownLoadEnd     0x42    //  命令: 下载结束

#define uCtrlData           0x30    //  控制: 数据寄存器
#define uCtrlCode           0x31    //  控制: 程序代码

//  
//#define ErasureALL          0xff
#define ErasureCODE         0x31
#define ErasureDataReg      0x34
#define ErasurebitReg       0x32

//  
#define PLC_S_Type          0x00    //  Sx(特殊寄存器、状态寄存器)
#define PLC_X_Type          0x04    //  Xx(输入)
#define PLC_Y_Type          0x05    //  Yx(输出)
#define PLC_T_Type          0x06    //  Tx(定时器)
#define PLC_M_Type          0x08    //  Mx(数据存储器、辅助继电器)
#define PLC_C_Type          0x0E    //  Cx(计数器)

//  FX1N 内存映象  --  详见 "FX1N内存映像.xls"
//  位值区 (位地址)
#define RAM_M_bitAddr       0x0000  //  Mx 辅助继电器            M0000 ~ M3071
#define RAM_Y_bitAddr       0x0C00  //  Yx 输出继电器            Y000  ~ Y267(0x0CB7)
#define RAM_SM_bitAddr      0x0E00  //  SMx 特殊辅助继电器     M8000 ~ M8255
#define RAM_Cp_bitAddr      0x0F00  //  Cx 计数器 Over Contact C000  ~ C255
#define RAM_Tp_bitAddr      0x1000  //  Tx 定时器 Over Contact T000  ~ T255(0x10ff)
#define RAM_X_bitAddr       0x1200  //  Xx 输入继电器            X000  ~ X277
#define RAM_S_bitAddr       0x1400  //  Sx 状态寄存器            S000  ~ S999
#define RAM_Cc_bitAddr      0x2700  //  Cx 计数器 Enable Coil  C000  ~ C255
#define RAM_Tc_bitAddr      0x2800  //  Tx 定时器 Enable Coil  T000  ~ T255
#define RAM_Cr_bitAddr      0x3700  //  Cx 计数器 Restet       C000  ~ C255
#define RAM_Tr_bitAddr      0x3800  //  Tx 定时器 Restet       T000  ~ T255
//  寄存器数据区(字节地址)
#define RAM_C_RegAddr       0x0A00  //  Cx 计数器(16bit)       C000  ~ C199
#define RAM_C2_RegAddr      0x0C00  //  Cx 计数器(32bit)       C200  ~ C234(0x0C8B)
#define RAM_SD_RegAddr      0xOE00  //  SDx 特殊数据寄存器     D8000 ~ D8255
#define RAM_T_RegAddr       0x1000  //  Tx 计数器(16bit)       T000  ~ T255
#define RAM_D_RegAddr       0x4000  //  Dx 数据寄存器            D0000 ~ D7999

#define RAM_M_Reg           0x01    //  Reg 类型定义
#define RAM_Y_Reg           0x02
#define RAM_SM_Reg          0x03
#define RAM_Cp_Reg          0x04
#define RAM_Tp_Reg          0x05
#define RAM_X_Reg           0x06
#define RAM_S_Reg           0x07
#define RAM_Cc_Reg          0x08
#define RAM_Tc_Reg          0x09
#define RAM_Cr_Reg          0x0A
#define RAM_Tr_Reg          0x0B
#define RAM_D_Reg           0x0C

#define GetuiDataH(uiData) (uiData>>8)              //  取 UINT 型 高位 
#define GetuiDataL(uiData) ((unsigned char )uiData) //  取 UINT 型 低位 


#define MonitorRegNoMAX     50          //  一屏 最多监控  50 个 ( 寄存器 + 位元件 )

#define STATE_ON            0x01
#define STATE_OFF           0x00


void ErasurePLC(u8t ErasureScope);
void CODE_scan (void);
void PLCReset(void);

#endif
