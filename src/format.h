#ifndef FORMAT_H
#define FORMAT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

struct bFLT_header {
  char magic[4];
  uint32_t version;
  uint32_t entry;
  uint32_t data_start;
  uint32_t data_end;
  uint32_t bss_end;
  uint32_t stack_size;
  uint32_t reloc_start;
  uint32_t reloc_count;
  uint32_t flags;
  uint32_t build_date;
  uint32_t padding[5];
};

/**
 * reads a bFLT header
 */
struct bFLT_header read_header(FILE *fd);

/**
 * validate the magic, the entry point
 */
void validate_header(struct bFLT_header *header);

#endif // FORMAT_H
