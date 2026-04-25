#ifndef __PORT_H
#define __PORT_H

#ifndef NULL
#define NULL        ((void *) 0)
#endif

typedef char               s8t;
typedef unsigned char      u8t;
typedef short              s16t;
typedef unsigned short     u16t;
typedef int                s32t;
typedef unsigned int       u32t;


#define  OS_ENTER_CRITICAL()  {cpu_sr = OS_CPU_SR_Save();}
#define  OS_EXIT_CRITICAL()   {OS_CPU_SR_Restore(cpu_sr);}

int OS_CPU_SR_Save(void);
void OS_CPU_SR_Restore(int cpu_sr);

#endif
