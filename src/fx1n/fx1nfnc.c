#include "fx1ntype.h"
#include "fx1nuart.h"
#include "fx1nvar.h"


static u8t CY;

//by ykb change 32
static u32t ACC_BIT;                            //  位运算器及32级中间过渡栈
static u32t MPS_BIT;                            //  位元件 8级堆栈

static void plc_get_next_cmd(void);
static u8t* _ADDR_int_ppp(u32t a, u32t index);

#define ADDR_int_ppp(a) _ADDR_int_ppp(a, 2)
#define ADDR_int_ppp_old(a) _ADDR_int_ppp(a, 3)

/**********************************************  散转子函数表  ****************************************/ 
typedef void(*FunType)(void);

static void FNC_ERROR(void);
static void FNC_OR_GE(void);
static void FNC_OR_LE(void);
static void FNC_OR_NE(void);
static void FNC_OR_LT(void);
static void FNC_OR_GT(void);
static void FNC_OR_EQ(void);
static void FNC_AND_GE(void);
static void FNC_AND_LE(void);
static void FNC_AND_NE(void);
static void FNC_AND_LT(void);
static void FNC_AND_GT(void);
static void FNC_AND_EQ(void);
static void FNC_LD_GE(void);
static void FNC_LD_LE(void);
static void FNC_LD_NE(void);
static void FNC_LD_LT(void);
static void FNC_LD_GT(void);
static void FNC_LD_EQ(void);
static void FNC_WP3A(void);
static void FNC_RD3A(void);
static void FNC_GBIN(void);
static void FNC_GRY(void);
static void FNC_HOUR(void);
static void FNC_TWR(void);
static void FNC_TRD(void);
static void FNC_TSUB(void);
static void FNC_TADD(void);
static void FNC_TZCP(void);
static void FNC_TCMP(void);
static void FNC_DRVA(void);
static void FNC_DRVI(void);
static void FNC_PLSV(void);
static void FNC_ZRN(void);
static void FNC_ABS(void);
static void FNC_SWAP(void);
static void FNC_TAN(void);
static void FNC_COS(void);
static void FNC_SIN(void);
static void FNC_INT(void);
static void FNC_ESOR(void);
static void FNC_EDIV(void);
static void FNC_EMUL(void);
static void FNC_ESUB(void);
static void FNC_EADD(void);
static void FNC_EBIN(void);
static void FNC_EBCD(void);
static void FNC_EZCP(void);
static void FNC_ECMP(void);
static void FNC_PID(void);
static void FNC_VRSC(void);
static void FNC_HEX(void);
static void FNC_CCD(void);
static void FNC_VRRD(void);
static void FNC_ASCI(void);
static void FNC_FROM(void);
static void FNC_TO(void);
static void FNC_RS(void);
static void FNC_PRUN(void);
static void FNC_PR(void);
static void FNC_ROTC(void);
static void FNC_SORT(void);
static void FNC_TKY(void);
static void FNC_HKY(void);
static void FNC_DSW(void);
static void FNC_SEGD(void);
static void FNC_SEGL(void);
static void FNC_ARWS(void);
static void FNC_ASC(void);
static void FNC_PWM(void);
static void FNC_PLSR(void);
static void FNC_IST(void);
static void FNC_SER(void);
static void FNC_ABSD(void);
static void FNC_INCD(void);
static void FNC_TTMR(void);
static void FNC_STMR(void);
static void FNC_RAMP(void);
static void FNC_REFF(void);
static void FNC_MTR(void);
static void FNC_HSCS(void);
static void FNC_HSCR(void);
static void FNC_HSZ(void);
static void FNC_SPD(void);
static void FNC_PLSY(void);
static void FNC_REF(void);
static void FNC_DNCO(void);
static void FNC_SUM(void);
static void FNC_BON(void);
static void FNC_MEAN(void);
static void FNC_ANS(void);
static void FNC_ANR(void);
static void FNC_SOR(void);
static void FNC_FLT(void);
static void FNC_DECO(void);
static void FNC_RCR(void);
static void FNC_RCL(void);
static void FNC_SFTR(void);
static void FNC_SFTL(void);
static void FNC_WSFR(void);
static void FNC_WSFL(void);
static void FNC_SFWR(void);
static void FNC_SFRD(void);
static void FNC_ZRST(void);
static void FNC_ROL(void);
static void FNC_ALT(void);
static void FNC_ROR(void);
static void FNC_FMOV(void);
static void FNC_BMOV(void);
static void FNC_SMOV(void);
static void FNC_ZCP(void);
static void FNC_CMP(void);
static void FNC_IRET(void);
static void FNC_EI(void);
static void FNC_DI(void);
static void FNC_FEND(void);
static void FNC_WDT(void);
static void FNC_FOR(void);
static void FNC_NEXT(void);
static void FNC_NEG(void);
static void FNC_WXOR(void);
static void FNC_WOR(void);
static void FNC_WAND(void);
static void FNC_DEC(void);
static void FNC_INC(void);
static void FNC_DIV(void);
static void FNC_MUL(void);
static void FNC_SUB(void);
static void FNC_ADD(void);
static void FNC_BIN(void);
static void FNC_BCD(void);
static void FNC_MOV(void);
static void FNC_XCH(void);
static void FNC_CML(void);
static void FNC_CJ(void);
static void FNC_CALL(void);
static void FNC_SRET(void);

static void ORFMp(void);
static void ORF(void);
static void ORPMp(void);
static void ORP(void);
static void LDP(void);
static void LDPMp(void);
static void LDF(void);
static void LDFMp(void);
static void ANDP(void);
static void ANDPMp(void);
static void ANDF(void);
static void ANDFMp(void);
static void RSTD(void);
static void RSTTC(void);
static void MC(void);
static void MCR(void);
static void PLF(void);
static void PLS(void);
static void OUTM8xxx(void);
static void OUTMp(void);
static void SETM8xxx(void);
static void SETMp(void);
static void RSTM8xxx(void);
static void RSTMp(void);
static void OUTS(void);
static void SETS(void);
static void RSTS(void);
static void ANB(void);
static void ORB(void);
static void MPS(void);
static void MRD(void);
static void MPP(void);
static void INV(void);
static void NOP(void);
static void NULLCMD(void);
static void END(void);

static void AND(void);
static void ANDMp(void);
static void ANI(void);
static void ANIMp(void);
static void OR(void);
static void ORMp(void);
static void ORI(void);
static void ORIMp(void);
static void CMDP(void);
static void LD(void);
static void LDMp(void);
static void LDI(void);
static void LDIMp(void);

static void OUTYM(void);
static void SETYM(void);
static void RSTYM(void);
static void CMDCH(void);
static void CMDER(void);
static void CMDFNC(void);

static const FunType key_list[16]={
          CMDFNC ,   // 0 (FNC应用指令)   
          CMDP   ,   // 1 (P 应用指令)  
          LD     ,   // 2 (LD指令, 5000+ppp, 扩展 Mp除外)  
          LDI    ,   // 3 (LDI指令, 5000+ppp, 扩展 Mp除外)   
          AND    ,   // 4 (AND指令, 5000+ppp, 扩展 Mp除外)   
          ANI    ,   // 5 (ANI指令, 5000+ppp, 扩展 Mp除外)   
          OR     ,   // 6 (OR指令, 5000+ppp, 扩展 Mp除外)   
          ORI    ,   // 7 (ORI指令, 5000+ppp, 扩展 Mp除外)   
          CMDER  ,   // 8 (多字指令，第二字及以后有效) 
          CMDER  ,   // 9  
          CMDER  ,   // A (多字指令，第二字及以后有效, 仅对M1536-M3071有效，需加偏移量200)  
          CMDER  ,   // B (Pn指令, 仅对CJ，CALL有效)
          OUTYM  ,   // C (OUT指令, 仅对Y，M有效) 
          SETYM  ,   // D (SET指令, 仅对Y，M有效) 
          RSTYM  ,   // E (RST指令, 仅对Y，M有效) 
          CMDCH };   // F (纯单字指令)  

static const FunType key_list_1[12][2] = {
         {OUTM8xxx ,     // (OUT指令, 0002 8000+ppp ；仅对M8xxx有效) 
          OUTMp      },  // (OUT指令, 0002 A000+ppp ；仅对Mp有效)  
         {SETM8xxx ,     // (SET指令, 0003 8000+ppp ；仅对M8xxx有效) 
          SETMp      },  // (SET指令, 0003 A000+ppp ；仅对Mp有效)  
         {RSTM8xxx ,     // (RST指令, 0004 8000+ppp ；仅对M8xxx有效) 
          RSTMp      },  // (RST指令, 0004 A000+ppp ；仅对Mp有效)  
         {OUTS ,CMDER},  // (OUT指令, 0005 8000+ppp ；仅对S有效) 
         {SETS ,CMDER},  // (SET指令, 0006 8000+ppp ；仅对S有效)  
         {RSTS ,CMDER},  // (RST指令, 0007 8000+ppp ；仅对S有效) 
         {PLS  ,CMDER},  // (PLS指令, 0008 8000+ppp ；仅对Y，M有效) 
         {PLF  ,CMDER},  // (PLF指令, 0009 8000+ppp ；仅对Y，M有效)  
         {MC   ,CMDER},  // (MC指令, 000A 8000+(N) 8000+ppp ；仅对Y，M有效)  
         {MCR  ,CMDER},  // (MCR指令, 000B 8000+(N) ； N=0-7 
         {RSTTC,CMDER},  // (RST指令, 000C 8000+ppp ；仅对T，C，Cp有效) 
         {RSTD ,CMDER}}; // (RST指令, 000D 8m00+xx  8n00+yy ；仅对 D 有效，包含Z，V) 

static const FunType key_list_2[14][2]={
         {CMDER ,LDMp  }, // (LD指令, 01C2 A000+ppp ；仅对Mp有效)  
         {CMDER ,LDIMp }, // (LDI指令, 01C3 A000+ppp ；仅对Mp有效)  
         {CMDER ,ANDMp }, // (AND指令, 01C4 A000+ppp ；仅对Mp有效)  
         {CMDER ,ANIMp }, // (ANI指令, 01C5 A000+ppp ；仅对Mp有效)  
         {CMDER ,ORMp  }, // (OR指令, 01C6 A000+ppp ；仅对Mp有效)   
         {CMDER ,ORIMp }, // (ORI指令, 01C7 A000+ppp ；仅对Mp有效)   
         {CMDER ,CMDER }, 
         {CMDER ,CMDER }, 
         {LDP      ,      // (LDP指令, 01CA 8000+ppp ；扩展 Mp除外)   
          LDPMp        }, // (LDP指令, 01CA A000+ppp ；仅对 Mp有效)   
         {LDF      ,      // (LDF指令, 01CB 8000+ppp ；扩展 Mp除外)     
          LDFMp        }, // (LDF指令, 01CB A000+ppp ；仅对 Mp有效)    
         {ANDP     ,      // (ANDP指令, 01CC 8000+ppp ；扩展 Mp除外)     
          ANDPMp       }, // (ANDP指令, 01CC A000+ppp ；仅对 Mp有效)     
         {ANDF     ,      // (ANDF指令, 01CD 8000+ppp ；扩展 Mp除外)      
          ANDFMp       }, // (ANDF指令, 01CD A000+ppp ；仅对 Mp有效)      
         {ORP      ,      // (ORP指令, 01CE 8000+ppp ；扩展 Mp除外)     
          ORPMp        }, // (ORP指令, 01CE A000+ppp ；仅对 Mp有效)     
         {ORF      ,      // (ORF指令, 01CF 8000+ppp ；扩展 Mp除外)     
          ORFMp       }}; // (ORF指令, 01CF A000+ppp ；仅对 Mp有效)     

