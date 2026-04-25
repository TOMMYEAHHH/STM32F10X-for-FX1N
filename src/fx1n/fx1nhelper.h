#ifndef _FX1NHELPER_H
#define _FX1NHELPER_H

#include "fx1ntype.h"

u32t hextoasc(u8t data);
u8t asctohex(u8t *buf);
u8t ascto0F(u8t data);
s32t str_cmp(const u8t *s, const u8t *d, u32t length);
void str_copy(const u8t *s, u8t *d, u32t length);
void str_clr(u8t *s, u32t length);
void str_set_bit(u8t *s, u32t index);

void u8t_bit_reverse(u8t *d, u32t i);
void u8t_bit_set_one(u8t *d, u32t i);
void u8t_bit_set_zero(u8t *d, u32t i);
//void u8t_bit_set(u8t *d, u32t mask, u32t v);
#endif
