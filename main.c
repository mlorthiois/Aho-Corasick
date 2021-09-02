#include <stdio.h>
#include <string.h>

#include "ac.h"

int main(int argc, char **argv) {
  Args args = parseArgs(argc, argv);

  ac(args->words, args->nbWords, args->text, strlen(args->text),
     args->verbosityLevel);

  freeArgs(args);

  return EXIT_SUCCESS;
}
