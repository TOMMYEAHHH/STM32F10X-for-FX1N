
#include "fx1niap.h"
#include "fx1nuart.h"
#include "stm32f10x_flash.h"


#define FLASH_START_POSITION   (0x8010000 - FX1N_FLASH_SIZE)
#define MAX_PAGE_SIZE   1024
#define MAX_PAGE_CNT    (FX1N_FLASH_SIZE / MAX_PAGE_SIZE)

static u32t _states[MAX_PAGE_CNT];

unsigned char IAPFlashReadMode(u32t addr);
void IAPFlashProgremMode(u32t addr, u8t data){
    if ((addr < PLCSoftwareVerAddr) || (addr >= CodeEndAddr)) {
        return;
    }
    if (*((u8t*)addr) != data) {
        u32t page = (addr - PLCSoftwareVerAddr) / MAX_PAGE_SIZE;
        if(_states[page] == 0) {
            _states[page] = 1;
        }
        *((u8t*)addr) = data;
    }
}
void IAPFlashErasureMode(u32t addr) {
    u32t i;
    for (i = 0; i< 512; i++) {
        *((u8t*)addr) = 0xff;
    }
}
unsigned char IAPFlashReadMode(u32t addr) {
    if ((addr < PLCSoftwareVerAddr) || (addr >= CodeEndAddr)) {
        return 0xff;
    }
    return *((const u8t*)addr);
}
void iap_write_array(u32t iapaddr, const u8t* buf, u32t length) {
    u32t i;
    for (i = 0; i < length; i++) {
        IAPFlashProgremMode(iapaddr++, *buf++);
    }
}
void iap_init(void) {
    u32t i;
    u16t * addr;

    addr = (u16t*)FLASH_START_POSITION;
    for (i = 0; i< FX1N_FLASH_SIZE / 2; i++) {
        flashbufs[i] = addr[i];
    }
    for (i = 0; i< MAX_PAGE_CNT; i++) {
        _states[i] = 0;
    }
}
static void iap_write_page_must(u32t page) {
    u16t *s;
    u16t *d;
    u32t i;

    page *= MAX_PAGE_SIZE;
    s = (u16t*)(page + PLCSoftwareVerAddr);
    d = (u16t*)(page + FLASH_START_POSITION);
    for (i = 0; i < MAX_PAGE_SIZE / 2; i++) {
        if(*s != *d) {
            FLASH_ProgramHalfWord((u32t)d, *s);
        }
        d++;
        s++;
    }
}

static void iap_write_page_check(u32t page) {
    u16t *s;
    u16t *d;
    u32t i;

    page *= MAX_PAGE_SIZE;
    s = (u16t*)(page + PLCSoftwareVerAddr);
    d = (u16t*)(page + FLASH_START_POSITION);
    for (i = 0; i < MAX_PAGE_SIZE / 2; i++) {
        if (*s != *d) {
            if (*d != 0xffff) {
                FLASH_ErasePage(page + FLASH_START_POSITION);
                return;
            }
        }
        s++;
        d++;
    }
}
static u32t index;
static u32t _fsm;
void iap_comit_(page) {
    //int cpu_sr;
    //OS_ENTER_CRITICAL();
    FLASH_Unlock();
    iap_write_page_check(page);
    iap_write_page_must(page);
    FLASH_Lock();
    //OS_EXIT_CRITICAL();
}
void iap_comit(void) {
    u32t i;
    static u32t state = 100;

    for (i = 0; i < MAX_PAGE_CNT; i++) {
        if(_states[i]) {
            _states[i] += 1;
        }
    }
    if (state == 0) {
        //
    }
    else {
        for (i = 0; i < MAX_PAGE_CNT; i++) {
            index++;
            if(index >= MAX_PAGE_CNT) {
                index = 0;
            }
            if(_states[index] > 10000) {
                _states[index] = 0;
                iap_comit_(index);
                return;
            }
        }  
    }
}

