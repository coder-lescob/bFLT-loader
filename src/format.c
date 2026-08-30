#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#include <format.h>

/**
 * reads a bFLT header
 */
struct bFLT_header read_header(FILE *fd) {
  SAFE_SEEK(fd, 0, SEEK_SET);
  
  // read the hedaer
  struct bFLT_header header;
  size_t read_size = fread(&header, 1, sizeof(header), fd); // 64 blocks of 1 byte thus 64 bytes (sizeof(header))
  
  // validate that header has been fully read
  if (read_size != sizeof(header)) {
    fprintf(stderr, "header truncated read %zu bytes, expected 64 bytes\n", read_size);
    exit(EXIT_FAILURE);
  }

#define ToHostLong(x) x = ntohl(x)
  ToHostLong(header.version);
  ToHostLong(header.entry);
  ToHostLong(header.data_start);
  ToHostLong(header.data_end);
  ToHostLong(header.bss_end);
  ToHostLong(header.stack_size);
  ToHostLong(header.reloc_start);
  ToHostLong(header.reloc_count);
  ToHostLong(header.flags);
  ToHostLong(header.build_date);
#undef ToHostLong

  SAFE_SEEK(fd, 0, SEEK_SET);
  return header;
}

/**
 * validate the magic, the entry point, etc...
 */
void validate_header(struct bFLT_header *header, size_t file_size) {
  if (strncmp(header->magic, "bFLT", 4) != 0) {
    // magic mismatch
    fprintf(stderr, "wrong magic number %.4s\n", header->magic);
    exit(EXIT_FAILURE);
  }

  if (header->entry < sizeof(struct bFLT_header)) {
    // entry in header??
    fprintf(stderr, "entry points to header\n");
    exit(EXIT_FAILURE);
  }

  if (header->data_end < header->data_start) {
    // data section has negative size??
    fprintf(stderr, "data section is backwards\n");
    exit(EXIT_FAILURE);
  }

  if (header->data_end - header->data_start > 0 && header->data_start < sizeof(struct bFLT_header)) {
    // data section in header??
    fprintf(stderr, "data section points to header\n");
    exit(EXIT_FAILURE);
  }

  if (header->reloc_count > 0 && header->reloc_start < sizeof(struct bFLT_header)) {
    // reloc points to header??
    fprintf(stderr, "reloc points to header\n");
    exit(EXIT_FAILURE);
  }

  if (header->entry >= file_size) {
    // entry outside of file??
    fprintf(stderr, "entry points to outside of file\n");
    exit(EXIT_FAILURE);
  }

  if (header->data_end - header->data_start > 0 && header->data_end > file_size) {
    // data section points outside of file??
    fprintf(stderr, "data section points outside of file\n");
    exit(EXIT_FAILURE);
  }

  size_t reloc_end = (size_t)header->reloc_start + (size_t)header->reloc_count * sizeof(struct bflt_reloc); 

  if (header->reloc_count > 0 && reloc_end > file_size) {
    // reloc table outside of file??
    fprintf(stderr, "reloc table outside of file\n");
    exit(EXIT_FAILURE);
  }
  
  if (header->reloc_count > 0 && header->data_end - header->data_start > 0
   && reloc_end > header->data_start && header->reloc_start < header->data_end) {
    // reloc and data overlap??
    fprintf(stderr, "reloc and data sections overlap\n");
    exit(EXIT_FAILURE);
  }

  if (header->bss_end < header->data_end) {
    // bss overlap with data??
    fprintf(stderr, "bss overlap with data\n");
    exit(EXIT_FAILURE);
  }
}

/**
 * allocates a reloc table. must be freed using free(table.ptr);
 */
struct bflt_reloc_table read_reloc_table(FILE *fd, struct bFLT_header *header) {
  if (header->reloc_count == 0) {
    return (struct bflt_reloc_table) { NULL, 0 };
  }
  // seek to relocation table
  SAFE_SEEK(fd, header->reloc_start, SEEK_SET);

  struct bflt_reloc *entries = calloc(header->reloc_count, sizeof(struct bflt_reloc));
  if (entries == NULL) {
    perror("bflt entries allocation failed");
    exit(EXIT_FAILURE);
  }
  fread((void *)entries, header->reloc_count, 1, fd);

  SAFE_SEEK(fd, 0, SEEK_SET);

  return (struct bflt_reloc_table) { entries, header->reloc_count };
}

/**
 * get the length of a file
 */
size_t get_file_size(FILE *fd) {
  SAFE_SEEK(fd, 0, SEEK_END);
  size_t len = ftell(fd);
  SAFE_SEEK(fd, 0, SEEK_SET);
  return len;
}
