#include "s21_cat.h"

int main(int argc, char *argv[]) {
  if (argc >= 2) {
    struct opt *opts = scan_options(argc, argv);
    int currentFile = optind;
    while (currentFile < argc) {
      s21_cat(argv[currentFile++], opts);
    }
    free(opts);
  } else
    fprintf(stderr, "Enter command line arguments next time.\n");
  return 0;
}

struct opt *init() {
  struct opt *opts = malloc(sizeof(struct opt));
  opts->opt_e = opts->opt_t = opts->opt_n = opts->opt_b = opts->opt_s =
      opts->opt_v = 0;
  return opts;
}

struct opt *scan_options(int argc, char *argv[]) {
  struct opt *opts = init();
  const char *short_options = "+beEnstTv";
  const struct option long_options[] = {
      {"number-nonblank", no_argument, NULL, 'b'},
      {"number", no_argument, NULL, 'n'},
      {"squeeze-blank", no_argument, NULL, 's'}};
  int rez, option_index;
  while ((rez = getopt_long(argc, argv, short_options, long_options,
                            &option_index)) != -1) {
    switch (rez) {
      case 'e':
        opts->opt_e = opts->opt_v = 1;
        break;
      case 'E':
        opts->opt_e = 1;
        break;
      case 't':
        opts->opt_t = opts->opt_v = 1;
        break;
      case 'T':
        opts->opt_t = 1;
        break;
      case 'b':
        opts->opt_b = 1;
        break;
      case 's':
        opts->opt_s = 1;
        break;
      case 'n':
        opts->opt_n = 1;
        break;
      case 'v':
        opts->opt_v = 1;
        break;
    }
  }
  return opts;
}

void s21_cat(char *filename, struct opt *opts) {
  FILE *file = fopen(filename, "r");
  if (file != NULL) {
    int not_stopped = 1;
    int line_count = 1;
    char c = fgetc(file);
    char next_c = '\0';
    if (opts->opt_b) opts->opt_n = 0;
    if (c != EOF) {
      print_line_number(opts, c, line_count);
      print_c(c, opts, file);
      line_count++;
      while ((next_c = fgetc(file)) != EOF && not_stopped) {
        if (opts->opt_b && c == '\n') {
          line_count--;
        }

        if (opts->opt_s) {
          while (c == '\n' && next_c == '\n') {
            next_c = fgetc(file);
            if (next_c == EOF) not_stopped = 0;
          }
        }
        if (not_stopped) {
          print_line_number(opts, next_c, line_count);
          line_count++;
          print_c(next_c, opts, file);

          c = next_c;
        }
      }
    }
    fclose(file);
  } else
    fprintf(stderr, "cat: %s: No such file or directory\n", filename);
}

void print_line_number(struct opt *opts, int next_c, int line_count) {
  if (opts->opt_n || (opts->opt_b && next_c != '\n'))
    printf("%6d\t", line_count);
}

void print_c(char next_c, struct opt *opts, FILE *file) {
  int not_stopped = 1;
  while (next_c != '\n' && not_stopped) {
    if ((opts->opt_t || (opts->opt_v && next_c != '\t')) &&
        (((int)next_c >= 0 && (int)next_c < 32) || (int)next_c == 127)) {
      if ((int)next_c == 127)
        printf("^?");
      else
        printf("^%c", next_c + 64);
    } else
      printf("%c", next_c);
    if ((next_c = fgetc(file)) == EOF) not_stopped = 0;
  }
  print_end_line(next_c, opts);
}

void print_end_line(char next_c, struct opt *opts) {
  if (next_c == '\n') {
    if (opts->opt_e) printf("$");
    printf("\n");
  }
}