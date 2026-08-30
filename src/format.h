#ifndef FORMAT_H
#define FORMAT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define SAFE_SEEK(fd, __offset, __whence)            \
  while (fseek((fd), (__offset), (__whence)) != 0) { \
    perror("seek failed");                           \
    exit(EXIT_FAILURE);                              \
  }

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

_Static_assert(sizeof(struct bFLT_header) == 64, "a flat binary header must be 64 bytes");

struct bflt_reloc {
  uint32_t offset;
};

struct bflt_reloc_table {
  struct bflt_reloc *ptr;
  size_t count;
};

/**
 * reads a bFLT header
 */
struct bFLT_header read_header(FILE *fd);

/**
 * validate the magic, the entry point
 */
void validate_header(struct bFLT_header *header, size_t file_size);

/**
 * allocates a reloc table. must be freed using free(table.ptr);
 */
struct bflt_reloc_table read_reloc_table(FILE *fd, struct bFLT_header *header);

/**
 * get the length of a file
 */
size_t get_file_size(FILE *fd);

#endif // FORMAT_H
