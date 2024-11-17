# RelocDoctor

**RelocDoctor** is a tool designed to manipulate the relocation entries of ELF (Executable and Linkable Format) files. It allows users to overwrite relocation entries with custom ones, making it a useful utility for developers working on low-level binary modifications, reverse engineering, or custom toolchains.


## Features


- Parse and analyze ELF files.
- Modify relocation entries with user-defined values.
- Works on ELF32 and ELF64 binaries.
- Command-line interface for ease of use.


The following disadvantages are easy to implement and will be implemented in the near future.

## Disadvantages


- Only working for dynamic symbols (.dynsym)
- Unique relocation section (.rel.plt)


## Installation

```
git clone https://github.com/S0S4/RelocDoctor.git
cd relocdoctor
make
```


## Usage

1. **Analyze desired elf** :
    ```bash
   $  readelf -r main

     Relocation section '.rel.plt' at offset 0x21b0 contains 3 entries:
        Offset     Info    Type            Sym.Value  Sym. Name
        0804c000  00000107 R_386_JUMP_SLOT   00000000   __libc_start_main@GLIBC_2.34
        0804c004  00000207 R_386_JUMP_SLOT   00000000   puts@GLIBC_2.0
        0804c008  00000407 R_386_JUMP_SLOT   00000000   putchar@GLIBC_2.0
    ```
2. **See possible options** : 

    ```bash
    ./relocdoctor --help 

    Usage: relocdoctor [OPTION...]
    Symbol Mangler
    -f, --file=FILE            Elf File
    -i, --info=INFO            r_info value for modifying selected symbol
    -n, --name=NAME            Symbol Name
    -o, --offset=OFFSET        Symbol Offset
    -r, --newoffset=NEW_OFFSET r_offset value for modifying selected symbol
    -?, --help                 Give this help list
      --usage                Give a short usage message

    Mandatory or optional arguments to long options are also mandatory or optional
    for any corresponding short options.
    ```

2. **Modify desired value/s**:

    ```bash

       ./relocdoctor -f main -n "puts@GLIBC_2.0" -o 0804c004 -i "00000107" -r "0804c008"

    ```

## Contribution

Contributions are welcome! If you find a bug or have an idea for improvement, please open an issue or submit a pull request.