#include "fx1ntype.h"
#include "fx1nuart.h"
#include "fx1nhelper.h"
#include "fx1niap.h"
#include "fx1nvar.h"

void plc_excute_cmd(void);
void fx1n_readX(u8t* buf, u32t length);
void fx1n_writeY(u8t* buf, u32t length);

static u8t LastPLCStateFlag;

/***************************************************************************************/ 

u8t Timer_5ms;         // 5ms时基计数器，5ms Timer0中断计数
u8t Timer_10ms;        // 10ms时基计数器，5ms Timer0中断计数
u8t Timer_100ms;       // 100ms时基计数器，5ms Timer0中断计数
u8t Pulse_val_Sec;     // 1s时基计数器
u8t Pulse_val_Min;     // 1min时基计数器
bit Pulse_10ms;
bit Pulse_100ms;
bit Pulse_Sec;
bit Pulse_Min;
extern void iap_init(void);
extern unsigned char       CODE_ERROR;
extern unsigned int    PS_BIT;                         //  P 指令16级暂存
extern unsigned int    PS1_BIT;                        //  P 指令16级暂存备份
extern unsigned int    Pi;                             //  P 指令16级暂存位置指针
/***************************************************************************************/ 

extern u8t CodeStart[];
extern u8t SDataReg[];

static u32t _cnt10ms = 0;
void fn1n_on10ms(void) {
    _cnt10ms++;
    //printf("%X\n", _cnt10ms);
}
static void fn1n_sync_10ms(void) {
    static u32t tick_10ms = 0;
    u32t i;

    tick_10ms++;
    
    // M8011: 10ms脉冲 (受限于10ms基准，每次翻转)
    u8t_bit_reverse((u8t*)&rM8xxx[1], 3); 
    
    // M8012: 100ms脉冲 (标准为50ms翻转一次 = 5个tick)
    if (tick_10ms % 5 == 0) u8t_bit_reverse((u8t*)&rM8xxx[1], 4); 
    
    // 定时器累加逻辑 (每100ms = 10个tick 触发一次)
    if (tick_10ms % 10 == 0) {
        for (i=0; i<_T_num; i++ ) { 
            if ((rTF[i/8].BYTE & (1 << (i%8))) != 0) { 
                if (_T[i][0] < _T[i][1]) _T[i][0]++; 
            }
        }
    }
    
    // M8013: 1秒脉冲 (标准为0.5秒翻转一次 = 50个tick)
    if (tick_10ms % 50 == 0) u8t_bit_reverse((u8t*)&rM8xxx[1], 5); 
    
    // M8014: 1分钟脉冲 (标准为30秒翻转一次 = 3000个tick)
    if (tick_10ms % 3000 == 0) u8t_bit_reverse((u8t*)&rM8xxx[1], 6); 
    
    // 防溢出，60000是个公倍数（相当于10分钟归零一次）
    if (tick_10ms >= 60000) tick_10ms = 0;
}
static void fx1n_sync_timer(void) {
    static u32t ii;
    u32t cnt;

    cnt = _cnt10ms - ii;
    ii += cnt;
    if (cnt > 10) {
        cnt = 10;
    }
    while (cnt-- > 0) {
        fn1n_sync_10ms();
    }
}

void reset_M8xxx(void)             // PLCRunFlag 后输出一个扫描周期初始化
{   rM8xxx[0].BYTE = 0x09;
}

void reset_RAM(void)
{   u8t i;
    for (i=0; i<16;  i++)  { _D[i] = 0; }
    for (i=0; i<22;  i++)  { _T[i][0] = 0; }
    for (i=0; i<_T_num;  i++)  { _T[i][1] = 0x7fff; }
    for (i=0; i<16;  i++)  { _C[i]    = 0; }
    for (i=0; i<_T_BYTE; i++)  { rT[i].BYTE = 0; rT1[i].BYTE = 0; rTF[i].BYTE = 0; }
    for (i=0; i<_C_BYTE; i++)  { rC[i].BYTE = 0; rC1[i].BYTE = 0; rCF[i].BYTE = 0; }
    for (i=0; i<_Y_BYTE; i++)  { rY[i].BYTE = 0; rY1[i].BYTE = 0; }
    for (i=0; i<_M_BYTE; i++)  { rM[i].BYTE = 0; rM1[i].BYTE = 0; }
    for (i=0; i<_S_BYTE; i++)  { rS[i].BYTE = 0; rS1[i].BYTE = 0; }
    for (i=0; i<_M8xxx_BYTE; i++) { rM8xxx[i].BYTE = 0; rM8xxx1[i].BYTE = 0; }
    rM8xxx[0].BYTE  = 0x05;
    rM8xxx1[0].BYTE = 0x05;
    Timer_5ms     = 0;           // 5ms时基计数器，5ms Timer0中断计数
    Timer_10ms    = 0;           // 10ms时基计数器，5ms Timer0中断计数
    Timer_100ms   = 0;           // 100ms时基计数器，5ms Timer0中断计数
    Pulse_val_Sec = 0;           // 1s时基计数器
    Pulse_val_Min = 0;           // 1min时基计数器
    CODE_ERROR    = 0;
    for (i=0; i<_X_BYTE; i++) { rX1[i].BYTE = rX[i].BYTE; }
}

