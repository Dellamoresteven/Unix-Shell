
/*
 *
 * CS-252 
 * shell.l: lexical analyzer for shell
 * You have to extend it.
 *
 */

%{
#include <stdlib.h>
#include <cstring>
#include "y.tab.hh"

static  void yyunput (int c,char *buf_ptr  );

void myunputc(int c) {
  unput(c);
}

%}

%option noyywrap

%%

\n 	{
		// printf("NewLine\n");
		return NEWLINE;
	}

[ \t] 	{
		/* Discard spaces and tabs */
     	}	

">>&" 	{
		// printf("GREATGREATAMPERSAND\n");
		return GREATGREATAMPERSAND;
		}

">>"	{
		// printf("GREATGREAT\n");
		return GREATGREAT;
		}

">&"	{
		// printf("GREATAMPERSAND\n");
		return GREATAMPERSAND;
		}

">"	{
		// printf("GREAT\n");
		return GREAT;
	}

"&"	{
		// printf("AMPERSAND\n");
		return AMPERSAND;
	}

"|"	{
		// printf("PIPE\n");
		return PIPE;
	}

"<"	{
		// printf("LESS\n");
		return LESS;
	}

[ \t] {
  /* Discard spaces and tabs */
}


">" {
  return GREAT;
}

[^ \t\n][^ \t\n]*  {
  /* Assume that file names have only alpha chars */
  yylval.cpp_string = new std::string(yytext);
  // printf("1%d\n", WORD);
  return WORD;
}