static const FunType key_list_3[256] = {
       FNC_CJ    ,    // 0
       FNC_CALL  ,    // 1
       FNC_SRET  ,    // 2    
       FNC_IRET  ,    // 3
       FNC_EI    ,    // 4
       FNC_DI    ,    // 5
       FNC_FEND  ,    // 6
       FNC_WDT   ,    // 7
       FNC_FOR   ,    // 8
       FNC_NEXT  ,    // 9
       FNC_CMP   ,    // 10
       FNC_ZCP   ,    // 11
       FNC_MOV   ,    // 12
       FNC_SMOV  ,    // 13
       FNC_CML   ,    // 14
       FNC_BMOV  ,    // 15
       FNC_FMOV  ,    // 16
       FNC_XCH   ,    // 17
       FNC_BCD   ,    // 18
       FNC_BIN   ,    // 19
       FNC_ADD   ,    // 20
       FNC_SUB   ,    // 21
       FNC_MUL   ,    // 22
       FNC_DIV   ,    // 23
       FNC_INC   ,    // 24
       FNC_DEC   ,    // 25
       FNC_WAND  ,    // 26
       FNC_WOR   ,    // 27
       FNC_WXOR  ,    // 28
       FNC_NEG   ,    // 29
       FNC_ROR   ,    // 30
       FNC_ROL   ,    // 31
       FNC_RCR   ,    // 32
       FNC_RCL   ,    // 33
       FNC_SFTR  ,    // 34
       FNC_SFTL  ,    // 35
       FNC_WSFR  ,    // 36
       FNC_WSFL  ,    // 37
       FNC_SFWR  ,    // 38
       FNC_SFRD  ,    // 39
       FNC_ZRST  ,    // 40
       FNC_DECO  ,    // 41
       FNC_DNCO  ,    // 42
       FNC_SUM   ,    // 43
       FNC_BON   ,    // 44
       FNC_MEAN  ,    // 45
       FNC_ANS   ,    // 46
       FNC_ANR   ,    // 47
       FNC_SOR   ,    // 48
       FNC_FLT   ,    // 49
       FNC_REF   ,    // 50
       FNC_REFF  ,    // 51
       FNC_MTR   ,    // 52
       FNC_HSCS  ,    // 53
       FNC_HSCR  ,    // 54
       FNC_HSZ   ,    // 55 
       FNC_SPD   ,    // 56
       FNC_PLSY  ,    // 57
       FNC_PWM   ,    // 58
       FNC_PLSR  ,    // 59
       FNC_IST   ,    // 60
       FNC_SER   ,    // 61
       FNC_ABSD  ,    // 62
       FNC_INCD  ,    // 63
       FNC_TTMR  ,    // 64
       FNC_STMR  ,    // 65
       FNC_ALT   ,    // 66
       FNC_RAMP  ,    // 67
       FNC_ROTC  ,    // 68
       FNC_SORT  ,    // 69
       FNC_TKY   ,    // 70
       FNC_HKY   ,    // 71
       FNC_DSW   ,    // 72
       FNC_SEGD  ,    // 73
       FNC_SEGL  ,    // 74
       FNC_ARWS  ,    // 75
       FNC_ASC   ,    // 76
       FNC_PR    ,    // 77
       FNC_FROM  ,    // 78
       FNC_TO    ,    // 79
       FNC_RS    ,    // 80
       FNC_PRUN  ,    // 81
       FNC_ASCI  ,    // 82
       FNC_HEX   ,    // 83
       FNC_CCD   ,    // 84
       FNC_VRRD  ,    // 85
       FNC_VRSC  ,    // 86
       FNC_ERROR ,    // 87
       FNC_PID   ,    // 88
       FNC_ERROR ,    // 89
       FNC_ERROR ,    // 90
       FNC_ERROR ,    // 91   
       FNC_ERROR ,    // 92   
       FNC_ERROR ,    // 93   
       FNC_ERROR ,    // 94   
       FNC_ERROR ,    // 95   
       FNC_ERROR ,    // 96   
       FNC_ERROR ,    // 97   
       FNC_ERROR ,    // 98   
       FNC_ERROR ,    // 99
       FNC_ERROR ,    // 100   
       FNC_ERROR ,    // 101   
       FNC_ERROR ,    // 102   
       FNC_ERROR ,    // 103   
       FNC_ERROR ,    // 104   
       FNC_ERROR ,    // 105   
       FNC_ERROR ,    // 106  
       FNC_ERROR ,    // 107   
       FNC_ERROR ,    // 108   
       FNC_ERROR ,    // 109
       FNC_ECMP  ,    // 110
       FNC_EZCP  ,    // 111   
       FNC_ERROR ,    // 112   
       FNC_ERROR ,    // 113   
       FNC_ERROR ,    // 114   
       FNC_ERROR ,    // 115   
       FNC_ERROR ,    // 116   
       FNC_ERROR ,    // 117  
       FNC_EBCD  ,    // 118  
       FNC_EBIN  ,    // 119
       FNC_EADD  ,    // 120  
       FNC_ESUB  ,    // 121    
       FNC_EMUL  ,    // 122    
       FNC_EDIV  ,    // 123   
       FNC_ERROR ,    // 124    
       FNC_ERROR ,    // 125   
       FNC_ERROR ,    // 126   
       FNC_ESOR  ,    // 127  
       FNC_ERROR ,    // 128  
       FNC_INT   ,    // 129 
       FNC_SIN   ,    // 130   
       FNC_COS   ,    // 131 
       FNC_TAN   ,    // 132 
       FNC_ERROR ,    // 133   
       FNC_ERROR ,    // 134   
       FNC_ERROR ,    // 135   
       FNC_ERROR ,    // 136   
       FNC_ERROR ,    // 137   
       FNC_ERROR ,    // 138   
       FNC_ERROR ,    // 139
       FNC_ERROR ,    // 140   
       FNC_ERROR ,    // 141   
       FNC_ERROR ,    // 142   
       FNC_ERROR ,    // 143   
       FNC_ERROR ,    // 144   
       FNC_ERROR ,    // 145   
       FNC_ERROR ,    // 146   
       FNC_SWAP  ,    // 147
       FNC_ERROR ,    // 148   
       FNC_ERROR ,    // 149
       FNC_ERROR ,    // 150   
       FNC_ERROR ,    // 151   
       FNC_ERROR ,    // 152   
       FNC_ERROR ,    // 153   
       FNC_ERROR ,    // 154   
       FNC_ABS   ,    // 155
       FNC_ZRN   ,    // 156 
       FNC_PLSV  ,    // 157  
       FNC_DRVI  ,    // 158  
       FNC_DRVA  ,    // 159
       FNC_TCMP  ,    // 160
       FNC_TZCP  ,    // 161
       FNC_TADD  ,    // 162  
       FNC_TSUB  ,    // 163  
       FNC_ERROR ,    // 164   
       FNC_ERROR ,    // 165    
       FNC_TRD   ,    // 166 
       FNC_TWR   ,    // 167 
       FNC_ERROR ,    // 168   
       FNC_HOUR  ,    // 169
       FNC_GRY   ,    // 170
       FNC_GBIN  ,    // 171  
       FNC_ERROR ,    // 172   
       FNC_ERROR ,    // 173  
       FNC_ERROR ,    // 174   
       FNC_ERROR ,    // 175  
       FNC_RD3A  ,    // 176  
       FNC_WP3A  ,    // 177  
       FNC_ERROR ,    // 178   
       FNC_ERROR ,    // 179
       FNC_ERROR ,    // 180
       FNC_ERROR ,    // 181   
       FNC_ERROR ,    // 182   
       FNC_ERROR ,    // 183   
       FNC_ERROR ,    // 184   
       FNC_ERROR ,    // 185   
       FNC_ERROR ,    // 186   
       FNC_ERROR ,    // 187   
       FNC_ERROR ,    // 188   
       FNC_ERROR ,    // 189
       FNC_ERROR ,    // 190   
       FNC_ERROR ,    // 191   
       FNC_ERROR ,    // 192   
       FNC_ERROR ,    // 193   
       FNC_ERROR ,    // 194   
       FNC_ERROR ,    // 195  
       FNC_ERROR ,    // 196   
       FNC_ERROR ,    // 197   
       FNC_ERROR ,    // 198   
       FNC_ERROR ,    // 199
       FNC_ERROR ,    // 200   
       FNC_ERROR ,    // 201   
       FNC_ERROR ,    // 202   
       FNC_ERROR ,    // 203   
       FNC_ERROR ,    // 204   
       FNC_ERROR ,    // 205   
       FNC_ERROR ,    // 206   
       FNC_ERROR ,    // 207   
       FNC_ERROR ,    // 208   
       FNC_ERROR ,    // 209
       FNC_ERROR ,    // 210   
       FNC_ERROR ,    // 211   
       FNC_ERROR ,    // 212   
       FNC_ERROR ,    // 213   
       FNC_ERROR ,    // 214   
       FNC_ERROR ,    // 215   
       FNC_ERROR ,    // 216   
       FNC_ERROR ,    // 217   
       FNC_ERROR ,    // 218   
       FNC_ERROR ,    // 219
       FNC_ERROR ,    // 220   
       FNC_ERROR ,    // 221   
       FNC_ERROR ,    // 222   
       FNC_ERROR ,    // 223   
       FNC_LD_EQ ,    // 224
       FNC_LD_GT ,    // 225   
       FNC_LD_LT ,    // 226   
       FNC_ERROR ,    // 227   
       FNC_LD_NE ,    // 228   
       FNC_LD_LE ,    // 229
       FNC_LD_GE ,    // 230   
       FNC_ERROR ,    // 231   
       FNC_AND_EQ,    // 232   
       FNC_AND_GT,    // 233   
       FNC_AND_LT,    // 234   
       FNC_ERROR ,    // 235   
       FNC_AND_NE,    // 236   
       FNC_AND_LE,    // 237  
       FNC_AND_GE,    // 238  
       FNC_ERROR ,    // 239
       FNC_OR_EQ ,    // 240 
       FNC_OR_GT ,    // 241   
       FNC_OR_LT ,    // 242  
       FNC_ERROR ,    // 243  
       FNC_OR_NE ,    // 244   
       FNC_OR_LE ,    // 245   
       FNC_OR_GE ,    // 246   
       FNC_ERROR ,    // 247   
       FNC_ERROR ,    // 248   
       FNC_ERROR ,    // 249
       FNC_ERROR ,    // 250 
       FNC_ERROR ,    // 251 
       FNC_ERROR ,    // 252 
       FNC_ERROR ,    // 253
       FNC_ERROR ,    // 254 
       FNC_ERROR};    // 255

int RD_ADDR(u32t a, u8t c);

//-----------------------------------------------------------------------------------------//
//                                                                                         //
//                                  FNC 指令编程格式描述                                   //
//                                                                                         //
//    函数名称:   FNC_xxx                                                                  //
//    函数类型:   void FNC_xxx(void)                                                       //
//                                                                                         //
//    功能描述：  执行 FNC 扩展命令                                                        //
//                                                                                         //
//    入口参数：  无                                                                       //
//                                                                                         //
//    出口参数：  无                                                                       //
//                                                                                         //
//    全局变量:   addr0   （addr0x.WORD        ）   // 数据位第一字地址/数据缓冲区         //
//                addr0L  （addr0x.BYTES.BYTEL ）   // 数据位第一字地址/数据低位           //
//                addr0H  （addr0x.BYTES.BYTEH ）   // 数据位第一字地址/数据高位           //
//                addr1   （addr1x.WORD        ）   // 数据位第二字地址缓冲区               //
//                addr1L  （addr1x.BYTES.BYTEL ）   // 数据位第二字地址低位                //
//                addr1H  （addr1x.BYTES.BYTEH ）   // 数据位第二字地址高位                //
//                addr2   （addr2x.WORD        ）   // 数据位第三字地址缓冲区               //
//                addr2L  （addr2x.BYTES.BYTEL ）   // 数据位第三字地址低位                //
//                addr2H  （addr2x.BYTES.BYTEH ）   // 数据位第三字地址高位                //
//                addr3   （addr3x.WORD        ）   // 数据位第四字地址缓冲区               //
//                addr3L  （addr3x.BYTES.BYTEL ）   // 数据位第四字地址低位                //
//                addr3H  （addr3x.BYTES.BYTEH ）   // 数据位第四字地址高位                //
//                addr4   （addr4x.WORD        ）   // 数据位第五字地址缓冲区               //
//                addr4L  （addr4x.BYTES.BYTEL ）   // 数据位第五字地址低位                //
//                addr4H  （addr4x.BYTES.BYTEH ）   // 数据位第五字地址高位                //
//                                                                                         //
//                                                                                         //
//-----------------------------------------------------------------------------------------//
//    调用模块：  void RD_ADDR(int a, u8t c)                                     //
//                                                                                         //
//    注    释：  从 PLC 指定的点地址读入数据                                              //
//                                                                                         //
//    入口参数：  int a            点地址                                                  //
//                u8t c  地址格式 m=(c&0xe0)>>4, n=(c&0x1f);                     //
//                                                                                         //
//    出口参数：  int              读入的数据                                              //
//-----------------------------------------------------------------------------------------//
//    调用模块:   void WR_ADDR(int a, int b, u8t c)                              //
//                                                                                         //
//    注    释：  将数据写入 PLC 指定的点地址                                              //
//                                                                                         //
//    入口参数：  int a            点地址                                                  //
//                int b            写入的数据                                              //
//                u8t c  地址格式 m=(c&0xe0)>>4, n=(c&0x1f);                     //
//                                                                                         //
//    出口参数：  无                                                                       //
//-----------------------------------------------------------------------------------------//
//                                                                                         //
//    模块描述：                                                                           //
//                                                                                         //
//                                                                                         //
//    作　  者:   许意义                                                                 //
//    日　  期:   2009年9月26日                                                          //
//    备    注:                                                                          //
//-----------------------------------------------------------------------------------------//
//    修 改 人:                                                                               //
//    日　  期:                                                                               //
//    备    注:                                                                          //
//-----------------------------------------------------------------------------------------//

/******************************************************************************************************/ 
#define   CSP_Pn_MAX    8
#define   CALL_SP_MAX   2

static u8t         CALL_SP_i;
const  u8t         *CALL_SP[CALL_SP_MAX];
const  u8t         *CSP_Pn[CSP_Pn_MAX+1];

u8t       CODE_ERROR;
u32t    PS_BIT;                         //  P 指令16级暂存
u32t    PS1_BIT;                        //  P 指令16级暂存备份
u32t    Pi;                             //  P 指令16级暂存位置指针

/********************************************  程序开始  **********************************************/ 

//-----------------------------------------------------------------------------------------//
//    数组名称:   FNC_CMD_LNE                                                              //
//    数组类型:   u8t code FNC_CMD_LNE[256]                                      //
//                                                                                         //
//    功能描述：  返回单条 FNC指令后面跟随的命令/数据字长度                                //
//                                                                                         //
//    入口参数：  FNC指令代码编号                                                          //
//                                                                                         //
//    出口参数：  返回本条 FNC指令后面跟随的命令/数据字长度                                //
//                                                                                         //
//                                                                                         //
//    作　  者:   许意义                                                                 //
//    日　  期:   2009年9月26日                                                          //
//    备    注:                                                                          //
//-----------------------------------------------------------------------------------------//
//    修 改 人:                                                                               //
//    日　  期:                                                                               //
//    备    注:                                                                          //
//-----------------------------------------------------------------------------------------//

static const u8t FNC_CMD_LNE[256] = {
          1,1,0,0,0,0,0,0,1,0,
          3,4,2,5,2,3,3,2,2,2,
          3,3,3,3,1,1,3,3,3,1,
          2,2,2,2,4,4,4,4,3,3,
          2,3,3,2,3,3,3,0,2,2,
          2,1,4,3,3,4,3,3,3,4,
          3,4,4,4,2,3,1,4,4,5,
          3,4,4,2,3,4,2,2,4,4,
          4,2,3,3,3,2,2,0,4,0,
          0,0,0,0,0,0,0,0,0,0,
          0,0,0,0,0,0,0,0,0,0,
          3,4,0,0,0,0,0,0,2,2,
          3,3,3,3,0,0,0,2,0,2,
          2,2,2,0,0,0,0,0,0,0,
          0,0,0,0,0,0,0,1,0,0,
          0,0,0,0,0,3,4,3,4,4,
          5,4,3,3,0,0,1,1,0,3,
          2,2,0,0,0,0,3,3,0,0,
          0,0,0,0,0,0,0,0,0,0,
          0,0,0,0,0,0,0,0,0,0,
          0,0,0,0,0,0,0,0,0,0,
          0,0,0,0,0,0,0,0,0,0,
          0,0,0,0,2,2,2,0,2,2,
          2,0,2,2,2,0,2,2,2,0,
          2,2,2,0,2,2,2,0,0,0,
          0,0,0,0,0,0       };


