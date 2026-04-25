#define _MAIN_C

#include "fx1ntype.h"
#include "stm32f10x.h"
#include "SysTick.h"
#include "port.h"
#include "fx1nvar.h"
#include "sio.h"
#include "fx1niap.h"
#include <string.h> // ????,???? memset ??
#include "stm32f10x_pwr.h"  // ???????????

volatile u8t debug_pb3_state = 0xFF; // ?? volatile ?????????
// =================================================================================
// 工業級串口架構：中斷環形緩衝 (FIFO) + 無盲區接收
// =================================================================================
#define RX_BUF_SIZE 2048
u8t rx_fifo[RX_BUF_SIZE];
volatile u32t rx_head = 0;
volatile u32t rx_tail = 0;


u8t _buf[1024];          // DMA 專用硬體接收緩衝
u8t _tx_buf[1024];       // 專屬發送緩衝 (防踩踏)
u8t _rx_buf[1024];       // 協議解析專用緩衝
u32t _rx_len = 0;        
u32t _length = 0;        
extern void System_Flash_Init(void);
extern void reset_RAM(void);

volatile int is_tx_busy = 0;
// =========================================================================
// ?? PVD ??????? (2.8V ????) ??
// =========================================================================
// =========================================================================
// ?? PVD ??????? (2.8V ???? + EXTI ? 16 ??) ??
// =========================================================================
void PVD_Config(void) {
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure; // ?? ??:???????
    
    // 1. ?? PWR ??????
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    
    // 2. ?? PVD ????? 2.8V
    PWR_PVDLevelConfig(PWR_PVDLevel_2V8); 
    PWR_PVDCmd(ENABLE); // ?? PVD
    
    // ===================================================================
    // ?????? ????:?? EXTI ? 16 (PVD ???????) ??????
    // ?????,CPU ?????????!
    // ===================================================================
    EXTI_ClearITPendingBit(EXTI_Line16);
    EXTI_InitStructure.EXTI_Line = EXTI_Line16;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; // ?????2.8V,????????
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    // 3. ?? NVIC ???????,????
    NVIC_InitStructure.NVIC_IRQChannel = PVD_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
void UartSendchar(u8t c) {
    if(_length >= 1024) _length = 0;
    _tx_buf[_length++] = c; 
}

void UartSendByte(u8t *buf, u32t length) {
    u32t i;
    for (i = 0; i< length; i++) {
        UartSendchar(*buf);
        buf++;
    }
}
// =========================================================================
// ?? ????:????????? ??
// =========================================================================
void Restore_Retentive_Data(void) {
    int i;
    u32t* flash_D = (u32t*)0x0800F800; // Flash ?? D ??????
    u32t* flash_C = (u32t*)0x0800FA00; // Flash ?? C ??????
		u32t* flash_T = (u32t*)0x0800FB80; // ?? T ? Flash ????

    // ?? Flash ???????? (???? 0xFFFFFFFF ?????????)
    if (flash_D[0] != 0xFFFFFFFF) {
        // ? Flash ????? RAM
        for(i = 0; i < 112; i++) {
            _D[16 + i] = flash_D[i];
            _C[16 + i] = flash_C[i];
        }
				for(i = 0; i < 10; i++) {
            _T[22 + i][0] = flash_T[i];
        }
    }
}
void plc_idle(void) {}
void fx1n_init(void);
void fx1n_excute(void);
extern void FX1NProcessing(u8t *UartReceiveBuffer, u32t UartReceiveCounter);
extern unsigned char CODE_ERROR; 

// =================================================================================
// 硬體與 IO 橋接
// =================================================================================
void PLC_HW_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    // 1. ??????? GPIO ??,??????? AFIO (????) ??
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | 
                           RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);

    // =========================================================================
    // ?? ?????????:???? X1(PB3), X4(PD1), X6(PC15) ????!
    // =========================================================================

    // ?? ?? X6 (PC15):???? RCC ??????,???? BKP ??????!
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE); 
    PWR_BackupAccessCmd(ENABLE); 
    RCC_BackupResetCmd(ENABLE);  // ??????????
    RCC_BackupResetCmd(DISABLE); // ????,PC13-PC15 ??????!
    RCC_LSEConfig(RCC_LSE_OFF);  // ??????

    // ?? ?? X4 (PD1):??????,??????????!
    RCC_HSEConfig(RCC_HSE_OFF);  
    // ????:? 48/64 ?? STM32 ?,PD0 ? PD1 ???????? GPIO ?!
    GPIO_PinRemapConfig(GPIO_Remap_PD01, ENABLE); 

    // ?? ?? X1 (PB3):?? JTAG,??? SWD ??!
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); 

    // =========================================================================

    // 2. ??????? (X0 - X13)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // ??????
    
    // GPIOA ??
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_15; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // GPIOB ?? (??????? PB3)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9; 
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // GPIOC ?? (??????? PC15)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15; 
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    // GPIOD ?? (??????? PD1)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);


    // 3. ??????? (Y0 - Y7)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // ????
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    // GPIOA ??
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_7 | GPIO_Pin_11 | GPIO_Pin_12; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // GPIOB ??
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15; 
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 4. ?????? (????)
    GPIO_ResetBits(GPIOA, GPIO_Pin_4 | GPIO_Pin_7 | GPIO_Pin_11 | GPIO_Pin_12);
    GPIO_ResetBits(GPIOB, GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
		
}
void fx1n_readX(u8t* buf, u32t length) {
    if(length < 2) return; 
    buf[0] = 0; buf[1] = 0;
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0)  buf[0] |= (1 << 0); 
    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3) == 0)  buf[0] |= (1 << 1); 
    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) == 0)  buf[0] |= (1 << 2); 
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 0)  buf[0] |= (1 << 3); 
    if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_1) == 0)  buf[0] |= (1 << 4); 
    if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14) == 0) buf[0] |= (1 << 5); 
    if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_15) == 0) buf[0] |= (1 << 6); 
    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9) == 0)  buf[0] |= (1 << 7); 
    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7) == 0)  buf[1] |= (1 << 0); 
    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6) == 0)  buf[1] |= (1 << 1); 
    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5) == 0)  buf[1] |= (1 << 2); 
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15) == 0) buf[1] |= (1 << 3); 
}

