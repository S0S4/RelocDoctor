#ifndef ARGS_H
#define ARGS_H
#include <argp.h>
#include <stdio.h>

struct arguments {
  char *file;
  char *name;
  char *offset;
  char *info;
  char *offset2;
  int has_f, has_n, has_o, has_i, has_r;
};

extern struct argp_option options[];
extern struct argp argp;

error_t parse_opt(int key, char *arg, struct argp_state *state);
int validate_args(struct arguments *args,struct argp_state *state);
#endif // ARGS_H