//-----------------------------------------------------------------------------------------//
//    函数名称:   ADDR_int_ppp                                                             //
//    函数类型:   char* ADDR_int_ppp(u32t a)                                       //
//                                                                                         //
//    功能描述：  读入并列的n位软元件点的起始地址，返回地址绝对指针                        //
//                                                                                         //
//    入口参数：  u32t  a ...... 读入软元件点的起始地址                            //
//                                                                                         //
//    出口参数：  返回地址绝对指针                                                         //
//                                                                                         //
//    全局变量:   无                                                                    //
//                                                                                         //
//    调用模块:   无                                                                       //
//    模块描述：                                                                           //
//                                                                                         //
//                                                                                         //
//    作　  者:   许意义                                                                 //
//    日　  期:   2009年9月26日                                                          //
//    备    注:                                                                          //
//-----------------------------------------------------------------------------------------//
//    修 改 人:                                                                               //
//    日　  期:                                                                               //
//    备    注:                                                                          //
//-----------------------------------------------------------------------------------------//
/*
u8t* ADDR_int_ppp_1(u32t a) {   // (读入int点内容，返回地址绝对指针)
    u8t *p;

    a &= 0xfff; 
    if (a<0x400) {
        if (a < _S_num) {
            p  = (u8t*)rS + (a / 8);
        }
    }
    else if  (a<0x500) {
        a -= 0x400;
        if (a < _X_num) {
            p = (u8t*)rX + (a / 8);
        }
    }
    else if  (a<0x600) {
        a -= 0x500;
        if (a < _Y_num) {
            p = (u8t*)rY + (a / 8);
        }
    }
    else if  (a<0x800) {
        a -= 0x600;
        if (a < _T_num)
        { p = (u8t*)rT + (a / 8);
        }
    }
  else if  (a<0xe00)
    { a -= 0x800;
      if (a < _M_num)
        { p = (u8t*)rM + (a / 8);
        }
    }
  else if  (a<0xf00)
    { a -= 0xe00;
      if (a < _C_num)
        { p = (u8t*)rC + (a / 8);
        }
    }
  else 
    { a -= 0xf00;
      if (a < _M8xxx_num)
        { p = (u8t*)rM8xxx + (a / 8);
        }
    }
  return(p);
}*/
/*
static const u32t ppptoplcmems[14] = { 
    0x000, _S_num, 
    0x400, 0x400 + _X_num, 
    0x500, 0x500 + _Y_num,
    0x600, 0x600 + _T_num,
    0x800, 0x800 + _M_num,
    0xe00, 0xe00 + _C_num,
    0xf00, 0xf00 + _M8xxx_num
};*/
static const u32t ppptoplcindex[64] = {
    0x000, _S_num,     (u32t)rS,    (u32t)rS1,    //0x000
    0x000, _S_num,     (u32t)rS,    (u32t)rS1,    //0x001
    0x000, _S_num,     (u32t)rS,    (u32t)rS1,    //0x002
    0x000, _S_num,     (u32t)rS,    (u32t)rS1,    //0x003
    0x400, _X_num,     (u32t)rX,    (u32t)rX1,    //0x004
    0x500, _Y_num,     (u32t)rY,    (u32t)rY1,    //0x005
    0x600, _T_num,     (u32t)rT,    (u32t)rT1,    //0x006
    0x600, _T_num,     (u32t)rT,    (u32t)rT1,    //0x007
    0x800, _M_num,     (u32t)rM,    (u32t)rM1,    //0x008
    0x800, _M_num,     (u32t)rM,    (u32t)rM1,    //0x009
    0x800, _M_num,     (u32t)rM,    (u32t)rM1,    //0x00a
    0x800, _M_num,     (u32t)rM,    (u32t)rM1,    //0x00b
    0x800, _M_num,     (u32t)rM,    (u32t)rM1,    //0x00c
    0x800, _M_num,     (u32t)rM,    (u32t)rM1,    //0x00d
    0xe00, _C_num,     (u32t)rC,    (u32t)rC1,    //0x00e
    0xf00, _M8xxx_num, (u32t)rM8xxx,(u32t)rM8xxx1 //0x00f
};
static u8t* _ADDR_int_ppp(u32t a, u32t index) { 
    u32t t;

    a &= 0xfff; 
    t = a >> 8;
    t *= 4;
    a = a - ppptoplcindex[t];
    if (a < ppptoplcindex[t + 1]) {
        a = ppptoplcindex[t + index] + (a / 8);
        return (u8t*)a;
    }
    return &plcnullvar;
}
//static s32t cp_ADDR_int_ppp(void *ds, u32t length, u32t index, u32t cc) {
    //
//}
//-----------------------------------------------------------------------------------------//
//    函数名称:   ADDR_int_ppp_old                                                         //
//    函数类型:   char* ADDR_int_ppp_old(u32t a)                                   //
//                                                                                         //
//    功能描述：  从 old 备份存储器内读入并列的n位软元件点的起始地址，返回地址绝对指针     //
//                                                                                         //
//    入口参数：  u32t  a ...... 读入软元件点的起始地址                            //
//                                                                                         //
//    出口参数：  返回地址绝对指针                                                         //
//                                                                                         //
//    全局变量:   无                                                                    //
//                                                                                         //
//    调用模块:   无                                                                       //
//    模块描述：                                                                           //
//                                                                                         //
//                                                                                         //
//    作　  者:   许意义                                                                 //
//    日　  期:   2009年9月26日                                                          //
//    备    注:                                                                          //
//-----------------------------------------------------------------------------------------//
//    修 改 人:                                                                               //
//    日　  期:                                                                               //
//    备    注:                                                                          //
//-----------------------------------------------------------------------------------------//
/*
char* ADDR_int_ppp_old(u32t a)    // (从 old 备份存储器内读入int点内容，返回地址绝对指针)
{ u8t *p;
  a &= 0xfff; 
  if (a<0x400)
    { if (a < _S_num) 
        { p  = (u8t*)rS1 + (a / 8);
        }
    }
  else if  (a<0x500)
    { a -= 0x400;
      if (a < _X_num)
        { p = (u8t*)rX1 + (a / 8);
        }
    }
  else if  (a<0x600)
    { a -= 0x500;
      if (a < _Y_num)
        { p = (u8t*)rY1 + (a / 8);
        }
    }
  else if  (a<0x800)
    { a -= 0x600;
      if (a < _T_num)
        { p = (u8t*)rT1 + (a / 8);
        }
    }
  else if  (a<0xe00)
    { a -= 0x800;
      if (a < _M_num)
        { p = (u8t*)rM1 + (a / 8);
        }
    }
  else if  (a<0xf00)
    { a -= 0xe00;
      if (a < _C_num)
        { p = (u8t*)rC1 + (a / 8);
        }
    }
  else 
    { a -= 0xf00;
      if (a < _M8xxx_num)
        { p = (u8t*)rM8xxx1 + (a / 8);
        }
    }
  return(p);
}
*/

//-----------------------------------------------------------------------------------------//
//    函数名称:   RD_int_ppp                                                               //
//    函数类型:   u32t RD_int_ppp(u32t a)                                  //
//                                                                                         //
//    功能描述：  将并列的n位软元件点的内容从存储器内读入                                  //
//                                                                                         //
//    入口参数：  u32t  a ...... 读入软元件点的起始地址                            //
//                                                                                         //
//    出口参数：  并列的n位软元件点的内容                                                  //
//                                                                                         //
//    全局变量:   无                                                                    //
//                                                                                         //
//    调用模块:   char* ADDR_int_ppp(u32t a)                                       //
//    模块描述：  读入并列的n位软元件点的起始地址，返回地址绝对指针                        //
//                                                                                         //
//                                                                                         //
//    作　  者:   许意义                                                                 //
//    日　  期:   2009年9月26日                                                          //
//    备    注:                                                                          //
//-----------------------------------------------------------------------------------------//
//    修 改 人:                                                                               //
//    日　  期:                                                                               //
//    备    注:                                                                          //
//-----------------------------------------------------------------------------------------//
/*
u32t RD_int_ppp(u32t a) {   // (读入int点内容)
    u8t *p;
    u8t i, n;
    u32t  Ia;

    p = ADDR_int_ppp(a); 
    n = 0x10;
    i = a % 8;
    Ia = (u32t)(*p++);
    n -= (8-i);
    for (; n>=8; ) {  
        Ia = ((u32t)(*p++) << 8) | Ia; 
        n  -= 8;
    }
    if (n != 0) {
        Ia = ((u32t)(*p++) << (16-n)) | (Ia >> n); 
    }
    else {
      //; 
    }
    return Ia;
}*/

u32t RD_int_ppp(u32t a) {   // (读入int点内容)
    u8t *p;
    //u8t i;//, n;
    u32t  Ia;

    p = ADDR_int_ppp(a); 
    Ia = (u32t)(*p);
    p = ADDR_int_ppp(a + 8); 
    Ia = Ia | (((u32t)(*p)) << 8);
    a %= 8;
    if (a > 0) {
        p = ADDR_int_ppp(a + 16); 
        Ia = Ia | (((u32t)(*p)) << 16);
        Ia >>= a;
        Ia &= 0xffff;
    }
    return Ia;
}

//-----------------------------------------------------------------------------------------//
//    函数名称:   WR_int_ppp                                                               //
//    函数类型:   void WR_int_ppp(u32t a,u32t b,u8t n)           //
//                                                                                         //
//    功能描述：  将并列的n位软元件点的内容写入存储器                                      //
//                                                                                         //
//    入口参数：  u32t  a ...... 写入软元件点的起始地址                            //
//                u32t  b ...... 写入内容                                          //
//                u8t n ...... 写入宽度，n=2,4bit;  n=4,8bit;                    //
//                                                 n=6,12bit; n=8,16bit; 依次类推          //
//                                                                                         //
//    出口参数：  返回写入点地址下一个地址                                                 //
//                                                                                         //
//    全局变量:   无                                                                    //
//                                                                                         //
//    调用模块:   char* ADDR_int_ppp(u32t a)                                       //
//    模块描述：  读入并列的n位软元件点的起始地址，返回地址绝对指针                        //
//                                                                                         //
//                                                                                         //
//    作　  者:   许意义                                                                 //
//    日　  期:   2009年9月26日                                                          //
//    备    注:                                                                          //
//-----------------------------------------------------------------------------------------//
//    修 改 人:                                                                               //
//    日　  期:                                                                               //
//    备    注:                                                                          //
//-----------------------------------------------------------------------------------------//
/*u32t WR_int_ppp(u32t a,u32t b,u8t n) {    // (写入int点内容)
    u8t *p;
    u8t i, j;
    u8t Ca;

    p = ADDR_int_ppp(a); 
    n <<= 1;
    if (n == 0)
    { if ((b&0x01) == 0)   *p &= ~(1 << (a % 8));
               else  *p |= 1 << (a % 8);
      a++;
    }
  else 
    { i = a % 8;
      if (n <= (8-i))
        { Ca = BYTE_BIT_s[i];
          for (j=1; j<n; j++)
            { Ca |= (Ca << 1);
            }
          *p++ = ((*p & ~Ca) | ((u8t)((b<<i) & 0xff) & Ca)); 
          b >>= n;
          a  += n;
          n   = 0;
        }
      else 
        { Ca = BYTE_BIT_w[i];
          *p++ = ((*p & ~Ca) | ((u8t)((b<<i) & 0xff) & Ca)); 
          i   = 8-i;
          b >>= i;
          a  += i;
          n  -= i;
        }
      for (; n>=8; )   
        { *p++ = (u8t)(b & 0xff); 
          b >>= 8;
          a  += 8;
          n  -= 8;
        }
      if (n != 0)
        { Ca = BYTE_BIT_e[n-1];
          *p  = ((*p & ~Ca) | ((u8t)(b & 0xff) & Ca)); 
          a  += n;
        }
      else; 
    }
  return a;
}*/
u32t WR_int_ppp(u32t a,u32t b,u8t n) {    // (写入int点内容)
    u8t *p;
    u8t i, j;
    u8t Ca;

    p = ADDR_int_ppp(a); 
    n <<= 1;
    if (n == 0) {
        if ((b&0x01) == 0) {
            *p &= ~(1 << (a % 8));
        }
        else {
            *p |= 1 << (a % 8);
        }
        a++;
    }
    else {
        i = a % 8;
        if (n <= (8-i)) {
            Ca = BYTE_BIT_s[i];
            for (j=1; j<n; j++)
            { Ca |= (Ca << 1);
            }
            //*p++ = ((*p & ~Ca) | ((u8t)((b<<i) & 0xff) & Ca)); 
            *p = ((*p & ~Ca) | ((u8t)((b<<i) & 0xff) & Ca));
            p++;
            b >>= n;
            a  += n;
            n   = 0;
        }
        else {
            Ca = BYTE_BIT_w[i];
            //*p++ = ((*p & ~Ca) | ((u8t)((b<<i) & 0xff) & Ca)); 
            *p = ((*p & ~Ca) | ((u8t)((b<<i) & 0xff) & Ca)); 
            p++;
            i   = 8-i;
            b >>= i;
            a  += i;
            n  -= i;
        }
        for (; n>=8; ) {  
            *p++ = (u8t)(b & 0xff); 
            b >>= 8;
            a  += 8;
            n  -= 8;
        }
        if (n != 0) {
            Ca = BYTE_BIT_e[n-1];
            *p  = ((*p & ~Ca) | ((u8t)(b & 0xff) & Ca)); 
            a  += n;
        }
    }
    return a;
}

//-----------------------------------------------------------------------------------------//
//    函数名称:   char RD_ppp                                                              //
//    函数类型:   u8t RD_ppp(u32t a)                                     //
//                                                                                         //
//    功能描述：  将1点软元件的内容从存储器内读入                                          //
//                                                                                         //
//    入口参数：  u32t  a ...... 读入软元件点的起始地址                            //
//                                                                                         //
//    出口参数：  1点软元件的内容                                                          //
//                                                                                         //
//    全局变量:   无                                                                    //
//                                                                                         //
//    调用模块:   无                                                                    //
//    模块描述：                                                                        //
//                                                                                         //
//                                                                                         //
//    作　  者:   许意义                                                                 //
//    日　  期:   2009年9月26日                                                          //
//    备    注:                                                                          //
//-----------------------------------------------------------------------------------------//
//    修 改 人:                                                                               //
//    日　  期:                                                                               //
//    备    注:                                                                          //
//-----------------------------------------------------------------------------------------//

u8t RD_ppp(u32t a) {   // (读入点内容)
    u8t n;
    u8t *p;

    p = ADDR_int_ppp(a); 
    n = *p >> (a % 8);
    return (n & 0x01);
}

//-----------------------------------------------------------------------------------------//
//    函数名称:   char RD_Mp                                                               //
//    函数类型:   u8t RD_Mp(u32t a)                                      //
//                                                                                         //
//    功能描述：  将1点软元件的内容从扩展 Mp 存储器内读入                                  //
//                                                                                         //
//    入口参数：  u32t  a ...... 读入软元件点的起始地址                            //
//                                                                                         //
//    出口参数：  1点软元件的内容                                                          //
//                                                                                         //
//    全局变量:   无                                                                    //
//                                                                                         //
//    调用模块:   无                                                                    //
//    模块描述：                                                                        //
//                                                                                         //
//                                                                                         //
//    作　  者:   许意义                                                                 //
//    日　  期:   2009年9月26日                                                          //
//    备    注:                                                                          //
//-----------------------------------------------------------------------------------------//
//    修 改 人:                                                                               //
//    日　  期:                                                                               //
//    备    注:                                                                          //
//-----------------------------------------------------------------------------------------//
u8t RD_Mp(u32t a)    // (读入扩展Mp存储器内的点内容)
{ u8t n=0;
  return(n & 0x01);
}

//-----------------------------------------------------------------------------------------//
//    子函数名：  u8t RD_ppp_old(u32t a)                                 //
//                                                                                         //
//    注    释：  将1点软元件的内容从 old 备份存储器内读入                                 //
//                                                                                         //
//    入口参数：  u32t  a ...... 读入软元件点的起始地址                            //
//    出口参数：  1点软元件的内容                                                          //
//-----------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------//
//    函数名称:   char RD_ppp                                                              //
//    函数类型:   u8t RD_ppp(u32t a)                                     //
//                                                                                         //
//    功能描述：  将1点软元件的内容从存储器内读入                                          //
//                                                                                         //
//    入口参数：  u32t  a ...... 读入软元件点的起始地址                            //
//                                                                                         //
//    出口参数：  1点软元件的内容                                                          //
//                                                                                         //
//    全局变量:   无                                                                    //
//                                                                                         //
//    调用模块:   无                                                                    //
//    模块描述：                                                                        //
//                                                                                         //
//                                                                                         //
//    作　  者:   许意义                                                                 //
//    日　  期:   2009年9月26日                                                          //
//    备    注:                                                                          //
//-----------------------------------------------------------------------------------------//
//    修 改 人:                                                                               //
//    日　  期:                                                                               //
//    备    注:                                                                          //
//-----------------------------------------------------------------------------------------//
u8t RD_ppp_old(u32t a)    // (读入点内容)
{ u8t n;
  u8t *p;
  p = ADDR_int_ppp_old(a); 
  n = *p >> (a % 8);
  return(n & 0x01);
}

