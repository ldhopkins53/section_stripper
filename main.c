/*
 * TODO: Remove the section headers altogether
 */

#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>

struct elf_handle {
  char *elf_file;
  size_t file_size;
};

struct elf_handle read_elf_file(const char *filename) {
  // Open file to read/wrtite
  printf("[+] Opening %s\n", filename);
  FILE *fd = fopen(filename, "rw+");
  if (fd == NULL) {
    perror("fopen");
    exit(EXIT_FAILURE);
  }

  // Get file size
  struct stat file_stat;
  stat(filename, &file_stat);
  size_t file_size = file_stat.st_size;
  printf("[+] File is %lu bytes\n", file_size);

  // Map file into memory
  printf("[+] Mapping file into memory\n");
  char *elf_file = (char *)mmap(NULL, file_size, PROT_READ | PROT_WRITE,
                                MAP_SHARED, fileno(fd), 0);
  if (elf_file == MAP_FAILED) {
    perror("mmap");
    exit(EXIT_FAILURE);
  }
  fclose(fd);

  // Build output struct
  struct elf_handle eh;
  eh.elf_file = elf_file;
  eh.file_size = file_size;
  return eh;
}

int main(int argc, char **argv) {
  // Check argument provided
  if (argc != 2) {
    fprintf(stderr, "[-] Usage: %s INPUT_FILE\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  // Build view of elf file internals
  struct elf_handle eh = read_elf_file(argv[1]);
  Elf64_Ehdr *elf_header = (Elf64_Ehdr *)eh.elf_file;
  Elf64_Shdr *shdr = (Elf64_Shdr *)(eh.elf_file + elf_header->e_shoff);
  char *strhdr_table = eh.elf_file + shdr[elf_header->e_shstrndx].sh_offset;

  // Strip sections
  for (int i = 1; i < elf_header->e_shnum; ++i) {
    printf("[+] Wiping out section index %d -> %s\n", i,
           &strhdr_table[shdr[i].sh_name]);
    memset(&shdr[i], 0, elf_header->e_shentsize);
  }
  elf_header->e_shnum = 0;
  elf_header->e_shentsize = 0;
  elf_header->e_shoff = 0;

  printf("[+] Saving file back to disk and cleaning up\n");
  msync(eh.elf_file, eh.file_size, MS_SYNC);
  munmap(eh.elf_file, eh.file_size);
  return EXIT_SUCCESS;
}
