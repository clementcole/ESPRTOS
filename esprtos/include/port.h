#ifndef _PORT_H_
#define _PORT_H_

#include <stdint.h>
#include <stdbool.h>

// ESP8266 Xtensa LX106 specific definitions
#define XTENSA_LX106

// Memory mapped registers for ESP8266
#define PERIPHS_BASE          0x60000000
#define DPORT_BASE            0x3FF00000

// Timer registers (Timer1 for system tick)
#define TIMER1_BASE           (PERIPHS_BASE + 0x60000)
#define TIMER1_LOAD           (*(volatile uint32_t*)(TIMER1_BASE + 0x00))
#define TIMER1_COUNT          (*(volatile uint32_t*)(TIMER1_BASE + 0x04))
#define TIMER1_CTRL           (*(volatile uint32_t*)(TIMER1_BASE + 0x08))
#define TIMER1_INT            (*(volatile uint32_t*)(TIMER1_BASE + 0x0C))
#define TIMER1_INT_CLR        (*(volatile uint32_t*)(TIMER1_BASE + 0x10))
#define TIMER1_INT_ENA        (*(volatile uint32_t*)(TIMER1_BASE + 0x14))

// Timer control bits
#define TIMER1_CTRL_ENABLE    (1 << 0)
#define TIMER1_CTRL_AUTO_RELOAD (1 << 1)

// Interrupt controller
#define INT_ENA               (*(volatile uint32_t*)(DPORT_BASE + 0x00C))
#define INT_STATUS            (*(volatile uint32_t*)(DPORT_BASE + 0x010))

// Interrupt numbers
#define ETS_TIMER1_INUM       6

// GPIO registers
#define GPIO_BASE             (PERIPHS_BASE + 0x30000)
#define GPIO_OUT              (*(volatile uint32_t*)(GPIO_BASE + 0x00))
#define GPIO_OUT_SET          (*(volatile uint32_t*)(GPIO_BASE + 0x04))
#define GPIO_OUT_CLEAR        (*(volatile uint32_t*)(GPIO_BASE + 0x08))
#define GPIO_ENABLE           (*(volatile uint32_t*)(GPIO_BASE + 0x0C))
#define GPIO_IN               (*(volatile uint32_t*)(GPIO_BASE + 0x10))

// Pin function select
#define PERIPHS_IO_MUX        0x60000800
#define PERIPHS_IO_MUX_GPIO2  (*(volatile uint32_t*)(PERIPHS_IO_MUX + 0x38))

// Function select bits
#define FUNC_GPIO2            0x00

// UART registers (for debugging)
#define UART0_BASE            (PERIPHS_BASE + 0x50000)
#define UART_FIFO             (*(volatile uint32_t*)(UART0_BASE + 0x00))
#define UART_STATUS           (*(volatile uint32_t*)(UART0_BASE + 0x1C))
#define UART_CLKDIV           (*(volatile uint32_t*)(UART0_BASE + 0x14))

// UART status bits
#define UART_STATUS_TXFULL    (1 << 1)

// System clock frequency
#define APB_CLK_FREQ          80000000  // 80 MHz

// Stack alignment
#define PORT_STACK_ALIGNMENT  16

// Initial stack pattern for stack overflow detection
#define PORT_STACK_INIT_PATTERN 0xDEADBEEF

// Context frame structure - matches what we save in assembly
typedef struct port_context_frame {
    uint32_t a0;    // Return address
    uint32_t a1;    // Stack pointer
    uint32_t a2;
    uint32_t a3;
    uint32_t a4;
    uint32_t a5;
    uint32_t a6;
    uint32_t a7;
    uint32_t a8;
    uint32_t a9;
    uint32_t a10;
    uint32_t a11;
    uint32_t a12;
    uint32_t a13;
    uint32_t a14;
    uint32_t a15;
    uint32_t sar;   // Shift amount register
    uint32_t pc;    // Program counter
    uint32_t ps;    // Processor state
} port_context_frame_t;

// ============================================================================
// Assembly Function Declarations
// ============================================================================

/**
 * @brief Perform a context switch
 * 
 * @param old_sp Pointer to where to save current stack pointer
 * @param new_sp New stack pointer to switch to
 * 
 * This function saves the current context and restores the new context.
 * It's called from the scheduler when switching tasks.
 */
extern void port_context_switch(uint32_t **old_sp, uint32_t *new_sp);

