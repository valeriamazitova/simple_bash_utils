#include "s21_grep.h"

int main(int argc, char *argv[]) {
  if (argc >= 3) {
    char patterns[ARGMAX][NMAX];
    int pattern_count = 1;
    struct opt *opts = scan_options(argc, argv, patterns, &pattern_count);
    if (!opts->opt_e && !opts->opt_f)
      strcpy(patterns[pattern_count - 1], argv[optind++]);
    int currentFile = optind;
    int more_than_one_file = (currentFile != argc - 1) ? 1 : 0;
    while (currentFile < argc)
      s21_grep(opts, patterns, argv[currentFile++], more_than_one_file,
               pattern_count);
    free(opts);
  } else
    fprintf(stderr, "Enter command line arguments next time.\n");
  return 0;
}

struct opt *init() {
  struct opt *opts = malloc(sizeof(struct opt));
  opts->opt_e = opts->opt_i = opts->opt_v = opts->opt_c = opts->opt_l =
      opts->opt_n = opts->opt_h = opts->opt_s = opts->opt_f = opts->opt_o = 0;
  return opts;
}

struct opt *scan_options(int argc, char *argv[], char patterns[ARGMAX][NMAX],
                         int *pattern_count) {
  struct opt *opts = init();
  int rez, option_index;

  const char *short_options = "e:ivclnhsf:o";
  const struct option long_options[] = {
      {"e", required_argument, NULL, 'e'}, {"i", no_argument, NULL, 'i'},
      {"v", no_argument, NULL, 'v'},       {"c", no_argument, NULL, 'c'},
      {"l", no_argument, NULL, 'l'},       {"n", no_argument, NULL, 'n'},
      {"h", no_argument, NULL, 'h'},       {"s", no_argument, NULL, 's'},
      {"f", required_argument, NULL, 'f'}, {"o", no_argument, NULL, 'o'}};
  int first_e_option = 0;
  while ((rez = getopt_long(argc, argv, short_options, long_options,
                            &option_index)) != -1) {
    switch (rez) {
      case 'e':
        opts->opt_e = 1;
        if (first_e_option) (*pattern_count)++;
        strcpy(patterns[*pattern_count - 1], optarg);
        first_e_option = 1;
        break;
      case 'i':
        opts->opt_i = 1;
        break;
      case 'v':
        opts->opt_v = 1;
        opts->opt_o = 0;
        break;
      case 'c':
        opts->opt_c = 1;
        opts->opt_o = 0;
        opts->opt_n = 0;
        break;
      case 'l':
        opts->opt_l = 1;
        opts->opt_o = 0;
        break;
      case 'n':
        opts->opt_n = 1;
        break;
      case 'h':
        opts->opt_h = 1;
        break;
      case 's':
        opts->opt_s = 1;
        break;
      case 'f':
        opts->opt_f = 1;
        if (first_e_option) (*pattern_count)++;
        file_patterns_to_array(opts, optarg, patterns, pattern_count);
        break;
      case 'o':
        opts->opt_o = 1;
        if (opts->opt_v || opts->opt_c || opts->opt_l) opts->opt_o = 0;
        break;
    }
  }
  return opts;
}

void file_patterns_to_array(struct opt *opts, char *filename,
                            char patterns[ARGMAX][NMAX], int *pattern_count) {
  FILE *pattern_file = fopen(filename, "r");
  if (pattern_file != NULL) {
    char pattern_str[NMAX];
    while (fgets(pattern_str, NMAX, pattern_file) != NULL) {
      unsigned long len = strlen(pattern_str);
      if (pattern_str[len - 1] == '\n' && len != 1) pattern_str[len - 1] = '\0';
      strcpy(patterns[*pattern_count - 1], pattern_str);
      (*pattern_count)++;
    }
    (*pattern_count)--;
    fclose(pattern_file);
  } else if (!opts->opt_s)
    fprintf(stderr, "grep: %s: No such file or directory\n", filename);
}

