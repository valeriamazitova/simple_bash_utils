#ifndef SRC_CAT_H_
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct opt {
  int opt_e;
  int opt_t;
  int opt_b;
  int opt_s;
  int opt_n;
  int opt_v;
};

struct opt *init();
struct opt *scan_options(int argc, char *argv[]);
void s21_cat(char *filename, struct opt *opts);
void print_line_number(struct opt *opts, int next_c, int line_count);
void print_c(char next_c, struct opt *opts, FILE *file);
void print_end_line(char next_c, struct opt *opts);

#endif  //  SRC_CAT_H_