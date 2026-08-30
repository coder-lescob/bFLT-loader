#ifndef LOADER_H
#define LOADER_H

#include <stdio.h>
#include <format.h>

/**
 * allocates and copy the bFLT to a special region.
 * @note the bFLT is given through a file descriptor called `fd`
 */
void *mmap_new_bFLT(FILE *fd, struct bFLT_header *header);

/**
 * apply a reloc table to the code.
 */
void apply_reloc_table(void *code, struct bflt_reloc_table *reloc_table);

#endif
