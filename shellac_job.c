#include "shellac.h"
#include <stdlib.h>
// shellac_job.c: functions related the job_t struct abstracting a
// running command. Most functions maninpulate jot_t structs.

void job_print(job_t *job){
// Prints a representation of the job. Used primarily in testing but
// useful as well for debugging. Several provided utility functions
// from shellac_util.c are useful to simplify the formatting process:
// strnull() simplifies printing nice "NULL" strings and
// job_condition_str() simplifies creating a string based on condition
// codes.
// 
// SAMPLE OUTPUT FORMAT: 
// job {
//   .jobname       = 'diff'
//   .pid           = #2378
//   .retval        = 1
//   .condition     = EXIT(1)
//   .output_file   = diff_output.txt
//   .input_file    = NULL
//   .is_background = 1
//   .argc          = 3
//   .argv[] = {
//     [ 0] = diff
//     [ 1] = file1.txt
//     [ 2] = file2.txt
//     [ 3] = NULL
//    }
// }
  if(job==NULL){
    printf("NULL\n");
    return;
  }
  printf("job {\n");
  printf("  .jobname       = '%s'\n", strnull(job->jobname));
  if (job->pid > 0){    //check if the job->pid is less than 0 for the # format
    printf("  .pid           = #%d\n", job->pid);    //if job->pid is greater than 0, put # infront of the number
  } else {
    printf("  .pid           = %d\n", job->pid);    //else don't put the #
  }
  printf("  .retval        = %d\n", job->retval);
  printf("  .condition     = %s\n", job_condition_str(job));
  printf("  .output_file   = %s\n", strnull(job->output_file));
  printf("  .input_file    = %s\n", strnull(job->input_file));
  printf("  .is_background = %d\n", job->is_background);
  printf("  .argc          = %d\n", job->argc);
  printf("  .argv[] = {\n");
  for (int i = 0; i <= job->argc; i++){                  //loop for going through argv
    printf("    [ %d] = %s\n", i, strnull(job->argv[i]));
  }
  printf("   }\n");
  printf("}\n");
  return;
}

job_t *job_new(char *argv[]){
// Create a new job based on the argv[] provided. The parameter argv[]
// will be NULL terminated to allow detecing the end of the
// array. Allocates heap memory for a job_t struct and creates heap
// copies of argv[] via string duplication. The last element in
// job.argv[] will be NULL terminated as the paramter array is. The
// initial condition of the job is INIT with -1 for its pid and
// retval. The jobname is argv[0] and.  Normal foreground jobs have
// is_background set to 0.
//
// PROBLEM 3: If argv[] contains input or output redirection (> outfile
// OR < infile) or the background symbol &, then removes these from
// the argv[] and sets appropriate other fields. If problems are found
// with input/output redirection such as a ">" with no following file,
// prints an error and return NULL.
// 
// HINTS for PROBLEM 3: The provided array_shift() function from the
// shellac_util.c may prove useful to shift over input / output
// redirection though other methods are possible. Note that the ">"
// "<" and "&" strings are removed from the command line so must be
// handled wth care: either don't duplicat them or free() any
// duplicates of them before returning.
  job_t *job = malloc(sizeof(job_t));    //malloc a job in the heap
  int count = 0;    //counts the elements up to the NULL element
  while(argv[count] != NULL){    //finds how many elements in the argv array, stops when reaches NULL as an element
    count++;    //increase count
  }
  int a = 0;    //index variable
  int l = count + 1;    //length of the argv with NULL included
  job->input_file = NULL;    //initializes input_file as NULL
  job->output_file = NULL;    //initializes output_file as NULL
  job->is_background = 0;    //initializes is_background as 0
  while(argv[a] != NULL){    //loops through the argv[] array up to NULL element
    if(strcmp("<", argv[a])==0){    //if the current element is "<"
      if(argv[a+1] == NULL){    //check if the next element is not NULL
        printf("ERROR: No file given for input redirection\n");    //if NULL, print the error
        free(job);    //frees the job struct
        return NULL;    //returns NULL for error
      }
      job->input_file = strdup(argv[a+1]);    //copies the next element into input_file
      array_shift(argv, a, l--);    //array shift the current element left
      array_shift(argv, a, l--);    //array shift the next element left
    } else if (strcmp(">", argv[a])==0){    //if the current element is ">"
      if(argv[a+1] == NULL){    //check if the next element is not NULL
        printf("ERROR: No file given for output redirection\n");    //if NULL, print the error
        free(job);    //frees the job struct
        return NULL;    //returns NULL for error
      }
      job->output_file = strdup(argv[a+1]);    //copies the next element into output_file
      array_shift(argv, a, l--);    //array shift the current element left
      array_shift(argv, a, l--);    //array shift the next element left
    } else if (strcmp("&", argv[a])==0){    //check if current element is &
      job->is_background = 1;    //set is_background to 1
      array_shift(argv, a, l--);    //array shift the current element left
    } else {
      a++;    //increase the index
    }
  }
  int i;    //index variable
  for (i = 0; i < l-1; i++){    //loops through argv[] array
    job->argv[i] = strdup(argv[i]);    //copies elements of argv[] into job->argv[]
  }
  job->argv[i] = NULL;    //set the last element to NULL
  job->argc = i;    //initializes argc
  job->condition = JOBCOND_INIT;    //set condition to INIT
  job->pid = -1;    //set pid to -1
  job->retval = -1;    //set retval to -1
  strcpy(job->jobname, argv[0]);    //set job to first element of argv[] array
  return job;    //return the pointer struct
}

