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

  struct bFLT_header header = read_header(fd);
  validate_header(&header);

  printf("header valid\n");

  fclose(fd);

  return 0;
}
