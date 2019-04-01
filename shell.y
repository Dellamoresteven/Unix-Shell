
/*
 * CS-252
 * shell.y: parser for shell
 *
 * This parser compiles the following grammar:
 *
 *	cmd [arg]* [> filename]
 *
 * you must extend it to understand the complete shell grammar
 *
 */


%code requires 
{
#include <string>


#if __cplusplus > 199711L
#define register      // Deprecated in C++11 so remove the keyword
#endif
}

%union
{
  char        *string_val;
  // Example of using a c++ type in yacc
  std::string *cpp_string;
}

%token <cpp_string> WORD
%token NOTOKEN GREAT NEWLINE GREATGREATAMPERSAND GREATGREAT AMPERSAND PIPE LESS GREATAMPERSAND

%{

//#define yylex yylex
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <regex.h>
#include <dirent.h>
#include <unistd.h>
#include <signal.h>
#include "command.hh"
#include "shell.hh"

void yyerror(const char * s);
int yylex();

%}

%%

goal:
  commands
  ;

commands:
  command
  | commands command
  ;

command: simple_command
       ;

simple_command:	
  pipe_list iomodifier_list background_optional NEWLINE {
    // printf("   Yacc: Execute command\n");
    Shell::_currentCommand.execute();
    // Shell::prompt();
  }
  | NEWLINE {
    Shell::prompt();
  }
  | error NEWLINE { yyerrok; }
  ;

command_and_args:
  command_word argument_list {

    Shell::_currentCommand.insertSimpleCommand( Command::_currentSimpleCommand );
  }
  ;

argument_list:
  argument_list argument{}
  | /* can be empty */
  ;

argument:
  WORD {
    // printf("   Yacc: insert argument \"%s\"\n", $1->c_str());
    Command::_currentSimpleCommand->insertArgument( $1 );\
  }
  ;

command_word:
  WORD {
    // printf("   Yacc: insert command1 \"%s\"\n", $1->c_str());
    Command::_currentSimpleCommand = new SimpleCommand();
    Command::_currentSimpleCommand->insertArgument( $1 );
  }
  ;//

pipe_list:
  pipe_list PIPE command_and_args
  | command_and_args
  ;


iomodifier_opt:
  GREATGREAT WORD{ //apends to outfile
    // printf("   Yacc: insert output0 \"%s\"\n", $2->c_str());
    Shell::_currentCommand._outFile = $2;
    Shell::_currentCommand._append = 1;
    // Command::_currentSimpleCommand._outFile = $2;
  }
  | GREAT WORD {//overrides the outfile
    // printf("   Yacc: insert output1 \"%s\"\n", $2->c_str());
    Shell::_currentCommand._outFile = $2;
  }
  | GREATAMPERSAND WORD { //changeds errfile and outfile
    // printf("   Yacc: insert output3 \"%s\"\n", $2->c_str());
    Shell::_currentCommand._outFile = $2;
    Shell::_currentCommand._errFile = $2;

  }
  | GREATGREATAMPERSAND WORD { //changes errfile and outfile but it appends
    // printf("   Yacc: insert output4 \"%s\"\n", $2->c_str());
    Shell::_currentCommand._outFile = $2;
    Shell::_currentCommand._errFile = $2;
    Shell::_currentCommand._append = 1;
  }
  | LESS WORD { //changes infile
    // printf("   Yacc: insert output5 \"%s\"\n", $2->c_str());
    Shell::_currentCommand._inFile = $2;
  }
  ;

iomodifier_list:
  iomodifier_list iomodifier_opt
  | iomodifier_opt
  |
  ;

background_optional:
  AMPERSAND {
    Shell::_currentCommand._background = true;
  }
  |
  ;


%%

//add methods if needed


void
yyerror(const char * s)
{
  fprintf(stderr,"%sS", s);
}

#if 0
main()
{
  yyparse();
}
#endif
