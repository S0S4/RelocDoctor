#include "../include/args.h"

struct argp_option options[] = {
  {"file", 'f', "FILE", 0, "Elf File", 0},
  {"name", 'n', "NAME", 0, "Symbol Name", 0},
  {"offset", 'o', "OFFSET", 0, "Symbol Offset", 0},
  {"info", 'i', "INFO", 0, "r_info value for modifying selected symbol ", 0},
  {"newoffset", 'r', "NEW_OFFSET", 0, "r_offset value for modifying selected symbol", 0},
  {0}};

struct argp argp = {options, parse_opt, NULL, "RelocDoctor\n\n Example usage: \n $ ./relocdoctor -f elf -n puts@GLIBC2.0 -o 08040000 -i 207 -r 08040004", NULL, NULL,NULL};

error_t parse_opt(int key, char *arg, struct argp_state *state) {
  struct arguments *args = state->input;

  switch (key) {
    case 'f':
      args->file = arg;
      args->has_f = 1;
      break;
    case 'n':
      args->name = arg;
      args->has_n = 1;
      break;
    case 'o':
      args->offset = arg;
      args->has_o = 1;
      break;
    case 'i':
      args->info = arg;
      args->has_i = 1;
      break;
    case 'r':
      args->offset2 = arg;
      args->has_r = 1;
      break;
    case ARGP_KEY_END:
      if(validate_args(args,state)){
        return ARGP_ERR_UNKNOWN;
      }
    default:
      return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

int validate_args(struct arguments *args,struct argp_state *state) {
  if (!(args->has_f && args->has_n && args->has_o)) {
    argp_usage(state);
    puts("[!] Error: -f, -n and -o are mandatory\n");
    return 1;
  }
  if (!(args->has_i || args->has_r)) {
    argp_usage(state);
    puts("Error: You have to specify -i or -r.\n");
    return 1;
  }
  return 0;
}
