all: clean s21_grep

rebuild: clean all

clean:
	-rm -f s21_grep

s21_grep: clean
	gcc -Wall -Werror -Wextra -std=c11 s21_grep.c -o s21_grep