//-----------------------------------------------------------------------------------------//
//    子函数名：  u8t RD_Mp_old(u32t a)                                  //
//                                                                                         //
//    注    释：  将1点软元件的内容从扩展 Mp 存储器 old 备份内读入                         //
//                                                                                         //
//    入口参数：  u32t  a ...... 读入软元件点的起始地址                            //
//    出口参数：  1点软元件的内容                                                          //
//-----------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------//
//    函数名称:   char RD_ppp                                                              //
//    函数类型:   u8t RD_ppp(u32t a)                                     //
//                                                                                         //
//    功能描述：  将1点软元件的内容从存储器内读入                                          //
//                                                                                         //
//    入口参数：  u32t  a ...... 读入软元件点的起始地址                            //
//                                                                                         //
//    出口参数：  1点软元件的内容                                                          //
//                                                                                         //
//    全局变量:   无                                                                    //
//                                                                                         //
//    调用模块:   无                                                                    //
//    模块描述：                                                                        //
//                                                                                         //
//                                                                                         //
//    作　  者:   许意义                                                                 //
//    日　  期:   2009年9月26日                                                          //
//    备    注:                                                                          //
//-----------------------------------------------------------------------------------------//
//    修 改 人:                                                                               //
//    日　  期:                                                                               //
//    备    注:                                                                          //
//-----------------------------------------------------------------------------------------//
u8t RD_Mp_old(u32t a)    // (读入点内容)
{ u8t n=0;
  return(n & 0x01);
}


//-----------------------------------------------------------------------------------------//
//    函数名称:   WR_YM                                                                    //
//    函数类型:   void WR_YM(u32t a,u8t i)                               //
//                                                                                         //
//    功能描述：  将1点软元件的内容写入Y,M存储器                                           //
//                                                                                         //
//    入口参数：  u32t  a ...... 读入软元件点的起始地址                            //
//                                                                                         //
//    出口参数：  无                                                                       //
//                                                                                         //
//    全局变量:   无                                                                    //
//                                                                                         //
//    调用模块:   无                                                                    //
//    模块描述：                                                                        //
//                                                                                         //
//                                                                                         //
//    作　  者:   许意义                                                                 //
//    日　  期:   2009年9月26日                                                          //
//    备    注:                                                                          //
//-----------------------------------------------------------------------------------------//
//    修 改 人:                                                                               //
//    日　  期:                                                                               //
//    备    注:                                                                          //
//-----------------------------------------------------------------------------------------//
/*
static void WR_YM(u32t a, u8t i) {   // (写入Y,M点内容)
    u8t *p;
    a &= 0xfff;
    i &= 0x01; 
    if ((a>=0x500) && (a<0x600)) {
        a -= 0x500;
        if (a < _Y_num)
           { p = (u8t*)rY + (a / 8);
             if (i == 0)   *p &= ~(1 << (a % 8));
               else  *p |= 1 << (a % 8);
           }
    }
    else if ((a>=0x800) && (a<0xe00)) {
        a -= 0x800;
        if (a < _M_num)
           { p = (u8t*)rM + (a / 8);
             if (i == 0)   *p &= ~(1 << (a % 8));
               else  *p |= 1 << (a % 8);
           }
    }
}*/
    
static void WR_YM_SET(u32t a) {
    u8t *p;

    p = ADDR_int_ppp(a);
    a = a % 8;
    a = 1 << a;
    *p = *p | a;
}
static void WR_YM_CLR(u32t a) {
    u8t *p;

    p = ADDR_int_ppp(a);
    a = a % 8;
    a = 1 << a;
    a = ~a;
    *p = *p & a;
}
static void WR_YM(u32t a, u8t i) {   // (写入Y,M点内容)
    if (i) {
        WR_YM_SET(a);
    }
    else {
        WR_YM_CLR(a);
    }
}
//-----------------------------------------------------------------------------------------//
//    函数名称:   WR_S                                                                     //
//    函数类型:   void WR_S(u32t a,u8t i)                                //
//                                                                                         //
//    功能描述：  将1点软元件的内容写入S存储器                                             //
//                                                                                         //
//    入口参数：  u32t  a ...... 读入软元件点的起始地址                            //
//                                                                                         //
//    出口参数：  无                                                                       //
//                                                                                         //
//    全局变量:   无                                                                    //
//                                                                                         //
//    调用模块:   无                                                                    //
//    模块描述：                                                                        //
//                                                                                         //
//                                                                                         //
//    作　  者:   许意义                                                                 //
//    日　  期:   2009年9月26日                                                          //
//    备    注:                                                                          //
//-----------------------------------------------------------------------------------------//
//    修 改 人:                                                                               //
//    日　  期:                                                                               //
//    备    注:                                                                          //
//-----------------------------------------------------------------------------------------//

void WR_S(u32t a,u8t i)    // (写入S点内容)
{ u8t *p;
  a &= 0xfff;
  i &= 0x01; 
  if (a<0x400)  
    { 
        if (a < _S_num)
           { p = (u8t*)rS + (a / 8);
             if (i == 0)   *p &= ~(1 << (a % 8));
               else  *p |= 1 << (a % 8);
           }
    }
  else ;
}


//-----------------------------------------------------------------------------------------//
//    函数名称:   WR_M8xxx                                                                 //
//    函数类型:   void WR_M8xxx(u32t a,u8t i)                            //
//                                                                                         //
//    功能描述：  将1点软元件的内容写入M8xxx存储器                                         //
//                                                                                         //
//    入口参数：  u32t  a ...... 读入软元件点的起始地址                            //
//                                                                                         //
//    出口参数：  无                                                                       //
//                                                                                         //
//    全局变量:   无                                                                    //
//                                                                                         //
//    调用模块:   无                                                                    //
//    模块描述：                                                                        //
//                                                                                         //
//                                                                                         //
//    作　  者:   许意义                                                                 //
//    日　  期:   2009年9月26日                                                          //
//    备    注:                                                                          //
//-----------------------------------------------------------------------------------------//
//    修 改 人:                                                                               //
//    日　  期:                                                                               //
//    备    注:                                                                          //
//-----------------------------------------------------------------------------------------//

void WR_M8xxx(u32t a,u8t i)    // (写入M8xxx点内容)
{ u8t *p;
  a &= 0xfff;
  i &= 0x01; 
  if (a>=0xf00)                       // 待修改地址映像
    { 
        a -= 0xf00;
        if (a < _M8xxx_num)
           { p = (u8t*)rM8xxx + (a / 8);
             if (i == 0)   *p &= ~(1 << (a % 8));
               else  *p |= 1 << (a % 8);
           }
    }
  else ;
}


//-----------------------------------------------------------------------------------------//
//    函数名称:   WR_Mp                                                                    //
//    函数类型:   void WR_Mp(u32t a,u8t i)                               //
//                                                                                         //
//    功能描述：  将1点软元件的内容写入Mp(扩展M)存储器                                     //
//                                                                                         //
//    入口参数：  u32t  a ...... 读入软元件点的起始地址                            //
//                                                                                         //
//    出口参数：  无                                                                       //
//                                                                                         //
//    全局变量:   无                                                                    //
//                                                                                         //
//    调用模块:   无                                                                    //
//    模块描述：                                                                        //
//                                                                                         //
//                                                                                         //
//    作　  者:   许意义                                                                 //
//    日　  期:   2009年9月26日                                                          //
//    备    注:                                                                          //
//-----------------------------------------------------------------------------------------//
//    修 改 人:                                                                               //
//    日　  期:                                                                               //
//    备    注:                                                                          //
//-----------------------------------------------------------------------------------------//

void WR_Mp(u32t a,u8t i)    // (写入Mp点内容)
{ ;
}


//-----------------------------------------------------------------------------------------//
//    函数名称:   WR_TK                                                                    //
//    函数类型:   void WR_TK(u32t a)                                               //
//                                                                                         //
//    功能描述：  将 K 值写入定时器 T                                                      //
//                                                                                         //
//    入口参数：  u32t  a ...... 写入软元件 T 的起始地址，                         //
//                                       *CODE_p 指针指向 K 值                             //
//                                                                                         //
//    出口参数：  无                                                                       //
//                                                                                         //
//    全局变量:   无                                                                    //
//                                                                                         //
//    调用模块:   无                                                                    //
//    模块描述：                                                                        //
//                                                                                         //
//                                                                                         //
//    作　  者:   许意义                                                                 //
//    日　  期:   2009年9月26日                                                          //
//    备    注:                                                                          //
//-----------------------------------------------------------------------------------------//
//    修 改 人:                                                                               //
//    日　  期:                                                                               //
//    备    注:                                                                          //
//-----------------------------------------------------------------------------------------//

void WR_TK(u32t a) {      // ,(K值写入T)
    u8t i;
    u8t *p;
    u32t  *Ip;

    addr0L = *CODE_p;
    CODE_p++;
    addr0T = (*CODE_p << 4) & 0xe0;
    CODE_p++;
    addr0H = *CODE_p;
    CODE_p++;
    addr0T |= (*CODE_p & 0x1f);
    CODE_p++;
    if (a < _T_num) {
        i = 1 <<(a % 8);
        p = (u8t*)rTF + (a / 8);
        //Ip = (u32t  *)_T + a;
        Ip = (u32t  *)(&_T[a]);//by ykb
        if ((ACC_BIT & 0x01) != 0) {   // _Tx, _Tx_F, _TKx, K
            if ((*p & i) == 0) {
                *p |= i;              // rTF相应位置1
                *Ip++ = 0; 
                *Ip = RD_ADDR(addr0, addr0T); 
            } 
        }
        else {
            *p &= ~i;             // rTF相应位清0
            *Ip = 0;
        }
    }
}


//-----------------------------------------------------------------------------------------//
//    函数名称:   WR_CK                                                                    //
//    函数类型:   void WR_CK(u32t a)                                               //
//                                                                                         //
//    功能描述：  将 K 值写入定时器 C                                                      //
//                                                                                         //
//    入口参数：  u32t  a ...... 写入软元件 C 的起始地址，                         //
//                                       *CODE_p 指针指向 K 值                             //
//                                                                                         //
//    出口参数：  无                                                                       //
//                                                                                         //
//    全局变量:   无                                                                    //
//                                                                                         //
//    调用模块:   无                                                                    //
//    模块描述：                                                                        //
//                                                                                         //
//                                                                                         //
//    作　  者:   许意义                                                                 //
//    日　  期:   2009年9月26日                                                          //
//    备    注:                                                                          //
//-----------------------------------------------------------------------------------------//
//    修 改 人:                                                                               //
//    日　  期:                                                                               //
//    备    注:                                                                          //
//-----------------------------------------------------------------------------------------//

void WR_CK(u32t a)       // ,(K值写入C)
{ u8t i;
  u32t  Ia;
  u8t *p;
  u32t  *Ip;
  addr0L = *CODE_p;
  CODE_p++;
  addr0T = (*CODE_p << 4) & 0xe0;
  CODE_p++;
  addr0H = *CODE_p;
  CODE_p++;
  addr0T |= (*CODE_p & 0x1f);
  CODE_p++;
  if (a < _C_num)
    { i = 1 <<(a % 8);
      p = (u8t*)rCF + (a / 8);
      Ip = _C + a;
      if ((ACC_BIT & 0x01) != 0)       // _Tx, _Tx_F, _TKx, K
        { if ((*p & i) == 0)
            { Ia = RD_ADDR(addr0, addr0T); 
              if (*Ip < Ia)
                { (*Ip)++;  
                }
              *p |= i;                 // rCF相应位置1
              p = (u8t*)rC + (a / 8);
              if (*Ip >= Ia)
                { *p |= i;         // rC相应位置1
                } 
              else
                { *p &= ~i;        // rC相应位清0
                }
            } 
          else  ;
        }
      else 
        { *p &= ~i;            // rCF相应位清0   
        }
    }
}


/**********************************************  纯单字指令  ******************************************/ 

static void END(void) {     // (END指令, 000F )  
    CODE_p = (const u8t *)CODE_START; 
    Pi = 0x01;
}

static void ANB(void) {     // (ANB指令, FFF8 )  
    u32t a;

    a = ACC_BIT & 0x01;
    ACC_BIT >>= 1;
    ACC_BIT  &= (a | 0xfe);
}

static void ORB(void) {     // (ORB指令, FFF9 )  
    u32t a;

    a = ACC_BIT & 0x01;
    ACC_BIT >>= 1;
    ACC_BIT  |= a;
}

static void MPS(void) {     // (MPS指令, FFFA )  
    MPS_BIT = (MPS_BIT << 1) | (ACC_BIT & 0x01);
}

static void MRD(void)      // (MRD指令, FFFB )  
{ ACC_BIT = (ACC_BIT & 0xfe) | (MPS_BIT & 0x01);
}

static void MPP(void)      // (MPP指令, FFFC ) 
{ ACC_BIT = (ACC_BIT & 0xfe) | (MPS_BIT & 0x01);
  MPS_BIT >>= 1;
}

static void INV(void)      // (INV指令, FFFD )  
{ ACC_BIT = (ACC_BIT & 0xfe) | (~ACC_BIT & 0x01);
}

static void NOP(void) {      // (NOP指令, FFFF ) 
}
static void NULLCMD(void) {
}
/*********************************************  扩展的基本指令  ***************************************/ 

static void OUTM8xxx(void) {   // (OUT指令, 0002 8000+ppp ；仅对M8xxx有效) 
    WR_M8xxx(ppp,ACC_BIT); 
}

static void OUTMp(void)    // (OUT指令, 0002 A000+ppp ；仅对Mp有效)  
{ WR_Mp(ppp,ACC_BIT);
}

static void SETM8xxx(void)    // (SET指令, 0003 8000+ppp ；仅对M8xxx有效) 
{ if ((ACC_BIT & 0x01) != 0)  WR_M8xxx(ppp,1); 
}

static void SETMp(void)    // (SET指令, 0003 A000+ppp ；仅对Mp有效)  
{ if ((ACC_BIT & 0x01) != 0)  WR_Mp(ppp,1); 
}

static void RSTM8xxx(void)    // (RST指令, 0004 8000+ppp ；仅对M8xxx有效) 
{ if ((ACC_BIT & 0x01) != 0)  WR_M8xxx(ppp,0); 
}

static void RSTMp(void)    // (RST指令, 0004 A000+ppp ；仅对Mp有效)  
{ if ((ACC_BIT & 0x01) != 0)  WR_Mp(ppp,0); 
}

static void OUTS(void)    // (OUT指令, 0005 8000+ppp ；仅对S有效) 
{ WR_S(ppp,ACC_BIT); 
}

static void SETS(void)    // (SET指令, 0006 8000+ppp ；仅对S有效)  
{ if ((ACC_BIT & 0x01) != 0)  WR_S(ppp,1);
}

static void RSTS(void)    // (RST指令, 0007 8000+ppp ；仅对S有效) 
{ if ((ACC_BIT & 0x01) != 0)  WR_S(ppp,0);
}

static void PLS(void) {   // (PLS指令, 0008 8000+ppp ；仅对Y，M有效) 
    //u8t Ca;
    if ((ACC_BIT & 0x01) != 0) 
    { PS_BIT |= Pi;
      if ((PS1_BIT & Pi) == 0)  WR_YM(ppp, 1);
        else  WR_YM(ppp, 0);
    }
    else
    { PS_BIT &= ~Pi; 
      WR_YM(ppp, 0);
    }
    Pi <<= 1; 
}

