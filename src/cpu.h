#ifndef CPU_H

/*
 * This header only serves as an interface for starting the emulated 16-bit CPU.
 * The callers aren't supposed to know what the CPU is doing :>
 */
#define CPU_H

#include <stdint.h>

#ifndef NULL
#define NULL    (void *)0
#endif /* NULL */

/* 8-bit byte */
typedef uint8_t byte;

/*
 * An intruction is expected to be aligned by 16-bits and structured like this:
 *
 * 0000   | 0000 0000 0000
 * opcode | operands
 * 
 * opcodes starting from 0000:
 * 
 * NOP [0x0] [0x0] [0x0]    ; No operation.
 * JMP [REG] [0x0] [0x0]    ; Jump to the address stored in register.
 * LDU [REG] [HEX] [HEX]    ; Load a byte to the upper 8 bits of a register.
 * LDL [REG] [HEX] [HEX]    ; Load a byte to the lower 8 bits of a register.
 * LDM [MOD] [REG] [REG]    ; Loads from or to a pointer or register.
 *                          ; Note: '*' means dereference.
 *                          ; MOD | DESCRIPTION
 *                          ; 0x0 - load *opr1 (reg) ->  opr2 (reg)
 *                          ; 0x1 - load  opr1 (reg) -> *opr2 (reg)
 *                          ; 0x2 - load *opr1 (reg) -> *opr2 (reg)
 *                          ; 0x3 - load  opr1 (reg) ->  opr2 (reg)
 *
 * Note: REG, HEX, and MOD are all 0x0–0xf, but REG refers to the register to
 *       use; MOD and HEX refers to a constant number.
 */

#define RAM_MAXSIZE UINT16_MAX

struct  cpu_config{
    /*
     * We currently only support one RAM. We also expect a ram that already has
     * instructions (16-bits aligned) within it starting from 0x0000.
     */
    byte *ram;
    uint16_t ram_size;
    /*
     * Our clock speed.
     */
    uint16_t clock_hz;
};

void    cpu_init(struct cpu_config *config);
void    cpu_cycle(void);

#define CPU_REGISTER_COUNT  4

struct cpu_state{
    uint16_t r[CPU_REGISTER_COUNT];
    uint16_t pc;
    uint16_t sp;
    uint16_t flags;
    uint16_t clock_hz;
};

void    cpu_printstate(void);

#define MHZ 1000000
#define cpu_microseconds_per_cycle(hz)  (MHZ / (hz))

#endif /* CPU_H */