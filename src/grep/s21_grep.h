#ifndef SRC_GREP_H_
#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ARGMAX 100
#define NMAX 500

struct opt {
  int opt_e;
  int opt_i;
  int opt_v;
  int opt_c;
  int opt_l;
  int opt_n;
  int opt_h;
  int opt_s;
  int opt_f;
  int opt_o;
};

struct opt *init();
struct opt *scan_options(int argc, char *argv[], char patterns[ARGMAX][NMAX],
                         int *pattern_count);
void file_patterns_to_array(struct opt *opts, char *filename,
                            char patterns[ARGMAX][NMAX], int *pattern_count);
void s21_grep(struct opt *opts, char patterns[ARGMAX][NMAX], char *filename,
              int more_than_one_file, int pattern_count);
void case_o_option(struct opt *opts, char *str, char *pattern,
                   int more_than_one_file, char *filename, int line_count);
void output(int more_than_one_file, struct opt *opts, char *filename,
            int line_count, char *str, int *number_count_case_o);

#endif  //  SRC_GREP_H_
