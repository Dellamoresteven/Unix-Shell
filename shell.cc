#include <cstdio>

#include "shell.hh"

int yyparse(void);

void Shell::prompt() {
  printf(""); //richard hansen came up with the name
  fflush(stdout);
}

int main() {
  Shell::prompt();
  yyparse();
}

Command Shell::_currentCommand;
