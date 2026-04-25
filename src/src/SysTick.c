#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "SysTick.h"
void fn1n_on10ms(void);
static unsigned int _tick;
void SysTickInit(void) {
    /* SystemFrequency / 1000 1ms 中断一次
    * SystemFrequency / 100000 10us 中断一次
    * SystemFrequency / 1000000 1us 中断一次
    */
    // if (SysTick_Config(SystemFrequency / 100000)) // ST3.0.0 库版本
    if (SysTick_Config(SystemCoreClock / 1000)) {// ST3.5.0 库版本
        /* Capture error */
        while (1);
    }
    // 关闭滴答定时器
    //SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;
    _tick = 0;
}
/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void) {
    _tick++;
    fn1n_on10ms();
    //GPIO_ResetBits(GPIOA,GPIO_Pin_0);
} 

void timer_init(struct timer *t) {
    t->cnt = _tick;
}
unsigned int timer_expired(struct timer *t, int dlyms) {
    int d = _tick - t->cnt;
    return (d >= dlyms) || (d <= -dlyms);
}
/*
void timer_poll(void) {
    static unsigned int t;
    unsigned int ii, i;
    ii = _tick - t;
    if (ii > 100) {
        t = _tick;
        return;
    }
    for (i= 0; i< ii; i++) {
        t++;
        fn1n_on10ms();
    }
}
*/
