// src/task.c
#include "rtos.h"
#include "port.h"
#include <string.h>

static tcb_t *current_task = NULL;
static tcb_t *ready_list = NULL;
static tcb_t task_table[MAX_TASKS];
static uint32_t task_count = 0;

// Idle task
static void idle_task(void *arg) {
    while(1) {
        // Sleep or enter low power mode
        asm volatile ("waiti 0");
    }
}

// Initialize task stack with context
static void task_init_stack(tcb_t *tcb, task_function_t func, void *arg) {
    // Stack grows downward, so start at top
    uint32_t *sp = (uint32_t*)((uint8_t*)tcb->stack_start + tcb->stack_size - sizeof(uint32_t));
    
    // Simulate stack frame for context switch
    // Registers in the order they'll be restored
    *--sp = (uint32_t)0x00000000; // PC (will be set later)
    *--sp = (uint32_t)0x00000000; // PS
    *--sp = (uint32_t)0x00000000; // A0
    *--sp = (uint32_t)0x00000000; // A1
    // ... initialize all registers
    
    // Set entry point
    sp[16] = (uint32_t)func; // PC
    sp[17] = (uint32_t)arg;  // A1 as argument
    
    tcb->sp = sp;
}

void task_create(task_function_t func, void *arg, uint32_t *stack, 
                 uint32_t stack_size, char *name, uint32_t priority) {
    if (task_count >= MAX_TASKS) return;
    
    tcb_t *tcb = &task_table[task_count++];
    
    tcb->stack_start = stack;
    tcb->stack_size = stack_size;
    tcb->state = TASK_READY;
    tcb->priority = priority;
    tcb->ticks_to_delay = 0;
    strncpy(tcb->name, name, sizeof(tcb->name)-1);
    
    // Initialize stack
    task_init_stack(tcb, func, arg);
    
    // Add to ready list
    tcb->next = ready_list;
    if (ready_list) ready_list->prev = tcb;
    ready_list = tcb;
    tcb->prev = NULL;
}