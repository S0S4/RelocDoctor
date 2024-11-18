#include "../include/aux.h"

void seek_file(FILE *fp, int offset) {
  if (fseek(fp, offset, SEEK_SET) != 0) {
    perror("There was an error seeking the file");
    exit(1);
  }
}

void write_data(FILE *fp, void *data, size_t size) {
  if (fwrite(data, size, 1, fp) != 1) {
    perror("Error overwriting file!");
    exit(1);
  }
}
void read_section(FILE *fp, void *section, size_t section_size) {
  if (fread(section, 1, section_size, fp) != section_size) {
    perror("Error reading section");
    exit(1);
  }
}

Elf32_Ehdr elf_validation(FILE *fp) {
  Elf32_Ehdr ehdr;
  if (fread(&ehdr, 1, sizeof(ehdr), fp) != sizeof(ehdr)) {
    perror("There was an error reading the file");
    memset(&ehdr, 0, sizeof(ehdr));
    return ehdr;
  }

  if (ehdr.e_ident[EI_MAG0] != ELFMAG0 || ehdr.e_ident[EI_MAG1] != ELFMAG1 ||
    ehdr.e_ident[EI_MAG2] != ELFMAG2 || ehdr.e_ident[EI_MAG3] != ELFMAG3 ||
    ehdr.e_ident[EI_CLASS] != ELFCLASS32) {
    perror("Invalid ELF!");
    memset(&ehdr, 0, sizeof(ehdr));
    return ehdr;
  }

  if (ehdr.e_type != ET_EXEC) {
    perror("Sorry, the only elf type supported is ET_EXEC");
    memset(&ehdr, 0, sizeof(ehdr));
    return ehdr;
  }
  if (ehdr.e_shnum == 0) {
    perror("There's no Section Headers!");
    memset(&ehdr, 0, sizeof(ehdr));
    return ehdr;
  }
  return ehdr;
}

Elf32_Shdr *get_sectionheaders(FILE *fp, Elf32_Ehdr ehdr) {

  size_t size = ehdr.e_shnum * sizeof(Elf32_Shdr);
  Elf32_Shdr *sections = malloc(size);
  if (sections == NULL) {
    perror("There was an error with malloc");
    exit(1);
  }

  seek_file(fp, ehdr.e_shoff);

  for (int i = 0; i < ehdr.e_shnum; i++) {
    read_section(fp, &sections[i], sizeof(Elf32_Shdr));
  }
  return sections;
}

char *get_stringTable(FILE *fp, Elf32_Ehdr ehdr, Elf32_Shdr *sections) {

  size_t size = sections[ehdr.e_shnum - 1].sh_size + 1;

  char *shstrtab = malloc(size);
  if (shstrtab == NULL) {
    perror("Error with malloc");
    exit(1);
  }

  Elf32_Shdr shdr;

  // Move FP to  .shstrtab location

  seek_file(fp, ehdr.e_shoff + ehdr.e_shstrndx * sizeof(Elf32_Shdr));

  // Read .shstrtab section

  read_section(fp, &shdr, sizeof(shdr));
  // Move the FP pointer to the strings sections

  seek_file(fp, shdr.sh_offset);

  // Read the strings

  read_section(fp, shstrtab, shdr.sh_size);

  return shstrtab;
}

char *get_strData(FILE *fp,
                  Elf32_Shdr strtab) { // Get the strings from the string table

  if (strtab.sh_size == 0) {
    puts("Empty strtab!");
    exit(1);
  }

  char *strtab_data = malloc(strtab.sh_size);
  if (strtab_data == NULL) {
    perror("Error with malloc");
    exit(1);
  }

  seek_file(fp, strtab.sh_offset);

  read_section(fp, strtab_data, strtab.sh_size);

  return strtab_data;
}

Elf32_Sym *getDynsymData(FILE *fp, Elf32_Shdr dynsym, int n_sym) {

  Elf32_Sym *dynsym_dat = malloc(n_sym * sizeof(Elf32_Sym));
  if (dynsym_dat == NULL) {
    perror("Error with malloc");
    exit(1);
  }

  seek_file(fp, dynsym.sh_offset);

  for (int i = 0; i < n_sym; i++) {
    read_section(fp, &dynsym_dat[i], sizeof(Elf32_Sym));
  }
  return dynsym_dat;
}
struct relocationInfo *getRel(FILE *fp, Elf32_Shdr reloc_sec,
                              Elf32_Sym *dynsym_data, char *strtab_data,
                              int n_reloc) {

  size_t size = (sizeof(struct relocationInfo) * n_reloc);
  struct relocationInfo *relocs = malloc(size);
  if (relocs == NULL) {
    perror("There was an error with malloc");
    return NULL;
  }
  Elf32_Rel temp_rel;

  seek_file(fp, reloc_sec.sh_offset);

  for (int i = 0; i < n_reloc; i++) {
    read_section(fp, &temp_rel, sizeof(Elf32_Rel));

    // Get symbol name from string table
    uint32_t symbol_index = ELF32_R_SYM(temp_rel.r_info);
    uint32_t symbol = dynsym_data[symbol_index].st_name;

    relocs[i].name = &strtab_data[symbol];
    relocs[i].info = temp_rel.r_info;
    relocs[i].offset = temp_rel.r_offset;
    relocs[i].disk_offset = reloc_sec.sh_offset + (i * sizeof(Elf32_Rel));
  }
  return relocs;
}

int modifyInfo(FILE *fp, struct relocationInfo *relocs, struct arguments args,
               int n_reloc) {

  Elf32_Addr offset = strtol(args.offset, NULL, 16);
  Elf32_Addr offset_data = 0;
  Elf32_Word rinfo_data = 0;
  int return_state = 0;

  for (int i = 0; i < n_reloc; i++) {
    if ((strncmp(relocs[i].name, args.name, strlen(relocs[i].name)) == 0) &&
      (relocs[i].offset == offset)) {
      if (args.has_i) {
        rinfo_data = strtol(args.info, NULL, 16);
        seek_file(fp, relocs[i].disk_offset + sizeof(Elf32_Addr));
        write_data(fp, &rinfo_data, sizeof(Elf32_Word));
        return_state++;
      }
      if (args.has_r) {
        seek_file(fp, relocs[i].disk_offset);
        offset_data = strtol(args.offset2, NULL, 16);
        write_data(fp, &offset_data, sizeof(Elf32_Addr));
        return_state++;
      }
    }
  }
  return return_state;
}