/**
 * @brief Start the first task
 * 
 * @param sp Stack pointer of the first task to run
 * 
 * This function sets up the initial context and starts the first task.
 * It never returns.
 */
extern void port_start_first_task(uint32_t *sp);

/**
 * @brief Enter critical section
 * 
 * @return Previous interrupt state to be restored later
 * 
 * Raises interrupt level to disable most interrupts.
 */
extern uint32_t port_enter_critical(void);

/**
 * @brief Exit critical section
 * 
 * @param state Previous interrupt state to restore
 */
extern void port_exit_critical(uint32_t state);

/**
 * @brief Yield to scheduler
 * 
 * Triggers a context switch to the next ready task.
 */
extern void port_yield(void);

// ============================================================================
// C Function Declarations  
// ============================================================================

/**
 * @brief Initialize hardware timer for RTOS tick
 * 
 * Configures Timer1 to generate periodic interrupts for task preemption.
 */
void port_init_timer(void);

/**
 * @brief Initialize UART for debugging output
 */
void port_init_uart(void);

/**
 * @brief Put character to UART (blocking)
 * 
 * @param c Character to send
 */
void port_putc(char c);

/**
 * @brief Get character from UART (non-blocking)
 * 
 * @return Character received, or -1 if no data
 */
int port_getc(void);

/**
 * @brief Initialize task stack with initial context
 * 
 * @param stack_top Top of the stack (highest address)
 * @param task_func Task function pointer
 * @param task_arg Task argument
 * @return Stack pointer value for the task
 * 
 * This function sets up the initial stack frame so that when the task
 * is scheduled for the first time, it starts executing at task_func.
 */
uint32_t* port_init_stack(uint32_t *stack_top, void (*task_func)(void*), void *task_arg);

/**
 * @brief Check for stack overflow
 * 
 * @param stack_start Start of stack memory
 * @param stack_size Size of stack in bytes
 * @return true if stack overflow detected, false otherwise
 * 
 * Checks the stack pattern for corruption.
 */
bool port_check_stack_overflow(uint32_t *stack_start, uint32_t stack_size);

/**
 * @brief Get free heap memory
 * 
 * @return Number of free bytes in heap
 * 
 * Useful for memory management and debugging.
 */
uint32_t port_get_free_heap(void);

/**
 * @brief System reset
 * 
 * Performs a soft reset of the ESP8266.
 */
void port_system_reset(void);

// ============================================================================
// Inline Utility Functions
// ============================================================================

/**
 * @brief Disable interrupts
 * 
 * @return Previous interrupt state
 */
static inline uint32_t port_disable_interrupts(void) {
    return port_enter_critical();
}

/**
 * @brief Enable interrupts
 */
static inline void port_enable_interrupts(void) {
    port_exit_critical(0);
}

/**
 * @brief Restore interrupt state
 * 
 * @param state Previous state from port_disable_interrupts()
 */
static inline void port_restore_interrupts(uint32_t state) {
    port_exit_critical(state);
}

/**
 * @brief No-operation delay
 */
static inline void port_nop(void) 
{
    asm volatile ("nop");
}

/**
 * @brief Memory barrier
 */
static inline void port_memory_barrier(void) 
{
    asm volatile ("memw");
}

// ============================================================================
// Debugging Macros
// ============================================================================

#ifdef PORT_DEBUG
#define PORT_ASSERT(condition) \
    do { \
        if (!(condition)) { \
            port_printf("ASSERT: %s:%d %s\n", __FILE__, __LINE__, #condition); \
            port_system_reset(); \
        } \
    } while(0)

#define PORT_LOG(fmt, ...) \
    port_printf("PORT: " fmt, ##__VA_ARGS__)
#else
#define PORT_ASSERT(condition) ((void)0)
#define PORT_LOG(fmt, ...) ((void)0)
#endif

// ============================================================================
// Utility Functions
// ============================================================================

/**
 * @brief Formatted printf to UART
 * 
 * @param fmt Format string
 * @param ... Arguments
 */
void port_printf(const char *fmt, ...);

/**
 * @brief Dump stack contents for debugging
 * 
 * @param sp Stack pointer
 * @param words Number of words to dump
 */
void port_dump_stack(uint32_t *sp, int words);

/**
 * @brief Dump registers for debugging
 * 
 * @param frame Pointer to context frame
 */
void port_dump_registers(port_context_frame_t *frame);

#endif /* _PORT_H_ */