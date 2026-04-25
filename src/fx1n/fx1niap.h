#ifndef _PLCIAP_H_
#define _PLCIAP_H_

#include "fx1ntype.h"

void IAPFlashProgremMode(u32t addr, u8t data);
void IAPFlashErasureMode(unsigned int pageaddr);
unsigned char IAPFlashReadMode(unsigned int codeaddr);
void iap_write_array(u32t addr, const u8t* buf, u32t length);
void iap_comit(void);
#endif