void fx1n_writeY(u8t* buf, u32t length) {
    u8t y_state;             
    if(length == 0) return;  
    y_state = buf[0];        
    if(y_state & (1 << 0)) GPIO_SetBits(GPIOA, GPIO_Pin_4);  else GPIO_ResetBits(GPIOA, GPIO_Pin_4);  
    if(y_state & (1 << 1)) GPIO_SetBits(GPIOA, GPIO_Pin_7);  else GPIO_ResetBits(GPIOA, GPIO_Pin_7);  
    if(y_state & (1 << 2)) GPIO_SetBits(GPIOB, GPIO_Pin_14); else GPIO_ResetBits(GPIOB, GPIO_Pin_14); 
    if(y_state & (1 << 3)) GPIO_SetBits(GPIOB, GPIO_Pin_15); else GPIO_ResetBits(GPIOB, GPIO_Pin_15); 
    if(y_state & (1 << 4)) GPIO_SetBits(GPIOA, GPIO_Pin_12); else GPIO_ResetBits(GPIOA, GPIO_Pin_12); 
    if(y_state & (1 << 5)) GPIO_SetBits(GPIOA, GPIO_Pin_11); else GPIO_ResetBits(GPIOA, GPIO_Pin_11); 
    if(y_state & (1 << 6)) GPIO_SetBits(GPIOB, GPIO_Pin_13); else GPIO_ResetBits(GPIOB, GPIO_Pin_13); 
    if(y_state & (1 << 7)) GPIO_SetBits(GPIOB, GPIO_Pin_12); else GPIO_ResetBits(GPIOB, GPIO_Pin_12); 
}

// =================================================================================
// 串口底層處理：零盲區機制 (Zero-Gap)
// =================================================================================
#define SIO_HANDLE  SIO_USART1

