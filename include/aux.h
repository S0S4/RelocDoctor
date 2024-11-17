#ifndef AUX_H
#define AUX_H

#include <elf.h>
#include <stdlib.h>
#include <string.h>
#include "../include/args.h"

struct relocationInfo{
  Elf32_Addr offset;
  Elf32_Addr disk_offset;
  Elf32_Word info;
  char * name;
};

Elf32_Ehdr elf_validation(FILE *fp);
Elf32_Shdr *get_sectionheaders(FILE *fp, Elf32_Ehdr ehdr);
char *get_stringTable(FILE *fp, Elf32_Ehdr ehdr, Elf32_Shdr *sections);
char *get_strData(FILE *fp, Elf32_Shdr strtab);
Elf32_Sym *getDynsymData(FILE *fp, Elf32_Shdr dynsym, int n_sym);
struct relocationInfo *getRel(FILE *fp,Elf32_Shdr reloc_sec , Elf32_Sym *dynsym_data, char *strtab_data,int n_reloc);
int modifyInfo(FILE *fp, struct relocationInfo *relocs, struct arguments args, int n_reloc);
void seek_file(FILE *fp, int offset);
void read_section(FILE *fp, void *section, size_t section_size);
#endif // AUX_H
