CC = gcc
UTILS = utilitaires
FILE = file
AC = ac
MAIN = main
OPT = -std=c11 -Wpedantic -Wall -Wextra -Wwrite-strings  -Werror -Wfatal-errors -O2 -g

all: $(MAIN).o $(AC).o
	$(CC) $(OPT) $(MAIN).o $(AC).o -o rechAC

$(MAIN).o: $(MAIN).c
	$(CC) -c $(OPT) $(MAIN).c

$(AC).o: $(AC).c
	$(CC) -c $(OPT) $(AC).c

clean:
	rm -f *.o