void job_free(job_t *job){
// Deallocates a job structure. Deallocates the strings in the argv[]
// array. Deallocates any input / output file associated with
// fields. Finally de-allocates the struct itself.
  int i = 0;    //index variable
  while(job->argv[i] != NULL){    //loops through argv[] up to NULL
    free(job->argv[i]);    //free all the duplicate elements
    i++;    //increase index
  }
  if(job->input_file != NULL){    //if input_file not NULL
    free(job->input_file);    //free the duplicate name
  }
  if(job->output_file != NULL){    //if output_file not NULL
    free(job->output_file);    //free the duplicate name
  }
  free(job);    //free the job struct itself
  return;
}

void job_start(job_t *job){
// Forks a process and executes the command described in the job as a
// process.  Changes the condition field to "RUN".
//
// PROBLEM 3: If input/output redirection is indicated by fields of
// the job, sets this up using dup2() calls. For output redirection,
// ensures any output files are created and if they already exist, are
// "clobbered" via appropriate options to open(). If input/output
// redirection fails, the child process should exit with JOBCOND_FAIL_OUTP or INP.
  job->condition = JOBCOND_RUN;    //set the condition to RUN(2)
  job->pid = fork();    //forks a process
  if (job->pid == 0){    //if pid is child's
    if (job->input_file != NULL){    //if input_file is not NULL
      int fd = open(job->input_file, O_RDONLY);    //open the input_file
      if(fd == -1){                    // check for errors opening file
        exit(JOBCOND_FAIL_INPT);    // exits if open fails
      }
      dup2(fd,STDIN_FILENO);    //change the file descripter for input to the open file
      close(fd);    //closes the file
    }
    if (job->output_file != NULL){
      int fd = open(job->output_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR|S_IWUSR);
      if(fd == -1){                    // check for errors opening file
        exit(JOBCOND_FAIL_OUTP);    // exits if open fails
      }
      dup2(fd,STDOUT_FILENO);    //change the file descripter for output to the open file
      close(fd);    //closes the file
    }
    execvp(job->jobname, job->argv);    //execute the child
    exit(JOBCOND_FAIL_EXEC);    //exits if execute fails
  } else {
    return;    //return if parent
  }
}

int job_update_status(job_t *job){
// Checks on the job for a status update. This utilizes a wait() or
// waitpid() system call. If the job has completed, updates its
// condition to reflect either EXIT or FAIL. For exits, uses macros to
// extract the exit status and assigns it the retval field.
// 
// PROBLEM 2: For foreground (default) jobs, blocks the parent process
// until the child is completed. Returns 1 for a condition change
// (e.g. RUN to EXIT / FAIL).
//
// PROBLEM 3: For background jobs, uses the WNOHANG option to avoid
// blocking the parent. If the job is finished, updates its retval,
// condition, and returns 1. If the job is not finished, just returns
// 0.
//
// For erroneous calls such as calls on a NULL job or on a non-running
// job without a pid, the behavior of this function is implementation
// dependent (may segfault, may exit with an error message, etc.) This
// situation is not tested.
  int status = 0;    //status variable
  int retcode = 0;    //pid variable
  if (job->is_background == 0){
    retcode = waitpid(job->pid, &status, 0);    //waits on child by blocking
  } else {
    retcode = waitpid(job->pid, &status, WNOHANG);    //waits on child by not blocking
  }
  if (retcode != 0){   //if reaches finishes
    if(WIFEXITED(status)){    //checks if completed successfully
      int ret = WEXITSTATUS(status);    //gets exit status
      if (ret <= 3){    //if exit status less than or equal to 3
        job->condition = JOBCOND_EXIT;    //set condition to EXIT (3)
        job->retval = ret;    //set retval to exit value
      } else if (ret == 128) {
        printf("ERROR: job failed to exec: No such file or directory\n");    //print error
        job->condition = JOBCOND_FAIL_EXEC;    //set condition to FAIL
      } else {
        job->condition = JOBCOND_FAIL_OTHER;    //set condition to fail other
      }
      return 1;    //return if child completed
    }
  }
  return 0;    //return if child does not finish/fails
}