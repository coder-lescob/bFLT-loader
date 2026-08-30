#include <stdio.h>
#include <format.h>

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

  struct bFLT_header header = read_header(fd);
  validate_header(&header, file_len);

  size_t bss_size = header.bss_end - header.data_end;
  size_t alloc_size = file_len + bss_size;

  fclose(fd);

  return 0;
}
