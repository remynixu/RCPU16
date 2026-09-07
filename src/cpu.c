#include "cpu.h"



static struct cpu_state cpu;
 
struct cpu_state cpu_get(void){
    return cpu;
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
static struct{
    byte *memory;
    uint16_t size;
}ram;

static uint16_t instr_fetch(void){
    return to_big_endian16(ram.memory[cpu.pc], ram.memory[cpu.pc + 1]);
}

/*
 * An intruction is expected to be structured like this:
 *
 * 0000   | 0000 0000 0000
 * opcode | operands
 * 
 * opcodes starting from 0000:
 * 
 * NOP [0x0] [0x0] [0x0]
 * JMP [REG] [0x0] [0x0]
 * LDR [REG] [HEX] [HEX]
 * LDM [MOD] [REG] [REG] 
 *
 * Note: REG, HEX, and MOD are all 0x0–0xf, but REG refers to the register to
 *       use; HEX refers to a constant number.
 */

#define OPERAND_AMOUNT  3

#define INSTR_FN_ARGS   byte opr0, byte opr1, byte opr2
#define INSTR_FN(name)  void name (INSTR_FN_ARGS)

typedef void (*instr_fn)(INSTR_FN_ARGS);

/* Nothing to do... */
INSTR_FN(handle_nop){
    (void)opr0;
    (void)opr1;
    (void)opr2;
}

/* Blindly expect the specified register to have a valid 16-bit aligned address. */
INSTR_FN(handle_jmp){
    (void)opr1;
    (void)opr2;
    cpu.pc = cpu.r[opr0];
}

/*
 * Loads a byte ((opr1 << 4) | opr2) to a chosen register (opr0):
 */
INSTR_FN(handle_ldr){
    cpu.r[opr0] = (opr1 << 4) | opr2;
}

/*
 * Loads from or to a pointer or register.
 * [The '*' means dereference; load uses the value in oprn as a memory address]
 * - 0x0 - load *opr1 (reg) ->  opr2 (reg)
 * - 0x1 - load  opr1 (reg) -> *opr2 (reg)
 * - 0x2 - load *opr1 (reg) -> *opr2 (reg)
 * - 0x3 - load  opr1 (reg) ->  opr2 (reg)
 *
 */
INSTR_FN(handle_ldm){
    switch(opr0){
        case 0x0:{
            cpu.r[opr2] = ram.memory[cpu.r[opr1]];
            break;
        }
        case 0x1:{
            ram.memory[cpu.r[opr2]] = cpu.r[opr1];
            break;
        }
        case 0x2:{
            ram.memory[cpu.r[opr2]] = ram.memory[cpu.r[opr1]];
            break;
        }
        case 0x3:{
            cpu.r[opr2] = cpu.r[opr1];
            break;
        }
        default:{
            /* THIS IS AN INVALID OPCODE! WE MUST DIE! */
            break;
        }
    }
}

instr_fn dispatch_table[16] = {
    handle_nop,
    handle_jmp,
    handle_ldm,
    handle_ldr,
    NULL
};

static void instr_decode(uint16_t instr){
    byte opc;
    byte opr[OPERAND_AMOUNT];
    opc     = (instr >> 12) & 0xf;
    opr[0]  = (instr >> 8) & 0xf;
    opr[1]  = (instr >> 4) & 0xf;
    opr[2]  = instr & 0xf;
    dispatch_table[opc](opr[0], opr[1], opr[2]);
}



static void cpu_init(struct cpu_config *config){
    ram.memory  = config->ram;
    ram.size    = config->ram_size;
    cpu.flags   = 0;
    cpu.pc      = 0;
    cpu.sp      = UINT16_MAX;
}

void cpu_start(struct cpu_config *config){
    cpu_init(config);
    while(1){
        instr_decode(instr_fetch());
        pc_increment();
    }
}