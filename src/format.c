#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <format.h>

/**
 * reads a bFLT header
 */
struct bFLT_header read_header(FILE *fd) {
  fseek(fd, 0, SEEK_SET);

  struct bFLT_header header;
  read(fileno(fd), &header, sizeof(header));

  fseek(fd, 0, SEEK_SET);
  return header;
}

/**
 * validate the magic, the entry point
 */
void validate_header(struct bFLT_header *header) {
  if (strncmp(header->magic, "bFLT", 4) != 0) {
    // magic mismatch
    perror("wrong magic number");
    exit(EXIT_FAILURE);
  }

  if (header->entry < 60) {
    // entry in header??
    perror("entry points to header");
    exit(EXIT_FAILURE);
  }

  if (header->data_start - header->data_end > 0 && header->data_start < 60) {
    // data section in header??
    perror("data section points to header");
    exit(EXIT_FAILURE);
  }
}