void fx1n_reg_bk(void) {
    u8t i;

    str_copy((u8t*)rX, (u8t*)rX1, _X_BYTE);
    str_copy((u8t*)rY, (u8t*)rY1, _Y_BYTE);
    str_copy((u8t*)rM, (u8t*)rM1, _M_BYTE);
    str_copy((u8t*)rS, (u8t*)rS1, _S_BYTE);
    str_copy((u8t*)rT, (u8t*)rT1, _T_BYTE);
    str_copy((u8t*)rC, (u8t*)rC1, _C_BYTE);
    str_copy((u8t*)rM8xxx, (u8t*)rM8xxx1, _M8xxx_BYTE);
    //
    str_clr((u8t*)&rT[0], _T_BYTE);
    for (i = 0; i < _T_num; i++) {
        if (_T[i][0] >= _T[i][1]) {
            str_set_bit((u8t*)&rT[0], i);
        }
    }
    PS1_BIT = PS_BIT;
}

//-------------------------------------------------------------------------------------//
//   PLC 入口 函数  main_PLC();                                                        //
//-------------------------------------------------------------------------------------//
void FX1NPLCSoftwareVerCheck(void) {
    const u8t ArrPLCSoftwareVer[8]  ="V1.01.07";//  PLC Software Version

    if (str_cmp((u8t*)PLCSoftwareVerAddr, ArrPLCSoftwareVer, 8) != 0) {
        //  更新软件版本号
        iap_write_array(PLCSoftwareVerAddr, ArrPLCSoftwareVer, 8);
        //for (i = 0; i < 8; i++) {                                   
            //IAPFlashProgremMode(PLCSoftwareVerAddr+i,ArrPLCSoftwareVer[i]);
        //}
        //  写入 PLC 初始化代码    查询地址0x8000~0x805B.
        iap_write_array(CodeStartAddr, CodeStart, 92);
        //for(i=0;i<92;i++) {                                  
            //IAPFlashProgremMode(CodeStartAddr+i,CodeStart[i]);
        //}
        //写入 PLC 初始化代码    查询地址0x0E00~0x0EFF
        //iap_write_array(SDataRegStartAddr+SDataRegOffset, SDataReg, 0x100);
        iap_write_array(SDataRegIAPStartAddr, SDataReg, 0x100);
        //for(i=0;i<0x100;i++) {                               //  
            //IAPFlashProgremMode(SDataRegStartAddr+SDataRegOffset+i,SDataReg[i]);
        //}
        /*for(i=0;i<0x28;i++)                                   //  写入 PLC 初始化代码    查询地址0x1790~0x
        { 
            IAPFlashProgremMode(PLCXYMonitorRealAddr+i,PLCXYMonitorData[i]);
        }*/
    }
}

void PLCStateCheck(void) {
    u8t tPLCState;
    

    tPLCState=IAPFlashReadMode(PLCStateRealAddr);
    if (tPLCState==PLCStateRun) {
        PLCRunFlag = 1;
    }
    else if (tPLCState==PLCStateStop) {
        PLCRunFlag=0;
    }
    else {
        PLCRunFlag = 1;
        //{while(1);                      //  错误处理*/
    }
}

//-------------------------------------------------------------------------------------//
//  主程序入口 主函数                                                                  //
//-------------------------------------------------------------------------------------//
void plc_idle(void);

