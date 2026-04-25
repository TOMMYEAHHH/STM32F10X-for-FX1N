#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "sio.h"
#include "port.h"
/*
USART2_TX  PA2   //6
USART2_RX  PA3   //6
USART3_TX  PB10  //6
USART3_RX  PB11  //6
USART1_TX  PA9   //6
USART1_RX  PA10  //6
*/
typedef struct {
    void (*read_complted)(int len);              //读完数据时的回调
    void (*write_complted)(void);                //发送完数据回调
    uint32_t readbuf_size;                       //表示读数据缓存区大小
} SIOT_PORTCfg;
static SIOT_PORTCfg _cfgs[] = {
    {
        NULL,                                         //read_complted
        NULL,                                         //write_complted
        0,                                            //readbuf_size
    },
    {
        NULL,                                         //read_complted
        NULL,                                         //write_complted
        0,                                            //readbuf_size
    },
    {NULL, NULL, 0}
};
typedef struct {
    USART_TypeDef *USART;                        //标识(USART1, USART2, USART3)
    DMA_Channel_TypeDef* DMAy_Channelx_Read;     //读数据的DMA channel
    DMA_Channel_TypeDef* DMAy_Channelx_Write;    //发送数据的DMA channel
} SIOT_USART_CFG;
const SIOT_USART_CFG _usarts[] = {
    {
        USART1,                                       //USART
        DMA1_Channel5,                                //DMAy_Channelx_Read
        DMA1_Channel4,                                //DMAy_Channelx_Write
    },
    {
        USART2, 
        DMA1_Channel6,                                //DMAy_Channelx_Read
        DMA1_Channel7,                                //DMAy_Channelx_Write
    },
    {USART3}
};
#define CHECK_PORT(port, r) 
//if((port != SIO_USART1) || (port != SIO_USART2) || (port != SIO_USART3)) {return r;}

#define USART1_DR_Base  (USART1_BASE+0x04)  //0x40013804
#define USART2_DR_Base  (USART2_BASE+0x04)

#define sio_en_write(p) {if(p == 1) {GPIO_SetBits(GPIOB,GPIO_Pin_1);}} 
#define sio_en_read(p)  {if(p == 1) {GPIO_ResetBits(GPIOB,GPIO_Pin_1);}}

static void sio_en_init(){
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);    //使能 PB,PE 端口时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;                //LED0-->PB.5 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;         //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;        //IO 口速度为 50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);                   //初始化 GPIOB.5
}

static void USART_GpioCfg(int port){
    typedef struct {
        uint16_t GPIO_PinTx;
        uint16_t GPIO_PinRx;
        GPIO_TypeDef *gpio;
    } SIOT_GPIOCfg;
    GPIO_InitTypeDef GPIO_InitStructure;
    const SIOT_GPIOCfg gcfg[] = { 
        {GPIO_Pin_9, GPIO_Pin_10, GPIOA}, 
        {GPIO_Pin_2, GPIO_Pin_3, GPIOA}, 
        {GPIO_Pin_10, GPIO_Pin_11, GPIOB} 
    };
    const SIOT_GPIOCfg *gc = &gcfg[port];

    /* USART_1 GPIO config */
    /* Configure USART_1 Tx (PA.09) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = gc->GPIO_PinTx;//GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(gc->gpio, &GPIO_InitStructure);    
    /* Configure USART_1 Rx (PA.10) as input floating */
    GPIO_InitStructure.GPIO_Pin = gc->GPIO_PinRx;//GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(gc->gpio, &GPIO_InitStructure);
}
/*
 * 函数名：USART_1_Config
 * 描述  ：USART1 GPIO 配置,工作模式配置。115200 8-N-1
 * 输入  ：无
 * 输出  : 无
 * 调用  ：在USART1_INIT()中被调用
 */
