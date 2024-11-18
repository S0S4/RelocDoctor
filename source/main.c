/*


Copyright 2024 Ivan Redondo aka S0S4

Permission is hereby granted, free of charge, to any person obtaining a copy of this
software and associated documentation files (the “Software”), to deal in the
Software without restriction, including without limitation the rights to use, copy,
modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so, subject to the
following conditions:

The above copyright notice and this permission notice shall be included in all copies
or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

/*

  This program is a simple and quick POC for ELF Relocation mangling
  I didn't use free() on purpose. For this case I let that job to the OS.

 */

#include "../include/aux.h"

int main(int argc, char **argv) {

  // Argument parsing stuff;
  struct arguments args = {0};
  argp_parse(&argp, argc, argv, 0, 0, &args);
  struct relocationInfo *relocations_info;

  Elf32_Shdr dynstr, reloc_sec, dynsym;
  Elf32_Ehdr ehdr;
  Elf32_Shdr *shdrs;
  Elf32_Sym *dynsym_data;

  char *shstrtab;
  char *dynstr_data;

  uint n_reloc, n_sym;

  FILE *fp = fopen(args.file, "rb+");
  if (!fp) {
    perror("There was an error opening the ELF");
    return 1;
  }

  ehdr = elf_validation(fp);

  shdrs = get_sectionheaders(fp, ehdr);

  shstrtab = get_stringTable(fp, ehdr, shdrs);

  seek_file(fp, ehdr.e_shoff);

  for (int i = 0; i < ehdr.e_shnum; i++) {
    read_section(fp, &shdrs[i], sizeof(Elf32_Shdr));
    if (strcmp(&shstrtab[shdrs[i].sh_name], ".rel.plt") == 0) {
      reloc_sec = shdrs[i];
    } else if (strcmp(&shstrtab[shdrs[i].sh_name], ".dynstr") == 0) {
      dynstr = shdrs[i];
    } else if (strcmp(&shstrtab[shdrs[i].sh_name], ".dynsym") == 0) {
      dynsym = shdrs[i];
    }
  }

  dynstr_data = get_strData(fp, dynstr);
  if (dynstr_data == NULL) {
    perror("Dynstr is empty!");
    return 1;
  }

  n_sym = dynsym.sh_size / sizeof(Elf32_Sym);
  dynsym_data = getDynsymData(fp, dynsym, n_sym);
  if (dynsym_data == NULL) {
    perror("Dynsym is empty!");
    return 1;
  }

  if (reloc_sec.sh_size == 0 && reloc_sec.sh_entsize == 0) {
    perror("Relocation section size is 0");
    return 1;
  }

  n_reloc = reloc_sec.sh_size / reloc_sec.sh_entsize;

  relocations_info = getRel(fp, reloc_sec, dynsym_data, dynstr_data, n_reloc);
  if (relocations_info == NULL) {
    perror("Relocation section is empty!");
    return 1;
  }

  if (modifyInfo(fp, relocations_info, args, n_reloc) == 0) {
    puts(" [!] There was an error modifying the elf");
    return 1;
  }

  fclose(fp);
  return 0;
}
