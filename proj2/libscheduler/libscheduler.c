/** @file libscheduler.c
 */
#include "libscheduler.h"
#include "../libpriqueue/libpriqueue.h"

/**
  Stores information making up a job to be scheduled including any statistics.

  You may need to define some global variables or a struct to store your job queue elements. 
*/
typedef struct _job_t
{
  int jobID;
  int arrivalTime;
  int runTime;
  int priority;
  int waitTime;//time its been wating from(waiting start time)
  int remainingTime;
  int prevstartTime;//time when last scheduled.
  bool scheduledyet;
} job_t;



int compareFCFS(const void * a, const void * b)//first come, first serve
{
  job_t * joba = (job_t *)a;
  job_t * jobb = (job_t *)b;
  return joba->arrivalTime - jobb->arrivalTime;
}

int compareSJF(const void * a, const void * b)//shortest job first & Preemptive shortest job first
{
  job_t * joba = (job_t *)a;
  job_t * jobb = (job_t *)b;
  return joba->runTime - jobb->runTime;
}

int comparePSJF(const void * a, const void * b)//shortest job first & Preemptive shortest job first
{
  job_t * joba = (job_t *)a;
  job_t * jobb = (job_t *)b;
  return joba->remainingTime - jobb->remainingTime;
}

int comparePRI(const void * a, const void * b)//Priority compare & Preemptive Priority compare
{
  job_t * joba = (job_t *)a;
  job_t * jobb = (job_t *)b;
  return joba->priority - jobb->priority;
}


int compareRR(const void * a, const void * b)//round robbin
{
  //job_t * joba = (job_t *)a;
  //job_t * jobb = (job_t *)b;
  return -1;
}


job_t * searchID(int id){
  int i = 0;
  while(priqueue_at(&q,i) != NULL){
    job_t * job = (job_t *)priqueue_at(&q,i);
    if (job->jobID == id){
      return job;
    }
    i++;
  }
  return NULL;
}

/**
  Initalizes the scheduler.
 
  Assumptions:
    - You may assume this will be the first scheduler function called.
    - You may assume this function will be called once once.
    - You may assume that cores is a positive, non-zero number.
    - You may assume that scheme is a valid scheduling scheme.

  @param cores the number of cores that is available by the scheduler. These cores will be known as core(id=0), core(id=1), ..., core(id=cores-1).
  @param scheme  the scheduling scheme that should be used. This value will be one of the six enum values of scheme_t
*/
void scheduler_start_up(int cores, scheme_t scheme)
{
  schm = scheme;
  int (*queueCompfunc)(const void *elma, const void *elmb) = compareFCFS;

  switch(scheme){
    case FCFS:
      queueCompfunc = &compareFCFS;
      break;
    case SJF:
      queueCompfunc = &compareSJF;
      break;
    case PSJF:

      break;
    case PRI:
    case PPRI:
      queueCompfunc = &comparePRI;
      break;
    case RR:
      queueCompfunc = &compareRR;
      break;
  }

  priqueue_init(&q, queueCompfunc);//change compare based on scheme

}


/**
  Called when a new job arrives.
 
  If multiple cores are idle, the job should be assigned to the core with the
  lowest id.
  If the job arriving should be scheduled to run during the next
  time cycle, return the zero-based index of the core the job should be
  scheduled on. If another job is already running on the core specified,
  this will preempt the currently running job.
  Assumptions:
    - You may assume that every job wil have a unique arrival time.

  @param job_number a globally unique identification number of the job arriving.
  @param time the current time of the simulator.
  @param running_time the total number of time units this job will run before it will be finished.
  @param priority the priority of the job. (The lower the value, the higher the priority.)
  @return index of core job should be scheduled on
  @return -1 if no scheduling changes should be made. 
 
 */
int scheduler_new_job(int job_number, int time, int running_time, int priority)
{
	job_t * job = malloc(sizeof(job_t));
  job->jobID = job_number;
  job->arrivalTime = time;
  job->runTime = running_time;
  job->priority = priority;
  job->remainingTime = running_time;//needed for premeptive things.
  job->scheduledyet = false;
  job->waitTime = time;

  if(priqueue_peek(&q) !=NULL){
    ((job_t *)priqueue_peek(&q)) remainingTime = runTime - (prevstartTime - time); //TODO:fix
  }

  int ret = priqueue_offer(&q, job);
  totTasksSch++;
  int returnval = -1;
   switch(schm){
    case FCFS:
      if(ret == 0){
        returnval = 0;//run on core 0;
      }else{
        returnval = -1;//is not first, do not change what is scheduled.
      }
    case SJF:
     if(ret == 0 && priqueue_at(&q,1) == NULL){//check if there is a second task and we are now the 
                                              //first task(old first task became second task), if so, then schedule it
        returnval = 0;//run on core 0;
      }else{
        returnval = -1;//is not first, do not change what is scheduled.
      }
    case PSJF:
      if(ret == 0){
        returnval = 0;//run on core 0; //may have just premepted something.
        if(priqueue_at(&q,1) != NULL){//if prempeted somethin else.
          ((job_t *)priqueue_at(&q,1))->waitTime = time;
        }

      }else{
        returnval = -1;//is not first, do not change what is scheduled.
      }
    case PRI:
      if(ret == 0 && priqueue_at(&q,1) == NULL){//check if there is a second task and we are now the 
                                              //first task(old first task became second task), if so, then schedule it.
        returnval = 0;//run on core 0;
      }else{
        returnval = -1;//is not first, do not change what is scheduled.
      }
    case PPRI:
      if(ret == 0){
        returnval = 0;//run on core 0; //may have just premepted something.
        if(priqueue_at(&q,1) != NULL){//just premepted something, set new wait start time.
          ((job_t *)priqueue_at(&q,1))->waitTime = time;
        }
      }else{
        returnval = -1;//is not first, do not change what is scheduled.
      }
    case RR:
    default:
      if(ret == 0){
        returnval = 0;//run on core 0;
      }else{
        returnval = -1;//is not first, do not change what is scheduled.
      }
  }
  if (returnval == -1){
    return -1;
  }else{
    prevstartTime = time;
    job->scheduledyet = true;
    return returnval;
  }

  //if here, its had an issue.
  //return 0;//need to check whats at the first of the queue//schedules the job on core 1
}


