; Architecture specific code for esp8266
; src/port_asm.S
.section .text

.global port_context_switch
.global port_start_first_task
.global port_enter_critical
.global port_exit_critical

// Save current context and switch to new task
port_context_switch:
    // a2 = pointer to current TCB's sp
    // a3 = pointer to new TCB's sp
    
    // Save current context
    movi a0, 0x3fffff00  // Save area in ROM
    s32i a1, a0, 0       // Save SP
    s32i a2, a0, 4
    // ... save all registers
    
    // Switch to new task
    l32i a1, a3, 0       // Load new SP
    // ... restore all registers
    
    ret

// Start the first task
port_start_first_task:
    // a2 = pointer to first TCB
    l32i a1, a2, 0       // Load initial SP
    // ... restore registers
    ret

port_enter_critical:
    rsil a0, 3           // Raise interrupt level to 3
    ret

port_exit_critical:
    wsr a0, ps           // Restore previous interrupt level
    ret