static void read_end(int len) {
    unsigned int cpu_sr; 
    int i;
    
    OS_ENTER_CRITICAL();
    sio_end_read(SIO_HANDLE); // 關閉本次 DMA

    // 【核心破解1】：在中斷裡極速轉存，剝離校驗位
    for(i = 0; i < len; i++) {
        rx_fifo[rx_head] = _buf[i] & 0x7f;
        rx_head = (rx_head + 1) % RX_BUF_SIZE;
    }

    // 【核心破解2】：不等主循環！立刻重啟 DMA，盲區 < 1微秒！
    sio_async_read(SIO_HANDLE, _buf, 1024, read_end);
    OS_EXIT_CRITICAL();
}

static void write_end(void) {
    unsigned int cpu_sr; 
    OS_ENTER_CRITICAL();
    sio_end_write(SIO_HANDLE);
    is_tx_busy = 0; // 釋放發送鎖
    OS_EXIT_CRITICAL();
}

static void init(void) {
    unsigned int cpu_sr; 
    usart_cfg cfg;
    cfg.USART_BaudRate = 9600;
    cfg.USART_WordLength = USART_WordLength_8b; 
    cfg.USART_StopBits = USART_StopBits_1;
    cfg.USART_Parity = USART_Parity_Even;

    OS_ENTER_CRITICAL();
    sio_open(SIO_HANDLE, &cfg);
    // 系統啟動，立刻開啟無間斷接收
    sio_async_read(SIO_HANDLE, _buf, 1024, read_end);
    OS_EXIT_CRITICAL();
}

static void poll(void) {
    unsigned int cpu_sr; 
    int i;
    int frame_length;
    int remain;
    u32t tx_wait;

    // 1. 將 FIFO 中的碎片提取到解析緩衝
    OS_ENTER_CRITICAL();
    while (rx_tail != rx_head) {
        if (_rx_len < 1024) {
            _rx_buf[_rx_len++] = rx_fifo[rx_tail];
        }
        rx_tail = (rx_tail + 1) % RX_BUF_SIZE;
    }
    OS_EXIT_CRITICAL();

parse_frame:
    if (_rx_len == 0) return;

    // 2. 嚴格過濾掉不是控制字符的雜音
    while (_rx_len > 0) {
        u8t b = _rx_buf[0];
        if (b == 0x02 || b == 0x04 || b == 0x05 || b == 0x06 || b == 0x15) break;
        for (i = 1; i < _rx_len; i++) _rx_buf[i-1] = _rx_buf[i];
        _rx_len--;
    }

    if (_rx_len == 0) return;

    // 3. 識別完整數據幀
    frame_length = 0;
    if (_rx_buf[0] == 0x02) {
        // 尋找 ETX (0x03)
        for (i = 1; i < _rx_len; i++) {
            if (_rx_buf[i] == 0x03) {
                if (_rx_len >= (u32t)(i + 3)) { // 包含 0x03 及 2 位校驗碼
                    frame_length = i + 3;
                }
                break;
            }
        }
    } else {
        frame_length = 1; // 單字節指令
    }

    // 4. 送交虛擬機處理並回覆
    if (frame_length > 0) {
        _length = 0; // 重置發送計數
        
        // =====================================================================
        // 🚨 【终极完美截胡】：让 E4 指令永远进不了黑盒运行库！ 🚨
        // =====================================================================
        if (_rx_buf[0] == 0x02 && _rx_buf[1] == 'E' && _rx_buf[2] == '4') {
            // 发现是 E4 指令！直接伪造真机的标准答案，填入发送缓存！
            _tx_buf[_length++] = 0x02; // STX
            _tx_buf[_length++] = '1';
            _tx_buf[_length++] = '8';
            _tx_buf[_length++] = '0';
            _tx_buf[_length++] = '6';
            _tx_buf[_length++] = '0';
            _tx_buf[_length++] = 0x03; // ETX
            _tx_buf[_length++] = '0';  // 校验和高位
            _tx_buf[_length++] = '2';  // 校验和低位
        } 
        else {
            // 不是 E4，才乖乖交给原作者的黑盒运行库处理
            FX1NProcessing(_rx_buf, frame_length); 
        }
        // =====================================================================

        // 移除已處理的幀，保留剩餘數據（完美解決連發指令）
        remain = _rx_len - frame_length;
        for (i = 0; i < remain; i++) _rx_buf[i] = _rx_buf[frame_length + i];
        _rx_len = remain;

        if (_length > 0) {
            tx_wait = 0;
            // 等待上一幀發送完畢，防止覆蓋
            while(is_tx_busy) {
                tx_wait++;
                if (tx_wait > 5000000) { is_tx_busy = 0; break; } // 安全超時
            }
            
            OS_ENTER_CRITICAL();
            is_tx_busy = 1;
            sio_async_write(SIO_HANDLE, _tx_buf, _length, write_end);
            OS_EXIT_CRITICAL();
        }
        
        // 繼續解析緩衝區中可能剩下的下一幀
        goto parse_frame;
    }
}

