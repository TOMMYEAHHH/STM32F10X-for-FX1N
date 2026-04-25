#define  _FX1N_C

#include "fx1ntype.h"
#include "fx1nuart.h"
#include "fx1nhelper.h"
#include "fx1niap.h"
#include "fx1nvar.h"
#include "stm32f10x_flash.h"
// ==========================================
// ??????:??????????????
// ==========================================
volatile u32t trap_addr[16];
volatile u32t trap_len[16];
volatile u8t  trap_idx = 0;
volatile u8t  trap_locked = 0;
// ==========================================
// ??????:16????????
// ==========================================
typedef struct {
    u32t Addr;
    u8t  Len;
    u8t  Cmd;
} DebugLogType;

volatile DebugLogType PLC_Log[16];
volatile u8t PLC_Log_Idx = 0;

void UartSendchar(u8t c);
void UartSendByte(u8t *buf, u32t length);

u8t GetXYMSBit(u8t *ucdat, u8t num);
u8t UartSendASCDate(u8t Readdat,u8t sumdata);
void FX1NUartFNCPro(void);
void UartResponseError(void);
void UartResponseCMDRead(void);
void UartResponseCMDWrite(void);
void UartResponseCMDErasure(void);
void UartResponseCMDEnforceOn(void);
void UartResponseCMDEnforceOff(void);
void UartResponseCMDunKnow34(void);
void UartResponseCMDDownLoadEnd(void);
void UartSendASCCRC(u8t sumdata);

//static u32t PLCRunFlag = 0;
volatile 
static u8t *UartReceiveBuffer;
static u32t UartReceiveCounter;

//u8t   RAMRegType[MonitorRegNoMAX]={0xff};
//u32t  RAMRegNum [MonitorRegNoMAX]={0xffff};
//u32t  MonitorRegNo;     //  ?????
//u32t  MonitorParaNo;    //  ??????
u32t  LastWriteAddr=0;  //  ??? ? ??
u8t   MonitorBitLen=0;  //  ??? ? ????

u16t  MonitorRegAddress[MonitorRegNoMAX];
u16t  MonitorRegLength = 0;
u16t  MonitorBitLength = 0;

