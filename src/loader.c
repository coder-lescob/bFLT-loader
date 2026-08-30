#include <loader.h>

#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

/**
 * allocates and copy the bFLT to a special region.
 * @note the bFLT is given through a file descriptor called `fd`
 */
void *mmap_new_bFLT(FILE *fd, struct bFLT_header *header) {
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

  read(fileno(fd), loader_base, file_len);

  // zero out the bss section
  memset((char *)loader_base + header->data_end /* data end is not inclusive */, 0, bss_size);

  return loader_base;
}
