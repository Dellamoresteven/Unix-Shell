/*
 * CS252: Shell project
 *
 * Template file.
 * You will need to add more code here to execute the command table.
 *
 * NOTE: You are responsible for fixing any bugs this code may have!
 *
 * DO NOT PUT THIS PROJECT IN A PUBLIC REPOSITORY LIKE GIT. IF YOU WANT 
 * TO MAKE IT PUBLICALLY AVAILABLE YOU NEED TO REMOVE ANY SKELETON CODE 
 * AND REWRITE YOUR PROJECT SO IT IMPLEMENTS FUNCTIONALITY DIFFERENT THAN
 * WHAT IS SPECIFIED IN THE HANDOUT. WE OFTEN REUSE PART OF THE PROJECTS FROM  
 * SEMESTER TO SEMESTER AND PUTTING YOUR CODE IN A PUBLIC REPOSITORY
 * MAY FACILITATE ACADEMIC DISHONESTY.
 */

#include <cstdio>
#include <cstdlib>
#include <fcntl.h>              /* Obtain O_* constant definitions */
#include <unistd.h>

#include <iostream>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include "command.hh"
#include <sys/wait.h>
#include "shell.hh"


Command::Command() {
    // Initialize a new vector of Simple Commands
    _simpleCommands = std::vector<SimpleCommand *>();

    _outFile = NULL;
    _inFile = NULL;
    _errFile = NULL;
    _background = false;
    _append = 0;
}

void Command::insertSimpleCommand( SimpleCommand * simpleCommand ) {
    // add the simple command to the vector
    _simpleCommands.push_back(simpleCommand);
}

void Command::clear() {
    // deallocate all the simple commands in the command vector
    for (auto simpleCommand : _simpleCommands) {
        delete simpleCommand;
    }

    // remove all references to the simple commands we've deallocated
    // (basically just sets the size to 0)
    int x = 0;
    _simpleCommands.clear();
    if(_outFile == _errFile){
        delete _outFile;
        x = 1;
    }

    if ( _outFile && (x==0)) {
        delete _outFile;
    }
    _outFile = NULL;

    if ( _inFile ) {
        delete _inFile;
    }
    _inFile = NULL;

    if ( _errFile && (x==0)) {
        delete _errFile;
    }
    _errFile = NULL;

    _background = false;
}

void Command::print() {
    printf("\n\n");
    printf("              COMMAND TABLE                \n");
    printf("\n");
    printf("  #   Simple Commands\n");
    printf("  --- ----------------------------------------------------------\n");

    int i = 0;
    // iterate over the simple commands and print them nicely
    for ( auto & simpleCommand : _simpleCommands ) {
        printf("  %-3d ", i++ );
        simpleCommand->print();
    }

    printf( "\n\n" );
    printf( "  Output       Input        Error        Background\n" );
    printf( "  ------------ ------------ ------------ ------------\n" );
    printf( "  %-12s %-12s %-12s %-12s\n",
            _outFile?_outFile->c_str():"default",
            _inFile?_inFile->c_str():"default",
            _errFile?_errFile->c_str():"default",
            _background?"YES":"NO");
    printf( "\n\n" );
}

void Command::execute() {
    // Don't do anything if there are no simple commands
    if ( _simpleCommands.size() == 0 ) {
        Shell::prompt();
        // printf("SHOULDNT GO HERE\n");
        return;
    }
    // printf("STeve : %s\n\n\n", (_simpleCommands[0]->_arguments[0])->c_str());
    if (!strcmp(_simpleCommands[0]->_arguments[0]->c_str(), "exit")) {
        exit(1);
    }
    if (!strcmp(_simpleCommands[0]->_arguments[0]->c_str(), "cd")) {
        chdir(_simpleCommands[0]->_arguments[1]->c_str());
    }
    // int tempin = dup(0);
    // int tempout = dup(1);

    int tmpin = dup(0);
    int tmpout = dup(1);
    int tmperr = dup(2);
    int v;
    int vt;
    if(_outFile != NULL){
        if((_append == 1) && (_errFile != NULL)){
            printf("here2\n");
            v = creat( (_outFile)->c_str(), 0666 );
            vt = creat( (_errFile)->c_str(), 0666 );
            dup2(v , 1);
            dup2(vt , 2);
            close( v );
            close( vt );
        }else if(_append == 1){
            printf("here1\n");
            // printf("GOES HERE\n");
            FILE* outfile = fopen((_outFile)->c_str(), "a+");
            FILE* errfile = fopen((_errFile)->c_str(), "a+");
            // int v = open((_outFile)->c_str(),O_CREAT | O_RDWR | O_APPEND| S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
            // v = creat( (_outFile)->c_str(), 0666 );
            dup2(fileno(outfile), 1);
            dup2(fileno(errfile), 2);
            // dup2(v , 1);
            // cl);ose( v 
            fclose(outfile);
            fclose(errfile);
        }else if((_append == 0) && (_errFile != NULL)){
            printf("here\n");

        }else{
            // printf("here6\n");
            v = open( (_outFile)->c_str(),  O_CREAT | O_RDWR );
            dup2(v , 1);
            close( v );
        }
        // _outFile = "\n";
        // v = open( (_outFile)->c_str(),  O_APPEND | O_RDWR );
        // dup2(v, "\n");
        // close( v );
    }

    if(_inFile != NULL){
        printf("GOTHERE\n");
        int v = creat( (_inFile)->c_str(), 0666 | O_RDONLY );

        dup2(v , 0);
        close( v );
    }
    _append = 0;
    

    // Print contents of Command data structure
    // printf("STEVE: %d\n", _simpleCommands.size());
    int ret;
    for (int i = 0; i < (int)_simpleCommands.size(); ++i)
    {

        if ( _simpleCommands.size() == 0 ) {
            Shell::prompt();
            return;
        }
        ret = fork();
        if(ret == 0){
            char *x[_simpleCommands[i]->_arguments.size()];
            for (int j = 0; j < (int)_simpleCommands[i]->_arguments.size(); j++)
            {
                x[j] = const_cast<char*>((_simpleCommands[i]->_arguments[j])->c_str());
                x[j+1] = NULL;
            }
            execvp((x[0]), x);
            free(x);
            perror("execvp");
            _exit(1);
        }else if(ret < 0){
            perror("Fork");
            return;
        }
    }
    if(!_background){
        waitpid(ret, NULL, 0);
    }
    dup2( tmpin, 0 );
    dup2( tmpout, 1 );
    dup2( tmperr, 2 );
    // close(fdpipe[0]);
    // close(fdpipe[1]);
    close( tmpin );
    close( tmpout );
    close( tmperr );
  // int ret; 
  // for ( int i = 0; i < _numberOfSimpleCommands; i++ ) { 
  //   ret = fork(); 
  //   if (ret == 0) { 
  //   //child 
  //       execvp(sCom[i]­>_args[0], sCom[i]­>_args); 
  //       perror("Exevvp");
  //       _exit(1);
  //   } 
  //   else if (ret < 0) { 
  //       perror("Fork");
  //       return;
  //   }  
  // } // for 
  // if (!background) { 
  //   waitpid(ret, NULL);
  // }    
  // print();

    // Add execution here
    // For every simple command fork a new process
    // Setup i/o redirection
    // and call exec

    // Clear to prepare for next command
    clear();

    // Print new prompt
    Shell::prompt();
}

SimpleCommand * Command::_currentSimpleCommand;