void fx1n_init(void) { 
    //u8t reset_i=1;
    //unsigned int  i;
    //const u8t ArrPass[]    ="PASS!\r\n\0";
    //const u8t DefaultSet[] ="System in factory default.\r\n\0";
    //u8t PLCButtonState;

    iap_init();
    reset_RAM();

    FX1NPLCSoftwareVerCheck();               //  上电,核实 PLC 软件版本号

    //  MJ 2010.01.11
    //P1M0|=0x03;
    //P1M1&=0xfc;
    //  核查 复位标志 . 判断是 冷启动 还是 热启动 .
    //      冷启动 指从加电开始直到系统的处理命令进行完毕为止;  
    //      热启动 指在冷启动后，系统不断电的情况下在系统中通过软件实现的重新启动.
    //if(((PCON&0x10)==0x10))                     //  冷启动
    {   
        //PCON&=~0x10;
        if((P1 & 0x03) == 0)                    //  为合理使用 [运行/停止]按钮 .去掉 X0 X1两点输入.
        {                                       //  请不要再 PLC 程序中编写带有 X0 X1 的程序. 谢谢.
            IAPFlashProgremMode(PLCStateRealAddr,PLCStateStop);
            PLCRunFlag=0;
        }
        else
        {
            IAPFlashProgremMode(PLCStateRealAddr,PLCStateRun);
            PLCRunFlag=1;
        }
    }
    //else 
        PLCStateCheck();                       //  热启动
    //PLCButtonState=P1 & 0x03;
        PLCRunFlag = 1;
    LastPLCStateFlag = PLCRunFlag;

    CODE_scan();
    PLCRunFlag = 1;
    rM8xxx[0].BYTE = (rM8xxx[0].BYTE & 0xfc) | 0x01;
    IAPFlashProgremMode(PLCStateRealAddr, PLCStateRun); 
}

void fx1n_excute(void) {
    u8t reset_i = 1;
    //while(1)
    {
        //  =============================================================================
        //  关于[运行/停止]按钮(以下简称[按钮])的相关功能描述:   MJ 2010.01.11
        //  1.硬件上电(即上电复位)时,硬件[按钮]是何种状态,PLC 就是何种状态.即[按钮]是[运行]状态的话,PLC就是运行状态.反之亦然.
        //  2.FXGPWIN  软件操作 优先于 硬件[按钮] 响应.即就算是在硬件[按钮]的[运行]状态下.也可以通过软件使得 PLC 处于[停止]状态.反之亦然.
        //  3.如果是在 PLC 运行中,更改 运行状态 ,都需要做复位处理.只不过是效果不同:
        //    如果是硬件[按钮]更改状态,那么复位(软复位)之后,同( 1.).硬件[按钮]是何种状态,PLC 就是何种状态.
        //    如果是 FXGPWIN 软件操作更改状态 ,那么复位(软复位)之后,PLC 的运行状态依赖于 PLC 内部的状态位.与硬件[按钮]无关,有可能相同,也有可能相反.
        /*if((P1 & 0x03) != PLCButtonState)
        {
            delay_ms(20);
            if((P1 & 0x03) != PLCButtonState)
            {
                if((P1 & 0x03) == 0x00)
                {
                    IAPFlashProgremMode(PLCStateRealAddr,PLCStateStop);
                    PLCButtonState=0x00;
                }
                else if((P1 & 0x03) == 0x03)
                {
                    IAPFlashProgremMode(PLCStateRealAddr,PLCStateRun);
                    PLCButtonState=0x03;
                }
                //else while(1);    //  注意:请不要再 PLC 程序中编写带有 X0 X1 的程序. 
            }
        }*/
       // PLCStateCheck();
        //printf("\r\nsystempoll ");
        if (LastPLCStateFlag != PLCRunFlag) 
            PLCReset();
        LastPLCStateFlag = PLCRunFlag;
        //  [====]以上内容由 MJ.2009.12.23 凌晨加入.
        //  =============================================================================
        //FX1NComPortProcessing();                //  PLC 下载通信处理
        if (PLCRunFlag) {
            //printf("\r\n cmd =");
            //pthexstr((const u8t *)CODE_START, 20);
            fx1n_sync_timer();
            fx1n_readX((u8t*)rX, _X_num);
            plc_excute_cmd();
            fx1n_writeY((u8t*)rY, _Y_num);
            //printf("\r\n cmd =");
            //pthexstr((const u8t *)CODE_START, 20);
            /*if (CODE_ERROR == 0)
                main_PLC();
            else 
            {
                if (Pulse_Sec != 0)P1 &= 0xfb;  //  PLC 程序出错报警，X2灯闪烁。MJ 2009.01.11
                else P1 |= 0x04;
            }*/
            fx1n_reg_bk();
            if (reset_i != 0) 
            {
                reset_i--;
                //reset_M8xxx(); 
            }
        }
        plc_idle();
    }
}
