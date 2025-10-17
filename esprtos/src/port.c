// src/port.c
#include "port.h"
#include "rtos.h"

// Timer interrupt handler for preemption
void IRAM_ATTR timer_isr(void *arg) 
{
    // Clear interrupt
    TIMER1_INT_CLR |= 1;
    
    // Call RTOS tick
    tick();
}

void port_init_timer(void) 
{
    // Configure timer1 for 1ms ticks
    TIMER1_CTRL = 0;
    TIMER1_LOAD = (APB_CLK_FREQ / 1000); // 1ms interval
    TIMER1_CTRL = TIMER1_CTRL_ENABLE | TIMER1_CTRL_AUTO_RELOAD;
    
    // Enable timer interrupt
    TIMER1_INT_ENA |= 1;
    _xtos_set_interrupt_handler(ETS_TIMER1_INUM, timer_isr, NULL);
    _xtos_ints_on(1 << ETS_TIMER1_INUM);
}