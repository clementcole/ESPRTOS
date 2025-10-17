#ifndef _TASK_H_
#define _TASK_H_

#include <stdint.h>

typedef enum{
    TASK_READY,
    TASK_RUNNING,
    TASK_BLOCKED,
    TASK_SUSPENDED
}task_state_t;


typedef struct task_control_block {
    //Context save area
    uint32_t *sp; //stack pointer
    uint32_t *stack_start; //Stack head
    uint32_t stack_size;  //Stack size

    //Metadata
    char name[16];
    task_state_t state;
    uint32_t priority; 
    uint32_t ticks_to_delay;

    //Linked list
    struct task_control_block *next; 
    struct task_control_block *prev;
}tcb_t;


// Task function prototype
typedef void (*task_function_t)(void*);

// Public API
void task_create(task_function_t func, void *arg, uint32_t *stack, 
                 uint32_t stack_size, char *name, uint32_t priority); 


void task_yeild(void);

void task_delay(uint32_t ticks);

#endif