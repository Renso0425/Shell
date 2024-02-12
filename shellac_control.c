#include "shellac.h"
// shellac_control.c: functions related the shellac_t struct that controls
// multiple jobs

void shellac_init(shellac_t *shellac){
// Initialize all fields of the shellac argv[] array to NULL and set
// the job_count to 0
  for (int i = 0; i < MAX_JOBS; i++){    //Loops through shellac->jobs array
    shellac->jobs[i] = NULL;    //sets each element to NULL
  }
  shellac->job_count = 0;    //set job_count to 0
  return;
}

int shellac_add_job(shellac_t *shellac, job_t *job){
// Add a single job to the argv[] array. Search for the first NULL
// entry in the array and add that on. Does basic error checking to
// see if array is full and prints an error message of some kind of
// so.
  int i = 0;    //index of the NULL element
  while(shellac->jobs[i] != NULL){    //loops through jobs to find first NULL element
    i++;    //increase index
  }    
  if (i < MAX_JOBS) {    //if index is less thant the jobs.length
    shellac->jobs[i] = job;    //set the jobs element to this job struct
    shellac->job_count++;    //increase job count
    return 0;    //returns succesfully adding 
  } else {    //if too many jobs
    printf("TOO MANY JOBS!");    //print error
    return 1;    //returns if error
  }
}

int shellac_remove_job(shellac_t *shellac, int jobnum){
// Remove the indicated job from the jobs array and replace its entry
// with NULL. Decrements the job count. De-allocates memory associated
// with the job via a call to job_free(). Does basic error checking so
// that if the specified jobnum is already NULL, prints an error to
// that effect.
  if (shellac->jobs[jobnum] == NULL){    //check if the current job is NULL
    printf("ERROR: No such job '%d'\n", jobnum);    //print error
    return 1;
  } else {    //if current job is not NULL
    job_free(shellac->jobs[jobnum]);    //free the current job
    shellac->jobs[jobnum] = NULL;    //replace the job with NULL
    shellac->job_count--;    //decreases the job count
    return 0;
  }
}

void shellac_start_job(shellac_t *shellac, int jobnum){
// Starts the specified job number. First prints a message about
// starting the job of the format
// 
//   === JOB %d STARTING: %s ===\n
// 
// with jobnum and jobname filled in. Then uses a call to job_start()
// to start the job.
  if (shellac->jobs[jobnum] != NULL){    //checks if the current job is non NULL
    printf("=== JOB %d STARTING: %s ===\n", jobnum, shellac->jobs[jobnum]->jobname);
    job_start(shellac->jobs[jobnum]);    //starts the current job
  }
  return;
}

void shellac_print_jobs(shellac_t *shellac){
// Prints the job number and jobname of all non-NULL jobs in the jobs
// array.
  for (int i = 0; i < MAX_JOBS; i++){    //loops through the jobs array
    if (shellac->jobs[i] != NULL){    //checks if the current job is not NULL
      printf("[%d] %s\n", i, shellac->jobs[i]->jobname);
    }
  }
  printf("%d total jobs\n", shellac->job_count);    //prints total num of jobs
  return;
}

void shellac_free_jobs(shellac_t *shellac){
// Traverses the jobs array and de-allocates any non-null jobs.
  for (int i = 0; i < MAX_JOBS; i++){    //loops through the jobs array
    if (shellac->jobs[i] != NULL){    //if the current job is not NULL
      shellac_remove_job(shellac, i);    //removes the current job
    }
  }
  return;
}

void shellac_update_one(shellac_t *shellac, int jobnum){
// Updates a single job via a cal to job_update_status().  If that
// functions return value indicates that the job completed, prints a
// message of the form
//
// === JOB %d COMPLETED %s [#%d]: %s ===\n
//
// with jobnum, name, pid number, and ending condition reported. Uses
// the job_condition_str() functon to create the condition string. For
// completed jobs, de-allocates them and removes them from the jobs
// array.
// 
// Examples of the printed message:
// === JOB 0 COMPLETED bash [#1000]: EXIT(0) ===
// === JOB 5 COMPLETED gcc [#22830]: EXIT(1) ===
// === JOB 1 COMPLETED cat [#22833]: FAIL(INPT) ===
  int res = job_update_status(shellac->jobs[jobnum]);    //updates the job
  if (res == 1){    //if update results is 1 or child is completed
    printf("=== JOB %d COMPLETED %s [#%d]: %s ===\n", jobnum, shellac->jobs[jobnum]->jobname, shellac->jobs[jobnum]->pid, job_condition_str(shellac->jobs[jobnum]));
    shellac_remove_job(shellac, jobnum);    //removes the current job
    }
  return;
}

void shellac_update_all(shellac_t *shellac){
// Iterates through all jobs in the jobs array and updates them. Used
// mainly to check for the completion of background jobs at the end of
// each interactive loop iteration.
  for(int i = 0; i < MAX_JOBS; i++){    //loops through the jobs array
    if (shellac->jobs[i] != NULL){    //if the current job is not NULL
      shellac_update_one(shellac, i);    //updates the current job
    }
  }
  return;
}

void shellac_wait_one(shellac_t *shellac, int jobnum){
// Change the status of a background job to foreground
// (e.g. is_background becomes 0) then update that job to wait for
// it. Does basic error checking so that if the jobnum indicated
// doesn't exit, an error message of some sort is printed.
  if (shellac->jobs[jobnum] != NULL){    //if the current is not NULL
    shellac->jobs[jobnum]->is_background = 0;    //sets the is_background to 0
    job_update_status(shellac->jobs[jobnum]);    //updates the current job
  } else {
    printf("ERROR: No job '%d' to wait for\n", jobnum);    //prints error
  }
  return;
}
