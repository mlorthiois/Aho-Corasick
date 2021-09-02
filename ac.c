#include "ac.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const size_t NB_ETATS = 1;
const int ALPHABET = 128; // ASCII
const size_t TAILLE_MOTS = 30;

////////////////////////////////////////////////
// Files
////////////////////////////////////////////////
Queue emptyQueue() {
  Queue nouvelleFile = malloc(sizeof(struct _queue));
  nouvelleFile->head = NULL;
  nouvelleFile->tail = NULL;
  return nouvelleFile;
}

bool isEmptyQueue(Queue Q) { return (Q->head == NULL && Q->tail == NULL); }

Queue pushQueue(Queue Q, int value) {
  element el = malloc(sizeof(struct _element));
  el->value = value;
  el->next = NULL;

  if (isEmptyQueue(Q)) {
    Q->head = el;
    Q->tail = el;
    return Q;
  }

  Q->tail->next = el;
  Q->tail = el;
  return Q;
}

int getHead(Queue Q) {
  if (isEmptyQueue(Q))
    exit(EXIT_FAILURE);
  return Q->tail->value;
}

Queue popQueue(Queue Q) {
  if (isEmptyQueue(Q) || Q->head == Q->tail)
    return emptyQueue();

  element tmp = Q->head;
  Q->head = Q->head->next;
  free(tmp);
  return Q;
}

void freeQueue(Queue Q) {
  if (!isEmptyQueue(Q)) {
    element q = Q->head;
    while (q != NULL) {
      element tmp = q->next;
      free(q);
      q = tmp;
    }
  }
  free(Q);
}

////////////////////////////////////////////////
// Aho-Corasick
////////////////////////////////////////////////
int delta(AC ac, size_t e, char c) { return ac->matrix[e][(int)c]; }

void addWord(AC a, char *mot) {
  size_t i = 0;
  size_t e = 0;
  size_t n = strlen(mot);

  // Le début du mot est dans l'automate
  while (i < n && delta(a, e, mot[i]) != -1) {
    e = a->matrix[e][(int)mot[i]];
    i++;
  }

  // On ajoute le reste à l'automate
  while (i < n) {
    a->matrix[e][(int)mot[i]] = a->nextState;
    e = a->nextState;
    a->nextState++;

    // Ajout d'un état dans la matrix, la supp et les exits
    a->matrix = realloc(a->matrix, a->nextState * sizeof(int *));
    a->matrix[a->nextState - 1] = malloc(a->alphabetSize * sizeof(int));
    for (int j = 0; j < a->alphabetSize; j++) {
      a->matrix[a->nextState - 1][j] = -1;
    }
    a->exits = realloc(a->exits, a->nextState * sizeof(int));
    a->exits[a->nextState - 1] = 0;
    a->supp = realloc(a->supp, a->nextState * sizeof(int));
    a->supp[a->nextState - 1] = 0;
    i++;
  }

  // A la fin du mot, état fini
  a->exits[e] = 1;
}

AC createAc() {
  AC ac = malloc(sizeof(struct _ac));
  if (ac == NULL) {
    fprintf(stderr, "Problème d'allocation de l'AC");
    return NULL;
  }

  // Allocation du tableau d'états exits
  ac->exits = malloc(NB_ETATS * sizeof(int));
  if (ac->exits == NULL) {
    fprintf(stderr, "Problème allocation états exits");
    return NULL;
  }

  // Allocation du tableau de suppléants
  ac->supp = malloc(NB_ETATS * sizeof(int));
  if (ac->supp == NULL) {
    fprintf(stderr, "Problème d'allocation des supp");
    return NULL;
  }

  // Allocation des lignes de la matrix
  ac->matrix = malloc(NB_ETATS * sizeof(int *));
  if (ac->matrix == NULL) {
    fprintf(stderr, "Problème allocation des lignes");
    return NULL;
  }

  // Allocation des colonnes de la matrix
  for (size_t i = 0; i < NB_ETATS; i++) {
    ac->matrix[i] = malloc(ALPHABET * sizeof(int));
    if (ac->matrix[i] == NULL) {
      fprintf(stderr, "Problème allocation de la colonne %zu", i);
      return NULL;
    }
    // Initalisation de chaque cellule de la matrix à -1 = non défini
    for (size_t j = 0; j < ALPHABET; j++) {
      ac->matrix[i][j] = -1;
    }

    // Initialisation des états exits à 0 : aucun n'est terminal
    ac->exits[i] = 0;
    ac->supp[i] = 0;
  }

  // Assignation des variables
  ac->nextState = 1;
  ac->alphabetSize = ALPHABET;

  return ac;
}

