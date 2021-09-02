#ifndef __AC__
#define __AC__

#include <stdbool.h>
#include <stdlib.h>

// Files ////////////////////////////////////
typedef struct _element {
  int value;
  struct _element *next;
} * element;

typedef struct _queue {
  struct _element *head;
  struct _element *tail;
} * Queue;

Queue emptyQueue();
bool isEmptyQueue(Queue Q);
Queue pushQueue(Queue Q, int value);
int getHead(Queue Q);
Queue popQueue(Queue Q);
void freeQueue(Queue Q);

// Aho-Corasick /////////////////////////////
typedef struct _list {
  int e;
  int a;
  int p;
  struct _list *next;
} * List;

typedef struct _ac {
  int *supp;
  int alphabetSize;
  size_t nextState;
  int *exits;
  int **matrix;
} * AC;

int delta(AC ac, size_t e, char c);
void addWord(AC a, char *mot);
AC createAc();
List transitionsList(AC ac, int valeur, int e);
List nextList(List l);
void fillAc(AC ac);
void printStates(size_t *etats, size_t n);
void printMatrix(AC ac);
void freeAc(AC ac);
AC preAC(char **X, size_t k);
void ac(char **X, size_t k, char *y, size_t n, size_t niveauVerbosite);

// Extrait infos de la ligne de commande //////////
typedef struct _args {
  char **words;
  char *text;
  size_t nbWords;
  size_t verbosityLevel;
} * Args;

size_t extractWordNumber(char *content, size_t n);
char **extractWords(char *contenus, size_t nbMots);
char *readFile(char *file);
char **readWords(char *file);
void printHelp();
Args parseArgs(int argc, char **argv);
void freeArgs(Args args);

#endif
