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
#include <fcntl.h> // open()
#include <signal.h>


#define TRUE 1
#define FALSE 0

#define YELLOW  "\x1B[33m"
#define MAGENTA   "\x1B[35m"
#define RESET "\x1B[0m"

void RunCommand(int, Command *);
void DebugPrintCommand(int, Command *);
void PrintPgm(Pgm *);
void stripwhite(char *);
void standardIO(Command* cmd);
void runPipedProcesses(Pgm *pgm, int savedin,char* lastcmd);
char* setCustomPrompt(char* cwd);
char* getCurrentWorkingDir();
_Bool checkBuiltIn(char* cmd);

pid_t pid = -1;

static void actionMain (int sig, siginfo_t *siginfo, void *context) {}

void killChild(int sig) {
  kill(pid,SIGTERM);
}


int main(void) {

  struct sigaction mainAct;
  memset (&mainAct, '\0', sizeof(mainAct));
  mainAct.sa_sigaction = &actionMain;
  mainAct.sa_flags = SA_SIGINFO;
  sigaction(SIGINT, &mainAct, NULL);

  Command cmd;
  int parse_result;

  while (TRUE) {

    char *line;
    char* cwd = getCurrentWorkingDir();
    char* prompt = setCustomPrompt(cwd);
    line = readline(prompt);

    /* If EOF encountered, exit shell */
    if (!line) {
      break;
    }


    /* Remove leading and trailing whitespace from the line */
    stripwhite(line);
    /* If stripped line not blank */
    if (*line) {
      add_history(line);
      parse_result = parse(line, &cmd);
      RunCommand(parse_result, &cmd);
    }

    /* Clear memory */
    free(line);
  }
  return 0;
}

/*
*Runs one command as a child. Background or not
*/
void runChild(Pgm* pgm,_Bool inBackground) {
    int* status;
    int options = 0;
    pid = fork();
    
    if (pid < 0) {
      printf("Error forking child.\n");
    } else if ( pid == 0 ) {
      int res = execvp(pgm->pgmlist[0],pgm->pgmlist);
      printf("Command not available, try installing it.\n");
      exit(res);
    } else {
      if ( inBackground == 0 ) {
        waitpid(pid, status, options);
      }
    }
}



/*
* Runs commands recursive piping the data until last command.
* savedin is used to restore the standard out since we modified the parent.
* lastcmd is the last command to execute.
*
*/
void runPipedProcesses(Pgm *pgm, int savedin, char* lastcmd) {
  
  if (pgm == NULL){
    return;
  } else {

    int last = 0;

    runPipedProcesses(pgm->next,savedin,lastcmd);
    
    if ( strcmp(pgm->pgmlist[0],lastcmd)==0 ){
      last = 1;
    }

    int pipet[2];
    pipe(pipet);


    int* status;
    int options = 0;
    pid_t pid = fork();

    if ( pid < 0 ) {
      printf("ERROR");
    } else if ( pid == 0 ) {
          close(pipet[0]);
          if ( !last) {
            dup2(pipet[1], 1); // Sista gågen låter vi stdout gå till terminalen istället för pipe
          }
          close(pipet[1]);
          int res = execvp(pgm->pgmlist[0],pgm->pgmlist);
          printf("Command not available, try installing it.\n");
          exit(res);
    } else {
          close(pipet[1]);
          if (!last) {
            dup2(pipet[0], 0);
          } else {
            dup2(savedin,0); // återställ stdin i parent
            close(savedin);
          }
          close(pipet[0]);
          waitpid(pid, status, options);
    }
  }
}



void RunCommand(int parse_result, Command *cmd) {
  if ( cmd->background == 0 ) {
    signal(SIGINT,killChild);
  } else {
    signal(SIGINT,SIG_IGN);
    signal(SIGCHLD,SIG_IGN); // För när child är färdig.
  }

  _Bool builtInCommand = checkBuiltIn(*cmd->pgm->pgmlist);

  if ( builtInCommand ) {

    builtInCommand = 0;
    if (strcmp(*cmd->pgm->pgmlist,"cd") == 0 ) {
      if ( chdir(cmd->pgm->pgmlist[1]) == -1 ) {
        printf("Error changing directory.\n");
      }

    } else if (strcmp(*cmd->pgm->pgmlist,"exit") == 0) {
      kill(getpid(),SIGTERM); //SIGKILL
    }


  } else {


      if ( cmd->rstdin!= NULL || cmd->rstdout!=NULL ) {
        standardIO(cmd);
      } else {

        if ( cmd->pgm->next != NULL ) {
          int savedin = dup(0);
          runPipedProcesses(cmd->pgm,savedin,cmd->pgm->pgmlist[0]);
        } else {
          runChild(cmd->pgm, cmd->background);
        }
      }
  //DebugPrintCommand(parse_result, cmd);
  }
}


void standardIO(Command * cmd) {

  int fileDescIn;
  int fileDescOut;

  if ( cmd->rstdin != NULL ) {
   fileDescIn = open(cmd->rstdin,0,O_RDONLY); //O_PATH skall ge en filedescriptor FLAGGA,,,,, O_RDONLY är mode WRONLY, RDWR
  }
  
  if ( cmd->rstdout != NULL ) {
    fileDescOut = open(cmd->rstdout,O_RDWR|O_CREAT|O_APPEND,0664);
  }

  int* status;
  int options = 0;
  pid_t pid = fork();

    if ( pid < 0 ) {
      printf("ERROR FORKING");
    } else if ( pid == 0 ) {

      if ( cmd->rstdin != NULL ) {
        dup2(fileDescIn,0);
      }

      if ( cmd->rstdout != NULL ) {
        dup2(fileDescOut,1);
      }

      int res = execvp(cmd->pgm->pgmlist[0],cmd->pgm->pgmlist);
      printf("Command not available, try installing it.\n");
      exit(res);


    } else {


      if (!cmd->background) {
        waitpid(pid,status,options);
      }

      if ( cmd->rstdin != NULL ) {
        close(fileDescOut);
      }

      if ( cmd->rstdout != NULL ) {
        close(fileDescIn);
      }
    }
}


/* 
 * Print a Command structure as returned by parse on stdout. 
 * 
 * Helper function, no need to change. Might be useful to study as inpsiration.
 */
void DebugPrintCommand(int parse_result, Command *cmd){
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
void PrintPgm(Pgm *p) {
  if (p == NULL){
    return;
  } else {


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


_Bool checkBuiltIn(char* cmd) { return strcmp(cmd,"cd")==0 || strcmp(cmd,"exit")==0; }


char* getCurrentWorkingDir() {
  char* buff = malloc(2048);
  getcwd(buff, 2048);
  return buff;
}


char* setCustomPrompt(char * cwd) {

  char* prompt = getlogin();
  strcat(prompt,MAGENTA);
  strcat(prompt,"||");
  strcat(prompt,YELLOW);
  strcat(prompt,".");
  strcat(prompt,cwd);
  strcat(prompt,"--> ");
  strcat(prompt,RESET);
  return prompt;

}