static void PLF(void) {   // (PLF指令, 0009 8000+ppp ；仅对Y，M有效)  
    //u8t Ca;
  if ((ACC_BIT & 0x01) != 0) 
    { PS_BIT |= Pi; 
      WR_YM(ppp, 0);
    }
  else
    { PS_BIT &= ~Pi;
      if ((PS1_BIT & Pi) != 0)  WR_YM(ppp, 1);
        else  WR_YM(ppp, 0);
    }
  Pi <<= 1; 
}

static void MC(void)    // (MC指令, 000A 8000+(N) 8000+ppp ；仅对Y，M有效)  
{ ; 
}

static void MCR(void)    // (MCR指令, 000B 8000+(N) ； N=0-7 
{ ; 
}

static void RSTTC(void)    // (RST指令, 000C 8000+ppp ；仅对T，C，Cp有效) 
{ u8t *p;
  u32t a;
  if ((ACC_BIT & 0x01) != 0) 
    { a = ppp & 0xfff; 
      if ((a>=0x600) && (a<0x800))
        { a -= 0x600;
          if (a < _T_num)
            { p = (u8t*)rT + (addr0 / 8);
              _T[a][0] = 0; 
              *p &= ~(1 <<(addr0 % 8));                 // rT相应位清0
            }
        }
      else if ((a>=0xe00) && (a<0xf00))
        { a -= 0xe00;
          if (a < _C_num)
            { p = (u8t*)rC + (addr0 / 8);
              _C[a] = 0; 
              *p &= ~(1 <<(addr0 % 8));                 // rC相应位清0
            }
        }
      else ;
    }
}

static void RSTD(void)    // (RST指令, 000D 8m00+xx  8n00+yy ；对 D, T, C 有效，包含Z，V) 
{ u8t *p;
  u32t  *Ip;
  addr0L = *CODE_p;
  CODE_p++;
  addr0T = (*CODE_p << 4) & 0xe0;
  CODE_p++;
  addr0H = *CODE_p;
  CODE_p++;
  addr0T |= (*CODE_p & 0x1f);
  CODE_p++;
  addr0 >>= 1;
  if ((ACC_BIT & 0x01) != 0)
    { if ((addr0 < _T_num) && (addr0T == 0x62))     // T0-T256, m=6 ，n=2 ；yy xx = Tx * 2
        { p = (u8t*)rT + (addr0 / 8);
          Ip = (u32t  *)_T + addr0;
          *p &= ~(1 <<(addr0 % 8));                 // rT相应位清0
          *Ip = 0;
        }
      else if ((addr0 < _C_num) && (addr0T == 0x64))    // C0-C234, m=6 ，n=4 ；yy xx = Cx * 2
        { p = (u8t*)rC + (addr0 / 8);
          Ip = _C + addr0;
          *p &= ~(1 <<(addr0 % 8));                  // rC相应位清0
          *Ip = 0;
        }
      else if ((addr0 < _D_num) && (addr0T == 0x66))    // D0-D999, m=6 ，n=6 ；yy xx = Dx * 2
        { Ip = _D + addr0;
          *Ip = 0;
        }
      else ;
    }
}

static void LDP(void) {   // (LDP指令, 01CA 8000+ppp ；扩展 Mp除外)   
    ACC_BIT <<= 1;
    ACC_BIT |= ((RD_ppp(ppp) & ~RD_ppp_old(ppp)) & 0x01);
}

static void LDPMp(void)    // (LDP指令, 01CA A000+ppp ；仅对 Mp有效)   
{ ACC_BIT <<= 1;
  ACC_BIT |= ((RD_Mp(ppp) & ~RD_Mp_old(ppp)) & 0x01);
}

static void LDF(void)    // (LDF指令, 01CB 8000+ppp ；扩展 Mp除外)     
{ ACC_BIT <<= 1;
  ACC_BIT |= ((~RD_ppp(ppp) & RD_ppp_old(ppp)) & 0x01); 
}

static void LDFMp(void)    // (LDF指令, 01CB A000+ppp ；仅对 Mp有效)    
{ ACC_BIT <<= 1;
  ACC_BIT |= ((~RD_Mp(ppp) & RD_Mp_old(ppp)) & 0x01);  
}

static void ANDP(void)    // (ANDP指令, 01CC 8000+ppp ；扩展 Mp除外)     
{ ACC_BIT &= ((RD_ppp(ppp) & ~RD_ppp_old(ppp)) | 0xfe); 
}

static void ANDPMp(void)    // (ANDP指令, 01CC A000+ppp ；仅对 Mp有效)     
{ ACC_BIT &= ((RD_Mp(ppp) & ~RD_Mp_old(ppp)) | 0xfe);  
}

static void ANDF(void)    // (ANDF指令, 01CD 8000+ppp ；扩展 Mp除外)      
{ ACC_BIT &= ((~RD_ppp(ppp) & RD_ppp_old(ppp)) | 0xfe);
}

static void ANDFMp(void)    // (ANDF指令, 01CD A000+ppp ；仅对 Mp有效)      
{ ACC_BIT &= ((~RD_Mp(ppp) & RD_Mp_old(ppp)) | 0xfe); 
}

static void ORP(void)    // (ORP指令, 01CE 8000+ppp ；扩展 Mp除外)     
{ ACC_BIT |= ((RD_ppp(ppp) & ~RD_ppp_old(ppp)) & 0x01);
}

static void ORPMp(void)    // (ORP指令, 01CE A000+ppp ；仅对 Mp有效)     
{ ACC_BIT |= ((RD_Mp(ppp) & ~RD_Mp_old(ppp)) & 0x01); 
}

static void ORF(void)    // (ORF指令, 01CF 8000+ppp ；扩展 Mp除外)     
{ ACC_BIT |= ((~RD_ppp(ppp) & RD_ppp_old(ppp)) & 0x01); 
}

static void ORFMp(void)    // (ORF指令, 01CF A000+ppp ；仅对 Mp有效)     
{ ACC_BIT |= ((~RD_Mp(ppp) & RD_Mp_old(ppp)) & 0x01);  
}


/******************************************  FNC 指令 散转子函数  *************************************/ 

//-----------------------------------------------------------------------------------------//
//    子函数名：  void RD_ADDR(u32t a, u8t c)                            //
//                                                                                         //
//    注    释：  从 PLC 指定的点地址读入数据                                              //
//                                                                                         //
//    入口参数：  int a            点地址                                                  //
//                u8t c  地址格式 m=(c&0xe0)>>4, n=(c&0x1f);                     //
//                                                                                         //
//    出口参数：  int              读入的数据                                              //
//-----------------------------------------------------------------------------------------//


int RD_ADDR(u32t a, u8t c) { 
    signed int Ia;
    signed int *Ip;
    if ((c & 0xe0) == 0x00)                //  m=0, K 以十六进制数表示读入 
    { Ia = a;                            
    }
    else if ((c & 0xe0) == 0x20)           //  m=2, H 十六进制数表示读入 
    { Ia = a;
    }
    else if ((c & 0xe0) == 0x40)           // 位(点)元件地址, m=4
    { if ((c & 0x1f) == 0x00)            // bit点, n=0, 实际地址=ppp 
        { Ia = (int)RD_ppp(a);
        }
      else if ((c & 0x1f) == 0x02)       // int点, 4bit，n=2, 实际地址=ppp 
        { Ia = RD_int_ppp(a) & 0x000f;
        }
      else if ((c & 0x1f) == 0x04)       // int点, 8bit，n=4, 实际地址=ppp 
        { Ia = RD_int_ppp(a) & 0x00ff;
        }
      else if ((c & 0x1f) == 0x06)       // int点, 12bit，n=6, 实际地址=ppp 
        { Ia = RD_int_ppp(a) & 0x0fff;
        }
      else if ((c & 0x1f) == 0x08)       // int点, 16bit，n=8, 实际地址=ppp 
        { Ia = RD_int_ppp(a);
        }
      else ;
    }
    else if ((c & 0xe0) == 0x60) {          // m=6
        if ((c & 0x1f) == 0x02) {           // n=2, T0-T256, 实际地址=Tx * 2 
            Ip = (signed int *)((u32t  *)_T + (a >> 1));
            Ia = *Ip;
        }
        else if ((c & 0x1f) == 0x04) {      // n=4, C0-C234, 实际地址=Cx * 2 
            Ip = (signed int *)(_C + (a >> 1));
            Ia = *Ip;
        }
        else if ((c & 0x1f) == 0x06) {      // n=6, D0-D999, 实际地址=Dx * 2 
            Ip = (signed int *)(_D + (a >> 1));
            Ia = *Ip;
        }
        else ;
    }
    else if ((c & 0xe0) == 0x80)           // m=8, n=0
    { Ia = a >> 1;                       //             实际地址=Px * 2 
    }
    else ;
    return(Ia);
}

//-----------------------------------------------------------------------------------------//
//    子函数名：  void WR_ADDR(u32t a, int b, u8t c)                     //
//                                                                                         //
//    注    释：  将数据写入 PLC 指定的点地址                                              //
//                                                                                         //
//    入口参数：  int a            点地址                                                  //
//                int b            写入的数据                                              //
//                u8t c  地址格式 m=(c&0xe0)>>4, n=(c&0x1f);                     //
//                                                                                         //
//    出口参数：  返回写入点地址下一个地址                                                 //
//-----------------------------------------------------------------------------------------//


u32t WR_ADDR(u32t a, int b, u8t c) {
    signed int *Ip;
    if ((c & 0xe0) == 0x40)           // 位(点)元件地址, m=4
    { a = WR_int_ppp(a, b, c & 0x1f);
    }
    else if ((c & 0xe0) == 0x60)           // m=6
    { 
        if ((c & 0x1f) == 0x02) {           // n=2, T0-T256, 实际地址=Tx * 2 
            Ip  = (signed int *)((u32t  *)_T + (a >> 1));
            *Ip = b;
            a  += 2;
        }
        else if ((c & 0x1f) == 0x04) {      // n=4, C0-C234, 实际地址=Cx * 2 
            Ip  = (signed int *)(_C + (a >> 1));
            *Ip = b;
            a  += 2;
        }
        else if ((c & 0x1f) == 0x06) {      // n=6, D0-D999, 实际地址=Dx * 2 
            Ip  = (signed int *)(_D + (a >> 1));
            *Ip = b;
            a  += 2;
        }
        else ;
    }
    else ;
    return (a);
}

/*******************************************  FNC 指令 子函数  ****************************************/ 

static void FNC_ADD(void) {
    signed int Ia;
    if ((ACC_BIT & 0x01) != 0) {
        Ia =  RD_ADDR(addr0, addr0T) + RD_ADDR(addr1, addr1T); 
        _M8022_ = CY;
        if (Ia == 0) { 
            _M8020_ = 1; 
        }
        else { 
            _M8020_ = 0; 
        }
      WR_ADDR(addr2, Ia, addr2T);
    }
}
 
static void FNC_SUB(void) {
    signed int Ia;
    if ((ACC_BIT & 0x01) != 0) {
        Ia =  RD_ADDR(addr0, addr0T) - RD_ADDR(addr1, addr1T); 
        _M8021_ = CY;
        if (Ia == 0) { 
            _M8020_ = 1; 
        }
        else { 
            _M8020_ = 0; 
        }
        WR_ADDR(addr2, Ia, addr2T);
    }
}

static void FNC_MUL(void)
{ signed int Ia;
  if ((ACC_BIT & 0x01) != 0) 
    { Ia =  RD_ADDR(addr0, addr0T) * RD_ADDR(addr1, addr1T); 
      WR_ADDR(addr2, Ia, addr2T);
    }
}

static void FNC_DIV(void)
{ signed int Ia;
  if ((ACC_BIT & 0x01) != 0) 
    { Ia =  RD_ADDR(addr0, addr0T) / RD_ADDR(addr1, addr1T); 
      WR_ADDR(addr2, Ia, addr2T);
    }
}

static void FNC_INC(void) 
{ signed int Ia;
  if ((ACC_BIT & 0x01) != 0) 
    { Ia = RD_ADDR(addr0, addr0T); 
      WR_ADDR(addr0, ++Ia, addr0T);
    }
}

static void FNC_DEC(void) 
{ signed int Ia;
  if ((ACC_BIT & 0x01) != 0) 
    { Ia = RD_ADDR(addr0, addr0T); 
      WR_ADDR(addr0, --Ia, addr0T);
    }
}

static void FNC_WAND(void)
{ signed int Ia;
  if ((ACC_BIT & 0x01) != 0) 
    { Ia =  RD_ADDR(addr0, addr0T) & RD_ADDR(addr1, addr1T); 
      WR_ADDR(addr2, Ia, addr2T);
    }
}

static void FNC_WOR(void)
{ signed int Ia;
  if ((ACC_BIT & 0x01) != 0) 
    { Ia =  RD_ADDR(addr0, addr0T) | RD_ADDR(addr1, addr1T); 
      WR_ADDR(addr2, Ia, addr2T);
    }
}

static void FNC_WXOR(void)
{ signed int Ia;
  if ((ACC_BIT & 0x01) != 0) 
    { Ia =  RD_ADDR(addr0, addr0T) ^ RD_ADDR(addr1, addr1T); 
      WR_ADDR(addr2, Ia, addr2T);
    }
}

static void FNC_NEG(void)
{ signed int Ia;
  if ((ACC_BIT & 0x01) != 0) 
    { Ia = ~RD_ADDR(addr0, addr0T) + 1;
      WR_ADDR(addr1, Ia, addr1T);
    }
}

static void FNC_ALT(void)
{ u8t i, Ca=0x01;
  u8t *p;
  if ((ACC_BIT & 0x01) != 0) 
    { p = ADDR_int_ppp(addr0);
      i = addr0 % 8;
      for ( ; i!=0; i--)
        { Ca <<= 1;
         }
      *p = ((*p & ~Ca) | (~(*p) & Ca));
    }
}

static void FNC_MOV(void)
{ signed int Ia;
  if ((ACC_BIT & 0x01) != 0) 
    { Ia = RD_ADDR(addr0, addr0T);
      WR_ADDR(addr1, Ia, addr1T);
    }
}

static void FNC_CML(void)
{ signed int Ia;
  if ((ACC_BIT & 0x01) != 0) 
    { Ia = ~RD_ADDR(addr0, addr0T);
      WR_ADDR(addr1, Ia, addr1T);
    }
}

static void FNC_XCH(void)
{ signed int Ia;
  if ((ACC_BIT & 0x01) != 0)
    { Ia     = RD_ADDR(addr0, addr0T);
      WR_ADDR(addr0, RD_ADDR(addr1, addr1T), addr0T);
      WR_ADDR(addr1, Ia, addr1T);
    }
}

static void FNC_BCD(void)
{ signed int Ia, Ic;
  if ((ACC_BIT & 0x01) != 0)
    { Ic = RD_ADDR(addr0, addr0T);
      Ia   = (Ic / 1000) << 12;
      Ic  %= 1000;
      Ia  |= (Ic / 100 ) << 8;
      Ic  %= 100;
      Ia  |= (Ic / 10 ) << 4;
      Ic  %= 10;
      Ia  |=  Ic;
      WR_ADDR(addr1, Ia, addr1T);
    }
}

static void FNC_BIN(void)
{ signed int Ia, Ic;
  if ((ACC_BIT & 0x01) != 0)
    { Ic = RD_ADDR(addr0, addr0T);
      Ia   = ((Ic >> 12) & 0x0f) * 1000;
      Ia  += ((Ic >> 8 ) & 0x0f) * 100;
      Ia  += ((Ic >> 4 ) & 0x0f) * 10;
      Ia  +=   Ic        & 0x0f;
      WR_ADDR(addr1, Ia, addr1T);
    }
}

