#ifndef __SIO_H
#define __SIO_H

#include "port.h"

typedef struct {
  u32t NVIC_IRQChannel;
  uint8_t NVIC_IRQChannelPreemptionPriority; 
  uint8_t NVIC_IRQChannelSubPriority;
  //FunctionalState NVIC_IRQChannelCmd;                                                   This parameter can be set either to ENABLE or DISABLE */   
} SIO_PORTCfg;

typedef struct {
  uint32_t USART_BaudRate;
  uint16_t USART_WordLength;
  uint16_t USART_StopBits;
  uint16_t USART_Parity;
  uint16_t USART_Mode;
  uint16_t USART_HardwareFlowControl;
} usart_cfg;

#define SIO_USART1      0
#define SIO_USART2      1
#define SIO_USART3      2

void sio_open(int port, usart_cfg *p);
void sio_close(int port);

void sio_async_read(int port, unsigned char *buf, int len, void (*on)(int len));
u32t sio_query_read(int port);
void sio_end_read(int port);
void sio_async_write(int port, const unsigned char *buf, int len, void (*on)(void));
void sio_end_write(int port);

#endif
