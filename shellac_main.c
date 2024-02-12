#include "shellac.h"

void print_help(){
  char *helpstr = "\
SHELLAC COMMANDS\n\
help               : show this message\n\
exit               : exit the program\n\
jobs               : list all background jobs that are currently running\n\
pause <secs>       : pause for the given number of seconds, fractional values supported\n\
wait <jobnum>      : wait for given background job to finish, error if no such job is present\n\
tokens [arg1] ...  : print out all the tokens on this input line to see how they apper\n\
command [arg1] ... : Non-built-in is run as a job\n\
";
  printf(helpstr);
}

int main(int argc, char *argv[]){
  int echo = 0;                                //controls echoing, 0: echo off, 1: echo on
  if(argc > 1 && strcmp("--echo",argv[1])==0) { //turn echoing on via -echo command line option
    echo=1;
  }
  
  char input[5000];    //direct user input
  char *result;    //result of using fget()
  char *tokens[255];    //the input into separate strings
  int ntok;    //number of tokens variable
  shellac_t shellac;    //eclaring shuttle
  shellac_init(&shellac);    //initializing shuttle

  while(1){
    printf("(shellac) ");
    result = fgets(input, 5000, stdin);    //reads the input
    if(result == NULL){                 //check for end of input
      printf("\nEnd of input\n");     //found end of input
      break;                          
    }
    if (result[0] == '\n'){    //check for enter as input to avoid seg errors
      tokens[0] = "\n";    //sets token[0] as enter
    } else {
      tokenize_string(input, tokens, &ntok);    //formats the command line input into tokens
    }
    if( strcmp("exit", tokens[0])==0 ){     //check for exit command
      if(echo){    //check for echo
        printf("exit\n");
      }
      break;    //exits the while loop
    }
    else if( strcmp("help", tokens[0])==0 ){ //help command         
      if(echo){    //check for echo
        printf("help\n");
      }
      print_help();    //calls print_help()
    }
    else if( strcmp("jobs", tokens[0])==0 ){ //jobs command         
      if(echo){    //check for echo
        printf("jobs\n");
      }
      shellac_print_jobs(&shellac);    //calls shellac_print_jobs();
    }
    else if( strcmp("pause", tokens[0])==0 ){ //pause command         
      if(echo){    //check for echo
        printf("pause %s\n", tokens[1]);
      }
      double time = strtod(tokens[1], NULL);    //converts the string into a double
      printf("Pausing for %.3f seconds\n", time);    //prints rounded to 3 decimals
      pause_for(time);    //calls pause()
    }
    else if( strcmp("wait", tokens[0])==0 ){ //wait command         
      if(echo){    //check for echo
        printf("wait %s\n", tokens[1]);
      }
      shellac_wait_one(&shellac, atoi(tokens[1]));    //calls shellac_wait_one
    }
    else if( strcmp("tokens", tokens[0])==0 ){ //tokens command         
      if(echo){    //check for echo
        printf("tokens");
        for (int i = 1; i < ntok; i++){ //loop to repeat tokens
          printf(" %s", tokens[i]);
        }
        printf("\n");
      }
      printf("%d tokens in input line\n", ntok);    //prints the numbers of tokens
      for (int i = 0; i < ntok; i++){ //loop to print indices and tokens
        printf("tokens[%d]: %s\n", i, tokens[i]);
      }
    } else if(result[0] == '\n'){    //enter command
      if(echo){    //check for echo
        printf("\n");
      }
      printf("\n");
    } else {    //else statement for running non-builtin commands                                 
      if(echo){    //check for echoo
        for (int i = 0; i < ntok; i++){ //loop to repeat tokens
          printf(" %s", tokens[i]);
        }
        printf("\n");
      }
      int i = 0;    //index variable
      while(shellac.jobs[i]!=NULL){    //loop to find first non null index
        i++;    //increases the index
      }
      job_t *job = job_new(tokens);    //creates a job struct with tokens as its argument
      int res = shellac_add_job(&shellac, job);    //adds the job to shellac
      if (res == 0){
        shellac_start_job(&shellac, i);    //starts the current job
        shellac_update_one(&shellac, i);    //updates the current job
      }
    }
    shellac_update_all(&shellac);    //updates all the jobs in shellac
  }
  shellac_free_jobs(&shellac);    //free all the jobs in shellac
  return 0;
}
