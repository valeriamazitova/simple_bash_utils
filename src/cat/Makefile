all: s21_cat

rebuild: clean all

clean:
	-rm -f s21_cat

s21_cat: clean
	gcc -Wall -Werror -Wextra -std=c11 s21_cat.c -o s21_cat
