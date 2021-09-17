/* 
 * Main source code file for lsh shell program
 *
 * You are free to add functions to this file.
 * If you want to add functions in a separate file 
 * you will need to modify Makefile to compile
 * your additional functions.
 *
 * Add appropriate comments in your code to make it
 * easier for us while grading your assignment.
 *
 * Submit the entire lab1 folder as a tar archive (.tgz).
 * Command to create submission archive: 
      $> tar cvf lab1.tgz lab1/
 *
 * All the best 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "parse.h"

#include<unistd.h> //execvp()



#include <sys/types.h>
#include <sys/wait.h> // waitpid()


#include <signal.h>


#include "paths.h"

#define TRUE 1
#define FALSE 0

void RunCommand(int, Command *);
void DebugPrintCommand(int, Command *);
void PrintPgm(Pgm *);
void stripwhite(char *);
char* setcustomprompt();
_Bool checkBuiltIn(Command * cmd);
void signalHandler(int signal);
void updatePrompt();

int main(void) {

  signal(SIGINT,signalHandler);


  Command cmd;
  int parse_result;

  char* prompt = setcustomprompt();

  while (TRUE) {

    char *line;
    line = readline(prompt);

    /* If EOF encountered, exit shell */
    if (!line) {
      break;
    }


    /* Remove leading and trailing whitespace from the line */
    stripwhite(line);
    /* If stripped line not blank */
    if (*line)
    {
      add_history(line);
      parse_result = parse(line, &cmd);
      RunCommand(parse_result, &cmd);
    }

    /* Clear memory */
    free(line);
  }
  return 0;
}


/* Execute the given command(s).

 * Note: The function currently only prints the command(s).
 * 
 * TODO: 
 * 1. Implement this function so that it executes the given command(s).
 * 2. Remove the debug printing before the final submission.
 */
void RunCommand(int parse_result, Command *cmd) {

  _Bool check = 0;
  _Bool* isavailable = &check;
  _Bool builtInCommand = checkBuiltIn(cmd);


  if ( builtInCommand ) {
   builtInCommand = 0;
   if (strcmp(*cmd->pgm->pgmlist,"cd") == 0 ) {
      chdir(cmd->pgm->pgmlist[1]);
    } else if (strcmp(*cmd->pgm->pgmlist,"exit") == 0) {
      kill(getpid(),SIGKILL);
    }
  } else {

    const char* location = extractpath(cmd, isavailable);

    if ( check ) {

      pid_t pid = fork();
      int* status;
      int options = 0;

      if ( pid < 0) {
        printf("Error forking child.\n");
      } else if ( pid == 0 ){

         check = 0;
         long length = (strlen(location)+1+strlen(*cmd->pgm->pgmlist));
         char* fullexec = malloc(length);

         strcpy(fullexec,location);
          strcat(fullexec,"/");
          strcat(fullexec,*cmd->pgm->pgmlist);

         execvp(fullexec,cmd->pgm->pgmlist);
          //exit(0); // Behövs inte ?
      } else {
        if ( cmd->background == 0 ) {
         waitpid(pid, status, options);
        }
      }  

    } else {
     printf("Command not available = %s\n",*cmd->pgm->pgmlist );
    }
  }

DebugPrintCommand(parse_result, cmd);

}

/* 
 * Print a Command structure as returned by parse on stdout. 
 * 
 * Helper function, no need to change. Might be useful to study as inpsiration.
 */
void DebugPrintCommand(int parse_result, Command *cmd)
{
  if (parse_result != 1) {
    printf("Parse ERROR\n");
    return;
  }
  printf("------------------------------\n");
  printf("Parse OK\n");
  printf("stdin:      %s\n", cmd->rstdin ? cmd->rstdin : "<none>");
  printf("stdout:     %s\n", cmd->rstdout ? cmd->rstdout : "<none>");
  printf("background: %s\n", cmd->background ? "true" : "false");
  printf("Pgms:\n");
  PrintPgm(cmd->pgm);
  printf("------------------------------\n");
}


/* Print a (linked) list of Pgm:s.
 * 
 * Helper function, no need to change. Might be useful to study as inpsiration.
 */
void PrintPgm(Pgm *p)
{
  if (p == NULL)
  {
    return;
  }
  else
  {
    char **pl = p->pgmlist;

    /* The list is in reversed order so print
     * it reversed to get right
     */
    PrintPgm(p->next);
    printf("            * [ ");
    while (*pl)
    {
      printf("%s ", *pl++);
    }
    printf("]\n");
  }
}


/* Strip whitespace from the start and end of a string. 
 *
 * Helper function, no need to change.
 */
void stripwhite(char *string)
{
  register int i = 0;

  while (isspace(string[i]))
  {
    i++;
  }

  if (i)
  {
    strcpy(string, string + i);
  }

  i = strlen(string) - 1;
  while (i > 0 && isspace(string[i]))
  {
    i--;
  }

  string[++i] = '\0';
}

char* setcustomprompt() {
  char* prompt;
  
  char* user = getlogin();
  size_t len = 25;
  char* host = malloc(len); 
  gethostname(host,len);
  char* sizedhost = malloc(len);
  char* downsized = sizedhost;
  while ( *host ) {
    *sizedhost = *host;
    sizedhost++;
    host++;
  }
  *sizedhost ='\0';
  strcat(user,"\x1B[33m");
  //strcat(prompt,user);
  strcat(user,"@");
  strcat(user,downsized);
  strcat(user,"--> ");
  strcat(user,"\x1B[37m");


 // strcat(user,"@");
  //strcat(user,downsized);
  //strcat(user,"--> ");
  return user;
}

_Bool checkBuiltIn(Command* cmd) {
  return strcmp(*cmd->pgm->pgmlist,"cd")==0 || 
    strcmp(*cmd->pgm->pgmlist,"exit")==0;
}

void signalHandler(int signal) {
  if ( signal == SIGINT )
    printf("SIGINT\n");
}

void updatePrompt(){
  //New dir
}
