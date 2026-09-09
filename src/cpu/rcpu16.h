#ifndef RCPU16_H

/*
 * This header provides the functionality of an rcpu16 CPU, a dream CPU made by
 * remynixu: https://github.com/remynixu
 * 
 * This emulated CPU is inspired by 16-bit RISC with Von Neumann architecture,
 * though it only has a single core.
 */
#define RCPU16_H

/* The CPU's byte. */
#ifndef rcpu16_byte
#define rcpu16_byte unsigned char
#endif /* rcpu16_byte */

/* The NULL value used by this header and implementation. */
#ifndef rcpu16_NULL
#define rcpu16_NULL ((void *)0)
#endif /* rcpu16_NULL */

/*
 * The maximum address the RCPU can access.
 */
#define rcpu16_MAXADDR  (unsigned short)(0xffff)

/*
 * Configurations:
 * - memory - is expected to be the size of rcpu16_MAXADDR.
 * 
 * Setting a member to zero or NULL will invoke the default behavior of that
 * member. Alternatively, passing NULL as argument will invoke all default
 * behaviors.
 */
struct rcpu16_config{
    rcpu16_byte *memory;
};

/*
 * Configures the CPU. Only expected to be called before any call to
 * `rcpu16_cycle()`.
 */
void rcpu16_configure(struct rcpu16_config *config);

/*
 * This CPU only does three things repeatedly:
 * -> fetch -> decode -> execute
 */
int rcpu16_cycle(void);

#endif /* RCPU16_H */