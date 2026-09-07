#include "cpu.h"

#include <stdio.h>
#include <string.h>

static byte ram[RAM_MAXSIZE] = {
    0x00, 0x00, /* NOP */
    0x30, 0x04, /* LDL */
    0x00, 0x00, /* NOP */
    0x10, 0x00, /* JMP */
};

int main(void){
    struct cpu_config config;
    {
        config.clock_hz = 1;
        config.ram = ram;
        config.ram_size = RAM_MAXSIZE;
    }
    cpu_init(&config);
    puts("CURRENT CPU STATE:");
    cpu_printstate();
    while(1){
        cpu_cycle();
        cpu_printstate();
    }
    return 0;
}