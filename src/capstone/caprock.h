#ifndef CAPROCK_H
#define CAPROCK_H
/* Caprock is wrapper around capstone */

#include <capstone/capstone.h>
#include <stddef.h>
#include <stdint.h>

#include "dbg.h"

int print_disasm(uint64_t addr, uint8_t *buf, size_t size);
cs_insn *next_instr(ctx_t *ctx, uint64_t addr);

#endif
