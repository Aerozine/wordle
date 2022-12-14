#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "solver.h"
#include "LinkedList.h"
#include "dict.h"
#include "wordle.h"

#define SIZEGET 7  // words of 5 characters + \n + terminator
#define SIZEWORD 5 // words of 5 characters
#define LETTERS 26 // nb of letters in the alphabet

struct Solver_t {
  List *guesses;
  List *answers;
};

static void terminate(char *m) {
  printf("%s\n", m);
  exit(EXIT_FAILURE);
}
// a copy of strdup , not present in C99
static char *newstrdup(const char *s) {
  size_t len = strlen(s) + 1;
  void *new = malloc(len);
  if (new == NULL)
    return NULL;
  return (char *)memcpy(new, s, len);
}

Solver *solverStart(char *answers_file, char *guesses_file) {

  Solver *solver = malloc(sizeof(Solver));
  solver->answers = llCreateEmpty();
  solver->guesses = llCreateEmpty();
  FILE *Fguess = fopen(guesses_file, "r");
  FILE *Fanswers = fopen(answers_file, "r");
  if (!Fguess || !Fanswers)
    terminate("SolverStart: guesses_file of answers_file cannot be opened");
  char wordg[SIZEGET], worda[SIZEGET];
  while (fgets(wordg, SIZEGET, Fguess)) {
    wordg[SIZEWORD] = '\0';
    llInsertLast(solver->guesses, llCreateNode((void *)newstrdup(wordg)));
  }
  while (fgets(worda, SIZEGET, Fanswers)) {
    worda[SIZEWORD] = '\0';
    llInsertLast(solver->answers, llCreateNode((void *)newstrdup(worda)));
  }
  fclose(Fguess);
  fclose(Fanswers);

  return solver;
}

void solverFree(Solver *solver) {
  llFreeData(solver->answers);
  llFreeData(solver->guesses);
}

int solverGetNbAnswers(Solver *solver) { return llLength(solver->answers); }
static int test(void *answer, void *data) {
  char *guess = ((char **)(data))[0];
  char *pattern = ((char **)(data))[1];
  char *newpatern = wordleComputePattern(guess, (char *)answer);
  return strcmp(pattern, newpatern)!=0;
}
int solverUpdate(Solver *solver, char *guess, char *pattern) {
  char *data[] = {guess, pattern};
  return llFilter(solver->answers, test, data, 0, 1);
}
double solverBestGuess(Solver *solver, char *guess) {
  Node *wg = llHead(solver->guesses);
  if (llLength(solver->answers) == 1) {
    strcpy(guess, (char *)llData(llHead(solver->answers)));
    return 0.0;
  }
  int bestSum = -1;
  while (wg) {
    int sum = 0;
    Node *wa = llHead(solver->answers);
    Dict *dict = dictCreate(llLength(solver->answers));
    while (wa) {
      char *data[] = {(char *)llData(wg),
                      wordleComputePattern(llData(wg), llData(wa))};
      if (!dictContains(dict, data[1])) {
        dictInsert(dict, data[1], (double)llFilter(solver->answers, test, data, 1, 0));
      }
      sum += (int)dictSearch(dict, data[1]);
      wa = llNext(wa);
    }
    if (bestSum <= sum) {
      bestSum = sum;
      strcpy(guess, (char *)llData(wg));
    }
    wg = llNext(wg);
  }
  return (double)bestSum / (double)llLength(solver->answers);
}