// =================================================================================
// 終極主循環
// =================================================================================
// =================================================================================
// ?????
// =================================================================================
int main(void) {
    unsigned int cpu_sr; 
    
    // ??????:????????????!???!
    u8t current_pb8;
    u8t last_pb8 = 0xFF; 
    u32t rx_timeout = 0;  

    OS_ENTER_CRITICAL();
    SysTickInit();
    PLC_HW_Init(); 
    init();        
    OS_EXIT_CRITICAL();
    
    // ??????:???? Flash ??????
    System_Flash_Init();
    fx1n_init();   
		PVD_Config();
		Restore_Retentive_Data();   // 2. ???�??:?????? Flash ?????? RAM ?!
		
		// =========================================================================
    // ??? 3. ?????????? (IEC 60204-1 ????)
    // =========================================================================
    
    // ?? ????:??????,?????????!
    PLCRunFlag = 0; 
    
    // ?? ????:?????????,??????????
    GPIO_ResetBits(GPIOA, GPIO_Pin_4 | GPIO_Pin_7 | GPIO_Pin_11 | GPIO_Pin_12);
    GPIO_ResetBits(GPIOB, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);

    // ?? ????:????????,?????????????�???�
    // ??:???????? if ???????!
    last_pb8 = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8);
   while (1) {
        // 1. ?? PB3 ?????? (??)
        debug_pb3_state = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3);
        
        // ??????
        if (_rx_len > 0) {
            rx_timeout++;
            if (rx_timeout > 5000000) { 
                _rx_len = 0; 
                rx_timeout = 0;
            }
        }

       current_pb8 = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8);
        if (current_pb8 != last_pb8) {
            if (current_pb8 == 1) {
                // ?? ?STOP ?? RUN ???,??? RUN ?????
                PLCRunFlag = 1;     
                
                // 1. ?? A ????:???? D ? C ???
                //reset_RAM(); 
                
                // 2. ?? ????:???????? (16~127) ???? Flash ???!??
                //Restore_Retentive_Data();
                
                // 3. ??????
                rM8xxx[0].BIT.BIT2 = 1; 
            } else {
                // ?? ?RUN ?? STOP ????
                PLCRunFlag = 0;                    
                GPIO_ResetBits(GPIOA, GPIO_Pin_4 | GPIO_Pin_7 | GPIO_Pin_11 | GPIO_Pin_12);
                GPIO_ResetBits(GPIOB, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
                reset_RAM(); // STOP ??????
            }
            last_pb8 = current_pb8;
        }

        // 3. RUN/STOP ????? (PB10)
        if (PLCRunFlag == 1) {
            GPIO_ResetBits(GPIOB, GPIO_Pin_10); // ??
        } else {
            GPIO_SetBits(GPIOB, GPIO_Pin_10);   // ??
        }

        // 4. ?????? & ??????? (PB11)
        CODE_ERROR = 0; 
        GPIO_SetBits(GPIOB, GPIO_Pin_11); 

        // =========================================================================
        // PLC ???????
        // =========================================================================
        iap_comit();
        fx1n_excute(); // ??????
        iap_comit();

        // ?? ??????M8002 ????????!(??? excute ???????) ??
        if (rM8xxx[0].BIT.BIT2 == 1) {
            rM8xxx[0].BIT.BIT2 = 0; // ?????????,???????�???�
        }

        // =========================================================================
        // ?????:???? X001 (??? poll ??)
        // =========================================================================
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3) == 0) {
            rX[0].BIT.BIT1 = 1;  // ????? X001
        } else {
            rX[0].BIT.BIT1 = 0;  // ????? X001
        }

        // ?????????? GX Works2 ???
        poll();        
    }
}