static void FNC_LD_GE(void)
{ ACC_BIT <<= 1;
  if (RD_ADDR(addr0, addr0T) >= RD_ADDR(addr1, addr1T))  { ACC_BIT |= 0x01; }
    else  { ACC_BIT &= 0xfe; }
}

static void FNC_LD_LE(void)
{ ACC_BIT <<= 1;
  if (RD_ADDR(addr0, addr0T) <= RD_ADDR(addr1, addr1T))  { ACC_BIT |= 0x01; }
    else  { ACC_BIT &= 0xfe; }
}

static void FNC_LD_GT(void)
{ ACC_BIT <<= 1;
  if (RD_ADDR(addr0, addr0T) > RD_ADDR(addr1, addr1T))  { ACC_BIT |= 0x01; }
    else  { ACC_BIT &= 0xfe; }
}

static void FNC_LD_LT(void)
{ ACC_BIT <<= 1;
  if (RD_ADDR(addr0, addr0T) < RD_ADDR(addr1, addr1T))  { ACC_BIT |= 0x01; }
    else  { ACC_BIT &= 0xfe; }
}

static void FNC_LD_NE(void)
{ ACC_BIT <<= 1;
  if (RD_ADDR(addr0, addr0T) != RD_ADDR(addr1, addr1T))  { ACC_BIT |= 0x01; }
    else  { ACC_BIT &= 0xfe; }
}

static void FNC_LD_EQ(void)
{ ACC_BIT <<= 1;
  if (RD_ADDR(addr0, addr0T) == RD_ADDR(addr1, addr1T))  { ACC_BIT |= 0x01; }
    else  { ACC_BIT &= 0xfe; }
}

static void FNC_AND_GE(void)
{ if (RD_ADDR(addr0, addr0T) >= RD_ADDR(addr1, addr1T))  { ; }
    else  { ACC_BIT &= 0xfe; }
}

static void FNC_AND_LE(void)
{ if (RD_ADDR(addr0, addr0T) <= RD_ADDR(addr1, addr1T))  { ; }
    else  { ACC_BIT &= 0xfe; }
}

static void FNC_AND_GT(void)
{ if (RD_ADDR(addr0, addr0T) > RD_ADDR(addr1, addr1T))  { ; }
    else  { ACC_BIT &= 0xfe; }
}

static void FNC_AND_LT(void)
{ if (RD_ADDR(addr0, addr0T) < RD_ADDR(addr1, addr1T))  { ; }
    else  { ACC_BIT &= 0xfe; }
}

static void FNC_AND_NE(void)
{ if (RD_ADDR(addr0, addr0T) != RD_ADDR(addr1, addr1T))  { ; }
    else  { ACC_BIT &= 0xfe; }
}

static void FNC_AND_EQ(void)
{ if (RD_ADDR(addr0, addr0T) == RD_ADDR(addr1, addr1T))  { ; }
    else  { ACC_BIT &= 0xfe; }
}

static void FNC_OR_GE(void)
{ if (RD_ADDR(addr0, addr0T) >= RD_ADDR(addr1, addr1T)) 
    { ACC_BIT |= 0x01;
    }
}

static void FNC_OR_LE(void)
{ if (RD_ADDR(addr0, addr0T) <= RD_ADDR(addr1, addr1T))  
    { ACC_BIT |= 0x01;
    }
}

static void FNC_OR_GT(void)
{ if (RD_ADDR(addr0, addr0T) > RD_ADDR(addr1, addr1T)) 
    { ACC_BIT |= 0x01;
    }
}

static void FNC_OR_LT(void)
{ if (RD_ADDR(addr0, addr0T) < RD_ADDR(addr1, addr1T))
    { ACC_BIT |= 0x01;
    }
}

static void FNC_OR_NE(void)
{ if (RD_ADDR(addr0, addr0T) != RD_ADDR(addr1, addr1T)) 
    { ACC_BIT |= 0x01;
    }
}

static void FNC_OR_EQ(void)
{ if (RD_ADDR(addr0, addr0T) == RD_ADDR(addr1, addr1T))  
    { ACC_BIT |= 0x01;
    }
}
  
static void FNC_CJ(void)
{ u8t Ca;
  if ((ACC_BIT & 0x01) != 0)
    { Ca   = addr0 >> 1;
      if (Ca < CSP_Pn_MAX)
        { CODE_p = CSP_Pn[Ca];
        }
      else 
        { CODE_p = CSP_Pn[CSP_Pn_MAX];
        }
    }
}

static void FNC_CALL(void) { 
    u8t Ca;

    if ((ACC_BIT & 0x01) != 0) { 
        Ca   = addr0 >> 1;
        if (++CALL_SP_i >= CALL_SP_MAX) { 
            CALL_SP_i = 0;
        }
        CALL_SP[CALL_SP_i] = CODE_p;
        if (Ca < CSP_Pn_MAX) { 
            CODE_p = CSP_Pn[Ca];
        }
        else { 
            CODE_p = CSP_Pn[CSP_Pn_MAX];
        }
    }
}

static void FNC_SRET(void) { 
    CODE_p = CALL_SP[CALL_SP_i];
    if (CALL_SP_i != 0) { 
        CALL_SP_i--;
    } 
    else { 
        CALL_SP_i = CALL_SP_MAX-1;
    }
}

static void FNC_IRET(void)
{ ;
}

static void FNC_EI(void)
{ ;
}

static void FNC_DI(void)
{ ;
}

static void FNC_FEND(void)
{ CODE_p = (const u8t *)CODE_START; 
  Pi = 0x01;
}

static void FNC_WDT(void)
{ ;
}

static void FNC_FOR(void)             // FOR
{ ;
}

static void FNC_NEXT(void)            // NEXT
{ ;
}

static void FNC_CMP(void)
{ signed int Ia, Ib;
  u32t addr;
  if ((ACC_BIT & 0x01) != 0) 
    { Ia = RD_ADDR(addr0, addr0T);
      Ib = RD_ADDR(addr1, addr1T);
      if (Ia > Ib)
        { addr = WR_ADDR(addr2, 1, addr2T);
        }
      else 
        { addr = WR_ADDR(addr2, 0, addr2T);
        }
      if (Ia == Ib)
        { addr = WR_ADDR(addr, 1, addr2T);
        }
      else 
        { addr = WR_ADDR(addr, 0, addr2T);
        }
      if (Ia < Ib)
        { WR_ADDR(addr, 1, addr2T);
        }
      else 
        { WR_ADDR(addr, 0, addr2T);
        }
    }
}

static void FNC_ZCP(void)  
{ signed int Ia, Ib, Ic;
  u32t addr;
  if ((ACC_BIT & 0x01) != 0) 
    { Ia = RD_ADDR(addr0, addr0T);
      Ib = RD_ADDR(addr1, addr1T);
      Ic = RD_ADDR(addr2, addr2T);
      if (Ia > Ic)
        { addr = WR_ADDR(addr3, 1, addr3T);
        }
      else 
        { addr = WR_ADDR(addr3, 0, addr3T);
        }
      if ((Ia <= Ic) && (Ic <= Ib))
        { addr = WR_ADDR(addr, 1, addr3T);
        }
      else 
        { addr = WR_ADDR(addr, 0, addr3T);
        }
      if (Ia < Ib)
        { WR_ADDR(addr, 1, addr3T);
        }
      else 
        { WR_ADDR(addr, 0, addr3T);
        }
    }
}

static void FNC_SMOV(void)            // SMOV
{ ;
}

static void FNC_BMOV(void)            // BMOV
{ ;
}

static void FNC_FMOV(void)
{ signed int Ia;
  u32t addr;
  signed char i;
  if ((ACC_BIT & 0x01) != 0) 
    { Ia = RD_ADDR(addr0, addr0T); 
      addr = addr1;
      for (i = (signed char)RD_ADDR(addr2, addr2T); i>0; i--)
        { addr = WR_ADDR(addr, Ia, addr1T);
        }
    }
}

static void FNC_ROR(void)
{ signed int Ia, Ib;
  u8t i;
  if ((ACC_BIT & 0x01) != 0)
    { Ia = RD_ADDR(addr0, addr0T); 
      i  = (u8t)RD_ADDR(addr1, addr1T);
      Ib = (Ia >> i) | (Ia << (16-i));
      WR_ADDR(addr0, Ib, addr0T);
    }
}

static void FNC_ROL(void)
{ signed int Ia, Ib;
  u8t i;
  if ((ACC_BIT & 0x01) != 0)
    { Ia = RD_ADDR(addr0, addr0T); 
      i  = (u8t)RD_ADDR(addr1, addr1T);
      Ib = (Ia << i) | (Ia >> (16-i));
      WR_ADDR(addr0, Ib, addr0T);
    }
}

static void FNC_RCR(void)             // RCR
{ ;
}

static void FNC_RCL(void)             // RCL
{ ;
}

static void FNC_SFTR(void)            // SFTR
{ ;
}

static void FNC_SFTL(void)            // SFTL
{ ;
}

static void FNC_WSFR(void)            // WSFR
{ ;
}

static void FNC_WSFL(void)            // WSFL
{ ;
}

static void FNC_SFWR(void)
{ ;
}

static void FNC_SFRD(void)
{ ;
}

static void FNC_ZRST(void) {
 //signed char i;
  u32t addr;
  if ((ACC_BIT & 0x01) != 0) 
    { for (addr=addr0; addr <= addr1; ) 
        { addr = WR_ADDR(addr, 0, addr0T);
        }
    }
}

static void FNC_DECO(void)
{ ;
}

static void FNC_DNCO(void)
{ ;
}

static void FNC_SUM(void)
{ ;
}

static void FNC_BON(void)
{ ;
}

static void FNC_MEAN(void)
{ ;
}

static void FNC_ANS(void)
{ ;
}

static void FNC_ANR(void)
{ ;
}

static void FNC_SOR(void)
{ ;
}

static void FNC_FLT(void)
{ ;
}

static void FNC_REF(void)         // X输入，Y输出刷新,    需完善
{ u8t i;
  i = P2;
  i = ((i << 1) & 0xaa) | ((i >> 1) & 0x55);
  i = ((i << 2) & 0xcc) | ((i >> 2) & 0x33);
  rX[0].BYTE = ~((P1 & 0x0f) | (i & 0xf0));
  rX[1].BYTE = ~(i & 0x0f) & 0x0f;
  P0 = rY[0].BYTE;
}

static void FNC_REFF(void)
{ ;
}

static void FNC_MTR(void)
{ ;
}

static void FNC_HSCS(void)
{ ;
}

static void FNC_HSCR(void)
{ ;
}

static void FNC_HSZ(void)
{ ;
}

static void FNC_SPD(void)
{ ;
}

static void FNC_PLSY(void)      // PLSY
{ ;
}

static void FNC_PWM(void)       // PWM
{ ;
}

static void FNC_PLSR(void)      // PLSR
{ ;
}

static void FNC_IST(void)
{ ;
}

static void FNC_SER(void)
{ ;
}

static void FNC_ABSD(void)
{ ;
}

static void FNC_INCD(void)
{ ;
}

static void FNC_TTMR(void)
{ ;
}

static void FNC_STMR(void)
{ ;
}

static void FNC_RAMP(void)
{ ;
}

static void FNC_ROTC(void)
{ ;
}

static void FNC_SORT(void)
{ ;
}

static void FNC_TKY(void)
{ ;
}

static void FNC_HKY(void)
{ ;
}

static void FNC_DSW(void)
{ ;
}

static void FNC_SEGD(void)
{ ;
}

static void FNC_SEGL(void)
{ ;
}

static void FNC_ARWS(void)
{ ;
}

static void FNC_ASC(void)      // ASC
{ ;
}

static void FNC_PR(void)
{ ;
}

static void FNC_FROM(void)
{ ;
}

static void FNC_TO(void)
{ ;
}

static void FNC_RS(void)
{ ;
}

static void FNC_PRUN(void)      // PRUN
{ ;
}

static void FNC_ASCI(void)
{ signed int Ia, Ib;
  u32t addr;
  u8t i, Ca;
  if ((ACC_BIT & 0x01) != 0)
    { Ia =  RD_ADDR(addr0, addr0T); 
      i = (u8t)RD_ADDR(addr2, addr2T);
      Ib = RD_ADDR(addr1, addr1T);
      if (i == 1)  { Ia <<= 12; }
      if (i == 2)  { Ia <<= 8; }
      if (i == 3)  { Ia <<= 4; }
      if (i != 0) 
        { i--;
          Ca = (u8t)((Ia >> 12) & 0x0f);
          Ca = (Ca<10) ? (Ca+0x30) : (Ca+0x41-0x0a);
          Ib = (Ib & 0xff00) | (u32t)Ca;
          if (i != 0)
            { i--;
              Ca = (u8t)((Ia >> 8) & 0x0f);
              Ca = (Ca<10) ? (Ca+0x30) : (Ca+0x41-0x0a);
              Ib = (Ib & 0x00ff) | ((u32t)Ca << 8);
            }
          addr = WR_ADDR(addr1, Ib, addr1T);
          if (i != 0) 
            { Ib = RD_ADDR(addr, addr1T); 
              if (i != 0)
                { i--;
                  Ca = (u8t)((Ia >> 4) & 0x0f);
                  Ca = (Ca<10) ? (Ca+0x30) : (Ca+0x41-0x0a);
                  Ib = (Ib & 0xff00) | (u32t)Ca;
                }
              if (i != 0)
                { Ca = (u8t)(Ia & 0x0f);
                  Ca = (Ca<10) ? (Ca+0x30) : (Ca+0x41-0x0a);
                  Ib = (Ib & 0x00ff) | ((u32t)Ca << 8);
                }
              WR_ADDR(addr, Ib, addr1T);
            }
        }
    }
}

static void FNC_HEX(void)
{ ;
}

static void FNC_CCD(void)
{ ;
}

static void FNC_VRRD(void)      // VRRD
{ ;
}

static void FNC_VRSC(void)      // VRSC
{ ;
}

static void FNC_PID(void)       // PID
{ ;
}

static void FNC_ECMP(void)
{ ;
}

static void FNC_EZCP(void)
{ ;
}

static void FNC_EBCD(void)
{ ;
}

static void FNC_EBIN(void)
{ ;
}

static void FNC_EADD(void)
{ ;
}

static void FNC_ESUB(void)
{ ;
}

static void FNC_EMUL(void)
{ ;
}

static void FNC_EDIV(void)
{ ;
}

static void FNC_ESOR(void)
{ ;
}

static void FNC_INT(void)
{ ;
}

static void FNC_SIN(void)
{ ;
}

static void FNC_COS(void)
{ ;
}

static void FNC_TAN(void)
{ ;
}

static void FNC_SWAP(void) 
{ signed int Ia;
  if ((ACC_BIT & 0x01) != 0) 
    { Ia = RD_ADDR(addr0, addr0T); 
      Ia = (Ia >> 8) | (Ia << 8);
      WR_ADDR(addr0, Ia, addr0T);
    }
}

static void FNC_ABS(void)
{ ;
}

static void FNC_ZRN(void)       // ZRN
{ ;
}

static void FNC_PLSV(void)      // PLSV
{ ;
}

static void FNC_DRVI(void)      // DRVI
{ ;
}

static void FNC_DRVA(void)      // DRVA
{ ;
}