List transitionsList(AC ac, int valeur, int e) {
  List transitions = NULL;
  List temp;
  for (int a = 0; a < ac->alphabetSize; a++) {
    // Ajouter la transition a la liste
    if (delta(ac, e, a) > valeur) {
      temp = malloc(sizeof(struct _list));
      temp->e = e;
      temp->a = a;
      temp->p = delta(ac, e, a);
      temp->next = transitions;
      transitions = temp;
    }
  }
  return transitions;
}

List nextList(List l) {
  List tmp = l;
  l = l->next;
  free(tmp);
  return l;
}

void fillAc(AC ac) {
  Queue f = emptyQueue();
  List l = transitionsList(ac, 0, 0);

  while (l != NULL) {
    int p = l->p;
    l = nextList(l);
    f = pushQueue(f, p);
    ac->supp[p] = 0;
  }

  while (!isEmptyQueue(f)) {
    int r = getHead(f);
    f = popQueue(f);
    l = transitionsList(ac, -1, r);

    while (l != NULL) {
      r = l->e;
      int a = l->a;
      int p = l->p;
      l = nextList(l);
      pushQueue(f, p);

      int s = ac->supp[r];
      while (delta(ac, s, a) == -1) {
        s = ac->supp[s];
      }
      ac->supp[p] = delta(ac, s, a);

      if (ac->exits[ac->supp[p]]) {
        ac->exits[p] = 1;
      }
    }
  }
  freeQueue(f);
}

void printStates(size_t *etats, size_t n) {
  printf("ETATS : (");
  for (size_t i = 0; i < n; i++) {
    printf("%zu, ", etats[i]);
  }
  printf(")\n\n");
}

void printMatrix(AC ac) {
  printf("AUTOMATE :\n      |");
  for (int i = 65; i < ac->alphabetSize; i++) {
    printf("%c|", i);
  }

  printf("\n――――");
  for (int i = 0; i < ac->alphabetSize; i++) {
    printf("―");
  }
  printf("\n");

  // matrix
  for (size_t j = 0; j < ac->nextState; j++) {
    if (j < 10)
      printf("%zu --> |", j);
    else
      printf("%zu -> |", j);

    for (int i = 65; i < ac->alphabetSize; i++) {
      if (delta(ac, j, i) == -1 || delta(ac, j, i) == 0)
        printf(" |");
      else
        printf("%d|", ac->matrix[j][i]);
    }
    printf("\n");
  }

  printf("――――");
  for (int i = 0; i < ac->alphabetSize; i++) {
    printf("―");
  }
  printf("\n\n");
}

void freeAc(AC ac) {
  for (size_t i = 0; i < ac->nextState; i++) {
    free(ac->matrix[i]);
  }
  free(ac->matrix);
  free(ac->exits);
  free(ac->supp);
  free(ac);
}

AC preAC(char **X, size_t k) {
  AC ac = createAc();
  // Insère tous les mots dans l'automate
  for (size_t i = 0; i < k; i++) {
    addWord(ac, X[i]);
  }

  // Renvoie toutes lettres de q0 à l'état 0
  for (int i = 0; i < ac->alphabetSize; i++) {
    if (delta(ac, 0, i) == -1)
      ac->matrix[0][i] = 0;
  }

  // Compléter avec les suppléants / exits
  fillAc(ac);

  return ac;
}

void ac(char **X, size_t k, char *y, size_t n, size_t niveauVerbosite) {
  size_t compteur = 0;
  AC ac = preAC(X, k);
  size_t e = 0;
  size_t *etats = malloc(2 * n * sizeof(size_t));

  for (size_t j = 0; j < n; j++) {
    while (delta(ac, e, y[j]) == -1) {
      e = ac->supp[e];
    }
    e = delta(ac, e, y[j]);
    etats[j] = e;
    if (ac->exits[e])
      compteur++;
  }

  // Affiche les infos selon le niveau de verbosite 0/v/vv choisi
  if (niveauVerbosite > 1)
    printMatrix(ac);
  if (niveauVerbosite == 1 || niveauVerbosite == 3)
    printStates(etats, n);

  printf("RESULTAT : L'ensemble des mots ");
  for (size_t i = 0; i < k; i++) {
    if (i == k - 1 && k > 1)
      printf("et \"%s\" ", X[i]);
    else
      printf("\"%s\", ", X[i]);
  }
  printf("apparait %zu fois dans le texte.\n", compteur);
  freeAc(ac);
  free(etats);
}

