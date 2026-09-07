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

struct cpu_config{
    /*
     * We currently only support one RAM. We also expect a ram that already has
     * instructions (16-bits aligned) within it starting from 0x0000.
     */
    byte *ram;
    uint16_t ram_size;
};

void cpu_start(struct cpu_config *config);

#define CPU_REGISTER_COUNT  16

struct cpu_state{
    uint16_t r[CPU_REGISTER_COUNT];
    uint16_t pc;
    uint16_t sp;
    uint16_t flags;
};

struct cpu_state cpu_get(void);

#endif /* CPU_H */