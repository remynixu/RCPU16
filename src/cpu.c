#include "cpu.h"

#include <stdio.h>




static struct{
    uint16_t r[CPU_REGISTER_COUNT];
    uint16_t pc;
    uint16_t sp;
    uint16_t flags;
    uint16_t clock_hz;
}cpu;

void cpu_printstate(void){
    int i;
    for(i = 0; i < CPU_REGISTER_COUNT; i++){
        printf("r[%d] = %d \t", i, cpu.r[i]);
    }
    putchar('\n');
    printf("pc = %d    \t", cpu.pc);
    printf("sp = %d \t", cpu.sp);
    printf("flags = %d \t", cpu.flags);
    printf("hz = %d\n", cpu.clock_hz);
}



#define CPU_FLAGS_DBZ   (1 << 0)
#define CPU_FLAGS_INT   (1 << 1)



/* Video :< */
static struct video video;

static void video_irq(void){
    switch(video.header){
    case VIDEO_VGA:
        /*
         * C translation:
         * uint16_t video_vga(uint16_t i, uint16_t c);
         */
        video.buffer[cpu.r[1]] = cpu.r[2];
        break;
    case VIDEO_GPH:
        /* Not supported yet... */
        break;
    default:
        /* INVALID!!! AAAAAA */
        break;
    }
}



/*
 * The current architecture of this CPU, the RCPU, is 16-bit RISC Big-endian.
 *
 * Inspired by the Von Neumann architecture where where code and data lives in
 * the same RAM.
 */

#define INSTR_BYTESIZE  sizeof(uint16_t)

static void pc_increment(void){
    cpu.pc += INSTR_BYTESIZE;
}

/* b1 + b2 = b1, b2*/
static uint16_t to_big_endian16(byte b1, byte b2){
    return (((uint16_t)b1 << 8) | (uint16_t)b2);
}
/* RAM!!! */
static struct ram ram;

static uint16_t instr_fetch(void){
    return to_big_endian16(ram.memory[cpu.pc], ram.memory[cpu.pc + 1]);
}



#define OPERAND_AMOUNT  3

#define INSTR_FN_ARGS   byte opr0, byte opr1, byte opr2
#define INSTR_FN(name)  int name (INSTR_FN_ARGS)

/* The cycle counts we return are fake and simulated... */
typedef INSTR_FN((*instr_fn));

/* Nothing to do... */
INSTR_FN(handle_nop){
    (void)opr0;
    (void)opr1;
    (void)opr2;
    return 1;
}

/* Blindly expect the specified register to have a valid 16-bit aligned address. */
INSTR_FN(handle_jmp){
    (void)opr1;
    (void)opr2;
    cpu.pc = cpu.r[opr0];
    return 1;
}

/*
 * Loads a byte ((opr1 << 4) | opr2) to the upper 8 bits of a chosen
 * register (opr0):
 */
INSTR_FN(handle_ldu){
    cpu.r[opr0] = (uint16_t)((opr1 << 4) | opr2) << 8;
    return 1;
}

/*
 * Loads a byte ((opr1 << 4) | opr2) to the lower 8 bits of a chosen
 * register (opr0):
 */
INSTR_FN(handle_ldl){
    cpu.r[opr0] = (uint16_t)((opr1 << 4) | opr2);
    return 1;
}

/*
 * - 0x0 - load *opr1 (reg) ->  opr2 (reg)
 * - 0x1 - load  opr1 (reg) -> *opr2 (reg)
 * - 0x2 - load *opr1 (reg) -> *opr2 (reg)
 * - 0x3 - load  opr1 (reg) ->  opr2 (reg)
 */
INSTR_FN(handle_ldm){
    switch(opr0){
    case 0x0:
        cpu.r[opr2] = ram.memory[cpu.r[opr1]];
        break;
    case 0x1:
        ram.memory[cpu.r[opr2]] = cpu.r[opr1];
        break;
    case 0x2:
        ram.memory[cpu.r[opr2]] = ram.memory[cpu.r[opr1]];
        break;
    case 0x3:
        cpu.r[opr2] = cpu.r[opr1];
        break;
    default:
        /* THIS IS AN INVALID OPCODE! WE MUST DIE! */
        break;
    }
    return 2;
}

INSTR_FN(handle_irq){
    (void)opr0;
    (void)opr1;
    (void)opr2;
    switch(cpu.r[0]){
    /* instead of making a big switch block, soon it'll be a function table. */
    case 0x0:
        video_irq();
        break;
    default:
        /* THIS IS AN INVALID REQUEST CODE! WE MUST DIE! */
        break;
    }
    return 4;
}

instr_fn dispatch_table[16] = {
    handle_nop,
    handle_jmp,
    handle_ldu,
    handle_ldl,
    handle_ldm,
    handle_irq,
    NULL
};

static int instr_decode(uint16_t instr){
    byte opc;
    byte opr[OPERAND_AMOUNT];
    opc    = (instr >> 12) & 0xf;
    opr[0] = (instr >> 8) & 0xf;
    opr[1] = (instr >> 4) & 0xf;
    opr[2] = instr & 0xf;
    return dispatch_table[opc](opr[0], opr[1], opr[2]);
}



void cpu_init(struct cpu_config *config){
    cpu.flags    = 0;
    cpu.pc       = 0;
    cpu.sp       = UINT16_MAX;
    cpu.clock_hz = config->clock_hz;
    ram          = config->ram_slot[0];
    video        = config->video;
}

#include <time.h>

#define MHZ 1000000
#define cpu_microseconds_per_cycle(hz)  (MHZ / (hz))

static void delay_microseconds(int microseconds){
    clock_t target_ticks = clock() + (clock_t)((float)microseconds * ((float)CLOCKS_PER_SEC / (float)MHZ));
    while(clock() < target_ticks){
        volatile int __no_optimize = microseconds;
    }
}

static void cycle_delay(int cycles_spent){
    delay_microseconds(cycles_spent * cpu_microseconds_per_cycle(cpu.clock_hz));
}

void cpu_cycle(void){
    cycle_delay(instr_decode(instr_fetch()));
    pc_increment();
}