////////////////////////////////////////////////
// Lignes de commande
////////////////////////////////////////////////
size_t extractWordNumber(char *content, size_t n) {
  size_t nbMots = 0;
  for (size_t i = 0; i < n; i++) {
    if (content[i] == '\n')
      nbMots++;
  }
  return nbMots;
}

char **extractWords(char *content, size_t nbWords) {
  char **mots = malloc(nbWords * sizeof(char *));
  size_t y = 0;
  for (size_t i = 0; i < nbWords; i++) {
    size_t position = 0;

    mots[i] = malloc(TAILLE_MOTS * sizeof(char));
    while (content[y] != '\n' && content[y] != '\0') {
      mots[i][position] = content[y];
      position++;
      y++;

      if (position >= TAILLE_MOTS) {
        mots[i] = realloc(mots[i], (position + TAILLE_MOTS) * sizeof(char));
      }
    }
    mots[i][position] = '\0';
    y++;
  }
  return mots;
}

char *readFile(char *file) {
  FILE *f;
  if ((f = fopen(file, "r")) == NULL) {
    printf("Impossible d'ouvrir le fichier %s\n", file);
    exit(EXIT_FAILURE);
  }
  fseek(f, 0, SEEK_END);
  size_t n = ftell(f);
  fseek(f, 0, SEEK_SET);
  char *texte = (char *)malloc((n + 1) * sizeof(char));
  if (texte == NULL) {
    fprintf(stderr, "Erreur dans l'allocation du texte");
    return NULL;
  }
  if (fread(texte, n, 1, f)) {
  };
  texte[n] = '\0';
  fclose(f);
  return texte;
}

char **readWords(char *file) {
  char *f = readFile(file);
  size_t nbMots = extractWordNumber(f, strlen(f));
  char **mots = extractWords(f, nbMots);
  free(f);
  printf("MOTS :\n");
  for (size_t i = 0; i < nbMots; i++) {
    printf("%s\n", mots[i]);
  }
  return mots;
}

void printHelp() {
  printf("USAGE: rechAC [-h] [-v] [-vv] mot texte\n\n");
  printf("Personal implementation of Aho-Corasick algorithm\n\n");
  printf("POSITIONAL ARGUMENTS:\n");
  printf("\tmot\t\tPath to file containing words to search\n");
  printf("\ttexte\t\tPath to file containing text to search in\n\n");

  printf("OPTIONAL ARGUMENTS:\n");
  printf("\t-h\t\tshow this help message and exit\n");
  printf("\t-v\t\tshow states during search\n");
  printf("\t-vv\t\tshow Aho-Corasick data structure\n");
}

Args parseArgs(int argc, char **argv) {
  // Si le seul argument est -h, affiche l'aide et quitte
  if (argc == 2 && strcmp(argv[1], "-h") == 0) {
    printHelp();
    exit(0);
  }

  // Si pas le bon nombre d'arguments, affiche l'aide et quitte
  if (argc < 3 || argc > 5) {
    printHelp();
    fprintf(stderr, "Pas assez d'arguments");
    exit(EXIT_FAILURE);
  }

  Args args = malloc(sizeof(struct _args));

  char *contenu_mots = readFile(argv[argc - 2]);
  args->nbWords = extractWordNumber(contenu_mots, strlen(contenu_mots));
  args->words = extractWords(contenu_mots, args->nbWords);
  args->text = readFile(argv[argc - 1]);

  // Parse le niveau de verbosite : rien, -v ou -v -vv / -vv -v
  args->verbosityLevel = 0;
  if ((argc == 4) && (strcmp(argv[1], "-v") == 0))
    args->verbosityLevel = 1;
  if ((argc == 4) && (strcmp(argv[1], "-vv") == 0))
    args->verbosityLevel = 2;
  // si 5 args et (-v -vv) ou (-vv -v)
  if ((argc == 5) &&
      (((strcmp(argv[1], "-v") == 0) && (strcmp(argv[2], "-vv") == 0)) ||
       ((strcmp(argv[1], "-vv") == 0) && (strcmp(argv[2], "-v") == 0))))
    args->verbosityLevel = 3;

  return args;
}

void freeArgs(Args args) {
  free(args->text);
  for (size_t i = 0; i < args->nbWords; i++) {
    free(args->words[i]);
  }
  free(args->words);
  free(args);
}
