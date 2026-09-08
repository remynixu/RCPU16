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
 * IRQ [0x0] [0x0] [0x0]    ; Calls an 'interrupt' function that automatically 
 *                          ; invokes arguments like so:
 *                          ; r3 irq[r0](r1, r2)
 *
 * Note: REG, HEX, and MOD are all 0x0–0xf, but REG refers to the register to
 *       use; MOD and HEX refers to a constant number.
 * 
 * Currently available interrupts:
 * - 0x0 - Writes to the ASCII video buffer; C translation:
 *       - uint16_t video_vga(uint16_t i, uint16_t c);
 */

#define RAM_MAXSIZE UINT16_MAX
#define MAX_RAMSLOT 16

struct ram{
    byte memory[RAM_MAXSIZE];
};

/*
 * Headers for a video, defining how it should be treated and used.
 */
enum video_header{
    VIDEO_VGA = 0,  /* VGA-esque behavior. */

    /* CURRENTLY UNIMPLEMENTED: */
    VIDEO_GPH,      /* Graphical mode, supports RGBA pixels. */
};

struct video{
    byte header;
    byte *buffer;
};

struct  cpu_config{
    /*
     * We now support more than one RAM. We also expect a ram that already has
     * instructions (16-bits aligned) within it starting from 0x0000.
     */
    struct ram ram_slot[MAX_RAMSLOT];
    uint8_t ram_count;
    /*
     * Our clock speed.
     */
    uint16_t clock_hz;
    /*
     * We currently only support VGA-esque display; we don't have real graphics.
     * A buffer that holds ASCII characters.
     */
    struct video video;
};

void    cpu_init(struct cpu_config *config);
void    cpu_cycle(void);

#define CPU_REGISTER_COUNT  4

void    cpu_printstate(void);

#endif /* CPU_H */