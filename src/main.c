#include <stdio.h>
#include <format.h>
#include <loader.h>

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <file>\n", argv[0]);
    fprintf(stderr, "tho it's likely done by your operating system\n");
    return 1;
  }

  FILE *fd = fopen(argv[1], "rb");
  if (fd == NULL) {
    perror("counldn't open input file");
    return 1;
  }

  size_t file_len = get_file_size(fd);
  
  // load and validate header
  struct bFLT_header header = read_header(fd);
  validate_header(&header, file_len);
  
  // allocate the process space.
  void *loader_base = mmap_new_bFLT(fd, &header);
  
  // apply the reloc table.
  struct bflt_reloc_table reloc_table = read_reloc_table(fd, &header);
  apply_reloc_table(loader_base, &reloc_table);
  free(reloc_table.ptr);
  
  // close the file
  fclose(fd);

  // load the program
  void (*entry)(int, char**) = (void (*)(int, char**))((char *)loader_base + header.entry);
  entry(argc - 1, argv + 1);

  return 0;
}
