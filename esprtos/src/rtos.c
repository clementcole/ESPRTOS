// src/rtos.c
#include "rtos.h"
#include "port.h"

static volatile uint32_t tick_count = 0;
static tcb_t *current_task = NULL;
static tcb_t idle_task_tcb;
static uint32_t idle_task_stack[IDLE_TASK_STACK_SIZE];

// Simple round-robin scheduler
static tcb_t* scheduler(void) {
    tcb_t *task = ready_list;
    tcb_t *next_task = NULL;
    tcb_t *highest_priority = NULL;
    
    // Find highest priority ready task
    while (task) {
        if (task->state == TASK_READY) {
            if (!highest_priority || task->priority > highest_priority->priority) {
                highest_priority = task;
            }
        }
        task = task->next;
    }
    
    return highest_priority ? highest_priority : &idle_task_tcb;
}

void rtos_tick(void) {
    tick_count++;
    
    // Update delayed tasks
    tcb_t *task = ready_list;
    while (task) {
        if (task->state == TASK_BLOCKED && task->ticks_to_delay > 0) {
            task->ticks_to_delay--;
            if (task->ticks_to_delay == 0) {
                task->state = TASK_READY;
            }
        }
        task = task->next;
    }
    
    // Trigger context switch if needed
    if (current_task != scheduler()) {
        port_context_switch();
    }
}

void task_delay(uint32_t ticks) {
    ENTER_CRITICAL();
    
    current_task->ticks_to_delay = ticks;
    current_task->state = TASK_BLOCKED;
    
    // Force context switch
    port_context_switch();
    
    EXIT_CRITICAL();
}

void rtos_init(void) {
    // Initialize idle task
    task_create(idle_task, NULL, 
                idle_task_stack, sizeof(idle_task_stack),
                "idle", 0);
    
    // Initialize hardware timer for preemption
    port_init_timer();
}

void rtos_start(void) {
    // Start the first task
    current_task = scheduler();
    port_start_first_task(current_task);
}