void s21_grep(struct opt *opts, char patterns[ARGMAX][NMAX], char *filename,
              int more_than_one_file, int pattern_count) {
  FILE *file = fopen(filename, "r");
  if (file != NULL) {
    int result, not_stopped, matches, line_count, match_printed, nomatch_count;
    not_stopped = line_count = 1;
    matches = match_printed = nomatch_count = 0;
    int cflags = (opts->opt_i) ? REG_ICASE : REG_NEWLINE;
    char str[NMAX];
    int case_n_o = 1;
    while (fgets(str, NMAX, file) != NULL && not_stopped) {
      int count_pattern_nomatch = 0;
      for (int i = 0; i < pattern_count && not_stopped; i++) {
        regex_t reg_ex;
        regcomp(&reg_ex, patterns[i], cflags);
        result = regexec(&reg_ex, str, 0, NULL, 0);
        if (result == 0) {
          if (!opts->opt_v) matches++;
          if (opts->opt_o)
            case_o_option(opts, str, patterns[i], more_than_one_file, filename,
                          line_count);
          else if (!opts->opt_v && !opts->opt_c && !opts->opt_l) {
            if (!match_printed) {
              output(more_than_one_file, opts, filename, line_count, str,
                     &case_n_o);
            }
            match_printed = 1;
          }
        } else if (result == REG_NOMATCH) {
          if (opts->opt_v) count_pattern_nomatch++;
          if (opts->opt_v && !opts->opt_c && !opts->opt_l) {
            if (pattern_count == 1)
              output(more_than_one_file, opts, filename, line_count, str,
                     &case_n_o);
            else {
              nomatch_count++;
              if (nomatch_count == pattern_count)
                output(more_than_one_file, opts, filename, line_count, str,
                       &case_n_o);
            }
          }
        }
        if ((result == 0 && !opts->opt_v) ||
            (result == REG_NOMATCH && opts->opt_v)) {
          if (count_pattern_nomatch == pattern_count) {
            matches++;
          }
        }
        if (opts->opt_l && matches > 0) not_stopped = 0;
        regfree(&reg_ex);
      }
      line_count++;
      match_printed = nomatch_count = 0;
    }
    if (opts->opt_c) {
      if (more_than_one_file && !opts->opt_h) printf("%s:", filename);
      printf("%d\n", matches);
    }
    if (opts->opt_l && matches > 0) printf("%s\n", filename);
    fclose(file);
  } else if (!opts->opt_s)
    fprintf(stderr, "grep: %s: No such file or directory\n", filename);
}

void case_o_option(struct opt *opts, char *str, char *pattern,
                   int more_than_one_file, char *filename, int line_count) {
  regex_t re_o;
  char *tmp = str;
  int e_flag = 0;
  regmatch_t pmatch;
  int cflags = (opts->opt_i) ? REG_ICASE : REG_NEWLINE;
  regcomp(&re_o, pattern, cflags);
  int case_n_o = 1;
  while (regexec(&re_o, tmp, 1, &pmatch, e_flag) == 0) {
    char *result_str = malloc((pmatch.rm_eo - pmatch.rm_so + 1) * sizeof(char));
    int j = 0;
    for (long long i = pmatch.rm_so; i < pmatch.rm_eo; i++) {
      result_str[j++] = tmp[i];
    }
    result_str[j] = '\0';
    output(more_than_one_file, opts, filename, line_count, result_str,
           &case_n_o);
    free(result_str);
    tmp += pmatch.rm_eo;
    e_flag = REG_NOTBOL;
  }
  regfree(&re_o);
}

void output(int more_than_one_file, struct opt *opts, char *filename,
            int line_count, char *str, int *number_count_case_o) {
  unsigned long len = strlen(str);
  if (str[len - 1] == '\n') str[len - 1] = '\0';
  if (more_than_one_file && !opts->opt_h) printf("%s:", filename);
  if (opts->opt_n && *number_count_case_o) {
    printf("%d:", line_count);
    if (opts->opt_o) *number_count_case_o = 0;
  }
  printf("%s\n", str);
}