static void USART_Config(int port, usart_cfg *p) {
    const uint32_t rccs[3] = {
        RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, 
        RCC_APB1Periph_USART2 , //other RCC_APB2Periph_USART2 *****
        RCC_APB1Periph_USART3 | RCC_APB2Periph_GPIOB
    };

    USART_InitTypeDef USART_InitStructure;
    USART_ClockInitTypeDef  USART_ClockInitStructure;
    //SIOT_PORTCfg *cfg;
    USART_TypeDef *USART;

    //cfg = &_cfgs[port];
    USART = (&_usarts[port])->USART;

    /* config USART clock */ 
    if(port == 1) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
    }
    RCC_APB2PeriphClockCmd(rccs[port], ENABLE);//RCC_APB2Periph_USART_1 | RCC_APB2Periph_GPIOA, ENABLE);
    if(port == 1){
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE );
    }
    
    USART_GpioCfg(port);
    //add by youkebing
    USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;
    USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
    USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;
    USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;
    
    /* Configure the USART synchronous paramters */
    USART_ClockInit(USART, &USART_ClockInitStructure);

    /* USART_1 mode config */
    USART_InitStructure.USART_BaudRate = p->USART_BaudRate;// 19200;//115200;
    USART_InitStructure.USART_WordLength = p->USART_WordLength;// USART_WordLength_9b;
    USART_InitStructure.USART_StopBits = p->USART_StopBits;//USART_StopBits_1;
    USART_InitStructure.USART_Parity = p->USART_Parity;//USART_Parity_Even ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART, &USART_InitStructure);

    USART_ITConfig(USART, USART_IT_IDLE, DISABLE);  //打开串口空闲中断
    //USART_ITConfig(USART_1, USART_IT_TC, ENABLE); //
    USART_ITConfig(USART, USART_IT_TXE, DISABLE);
    USART_ITConfig(USART, USART_IT_RXNE, DISABLE);  
    USART_ITConfig(USART, USART_IT_TC, DISABLE); 
    USART_DMACmd(USART, USART_DMAReq_Tx, ENABLE);
    USART_DMACmd(USART, USART_DMAReq_Rx, ENABLE);

    USART_Cmd(USART, ENABLE);
}

/*
 * 函数名：USART1_DMA_Config
 * 描述  ：串口1的DMA 的初始化配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：在USART1_INIT()中被调用
 */
void USART_DMA_Config(int port) {
    const uint32_t drbases[] = { USART1_DR_Base, USART2_DR_Base, };
    DMA_InitTypeDef DMA_InitStructure;
    //SIOT_PORTCfg *cfg;
    DMA_Channel_TypeDef* channel;

    CHECK_PORT(port, );
    //cfg = &_cfgs[port];
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  //开启DMA时钟

    //配置串口1发送DMA
    channel = (&_usarts[port])->DMAy_Channelx_Write;
    DMA_DeInit(channel);//DMA1_Channel_4
    /*设置DMA源：内存地址&串口数据寄存器地址*/
    DMA_InitStructure.DMA_PeripheralBaseAddr = drbases[port];//USART1_DR_Base;     

    /*内存地址(要传输的变量的指针)*/
    //DMA_InitStructure.DMA_MemoryBaseAddr = (u32)Txd1Buffer;  //发送的内存地址任意，请参考发送函数
    
    /*方向：从内存到外设*/       
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  
    
    /*传输大小DMA_BufferSize=0*/    
    DMA_InitStructure.DMA_BufferSize = 0;            //发送的数据个数任意，请参考发送函数
    
    /*外设地址不增*/      
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 
    
    /*内存地址自增*/
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 
    
    /*外设数据单位*/  
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    
    /*内存数据单位 8bit*/
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;  
    
    /*DMA模式：一次传输，循环*/
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;   
    
    /*优先级：中*/   
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;  //DMA_Priority_VeryHigh;
    
    /*禁止内存到内存的传输    */
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

    DMA_Init(channel, &DMA_InitStructure); //DMA1_Channel_4
        
    /*配置DMA1的4通道*/           
    DMA_ITConfig(channel, DMA_IT_TC,ENABLE);   //DMA1_Channel_4配置DMA发送完成后产生中断
    DMA_Cmd (channel, DISABLE);//DMA1_Channel_4

    //配置串口1接收DMA
    channel = (&_usarts[port])->DMAy_Channelx_Read;
    /*设置DMA源：内存地址&串口数据寄存器地址*/
    DMA_DeInit(channel);//DMA1_Channe_l5);
    DMA_InitStructure.DMA_PeripheralBaseAddr = drbases[port];//USART1_DR_Base;     

    /*内存地址(要传输的变量的指针)*/
    //DMA_InitStructure.DMA_MemoryBaseAddr = (u32)Rcv1Buffer;  //接收缓存是固定不变的
    
    /*方向：从内存到外设*/       
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  
    
    /*传输大小DMA_BufferSize=BUFFSIZE*/ 
    //DMA_InitStructure.DMA_BufferSize = Rcv1_BUFFSIZE;     //必须这样设置
    
    /*外设地址不增*/      
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 
    
    /*内存地址自增*/
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 
    
    /*外设数据单位*/  
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    
    /*内存数据单位 8bit*/
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;  
    
    /*DMA模式：一次传输，循环*/
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;   

    /*优先级：中*/   
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; // DMA_Priority_VeryHigh; 
    
    /*禁止内存到内存的传输    */
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    
    /*配置DMA1的5通道*/         
    DMA_Init(channel, &DMA_InitStructure);  //DMA1_Channe_l5     
    DMA_ITConfig(channel, DMA_IT_TC,DISABLE);   //DMA接收不产生中断DMA1_Channe_l5
    DMA_Cmd(channel, ENABLE);//DMA1_Channe_l5
}