/**
  Called when a job has completed execution.
 
  The core_id, job_number and time parameters are provided for convenience. You may be able to calculate the values with your own data structure.
  If any job should be scheduled to run on the core free'd up by the
  finished job, return the job_number of the job that should be scheduled to
  run on core core_id.
 
  @param core_id the zero-based index of the core where the job was located.
  @param job_number a globally unique identification number of the job.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled to run on core core_id
  @return -1 if core should remain idle.
 */
int scheduler_job_finished(int core_id, int job_number, int time)
{ 
  job_t * job = searchID(job_number);
  priqueue_remove(&q,job);
  totTurnAround += time - job->arrivalTime;
  //do number analtics.
  free(job);
  totTaskFin++;
  job = priqueue_peek(&q);
  if (job != NULL){
    //job is scheduled
    if(job-> scheduledyet == false){
      totResponceTime += time - job->arrivalTime;
      job->scheduledyet = true;

    }
    totWaitTime += time - job->waitTime;

    return job->jobID;
  }else{
    return -1;//else NULL, and nothing to do.
  }
	
}



/**
  When the scheme is set to RR, called when the quantum timer has expired
  on a core.
 
  If any job should be scheduled to run on the core free'd up by
  the quantum expiration, return the job_number of the job that should be
  scheduled to run on core core_id.

  @param core_id the zero-based index of the core where the quantum has expired.
  @param time the current time of the simulator. 
  @return job_number of the job that should be scheduled on core cord_id
  @return -1 if core should remain idle
 */
int scheduler_quantum_expired(int core_id, int time)
{
  job_t * job = priqueue_poll(&q);
  job->waitTime = time;//set the wait time.
  priqueue_offer(&q,job);//just offered, gantreed job in queue
  
  job = priqueue_peek(&q);//reuse job, new job below:

  //if (job != NULL){
    if(job-> scheduledyet == false){
      totResponceTime += time - job->arrivalTime;
      job->scheduledyet = true;
    }
    totWaitTime += time - job->waitTime;
    return job->jobID;
  //}else{
  //  return -1;
  //}
}


/**
  Returns the average waiting time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average waiting time of all jobs scheduled.
 */
float scheduler_average_waiting_time()
{

	return (float)totWaitTime / (float)totTaskFin;
}


/**
  Returns the average turnaround time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average turnaround time of all jobs scheduled.
 */
float scheduler_average_turnaround_time()
{

	return (float)totTurnAround / (float)totTaskFin;
}


/**
  Returns the average response time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average response time of all jobs scheduled.
 */
float scheduler_average_response_time()
{

	return (float)totResponceTime / (float)totTaskFin;
}


/**
  Free any memory associated with your scheduler.
 
  Assumptions:
    - This function will be the last function called in your library.
*/
void scheduler_clean_up()
{
  while(priqueue_peek(&q) != NULL){
    free(priqueue_peek(&q));
  }
  priqueue_destroy(&q);
}


/**
  This function may print out any debugging information you choose. This
  function will be called by the simulator after every call the simulator
  makes to your scheduler.
  In our provided output, we have implemented this function to list the jobs in the order they are to be scheduled. Furthermore, we have also listed the current state of the job (either running on a given core or idle). For example, if we have a non-preemptive algorithm and job(id=4) has began running, job(id=2) arrives with a higher priority, and job(id=1) arrives with a lower priority, the output in our sample output will be:

    2(-1) 4(0) 1(-1)  
  
  This function is not required and will not be graded. You may leave it
  blank if you do not find it useful.
 */
void scheduler_show_queue()
{
  int i = 0;
  job_t *job = NULL;
    printf("jobID  arivTime  runTime  priority\n");
  while(priqueue_at(&q,i)!= NULL){
    job = priqueue_at(&q,i);
    printf("\t %d\t%d\t  %d\t   %d\n",job->jobID, job->arrivalTime, job->runTime, job->priority);
    i++;
  }
}