static void FNC_TCMP(void)
{ ;
}

static void FNC_TZCP(void)
{ ;
}

static void FNC_TADD(void)
{ ;
}

static void FNC_TSUB(void)
{ ;
}

static void FNC_TRD(void)
{ ;
}

static void FNC_TWR(void)
{ ;
}

static void FNC_HOUR(void)
{ ;
}

static void FNC_GRY(void)
{ ;
}

static void FNC_GBIN(void)
{ ;
}

static void FNC_RD3A(void)
{ ;
}

static void FNC_WP3A(void)
{ ;
}

static void FNC_ERROR(void) { 
    ; // CODE_ERROR =1;
}


/*********************************************  主指令散转  *******************************************/ 
//-----------------------------------------------------------------------------------------//
//    修 改 人:     MJ.Lu                                                                     //
//    日　  期:     2009年12月01日                                                               //
//    备    注:  函数 CMDFNC 编译造成的警告无法手工去除.                                //
//               详情参见 keil software c51book.pdf 一文中的 377 页 相关说明.               //
//-----------------------------------------------------------------------------------------//
static void CMDFNC(void)     // 0 (应用指令)   
{
    u8t pa, pb;
    if (ppp == 0x00f)      //   (END指令, 000F ) 
    {
        END();  
    }
    else if (ppp <= 0x00e) 
    {
        pa = (u8t)ppp;
        if ((pa <= 0x001) || (pa == 0x00e)) ;
        else if ((pa == 0x00a) || (pa == 0x00d))
        {
            (*key_list_1[pa-2][0])();
        }
        else 
        {
            plc_get_next_cmd();
            if ((orderH & 0x80) == 0x80)
            { 
                (*key_list_1[pa-2][(orderH >> 5) & 0x01])();
            }
        }
    }
    else if ((ppp  >= 0x1c0) && (ppp <= 0x1cf))  
    {
        pa = (u8t)(ppp - 0x1c0);
        if ((pa <= 0x001) || (pa == 0x008) || (pa == 0x009)) ;
        else 
        {
            plc_get_next_cmd();
            if ((orderH & 0x80) == 0x80)
            { 
                (*key_list_2[pa-2][(orderH >> 5) & 0x01])();
            }    
        }
    }
    else if ((ppp >= 0x600) && (ppp < 0x800))        // 三字指令
    {
        WR_TK(ppp - 0x600);         // OUT  T,K      0000+(T)  8000+xx 8000+yy  
    }
    else if ((ppp >= 0xe00) && (ppp < (0xe00+200)))  // 三字指令
    {
        WR_CK(ppp - 0xe00);         // OUT  C,K      0000+(C)  8000+xx 8000+yy 
    } 
    // 五字指令(略)                  // OUT  Cp,K     0000+(Cp) 8000+xx 8000+yy 8000+zz 8000+ww 
    else if ((ppp&0x001) == 0)      // 基本应用指令  =  (FNC.No.n+8)*2 
    {
        pa = ((ppp >> 1) - 8) < 0x100 ? (u8t)((ppp >> 1) - 8) : 0xff;
        pb = FNC_CMD_LNE[pa];
        if (pb != 0) 
        {
            pb--;
            addr0L = *CODE_p;
            CODE_p++;
            addr0T = (*CODE_p << 4) & 0xe0;
            CODE_p++;
            addr0H = *CODE_p;
            CODE_p++;
            addr0T |= (*CODE_p & 0x1f);
            CODE_p++;
            if (pb != 0) 
            {
                pb--;
                addr1L = *CODE_p;
                CODE_p++;
                addr1T = (*CODE_p << 4) & 0xe0;
                CODE_p++;
                addr1H = *CODE_p;
                CODE_p++;
                addr1T |= (*CODE_p & 0x1f);
                CODE_p++;
                if (pb != 0) 
                {
                    pb--;
                    addr2L = *CODE_p;
                    CODE_p++;
                    addr2T = (*CODE_p << 4) & 0xe0;
                    CODE_p++;
                    addr2H = *CODE_p;
                    CODE_p++;
                    addr2T |= (*CODE_p & 0x1f);
                    CODE_p++;
                    if (pb != 0) 
                    {
                        pb--;
                        addr3L = *CODE_p;
                        CODE_p++;
                        addr3T = (*CODE_p << 4) & 0xe0;
                        CODE_p++;
                        addr3H = *CODE_p;
                        CODE_p++;
                        addr3T |= (*CODE_p & 0x1f);
                        CODE_p++;
                        if (pb != 0) 
                        {
                            addr4L = *CODE_p;
                            CODE_p++;
                            addr4T = (*CODE_p << 4) & 0xe0;
                            CODE_p++;
                            addr4H = *CODE_p;
                            CODE_p++;
                            addr4T |= (*CODE_p & 0x1f);
                            CODE_p++;
                        }
                    }   
                }
            } 
        }  
        (*key_list_3[pa])();        //  应用指令散转
    }
    else if ((ppp&0x001) != 0)    //  D 应用指令   =  (FNC.No.n+8)*2 +1 
    {
        ;
    }
    else ;
}

static void CMDP(void)    // 1 (P 应用指令)  
{
  ;
}
/*
          CMDFNC ,   // 0 (FNC应用指令)   
          CMDP   ,   // 1 (P 应用指令)  
          LD     ,   // 2 (LD指令, 5000+ppp, 扩展 Mp除外)  
          LDI    ,   // 3 (LDI指令, 5000+ppp, 扩展 Mp除外)   
          AND    ,   // 4 (AND指令, 5000+ppp, 扩展 Mp除外)   
          ANI    ,   // 5 (ANI指令, 5000+ppp, 扩展 Mp除外)   
          OR     ,   // 6 (OR指令, 5000+ppp, 扩展 Mp除外)   
          ORI    ,   // 7 (ORI指令, 5000+ppp, 扩展 Mp除外)   
          CMDER  ,   // 8 (多字指令，第二字及以后有效) 
          CMDER  ,   // 9  
          CMDER  ,   // A (多字指令，第二字及以后有效, 仅对M1536-M3071有效，需加偏移量200)  
          CMDER  ,   // B (Pn指令, 仅对CJ，CALL有效)
          OUTYM  ,   // C (OUT指令, 仅对Y，M有效) 
          SETYM  ,   // D (SET指令, 仅对Y，M有效) 
          RSTYM  ,   // E (RST指令, 仅对Y，M有效) 
          CMDCH };   // F (纯单字指令)  
*/
//by ykb
static void LD(void) {    // 2  (LD指令, 2000+ppp, 扩展 Mp除外) 
    ACC_BIT <<= 1;
    if (RD_ppp(ppp)) {
        ACC_BIT |= 1;
    }
}
//by ykb
static void LDI(void) {   // 3  (LDI指令, 3000+ppp, 扩展 Mp除外) 
    ACC_BIT <<= 1;
    if (RD_ppp(ppp) == 0) {
        ACC_BIT |= 1;
    }
}
//by ykb
static void AND(void) {   // 4  (AND指令, 4000+ppp, 扩展 Mp除外) 
    if (ACC_BIT & 0x01) {
        if (RD_ppp(ppp) == 0) {
            ACC_BIT &= 0xfffffffe;
        }
    }
    //ACC_BIT &= (RD_ppp(ppp) | 0xfe);
}
//by ykb
static void ANI(void) {   // 5  (ANI指令, 5000+ppp, 扩展 Mp除外) 
    if (ACC_BIT & 0x01) {
        if (RD_ppp(ppp)) {
            ACC_BIT &= 0xfffffffe;
        }
    }
    //ACC_BIT &= (~RD_ppp(ppp) | 0xfe);
}
//by ykb
static void OR(void) {    // 6  (OR指令, 6000+ppp, 扩展 Mp除外) 
    if ((ACC_BIT & 0x01) == 0) {
        if (RD_ppp(ppp)) {
            ACC_BIT |= 0x01;
        }
    }
    //ACC_BIT |= RD_ppp(ppp);
}
//by ykb
static void ORI(void) {   // 7  (ORI指令, 7000+ppp, 扩展 Mp除外)  
    if ((ACC_BIT & 0x01) == 0) {
        if (RD_ppp(ppp) == 0) {
            ACC_BIT |= 0x01;
        }
    }
    //ACC_BIT |= (~RD_ppp(ppp) & 0x01);
}
static void OUTYM(void) {   // C (OUT指令, C000+ppp, 仅对Y，M有效) 
    WR_YM(ppp, ACC_BIT & 0x01);
}

static void SETYM(void) {   // D (SET指令, D000+ppp, 仅对Y，M有效) 
    if (ACC_BIT & 0x01) {
        WR_YM_SET(ppp);
    }
}

static void RSTYM(void) {   // E (RST指令, E000+ppp, 仅对Y，M有效) 
    if (ACC_BIT & 0x01) {
        WR_YM_CLR(ppp);
    }
}


/*
static void CMDCH(void) {   // F (纯单字指令)  
    if (ppp == 0xff8) {
        ANB();    // ANB    FFF8 
    }
    else if (ppp == 0xff9) {
        ORB();    // ORB    FFF9 
    }
    else if (ppp == 0xffa) {
        MPS();    // MPS    FFFA 
    }
    else if (ppp == 0xffb) {
        MRD();    // MRD    FFFB 
    }
    else if (ppp == 0xffc) {
        MPP();    // MPP    FFFC 
    }
    else if (ppp == 0xffd)
        INV();    // INV    FFFD 
    else if (ppp == 0xfff)
        NOP();    // NOP    FFFF 
    else {
        ;
    }
}*/

static const FunType key_list_cmdchs[16]={
        NULLCMD,   // 0
        NULLCMD,   // 1
        NULLCMD,   // 2
        NULLCMD,   // 3
        NULLCMD,   // 4
        NULLCMD,   // 5
        NULLCMD,   // 6
        NULLCMD,   // 7
        ANB,   // 8
        ORB,   // 9
        MPS,   // A
        MRD,   // B
        MPP,   // C
        INV,   // D
        NULLCMD  ,   // E
        NOP    // F
    };
static void CMDCH(void) {
    if ((ppp & 0xff0) == 0xff0) {
        key_list_cmdchs[ppp & 0xf]();
    }
}

//
static void LDMp(void) {    //    (LD指令, 01C2 A000+ppp ；仅对Mp有效)  
    ACC_BIT <<= 1;
    ACC_BIT |= RD_Mp(ppp); 
}
static void LDIMp(void) {   //    (LDI指令, 01C3 A000+ppp ；仅对Mp有效)  
    ACC_BIT <<= 1;
    ACC_BIT |= (~RD_Mp(ppp) & 0x01);
}

static void ANDMp(void) {   //    (AND指令, 01C4 A000+ppp ；仅对Mp有效)  
    ACC_BIT &= (RD_Mp(ppp) | 0xfe);
}

static void ANIMp(void) {   //    (ANI指令, 01C5 A000+ppp ；仅对Mp有效)  
    ACC_BIT &= (~RD_Mp(ppp) | 0xfe); 
}

static void ORMp(void) {    //    (OR指令, 01C6 A000+ppp ；仅对Mp有效)   
    ACC_BIT |= RD_Mp(ppp);
}

static void ORIMp(void) {    //    (ORI指令, 01C7 A000+ppp ；仅对Mp有效)   
    ACC_BIT |= (~RD_Mp(ppp) & 0x01);
}

static void CMDER(void) {   //    (出错! ) 
    ;
}

static void Pn(void) {    // B (Pn指令, 仅对CJ，CALL有效) 
    if (ppp < CSP_Pn_MAX) {
        CSP_Pn[ppp] = CODE_p;
    }
    else {
        CSP_Pn[CSP_Pn_MAX] = (const u8t *)CODE_START; 
    }   
}


//-------------------------------------------------------------------------------------//
//   PLC 代码 初次扫描，求出标号 Pn 地址函数  main_PLC();                              //
//-------------------------------------------------------------------------------------//

