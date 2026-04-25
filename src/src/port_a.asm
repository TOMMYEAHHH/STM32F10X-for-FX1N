
    EXPORT  OS_CPU_SR_Save                                      ; Functions declared in this file
    EXPORT  OS_CPU_SR_Restore

;********************************************************************************************************
;                                                EQUATES
;********************************************************************************************************

;NVIC_INT_CTRL   EQU     0xE000ED04                              ; Interrupt control state register.
;NVIC_SYSPRI14   EQU     0xE000ED22                              ; System priority register (priority 14).
;NVIC_PENDSV_PRI EQU           0xFF                              ; PendSV priority value (lowest).
;NVIC_PENDSVSET  EQU     0x10000000                              ; Value to trigger PendSV exception.

;********************************************************************************************************
;                                      CODE GENERATION DIRECTIVES
;********************************************************************************************************

;        RSEG CODE:CODE:NOROOT(2)
    AREA |.text|, CODE, READONLY, ALIGN=2   ;AREA |.text| ѡձ׎ |.text|c
                                            ;CODEҭʾպë׎ìREADONLYҭʾֻׁèȱʡé
                                            ;ALIGN=2ҭʾ4ؖޚהǫcɴALIGN=nì֢2^nהǫ
    THUMB                                   ;Thumb պë
    REQUIRE8                                ;ָ֨ձǰτݾҪȳבջыؖޚהǫ
    PRESERVE8                               ;®ָ֨ձǰτݾѣԖבջыؖޚהǫ


OS_CPU_SR_Save
    MRS     R0, PRIMASK                                         ; Set prio int mask to mask all (except faults)
    CPSID   I
    BX      LR

OS_CPU_SR_Restore
    MSR     PRIMASK, R0
    BX      LR

    END