const u8t SDataReg[]=      //  0x0e00 ~ 0x0eff ?????
{0xD8,0x00,0x62,0x66,0x08,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x1E,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x0A,0x00,0x0A,0x00,0x0A,0x00,0x26,0x00,0x04,0x00,0x03,0x00
,0x0D,0x00,0x05,0x00,0x16,0x00,0x05,0x00,0x0A,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF4,0x01,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,0xFF,0x96,0x24,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x03,0x00,0x00,0x00,0x00,0x00};
u8t CodeStart[]=     //  0x8000 ~ 0x805b ?????
{0x08,0x00,0x7E,0xB4,0x00,0x00,0x00,0x00,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20
,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20
,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20
,0xF4,0x09,0xFF,0x0B,0xF4,0x01,0xE7,0x03,0x64,0x0E,0xC7,0x0E,0xDC,0x0E,0xFF,0x0E
,0x90,0x01,0xFE,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
u8t PLCXYMonitorData[]=      //  0x1790 ~ 0x ?????
{0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

/*
#define RAM_M_bitAddr       0x0000  //  Mx ?????            M0000 ~ M3071
#define RAM_Y_bitAddr       0x0C00  //  Yx ?????            Y000  ~ Y267(0x0CB7)
#define RAM_SM_bitAddr      0x0E00  //  SMx ???????     M8000 ~ M8255
#define RAM_Cp_bitAddr      0x0F00  //  Cx ??? Over Contact C000  ~ C255
#define RAM_Tp_bitAddr      0x1000  //  Tx ??? Over Contact T000  ~ T255(0x10ff)
#define RAM_X_bitAddr       0x1200  //  Xx ?????            X000  ~ X277
#define RAM_S_bitAddr       0x1400  //  Sx ?????            S000  ~ S999
#define RAM_Cc_bitAddr      0x2700  //  Cx ??? Enable Coil  C000  ~ C255
#define RAM_Tc_bitAddr      0x2800  //  Tx ??? Enable Coil  T000  ~ T255
#define RAM_Cr_bitAddr      0x3700  //  Cx ??? Restet       C000  ~ C255
#define RAM_Tr_bitAddr      0x3800  //  Tx ??? Restet       T000  ~ T255
*/
static const u32t _uu [11 * 3] = {
    RAM_M_bitAddr , _M_num,    (u32t)rM, 
    RAM_Y_bitAddr , _Y_num,    (u32t)rY,
    RAM_SM_bitAddr, _M8xxx_num,(u32t)rM8xxx,
    RAM_Cp_bitAddr, _C_num,    (u32t)rC,
    RAM_Tp_bitAddr, _T_num,    (u32t)rT,
    RAM_X_bitAddr , _X_num,    (u32t)rX,
    RAM_S_bitAddr , _M8xxx_num,(u32t)rM8xxx,
    RAM_Cc_bitAddr, _C_num,    (u32t)rC,
    RAM_Tc_bitAddr, _T_num,    (u32t)rTF,
    RAM_Cr_bitAddr, _C_num,    (u32t)rC,
    RAM_Tr_bitAddr, _T_num,    (u32t)rT
};
static u32t GetBitVarValue(u32t a) {
    u32t t;
    s32t i;

    for (i = 30; i >= 0; i -= 3) {
        if (a >= _uu[i]) {
            a = a - _uu[i];
            if(a >= _uu[i+1]) {
                return 0;
            }
            //t = *((u8t *)_uu[i + 2] + a / 8);
            t = _uu[i + 2] + a / 8;
            t = *((u8t*)t);
            a = a % 8;
            t >>= a;
            return t & 0x01;
        }
    }
    return 0;
}
/*
#define RAM_C_RegAddr       0x0A00  //  Cx ???(16bit)       C000  ~ C199
#define RAM_C2_RegAddr      0x0C00  //  Cx ???(32bit)       C200  ~ C234(0x0C8B)
#define RAM_SD_RegAddr      0xOE00  //  SDx ???????     D8000 ~ D8255
#define RAM_T_RegAddr       0x1000  //  Tx ???(16bit)       T000  ~ T255
#define RAM_D_RegAddr       0x4000  //  Dx ?????          D0000 ~ D7999
*/
/*
addr ??????

?????? 2 = 16 4 = 32
v    ??
*/
static u32t GetU32tVarValue(u32t addr) {
    if (addr >= RAM_D_RegAddr) {
        if (addr < (_D_num + RAM_D_RegAddr)) {
            return _D[(addr - RAM_D_RegAddr) / 2];
        }
    }
    else if (addr >= RAM_T_RegAddr) {
        if (addr < (_T_num + RAM_T_RegAddr)) {
            return _T[(addr - RAM_T_RegAddr) / 2][0];
        }
    }
    else if (addr >= RAM_C_RegAddr) {
        if (addr < (_C_num + RAM_C_RegAddr)) {
            return _C[(addr - RAM_C_RegAddr) / 2];
        }
    }
    return 0;
}

/*
#define PLC_FLASH_SIZE   (1024 * 4 * 64)

static u8t flashbufs[PLC_FLASH_SIZE];
*/
void WriteFlash(u32t WriteAddr, u8t *Buf, u32t WriteLen) {
    u32t i;
    u8t wrdata = 0;
    for (i = 0; i < WriteLen * 2; i += 2) {
        wrdata = asctohex((u8t *)(Buf + i));
        IAPFlashProgremMode(WriteAddr++, wrdata);
    }
}
void Write_D_Arr(u32t WriteAddr,u8t *Buf,u32t WriteLen) {  //  ? RAM? ??? D
    u32t i;
    u32t wrdata = 0;
    u32t tAddr = 0;

    tAddr = WriteAddr - GDataRegStartAddr;
    if ((tAddr >= 0) && (tAddr < _D_num * 2)) {
        for (i = 0; i < WriteLen * 2; i += 4) {
            wrdata  = asctohex((u8t *)(Buf + i + 2));
            wrdata <<= 8;
            wrdata |= asctohex((u8t *)(Buf + i));
            _D[tAddr / 2] = wrdata;
            tAddr += 2;
        }
    }
}
void Read_D_Arr(void) {  //  ? IAP(code)? ??? D
    u32t i;
    u32t wrdata=0;
    u32t tAddr=0;

    tAddr = GDataReg_D_StartAddr + GDataRegOffset;
    for (i = 0; i < _D_num; i++) {
        wrdata = _D[i] >> 8;
        IAPFlashProgremMode(tAddr + 2 * i + 1, wrdata);
        wrdata = (u8t)_D[i];
        IAPFlashProgremMode(tAddr + 2 * i, wrdata);
    }
}
void ErasurePLC(u8t ErasureScope)
{
    u32t i;
    switch(ErasureScope)
    {
    case ErasureCODE:
        // 1. ????(?????????,? Flash ??? FF)
        for(i = CodeStartAddr; i < CodeEndAddr; i += 512) {
            IAPFlashErasureMode(i);             
        }
        
        // ==========================================
        // 2. ?????(???? STM32 ?????,????)
        // ==========================================
        
        // ?????:?? Flash,?????????????
        FLASH_Unlock(); 
        FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

        // ?????:?? 92 ????(? 16 ?????,?? STM32 ???!)
        // 0x5C ? 92,???? i ?? 2
        for(i = 0; i < 0x5C; i += 2) { 
            // STM32 ?????:??????? 8 ?,??????? 8 ?
            u16t half_word = CodeStart[i] | ((u16t)CodeStart[i+1] << 8); 
            
            // ???????????????? 16 ???
            FLASH_ProgramHalfWord(CodeStartAddr + i, half_word);
        }
        
        // ?????:????? END ??!
        // 0x805C ??? 0x0F,0x805D ??? 0x00?
        // ?? 16 ??? 0x000F,?????!
        FLASH_ProgramHalfWord(CodeStartAddr + 0x5C, 0x000F);
        
        // ?????:????,????
        FLASH_Lock();   
        // ==========================================
        break;
        
    case ErasureDataReg:
        for(i = GDataRegStartAddr; i < GDataRegEndAddr; i += 512) {
            IAPFlashErasureMode(GDataRegOffset + i);
        }
        break;
        
    case ErasurebitReg:
        break;
    default:
        break;
    }
}
u8t sumcheck(u8t *buf,u8t length) {
    u32t i;
    u8t sumdata = 0;

    for (i = 0; i < length; i++) {
        sumdata += buf[i];
    }
    return sumdata;
}
//=======================================================================================================
// ????: GetRdorWrLen
//
// ????: ?? ?/? ????
//
// ? ?:   u8t *StartAddr
//          
// ? ?:   u8t
//         
//=======================================================================================================
u8t GetRdorWrLen(u8t *StartAddr) {
    u8t i = 0;
    for (i = 0; i < 2; i++) {
        *(StartAddr + i) = ascto0F(*(StartAddr + i));
    }
    *StartAddr <<= 4;
    return (*StartAddr + *(StartAddr + 1));
}
u32t  GetRdorWrAddr(u8t *StartAddr) {
    u8t i=0;
    u32t  tempAddr=0;

    for (i = 0; i < 4; i++) {
        *(StartAddr+i)=ascto0F(*(StartAddr+i));
    }
    tempAddr =*StartAddr << 12;
    tempAddr += *(StartAddr + 1) << 8;
    tempAddr += *(StartAddr + 2) << 4;
    tempAddr += *(StartAddr + 3);
    return tempAddr;
}
u32t  asctoINT_Ledian(u8t *StartAddr) {
    u8t i = 0;
    u32t tempAddr = 0;

    for (i = 0; i < 4; i++) {
        *(StartAddr + i) = ascto0F(*(StartAddr + i));
    }
    tempAddr = *StartAddr << 4;
    tempAddr += *(StartAddr + 1);
    tempAddr += *(StartAddr + 2) << 12;
    tempAddr += *(StartAddr + 3) << 8;
    return tempAddr;
}
//======================================================================================================================
//  IAP ??????:(31K)        ----ASCII?HEX???. 
//      1.IAP??? 0x8000 ~   0xbfff              ??PLC?? -- ??PLC ?0x8000~0xbfff ???8000?????
//      2.IAP????0 ??                           ??  PLC???????? ? 1 ???
//      3.IAP????1  ~   PLCTypeArrayLenMAX      ??  PLC????         ? PLCTypeArrayLen ??? PLCTypeArrayLen
//      4.IAP?????                              ????    ?????????!
//      5.??                                        ??
//======================================================================================================================
const u8t OrderSend1[]={0x02,0x36,0x32,0x36,0x36,0x03,0x44,0x37};  //  '6266'PLC?? FX1N;'C256'PLC?? FX1S.
const u8t OrderSend2[]={0x02,0x30,0x41,0x03,0x37,0x34};            //  '0A'PLC ??;'09'PLC ??
//********************************************************************************************//
//*                                                                                          *//
//* ????:                                                                                *//
//* 1.OrderSend3? PLC 0x8000 ????,??? 0x2e ??????;                             *//
//* 2.??? 0x8008????? 0x08 ??????,?PLC???????,??????.            *//
//*                                                                                          *//
//********************************************************************************************//
const u8t OrderSend3[]=
{0x08,0x00,0xDE,0xCB,0x00,0x00,0x00,0x00,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20
,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20};
//********************************************************************************************//
//*                                                                                          *//
//* ????:                                                                                *//
//* 1.OrderSend4? PLC 0x802e ????,??? 0x2e ??????;                             *//
//* 2.??? 0x8048????? 0x04 ??????,?PLC[?????]????,??????.      *//
//*                                                                                          *//
//********************************************************************************************//
const u8t OrderSend4[]=
{0x20,0x20,0xF4,0x09,0xFF,0x0B,0xF4,0x01,0xE7,0x03,0x64,0x0E,0xC7,0x0E,0xDC,0x0E,0xFF,0x0E,0x90,0x01,0xFE,0x03,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
const u8t  OrderSend5[]={0x02,0x31,0x30,0x30,0x30,0x03,0x43,0x34};  //  PLC???????
const u8t  OrderSend6[]={0x02,0x30,0x38,0x30,0x30,0x03,0x43,0x42};

void FX1NProcessing(u8t *buf, u32t length) {
    u32t  i = 0;
    u8t sumdata = 0;
    //u32t  ReadAddr = 0;

    UartReceiveBuffer = buf;
    UartReceiveCounter = length;

    if (UartReceiveBuffer[0] == uENQ) {  // ????                 ==== 1. 3. 5. 7. 10. 12. 14.(14??????) 
        UartSendchar(uACK);
    }
    else if ((UartReceiveCounter > 3) && (UartReceiveBuffer[0] == uSTX) && (UartReceiveBuffer[UartReceiveCounter-3] == uETX)) { // ????
        // sum??? ?02??????(?????????)?????03(???????????)??
        // ?????8?,????4???4?(?03???????)???ASCII? ???? ????   
        sumdata = asctohex((u8t *)(UartReceiveBuffer + UartReceiveCounter - 2));
        if (sumdata == sumcheck((u8t *)(UartReceiveBuffer + 1), (UartReceiveCounter - 3))) {
            if (UartReceiveBuffer[1] == uCMDRead) {     //  PC ?? PLC ????.
                //ReadAddr=GetRdorWrAddr((u8t *)(UartReceiveBuffer+2));
                //if(ReadAddr==PLCTypeAddr)
                {
                    for (i = 0; i < (UartReceiveCounter - 1); i++) {
                        UartReceiveBuffer[UartReceiveCounter + 1 - i] = UartReceiveBuffer[UartReceiveCounter - 1 - i];
                    }
                    UartReceiveBuffer[1] = uCMDFNC;
                    UartReceiveBuffer[2] = uCMDRead;
                }
            }
            FX1NUartFNCPro();                       //  ????--??????
        }
        else {
            UartResponseError();
        }
    }
    else {
        UartResponseError();
    }
}
void FX1NUartFNCPro(void) {              //  ????--??????
    switch(UartReceiveBuffer[1]) {
        case uCMDFNC:
        {
            switch(UartReceiveBuffer[2])
            {
            case uCMDRead:
                UartResponseCMDRead();
                break;
            case uCMDWrite:
                UartResponseCMDWrite();
                break;
            case uCMDErasure:
                if (PLCRunFlag == 0) 
                    UartResponseCMDErasure();
                break;
            case uCMDEnforceOn:
                UartResponseCMDEnforceOn();
                break;
            case uCMDEnforceOff:
                UartResponseCMDEnforceOff();
                break;
            default:UartResponseError();
            }
        }
        break;
    case uCMDunKnow34:
        UartResponseCMDunKnow34();
        break;
    case uCMDDownLoadEnd:
        //if (PLCRunFlag == 0) 
            UartResponseCMDDownLoadEnd();
        break;
    default:
        UartResponseError();
        break;
    }
}

// ==============================================================================
// ???????????,??????!
// ==============================================================================

extern u8t PLCRunFlag; // ????????,??????
void UartResponseCMDRead(void) {             
    u32t i = 0, j = 0;
    u8t Readdat = 0, ReadLen = 0, sumdata = 0, t = 0;
    u32t OrigReadAddr = 0;

    // ????,?????
    if ((UartReceiveBuffer[3] != uCtrlData) && (UartReceiveBuffer[3] != uCtrlCode)) return;

    OrigReadAddr = GetRdorWrAddr((u8t *)(UartReceiveBuffer+4));
    ReadLen = GetRdorWrLen((u8t *)(UartReceiveBuffer+8));

    // --- 1. ?? PLC ???? ---
    if (UartReceiveBuffer[3] == uCtrlData && OrigReadAddr == PLCMonitorAckAddr) {
        UartSendchar(uSTX);
        sumdata = 0;
        MonitorRegLength &= 0xff;
        for (i = 0; i < MonitorRegLength; i++) {
            u32t ii = GetU32tVarValue(MonitorRegAddress[i]);
            sumdata = UartSendASCDate((u8t)(ii), sumdata);
            sumdata = UartSendASCDate((u8t)(ii >> 8), sumdata);
        }
        t = 0; j = 0;
        for (i = 0; i < MonitorBitLength; i++) {
            if (GetBitVarValue(MonitorRegAddress[MonitorRegLength + i])) t |= (1 << j);
            j++;
            if (j == 8) { sumdata = UartSendASCDate(t, sumdata); j = 0; t = 0; }
        }
        if (j) sumdata = UartSendASCDate(t, sumdata);
        UartSendchar(uETX); UartSendASCCRC(sumdata);
        return;
    }

    // --- 2. ???????? ---
    UartSendchar(uSTX);
    sumdata = 0;

    for (i = 1; i <= ReadLen; i++) {
        u32t current_logical = OrigReadAddr + i - 1;

        // =========================================================
        // ???:?????? (0x8000 ~ 0xBFFF)
        // =========================================================
        if (current_logical >= 0x8000) {
            u32t offset = current_logical - 0x8000;
            
            // ??????:????????!??????? 9KB ???!
            if (offset < FX1N_FLASH_SIZE) {
                u32t physical_addr = CodeStartAddr + offset;
                Readdat = *(__IO uint8_t*)(physical_addr); // ???????????
                
                // ??????:?????? (0x8008-0x800F) ? 8 ?????
                if (current_logical >= 0x8008 && current_logical <= 0x800F) {
                    Readdat = 0x20; 
                }
            } else {
                // ?????? 9KB ??,???? 0x00 (NOP),????? 16KB ??,????!
                Readdat = 0x00; 
            }
        } 
        // =========================================================
        // ???:????????? (< 0x8000)
        // =========================================================
        else {
            if (current_logical == 0x0000) Readdat = 0x0C;       
            else if (current_logical == 0x0002) Readdat = 0x0A;
            else if (current_logical == 0x0005) Readdat = 0x08;
            else if (current_logical >= 10 && current_logical <= 15) Readdat = 0x20; 
            
            else if (current_logical >= 0x0E00 && current_logical <= 0x0EFF) {
                // ???? FX1N ???,????????
                Readdat = SDataReg[current_logical - 0x0E00]; 
            }
            else if (current_logical >= 0x0FF0 && current_logical <= 0x0FFF) {
                Readdat = 0x20; 
            }
            else if (current_logical >= 0x0800 && current_logical < 0x0800 + _T_num * 2) {
                u32t offset = current_logical - 0x0800;
                if (offset % 2 == 0) Readdat = _T[offset/2][0] & 0xFF;
                else Readdat = (_T[offset/2][0] >> 8) & 0xFF;
            }
            else if (current_logical >= GDataRegStartAddr && current_logical < GDataRegStartAddr + _D_num * 2) {
                u32t offset = current_logical - GDataRegStartAddr;
                if (offset % 2 == 0) Readdat = _D[offset/2] & 0xFF;
                else Readdat = (_D[offset/2] >> 8) & 0xFF;
            }
            else if (current_logical >= PLCStateAddr && current_logical < PLCStateAddr + 2) {
                Readdat = IAPFlashReadMode(PLCStateRealAddr + current_logical - PLCStateAddr);
                if (Readdat == 0xFF) Readdat = 0x00; 
            }
            else {
                Readdat = 0x00; 
            }
        }

        sumdata = UartSendASCDate(Readdat, sumdata);
    }
    
    UartSendchar(uETX); 
    UartSendASCCRC(sumdata);
}
u8t GetXYMSBit(u8t *ucdat, u8t num) {
    u8t numsh = num / 8;
    u8t numyu = num % 8;
    numyu = 1 << numyu;
    numsh = (*(ucdat + numsh)) & numyu;
    if (numsh) return 1;
    else return 0;
}


u8t UartSendASCDate(u8t Readdat, u8t sumdata) {
    u32t tempascdata = hextoasc(Readdat);
    u8t ucdata = tempascdata >> 8;
    sumdata += ucdata;
    UartSendchar(ucdata);
    ucdata = tempascdata & 0xff;
    sumdata += ucdata;
    UartSendchar(ucdata);
    return sumdata;
}

void UartSendASCCRC(u8t sumdata) {
    u8t usum = 0, Readdat = 0, tempsum = 0;
    sumdata += uETX;
    usum = sumdata & 0x0f;
    tempsum = (usum < 10) ? (usum + 0x30) : (usum + 0x41 - 0x0a);
    sumdata >>= 4;
    usum = sumdata & 0x0f;
    Readdat = (usum < 10) ? (usum + 0x30) : (usum + 0x41 - 0x0a);
    UartSendchar(Readdat);
    UartSendchar(tempsum);                          
}

void UartResponseCMDWrite(void) {        // ????--???
    u32t i = 0, temp;
    u8t WriteLen = 0, NoDataRegFlag = 0;
    u32t WriteAddr = 0, OrigWriteAddr = 0;

    if ((UartReceiveBuffer[3] != uCtrlData) && (UartReceiveBuffer[3] != uCtrlCode)) return;

    WriteLen = GetRdorWrLen((u8t *)(UartReceiveBuffer + 8));
    OrigWriteAddr = GetRdorWrAddr((u8t *)(UartReceiveBuffer + 4));

    // ?????????
    if (OrigWriteAddr >= 0x0FF0 && OrigWriteAddr <= 0x0FFF) { UartSendchar(uACK); return; }
    if (UartReceiveBuffer[3] == uCtrlCode) {
        if (OrigWriteAddr < 0x8000 || OrigWriteAddr > 0xBFFF) { UartSendchar(uACK); return; }
    }

    if (UartReceiveBuffer[3] == uCtrlData) {
        if (OrigWriteAddr == PLCStateAddr) {
            WriteAddr = PLCStateRealAddr;
        }
        else if (OrigWriteAddr == PLCMonitorPollAddr) {
            MonitorRegLength = asctoINT_Ledian((u8t *)(UartReceiveBuffer+10));
            MonitorBitLength = asctoINT_Ledian((u8t *)(UartReceiveBuffer+14));
            temp = (MonitorRegLength + MonitorBitLength) & 0xff;
            for (i = 0; i < temp; i++) MonitorRegAddress[i] = asctoINT_Ledian((u8t *)(UartReceiveBuffer+i * 4 + 18));
            NoDataRegFlag = 1;
        }
        else if (OrigWriteAddr == (PLCMonitorPollAddr+4)) {
            WriteAddr = PLCS0x1400RealAddr + 4;
        }
        else if ((OrigWriteAddr >= GDataRegStartAddr) && (OrigWriteAddr < GDataRegEndAddr)) {
            Write_D_Arr(OrigWriteAddr, (u8t *)(UartReceiveBuffer+10), (u8t)WriteLen);
            NoDataRegFlag = 1; // ??????,?? Flash
        }
        else if (OrigWriteAddr < 0x8000) {
            // ???? D8000 ? T/C ???????
            UartSendchar(uACK); return;
        }
        else {
            WriteAddr = OrigWriteAddr + CodeStartAddr - 0x8000;
        }
    } else {
        if (OrigWriteAddr < 0x8000) { UartSendchar(uACK); return; }
        WriteAddr = OrigWriteAddr + CodeStartAddr - 0x8000;
    }
    
    if (NoDataRegFlag == 0) {
        WriteFlash(WriteAddr, (u8t *)(UartReceiveBuffer+10), (u8t)WriteLen);
    }
    LastWriteAddr = WriteAddr;        
    UartSendchar(uACK);
}



// ==============================================================================
// ?????????????? (????,??? L6218E)
// ==============================================================================

void UartResponseCMDErasure(void) {      // ????--???? (?? Undefined symbol)
    u8t ReStartFlag = 0;
    switch(UartReceiveBuffer[3]) {
        case ErasureCODE: 
            ErasurePLC(ErasureCODE);        
            break;
        case ErasureDataReg: 
            ErasurePLC(ErasureDataReg);     
            ReStartFlag=1;
            break;
        case ErasurebitReg: 
            ErasurePLC(ErasurebitReg);      
            ReStartFlag=1;
            break;
        default:
            break;
    }
    UartSendchar(uACK);
    if (ReStartFlag) {
        PLCReset();
    }
}



void UartResponseCMDEnforceOn(void) {    //  ????--????On?? (?? STOP)
    u32t EnforceOnAddr = 0;
    EnforceOnAddr = GetRdorWrAddr((u8t *)(UartReceiveBuffer + 3));
    if (EnforceOnAddr == 0x250e) {
        IAPFlashProgremMode(PLCStateRealAddr,PLCStateStop);
        rM8xxx[0].BYTE = (rM8xxx[0].BYTE & 0xfc) | 0x02;
        // ??????:????????,??? STOP ????!
        PLCRunFlag = 0; 
    }
    UartSendchar(uACK);
}

void UartResponseCMDEnforceOff(void) {   //  ????--????OFF?? (?? RUN)
    u32t EnforceOffAddr = 0;
    EnforceOffAddr = GetRdorWrAddr((u8t *)(UartReceiveBuffer + 3));
    if(EnforceOffAddr==0x250e) {
        IAPFlashProgremMode(PLCStateRealAddr, PLCStateRun); 
        rM8xxx[0].BYTE = (rM8xxx[0].BYTE & 0xfc) | 0x01;
        // ??????:????????,??? RUN ????!
        PLCRunFlag = 1; 
    }
    UartSendchar(uACK);
}

void UartResponseCMDunKnow34(void) {     // ???? 0x34 ??
    UartSendchar(uACK);
}

void UartResponseCMDDownLoadEnd(void) {  // ??????
    if (UartReceiveBuffer[2] == uETX) {
        UartSendchar(uACK);
        PLCReset();
    }
}

void UartResponseError(void) {           // ????
    UartSendchar(uNACK);
}

void PLCReset(void) {}

// ==============================================================================
// ????:?? Flash ??????????
// ==============================================================================
void System_Flash_Init(void) {
    // ????????? (0x805C) ???? FF (0xFFFF)
    // ???,??????????,??????!
   // ??????????:0x0800805C
    if (*(__IO uint16_t*)(0x0800805C) == 0xFFFF) 
    {
        FLASH_Status status; // ???????????
        
        FLASH_Unlock();
        FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
        
        // ??????:??? 0x0800805C ?????? 0x000F!
        status = FLASH_ProgramHalfWord(0x0800805C, 0x000F);
        
        FLASH_Lock();
        
        // ==========================================
        // ?? ????:??????,????????
        // ==========================================
        if (status != FLASH_COMPLETE) {
            // ??????,?? STM32 ?????????!
            // ????????,?? status ??:
            // 2: FLASH_ERROR_PG (????,????????)
            // 3: FLASH_ERROR_WRP (???,?? Flash ????!)
            while(1); // ????
        }
			}
}

// (????????????,?????,?? Warning)