//  输入、输出端口号按8进制。程序开始地址:15CH  
//  如：X0-X7   -> 0-7(H)    X40-X47 -> 20-27(H)   X80-X87   -> 40-47(H)  
//       X10-X17 -> 8-F(H)    X50-X57 -> 28-2F(H)   X90-X97   -> 48-4F(H)  
//       X20-X27 -> 10-17(H)  X60-X67 -> 30-37(H)   X100-X107 -> 50-57(H)   
//       X30-X37 -> 18-1F(H)  X70-X77 -> 38-3F(H)   X110-X117 -> 58-5F(H)  
//  
//  
//  存储格式为： 
//             0E 24  
//             |  |  
//             |  -指令  
//             |  
//             ----操作数  
//  
//    
//  
//  指令对应16进制依次为：  
//  
//  
//             X   X  
//             |   |  
//             |   -0：Sx(特殊寄存器、状态寄存器)  
//             |   -4: Xx(输入)  
//             |   -5: Yx(输出)  
//             |   -6: Tx(定时器)  
//             |   -8: Mx(数据存储器、辅助继电器)  
//             |   -E: Cx(计数器)  
//             |  
//             |----2: LD  
//             |----3: LDI  
//             |----4: AND  
//             |----5: ANI  
//             |----6: OR  
//             |----7: ORI  
//             |----C: OUT  
//             |----D: SET  
//             |--8x 寄存器 00 0C      : RST T/C  
//             |--Ex 端口              : RST Y/M  
//             |--E(x>=8)  yy          : RST M(xyy-800) (M0-M1535)  
//             |--Ax yy 00 04          : RST M(xyy-200) (M1536-M3071)  
//  
//             |--8F xx 00 04          : RST M8xxx      (M8000-M8255) 
//  
//             |--86 xx 86 yy 00 0D    : RST D(XXYY/2) (D0-D1998) 
//             |--88 xx 86 yy 00 0D    : RST D(XXYY/2)+1000 (D1000-D7999) 
//             |--80 xx 86 yy 00 0D    : RST D(XXYY/2)+8000 (D8000-D8195) 
//  
//             |                             |-D8028   :Z0 
//             |                             |-D8029   :V0 
//  
//             |                             |-D8182   :Z1 
//  
//             |                             |-D8183   :V1 
//             |                             |-D8184   :Z2 
//             |                             |-D8185   :V2 
//             |                             |-D8186   :Z3 
//             |                             |-D8187   :V3 
//             |                             |-D8188   :Z4 
//             |                             |-D8189   :V4 
//  
//             |                             |-D8190   :Z5 
//             |                             |-D8191   :V5 
//             |                             |-D8192   :Z6 
//             |                             |-D8193   :V6 
//             |                             |-D8194   :Z7 
//             |                             |-D8195   :V7 
//             |--8x 寄存器 00 07      : RST Sx 
//             |--8x 端口   01 CA      : LDP 
//             |--8x 端口   01 CB      : LDF 
//             |--8x 端口   01 CC      : ANDP 
//             |--8x 端口   01 CD      : ANDF 
//             |--8x 端口   01 CE      : ORP 
//             |--8x 端口   01 CF      : ORF 
//             |--8x 端口   00 08      : PLS 
//             |--8x 端口   00 09      : PLF 
//             |--8x 端口   80 0x 00 0A: MC Nx Yx/Mx 
//             |               |-Nx 
//             |      
//             |------------80 0x 00 0B: MCR Nx 
//  
//             |               |-Nx 
//             | 
//             |----80 yy 80 xx 06 zz  : OUT TzzKyyxx 
//             |----80 yy 80 xx 0E zz  : OUT CzzKyyxx 
//             |--FF F8                : ANB 
//             |--FF F9                : ORB 
//             |--FF FA                : MPS 
//             |--FF FB                : MRD 
//             |--FF FC                : MPP 
//             |--FF FD                : INV 
//             |--00 0F                : END  
//  LD Xxx 24 xx  
//  LD Yxx 25 xx  
//  LD M0-M1535 2x yy LD M(xyy-800) 
//   28 00  
//   2D FF  
//  LD M1536-M3071 Ax yy 01 C2 LD M(xyy-200) 
//   A8 00  
//   AD FF  
//  LD M8000-M8255 2F xx LD Mxx 
//   2F 00  
//   2F FF  
//  LD S0-S999 2x xx LD Sxxx 
//   20 00  
//   23 E7  
//  LD T0-T255 2x xx LD Txx 
//   26 00  
//   26 FF  
//  LD C0-C255 2x xx LD Cxx 
//   2E 00  
//   2E FF 
//  LDP Xxx 84 xx 01 CA  
//  LDP Yxx 85 xx 01 CA  
//  LDP M0-M1535 8x yy 01 CA LDP M(xyy-800) 
//   88 00  
//   8D FF  
//  LDP M1536-M3071 Ax yy 01 CA LDP M(xyy-200) 
//   A8 00  
//   AD FF  
//  LDP M8000-M8255 8F xx 01 CA LDP Mxx 
//   8F 00  
//   8F FF  
//  LDP S0-S999 8x yy 01 CA LDP Sxxx 
//   80 00  
//   83 E7  
//  LDP T0-T255 8x yy 01 CA LDP Txx 
//   86 00  
//   86 FF  
//  LDP C0-C255 8x yy 01 CA LDP Cxx 
//   8E 00  
//   8E FF  
//  LDI Xxx 34 xx  
//  LDI Yxx 35 xx  
//  LDI M0-M1535 3x yy LDI M(xyy-800) 
//   38 00  
//   3D FF  
//  LDI M1536-M3071 Ax yy 01 C3 LDI M(xyy-200) 
//   A8 00  
//   AD FF  
//  LDI M8000-M8255 3F xx LDI Mxx 
//   3F 00  
//   3F FF  
//  LDI S0-S999 3x xx LDI Sxxx 
//   30 00  
//   33 E7  
//  LDI T0-T255 3x xx LDI Txx 
//   36 00  
//   36 FF  
//  LDI C0-C255 3x xx LDI Cxx 
//   3E 00  
//   3E FF 
//  LDF Xxx 84 xx 01 CB  
//  LDF Yxx 85 xx 01 CB  
//  LDF M0-M1535 8x yy 01 CB LDF M(xyy-800) 
//   88 00  
//   8D FF  
//  LDF M1536-M3071 Ax yy 01 CB LDF M(xyy-200) 
//   A8 00  
//   AD FF  
//  LDF M8000-M8255 8F xx 01 CB LDF Mxx 
//   8F 00  
//   8F FF  
//  LDF S0-S999 8x yy 01 CB LDF Sxxx 
//   80 00  
//   83 E7  
//  LDF T0-T255 8x yy 01 CB LDF Txx 
//   86 00  
//   86 FF  
//  LDF C0-C255 8x yy 01 CB LDF Cxx 
//  
//   8E 00  
//   8E FF  
//  AND Xxx 44 xx  
//  AND Yxx 45 xx  
//  AND M0-M1535 4x yy AND M(xyy-800) 
//   48 00  
//   4D FF  
//  AND M1536-M3071 Ax yy 01 C4 AND M(xyy-200) 
//   A8 00  
//   AD FF  
//  AND M8000-M8255 4F xx AND Mxx 
//   4F 00  
//   4F FF  
//  AND S0-S999 4x xx AND Sxxx 
//   40 00  
//   43 E7  
//  AND T0-T255 4x xx AND Txx 
//   46 00  
//   46 FF  
//  AND C0-C255 4x xx AND Cxx 
//   4E 00  
//   4E FF 
//  ANDP Xxx 84 xx 01 CC  
//  ANDP Yxx 85 xx 01 CC  
//  ANDP M0-M1535 8x yy 01 CC ANDP M(xyy-800) 
//   88 00  
//   8D FF  
//  ANDP M1536-M3071 Ax yy 01 CC ANDP M(xyy-200) 
//   A8 00  
//   AD FF  
//  ANDP M8000-M8255 8F xx 01 CC ANDP Mxx 
//   8F 00  
//   8F FF  
//  ANDP S0-S999 8x yy 01 CC ANDP Sxxx 
//   80 00  
//   83 E7  
//  ANDP T0-T255 8x yy 01 CC ANDP Txx 
//   86 00  
//   86 FF  
//  ANDP C0-C255 8x yy 01 CC ANDP Cxx 
//   8E 00  
//   8E FF 
//  ANI Xxx 54 xx  
//  ANI Yxx 55 xx  
//  ANI M0-M1535 5x yy ANI M(xyy-800) 
//   58 00  
//   5D FF  
//  ANI M1536-M3071 Ax yy 01 C5 ANI M(xyy-200) 
//   A8 00  
//   AD FF  
//  ANI M8000-M8255 5F xx ANI Mxx 
//   5F 00  
//   5F FF  
//  ANI S0-S999 5x xx ANI Sxxx 
//   50 00  
//   53 E7  
//  ANI T0-T255 5x xx ANI Txx 
//   56 00  
//   56 FF  
//  ANI C0-C255 5x xx ANI Cxx 
//   5E 00  
//   5E FF 
//  ANDF Xxx 84 xx 01 CD  
//  ANDF Yxx 85 xx 01 CD  
//  ANDF M0-M1535 8x yy 01 CD ANDF M(xyy-800) 
//   88 00  
//   8D FF  
//  ANDF M1536-M3071 Ax yy 01 CD ANDF M(xyy-200) 
//   A8 00  
//   AD FF  
//  ANDF M8000-M8255 8F xx 01 CD ANDF Mxx 
//   8F 00  
//   8F FF  
//  ANDF S0-S999 8x yy 01 CD ANDF Sxxx 
//   80 00  
//   83 E7  
//  ANDF T0-T255 8x yy 01 CD ANDF Txx 
//   86 00  
//   86 FF  
//  ANDF C0-C255 8x yy 01 CD ANDF Cxx 
//   8E 00  
//   8E FF 
//  OR Xxx 64 xx  
//  OR Yxx 65 xx  
//  OR M0-M1535 6x yy OR M(xyy-800) 
//   68 00  
//   6D FF  
//  OR M1536-M3071 Ax yy 01 C6 OR M(xyy-200) 
//   A8 00  
//   AD FF  
//  OR M8000-M8255 6F xx OR Mxx 
//   6F 00  
//   6F FF  
//  OR S0-S999 6x xx OR Sxxx 
//   60 00  
//   63 E7  
//  OR T0-T255 6x xx OR Txx 
//   66 00  
//   66 FF  
//  OR C0-C255 6x xx OR Cxx 
//   6E 00  
//   6E FF 
//  ORP Xxx 84 xx 01 CE  
//  ORP Yxx 85 xx 01 CE  
//  ORP M0-M1535 8x yy 01 CE ORP M(xyy-800) 
//   88 00  
//   8D FF  
//  ORP M1536-M3071 Ax yy 01 CE ORP M(xyy-200) 
//   A8 00  
//   AD FF  
//  ORP M8000-M8255 8F xx 01 CE ORP Mxx 
//   8F 00  
//   8F FF  
//  ORP S0-S999 8x yy 01 CE ORP Sxxx 
//   80 00  
//   83 E7  
//  ORP T0-T255 8x yy 01 CE ORP Txx 
//   86 00  
//   86 FF  
//  ORP C0-C255 8x yy 01 CE ORP Cxx 
//   8E 00  
//   8E FF  
//  ORI Xxx 74 xx  
//  ORI Yxx 75 xx  
//  ORI M0-M1535 7x yy ORI M(xyy-800) 
//   78 00  
//   7D FF  
//  ORI M1536-M3071 Ax yy 01 C7 ORI M(xyy-200) 
//   A8 00  
//   AD FF  
//  ORI M8000-M8255 7F xx ORI Mxx 
//   7F 00  
//   7F FF  
//  ORI S0-S999 7x xx ORI Sxxx 
//   70 00  
//   73 E7  
//  ORI T0-T255 7x xx ORI Txx 
//   76 00  
//   76 FF  
//  ORI C0-C255 7x xx ORI Cxx 
//   7E 00  
//   7E FF  
//  ORF Xxx 84 xx 01 CF  
//  ORF Yxx 85 xx 01 CF  
//  ORF M0-M1535 8x yy 01 CF ORF M(xyy-800) 
//   88 00  
//   8D FF  
//  ORF M1536-M3071 Ax yy 01 CF ORF M(xyy-200) 
//   A8 00  
//   AD FF  
//  ORF M8000-M8255 8F xx 01 CF ORF Mxx 
//   8F 00  
//   8F FF  
//  ORF S0-S999 8x yy 01 CF ORF Sxxx 
//   80 00  
//   83 E7  
//  ORF T0-T255 8x yy 01 CF ORF Txx 
//   86 00  
//   86 FF  
//  ORF C0-C255 8x yy 01 CF ORF Cxx 
//   8E 00  
//  
//    
//  
//   8E FF  
//  PLS Yxx 85 xx 00 08  
//  PLS M0-M1535 8x yy 00 08 PLS M(xyy-800) 
//   88 00  
//   8D FF  
//  PLS M1536-M3071 Ax yy 00 08 PLS M(xyy-200) 
//   A8 00  
//   AD FF  
//  PLF Yxx 85 xx 00 09  
//  PLF M0-M1535 8x yy 00 09 PLF M(xyy-800) 
//   88 00  
//   8D FF  
//  PLF M1536-M3071 Ax yy 00 09 PLF M(xyy-200) 
//   A8 00  
//   AD FF  
//  OUT Yxx C5 xx  
//  OUT M0-M1535 Cx yy OUT M(xyy-800) 
//   C8 00  
//   CD FF  
//  OUT M1536-M3071 Ax yy 00 02 OUT M(xyy-200) 
//   A8 00  
//   AD FF  
//  OUT M8000-M8255 8F xx 00 02 OUT Mxx 
//   8F 00  
//   8F FF  
//  OUT S0-S999 8x xx 00 05  
//   80 00  
//   83 E7  
//  OUT T0-T255 80 xx 80 yy 06 zz OUT Tzz Kxxyy 
//                  OO  
//                  FF  
//  OUT C0-C255 80 xx 80 yy 0E zz OUT CzzKxxyy 
//                  OO  
//                  FF  
//  MC Ny Yxx 85 0y 80 xx 00 0A  
//  MC Ny M0-M1535 8x yy 80 yy 00 0A MC Ny M(xyy-800) 
//   88 00  
//   8D FF  
//  MC Ny M1536-M3071 Ax yy 80 yy 00 0A MC Ny M(xyy-200) 
//   A8 00  
//   AD FF  
//  MCR Nx 80 0x 00 0B  
//  
//  SET Yx D5 xx SET Yx 
//  SET M0-M1535 zx yy SET M(xyy-800) 
//   D8 00  
//   DD FF  
//  SET M1536-M3071 zx yy zz zz SET Mxx 
//   8F 00 00 03  
//   8F FF 00 03  
//  SET S0-S999 zx yy zz zz SET Sxyy 
//   80 00 00 06  
//   83 E7 00 06  
//  RST Yx E5 xx RST Yx 
//  RST M0-M1535 zx yy RST M(xyy-800) 
//   E8 00  
//   ED FF  
//  RST M1536-M3071 zx yy zz zz RST M(xyy-200) 
//   A8 00 00 04  
//   AD FF  
//  RST M8000-M8255 zx yy zz zz RST Myy 
//   8F 00 00 04  
//   8F FF  
//  RST S0-S999 zx yy zz zz RST S0-S999 
//   80 00 00 07  
//   83 E7 00 07  
//  RST T0-T255 zx yy zz zz RST Tyy 
//   86 00 00 0C  
//   86 FF  
//  RST D0-D999 zz xx zz yy zz zz RST D(xxyy/2) 
//   86 xx 86 yy 00 0D  
//  RST D1000-D7000 88 xx 86 yy 00 0D RST D(xxyy/2)+1000 
//  RST D8000-D8195 80 xx 86 yy 00 0D RST D(xxyy/2+800) 
//  ANB FF F8 
//  ORB FF F9 
//  MPS FF FA 
//  MRD FF FB 
//  MPP FF FC 
//  INV FF FD 
//  END 00 0F 
//  NOP FF FF
#define PLCCMD_END      0x000F

static void plc_get_next_cmd(void) {
    orderL = *CODE_p;
    CODE_p++;
    orderH = *CODE_p;
    CODE_p++;
    ppp = order & 0xfff;
}
void plc_excute_cmd(void) {
    CODE_p = (const u8t *)CODE_START;
    Pi = 0x01;
    do {
      plc_get_next_cmd();
      (*key_list[orderH >> 4])();
    } while(((u32t)CODE_p < CODE_END) && ((u32t)CODE_p != CODE_START));
}
void CODE_scan(void) { 
    u8t pa, pb;

    CODE_p = (const u8t*)CODE_START;
    do {
        plc_get_next_cmd();

        if ((orderH>>4) <= 0x1) {
            if (ppp == 0x00f) {     //   (END指令, 000F ) 
                END();
            }
            else if (ppp <= 0x00e) {
                pa = (u8t)ppp;
                if ((pa <= 0x001) || (pa == 0x00e))        //  OUT C0-C255 80 xx 80 yy 0E zz OUT CzzKxxyy 
                    ;
                else if ((pa == 0x00a) || (pa == 0x00d)) {   //  MC RST
                    CODE_p += 4;
                }
                else {
                    CODE_p++;
                    CODE_p++;
                }
            }
            else if ((ppp  >= 0x1c0) && (ppp <= 0x1cf)) { 
                pa = (u8t)(ppp - 0x1c0);
                if ((pa <= 0x001) || (pa == 0x008) || (pa == 0x009)) {
                    ;
                }
                else {
                    CODE_p++;
                    CODE_p++;
                }
            }
            else if ((ppp >= 0x600) && (ppp < 0x800)) {       // 三字指令
                CODE_p += 4;
            }
            else if ((ppp >= 0xe00) && (ppp < (0xe00+200))) { // 三字指令
                CODE_p += 4;
            }
            // 五字指令(略)           // OUT  Cp,K 
            else if ((ppp&0x001) == 0) {   // 基本应用指令  =  (FNC.No.n+8)*2 
                pa = ((ppp >> 1) - 8) < 0x100 ? (u8t)((ppp >> 1) - 8) : 0xff;
                pb = FNC_CMD_LNE[pa];
                while (pb != 0) {
                    pb--;
                    CODE_p += 4;
                }  
            }
            else ;
        }
        if ((orderH>>4) == 0xb) {
            Pn();
        }
        else {
            ;
        }
    } while ((CODE_p < (u8t*)CODE_END) && (CODE_p != (u8t*)CODE_START));
    if (CODE_p >= (u8t*)CODE_END) {
        CODE_ERROR = 1;
    }
    CSP_Pn[CSP_Pn_MAX] = (const u8t *)CODE_START; 
}

/********************************************  程序结束  **********************************************/ 
