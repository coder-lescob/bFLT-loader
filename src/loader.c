#include <loader.h>

#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <endian.h>

/**
 * allocates and copy the bFLT to a special region.
 * @note the bFLT is given through a file descriptor called `fd`
 */
void *mmap_new_bFLT(FILE *fd, struct bFLT_header *header) {
  SAFE_SEEK(fd, 0, SEEK_SET);

  size_t file_len = get_file_size(fd);
  size_t bss_size = header->bss_end - header->data_end;
  size_t alloc_size = file_len + bss_size;

  // allocates the region
  void *loader_base = mmap(
    NULL,
    alloc_size,
    // pages may be read, written and executed
    PROT_READ | PROT_WRITE | PROT_EXEC,
    // it's private and anonymous
    MAP_PRIVATE | MAP_ANONYMOUS,
    -1,
    0
  );

  if (loader_base == MAP_FAILED) {
    perror("mmap");
    exit(EXIT_FAILURE);
  }

  if (fread(loader_base, 1, file_len, fd) != file_len) {
    fprintf(stderr, "unable to read the entire file\n");
    exit(EXIT_FAILURE);
  }
  
  if (header->data_end - header->data_start > 0) {
    // zero out the bss section to avoid reading the reloc table when reading bss
    memset((char *)loader_base + header->data_end /* data end is not inclusive */, 0, bss_size);
  }

  SAFE_SEEK(fd, 0, SEEK_SET);

  return loader_base;
}

/**
 * apply a reloc table to the code.
 */
void apply_reloc_table(void *loader_base, struct bflt_reloc_table *reloc_table) {
  for (int i = 0; i < reloc_table->count; i++) {
    size_t *p = (size_t *)((char *)loader_base + reloc_table->ptr[i].offset);
    *p = htole64(le64toh(*p) + (uintptr_t)loader_base);
  }
}