/*
 * 函数名：USART1_NVIC_Config
 * 描述  ：DMA 中断配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：在USART1_INIT()中被调用
 */
static void USART_NVIC_Config(int port) {
    const NVIC_InitTypeDef dmacfs[3] = {
        { DMA1_Channel4_IRQn, 1, 0, ENABLE },
        { DMA1_Channel7_IRQn, 1, 1, ENABLE },
        { DMA1_Channel4_IRQn, 1, 5, ENABLE }
    };
    const NVIC_InitTypeDef usartcfs[3] = {
        { USART1_IRQn, 1, 2, ENABLE },
        { USART2_IRQn, 1, 3, ENABLE },
        { USART1_IRQn, 1, 4, ENABLE }
    };

    NVIC_InitTypeDef NVIC_InitStructure;
  
    /* Configure one bit for preemption priority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
    /* 配置串口1 DMA发送 */
    NVIC_InitStructure.NVIC_IRQChannel = dmacfs[port].NVIC_IRQChannel; //DMA1_Channel4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = dmacfs[port].NVIC_IRQChannelPreemptionPriority;//1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = dmacfs[port].NVIC_IRQChannelSubPriority;//0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* 配置串口1 空闲中断接收*/
    NVIC_InitStructure.NVIC_IRQChannel = usartcfs[port].NVIC_IRQChannel;//USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = usartcfs[port].NVIC_IRQChannelPreemptionPriority;//1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = usartcfs[port].NVIC_IRQChannelSubPriority;//2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*
 * 函数名：USART1_SendOK_IQ
 * 描述  ：串口1发送数据完毕后，会调用的中断函数，将发送标志置0
 * 输入  ：无
 * 输出  : 无
 * 调用    ：此函数需在stm32f10x_it.c 的 DMA1_Channel4_IRQHandler()中被调用
 */
static void dma_write_interrupt(int port) {
    const uint32_t dma_flags[3] = {DMA1_FLAG_TC4, DMA1_FLAG_TC7, };
    const uint32_t dma_its[3] = {DMA1_IT_GL4, DMA1_IT_GL7, };
    unsigned int cpu_sr;
    //SIOT_PORTCfg *cfg;
    //cfg = &_cfgs[port];

    OS_ENTER_CRITICAL();
    //判断是否为DMA发送完成中断
    if(DMA_GetFlagStatus(dma_flags[port])==SET) {//DMA1_FLAG_TC4  
        DMA_ClearITPendingBit(dma_its[port]);//DMA1_IT_GL4
        DMA_ClearFlag(dma_flags[port]);//DMA1_FLAG_TC4
        DMA_Cmd ((&_usarts[port])->DMAy_Channelx_Write, DISABLE);      //DMA1_Channel_4
        USART_ITConfig((&_usarts[port])->USART, USART_IT_TC,ENABLE);  //USART_1
    }
    OS_EXIT_CRITICAL();
}
void DMA1_Channel4_IRQHandler(void) {
    dma_write_interrupt(0);
}
void DMA1_Channel7_IRQHandler(void) {
    dma_write_interrupt(1);
}
/*
 * 函数名：Usart1_RcvData_IQ
 * 描述  ：串口1收到数据，产生空闲中断，将接收标志置位，接收DAM通道恢复初始设置
 * 输入  ：无
 * 输出  : 无
 * 调用    ：此函数需在stm32f10x_it.c 的 USART1_IRQHandler()中被调用
 */
u32t sio_query_read(int port) {
    SIOT_PORTCfg *cfg;
    USART_TypeDef *USART;  
    DMA_Channel_TypeDef* channel;

    CHECK_PORT(port, );
    cfg = &_cfgs[port];
    USART = (&_usarts[port])->USART;
    channel = (&_usarts[port])->DMAy_Channelx_Read;

    if(USART_GetITStatus(USART, USART_IT_IDLE) != RESET) {
        //读串口接收缓存，清空闲中断标志
        USART_ReceiveData(USART);
    }
    return cfg->readbuf_size - channel->CNDTR;
}
static void usart_interrupt(int port) {
    unsigned int cpu_sr;

    SIOT_PORTCfg *cfg;
    USART_TypeDef *USART;
    cfg = &_cfgs[port];
    USART = (&_usarts[port])->USART;

    OS_ENTER_CRITICAL();
    if(USART_GetITStatus(USART, USART_IT_IDLE) != RESET) {
        USART_ReceiveData(USART);
        DMA_Cmd ((&_usarts[port])->DMAy_Channelx_Read,DISABLE);//DMA1_Channe_l5
        if (cfg->read_complted != NULL) {
            cfg->read_complted(cfg->readbuf_size - (&_usarts[port])->DMAy_Channelx_Read->CNDTR);//DMA1_Channe_l5
        }
    }
    if(USART_GetITStatus(USART, USART_IT_TC) != RESET) {  
        sio_en_read(port);
        //关闭发送完成中断  
        USART_ITConfig(USART,USART_IT_TC,DISABLE);  
        //发送完成  
        if (cfg->write_complted != NULL){
            cfg->write_complted();
        } 
    }   
    OS_EXIT_CRITICAL();
}
void USART1_IRQHandler(void) {
    usart_interrupt(0);
}
void USART2_IRQHandler(void) {
    usart_interrupt(1);
}
void sio_open(int port, usart_cfg *p) {  
    SIOT_PORTCfg *cfg;
    
    CHECK_PORT(port, );
    cfg = &_cfgs[port];
    
    cfg->read_complted = NULL;
    cfg->write_complted = NULL;
    
    USART_Config(port, p);

    USART_DMA_Config(port);
    USART_NVIC_Config(port);
    sio_en_init();
    sio_en_read(port);
}
void sio_close(int port) {
    CHECK_PORT(port, );
}

void sio_async_read(int port, unsigned char *buf, int len, void (*on)(int len)) {
    SIOT_PORTCfg *cfg;
    USART_TypeDef *USART;  
    DMA_Channel_TypeDef* channel;

    CHECK_PORT(port, );
    cfg = &_cfgs[port];
    USART = (&_usarts[port])->USART;
    channel = (&_usarts[port])->DMAy_Channelx_Read;

    cfg->read_complted = on;

    sio_en_read(port);
    if(USART_GetITStatus(USART, USART_IT_IDLE) != RESET) {
        //读串口接收缓存，清空闲中断标志
        USART_ReceiveData(USART);
    }
    DMA_Cmd(channel, DISABLE);                     //DMA1_Channel_5
    channel->CMAR =  (u32)buf;                     //Rcv1Buffer;DMA1_Channel_5
    channel->CNDTR =  len;                         //Rcv1_BUFFSIZE;DMA1_Channel_5
    cfg->readbuf_size = len;
    DMA_Cmd(channel, ENABLE);                      //DMA1_Channel_5
    if(on != NULL) {
        USART_ITConfig(USART, USART_IT_IDLE, ENABLE);  //打开串口空闲中断
    }
}

void sio_end_read(int port) {
    SIOT_PORTCfg *cfg;
    USART_TypeDef *USART;  

    CHECK_PORT(port, );
    cfg = &_cfgs[port];
    USART = (&_usarts[port])->USART;
    
    cfg->read_complted = NULL;
    DMA_Cmd((&_usarts[port])->DMAy_Channelx_Read, DISABLE);        //DMA1_Channel_5
    USART_ITConfig(USART, USART_IT_IDLE, DISABLE);
}

void sio_async_write(int port, const unsigned char *buf, int len, void (*on)(void)) {
    DMA_Channel_TypeDef* channel;

    CHECK_PORT(port, );
    channel = (&_usarts[port])->DMAy_Channelx_Write;
    
    sio_en_write(port);
    (&_cfgs[port])->write_complted = on;
    DMA_Cmd (channel,DISABLE);                     //DMA1_Channel_4
    channel->CMAR =  (u32)buf;                     //DMA1_Channel_4
    channel->CNDTR = len;                          //DMA1_Channel_4
    DMA_Cmd(channel, ENABLE);                      //DMA1_Channel_4使能DMA,开始发送
}
void sio_end_write(int port) {
    DMA_Channel_TypeDef* channel;
    USART_TypeDef *USART;
    
    CHECK_PORT(port, );
    channel = (&_usarts[port])->DMAy_Channelx_Write;
    USART = (&_usarts[port])->USART;

    (&_cfgs[port])->write_complted = NULL;
    DMA_Cmd(channel, DISABLE);                      //DMA1_Channel_4
    USART_ITConfig(USART, USART_IT_TC, DISABLE);    //
}
