#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>

#include "Schedule_sim.h"
#include "Struct_manipulation.h"

#define RUN 1
#define FIN 0
#define TRUE 1
#define FALSE 0
#define NULLEXIST -1
#define FIRSTELEMENT 0
#define GIVECURRENT 0
#define GIVENEXT 1
#define GAP 2
#define NOTCPU -1

//Funtions to determine which CPU is given to Processes
/*==============================================================*/
/*calculates the remaining exec time in a process list, based on the input process'
arrived time*/
double calc_remain_in_list(Process_list *list,Process * process){
  double output = 0;
  //if first then look for spec num, the number from which to calculate the remain
  Uint first = TRUE;
  double spec_num;
  Process *current;
  //if the input list is null, then return remain in list as 0
  if(list->head == NULL){
    return output;
  }else{
    current = list->head;
    //go through the linked list
    while(current){
      if(current->next != NULL){
        double temp = (double)current->remain_time + (double)current->time_arrived;
        if(first && temp < (double)current->next->time_arrived){
          output = 0;
          current = current->next;
        }else if(!first && output + spec_num + current->remain_time < current->next->time_arrived){
          current = current->next;
          first = TRUE;
        }
        else{
          if(first){
            spec_num = (double)current->time_arrived;
            output = 0;
            first = FALSE;
          }
          //add the remain time to the output to calculate total time
          output += (double)current->remain_time;
          current = current->next;
        }
      }else{
        if(first){
          spec_num = (double)current->time_arrived;
          output = 0;
          first = FALSE;
        }
        output +=(double) current->remain_time;
        current = current->next;
      }
    }
    if(first){
      spec_num = (double)list->head->time_arrived;
      output = (double)list->head->remain_time;
    }
    //calculate the remain time, which is the total time - the process arriving time and plus the spec num that begun the calculation
    //which is the arrival time of the first process calculated
    output = output - (double)process->time_arrived + spec_num;
  }
  if(output < 0){
    output = 0;
  }
  return output;
}


/*finds which process list has the lowest remainging time left
returns an index*/
Uint find_lowest_time(Process_list **list, Uint len, Process *process){
  double lowest = calc_remain_in_list(list[FIRSTELEMENT],process);
  Uint output = 0;
  double current;
  for(Uint i=1; i<len;i++){
    current = calc_remain_in_list(list[i],process);
    //if the current remain is lower than min, then set min as current and record the index
    if (current < lowest){
      lowest = current;
      output = i;
    }
  }
  return output;
}
//assign a singular process to a cpu
Uint assign_singular_process(Process_list ** list, Process *process,Uint len){
  Uint pList_num;
  Process *dup = dupProcess(process);
  //find the index of lowest remain time, and insert tha
  pList_num = find_lowest_time(list, len, dup);
  processlist_insert(list[pList_num],dup);
  process = process->next;
  return pList_num;
}
//check if the input of a process states the process is parallelisable or not
Uint parallelisable(char *str){
  Process *process = malloc(sizeof(*process));
  char delim[] = " ";
  //breaks string into tokens, 'splitting' it by deliminator of " "
  char temp[strlen(str)];
  strcpy(temp,str);
  char * ptr = strtok(temp,delim);
  while(ptr!=NULL){
    if(ptr[0] == 'n'){
      return 0;
    }
    if (ptr[0] == 'p'){
      return 1;
    }
    ptr = strtok(NULL,delim);
  }
  return -1;
}
//finding k for a process, if process has remain time lower than the number
//of cpu availble then k is the remain time
Uint findk(char * str, Uint value){
  Uint input_counter = 0;
  char delim[] = " ";
  char temp[strlen(str)];
  strcpy(temp,str);
  //breaks string into tokens, 'splitting' it by deliminator of " "
  char * ptr = strtok(temp,delim);
  char *endptr;
  while(ptr!= NULL){
    Uint temp = strtoul(ptr, &endptr, 10);
    if(input_counter != 2){
      input_counter += 1;
    }else if(input_counter == 2){
      if(temp < value){
        return temp;
      }else{
        return value;
      }
    }
    ptr = strtok(NULL,delim);
  }
  exit(EXIT_FAILURE);
}
/*creates an array of process list, of size of input value -p*/
Process_list * create_array_processList(Uint value, char *filename, Process_list **list, Process_list *subprocess){
  FILE * fp;
  char * content = NULL;
  size_t len = 0;
  size_t read;
  Uint k;
  Process *process;
  Process_list * readin = new_processlist(NOTCPU);
  //open file as read;
  fp = fopen(filename, "r");
  if (fp == NULL){
      exit(EXIT_FAILURE);
  }
  //initialise the cpu lists
  for(Uint i=0; i<value+1;i++){
    list[i] = new_processlist(i);
  }
  //read in process from file one by one and insert in readin
  while ((read = getline(&content, &len, fp)) != -1) {
    k = findk(content,value);
    if(!parallelisable(content)){
      process = new_process(content,-1,k);
      processlist_insert(readin,process);
    }else{
      //if parallelisable, mark them as parallelisable
      process = new_process(content,k,-1);
      processlist_insert(readin,process);
    }
  }
  fclose(fp);
  if (content){
    free(content);
  }
  return readin;
}

//==========================================================================
//simulation functions
//========================================================================
//find a process within the queue as the current process to process
/*finds the process with the shortest remaining time within makespan*/
Process *findcurrent(Process_list *Plist, double makespan){

  Wait_que *list = Plist->wait_que;
  Process *temp = list->head;
  Process *next = temp->next;

  Uint foundyes=FALSE;
  //while there is processes in Plist

  while(temp){
    if(temp->time_arrived <= makespan){
      while(next){
        if(next->time_arrived<=makespan){
          if(temp->remain_time < next->remain_time){
            next = next -> next;
          }else if(temp->remain_time == next->remain_time){
            if(temp->p_id < next->p_id){
              next = next -> next;
            }else{
              foundyes = TRUE;
              temp = next;
              next = next->next;
              break;
            }
          }
          else{
            foundyes = TRUE;
            temp = next;
            next = next->next;
            break;
          }
        }else{
          //maybe add guard here?
          next = next->next;
        }
      }
    }else{
      if(temp->next != NULL){
        temp = temp->next;
        next = temp->next;
      //  notfound = 1;
      }else{
        break;
      }
    }
    if(next == NULL){
      if(foundyes){
        return temp;
      }else{
        if(temp->time_arrived <= makespan){
          return temp;
        }else{
          return list->head;
        }
      }
    }
  }
  if(foundyes){
    return temp;
  }else{
    return list->head;
  }
}
//determing whether to give processor to current process or next, or if special case
Uint allocation_case(Process_list *list, Process *current, Process *next){
  Uint swi;
  double result = (double)current->remain_time + (double)list->stats->makespan - (double)next->time_arrived;
  double res = (double) current->remain_time - (double) next->time_arrived;
  if(list->stats->makespan >= current->time_arrived && list->stats->makespan >= next->time_arrived){
    if(res < 0){
      res = 0;
    }
    if(res < next->remain_time || (res == next->remain_time && current->p_id < next->p_id)){
      swi = GIVECURRENT;
    }else{
      swi = GIVENEXT;
    }
  }else{
    if(result < 0){
      result = 0;
    }
    if( result < next->remain_time || (result == next->remain_time && current->p_id < next->p_id)){
      swi = GIVECURRENT;
    }else{
      swi = GIVENEXT;
    }
  }
  if(current->remain_time + list->stats->makespan< next->time_arrived && list->wait_que->size == 0){
    swi = GAP;
  }
  return swi;
}
//check if a sub process is the last of the subprocess that finished executing
Uint lastin(Node *current,Process_list *subprocess){
  assert(subprocess);
  Process *now = subprocess->head;
  Uint output = 0;
  Uint def = 0;
  /*while there is still elemetn in subprcess, if the subprocess only found
  one process with same pid and same sub id, then it is last in,else it is not*/
  while(now){
    if(current->pid == now->p_id){
      if(current->sub_id == now->sub_id){
        if(now->next != NULL && now->prev != NULL){
          now->next->prev = now->prev;
          now->prev->next = now->next;
        }
        else if(now->next == NULL && now->prev != NULL){
          now->prev->next = NULL;
          subprocess->tail = now->prev->next;
        }
        else if(now->prev == NULL && now->next != NULL){
          now->next->prev = NULL;
          subprocess->head = now->next;
        }
        else{
          subprocess->head = NULL;
          subprocess->tail = NULL;
        }
        now = now->next;
      }else{
        def = 1;
        now = now->next;
      }
      output = 1;
    }
    else{
      now = now->next;
    }
  }
  if(def){
    output = 0;
  }
  return output;
}

//simulating allocation of processes
Process *schedule(Process_list *list, Process *current, Stats *perform_stat,Process_list *movesubprocess){
  Uint swi;
  Uint special = FALSE;
  Node *ar;
  Process *next  = current->next;
  //add the current process to the print queue as running
  add_print(current,list,RUN,list->cpu_num);
  while(next){
    swi = allocation_case(list,current,next);
    if(swi == GIVECURRENT){
      insert_waitQue(next,list->wait_que);
      next = next->next;
    }else if(swi == GAP){
      special = TRUE;
      break;
    }
    else{
      double temp = (double)current->remain_time + (double)list->stats->makespan - (double)next->time_arrived;
      if(temp < 0){
        current->remain_time = 0;
      }else{
        current->remain_time = current->remain_time + list->stats->makespan - next->time_arrived;
      }
      list->stats->makespan = (double)next->time_arrived;
      add_print(next,list,RUN,list->cpu_num);
      insert_waitQue(current,list->wait_que);
      current = next;
      next = current->next;
    }

  }
  //finish the current process
  list->stats->makespan += (double)current->remain_time;
  ar =   add_print(current,list,FIN,list->cpu_num);
  //if the process is a sub process and last in, then calculate turnaroun,overhead, etc.
  if(current->parallelisable == 1){
    if(lastin(ar,movesubprocess)){
      calc_total_turnaround(list->stats->makespan,current,&list->stats->total_turnaround);
      calc_overhead(current,list->stats->makespan,&list->stats->max_overhead,&list->stats->total_overhead);
    }
  }else{
    calc_total_turnaround(list->stats->makespan,current,&list->stats->total_turnaround);
    calc_overhead(current,list->stats->makespan,&list->stats->max_overhead,&list->stats->total_overhead);
  }
  list->size -= 1;
  //remove the current process from waiting queue as it is finished
  remove_process(list->wait_que,current);
  if(list->size != 0){
    if(special == TRUE){
      current = current->next;
      special = FALSE;
    }else{
      current = findcurrent(list,list->stats->makespan);
      current = remove_process(list->wait_que,current);
    }
    if(current->time_arrived > list->stats->makespan){
      list->stats->makespan = current->time_arrived;
    }
    if(current->next != NULL){
      list->current = current;
      next = current->next;
    }else{
      list->current = NULL;
      add_print(current,list,RUN,list->cpu_num);
      list->stats->makespan += (double) current->remain_time;
      ar = add_print(current,list,FIN,list->cpu_num);
      if(current->parallelisable == 1){
        if(lastin(ar,movesubprocess)){
          calc_total_turnaround(list->stats->makespan,current,&list->stats->total_turnaround);
          calc_overhead(current,list->stats->makespan,&list->stats->max_overhead,&list->stats->total_overhead);
        }
      }else{
        calc_total_turnaround(list->stats->makespan,current,&list->stats->total_turnaround);
        calc_overhead(current,list->stats->makespan,&list->stats->max_overhead,&list->stats->total_overhead);
      }
      list->size -= 1;
    }
  }
  else{
    list->current = NULL;
  }
  return current;
}

//================================================================================
//Print output functions
//=======================================================================
//pop the element with lowest priority
Node *popEarliest(Process_list ** list, Uint len){
  double lowestPri = -1;
  Uint popped = 0;
  for (Uint i=0; i< len;i++){
    if (list[i]->print_q->size != 0){
      lowestPri = (double)list[i]->print_q->head->priority;
      popped = i;
      break;
    }
  }
  if(lowestPri != -1){
    double currentPri;
    for(Uint i=1;i<len;i++){
      if(list[i]->print_q->size != 0){
        currentPri = (double)list[i]->print_q->head->priority;
        if(currentPri < lowestPri){
          lowestPri = currentPri;
          popped = i;
        }
      }
    }
    Node *node = remove_head(list[popped]->print_q);
    return node;
  }else{
    return NULL;
  }
}
//removes an element from the list
void remove_list(Process_list ** list, Uint index, Uint len){
  for(Uint i= index;i<len-1;i++){
    list[i] = list[i+1];
  }
}
//check if print queue is empty
Uint finished(Process_list ** list,Uint len){
  for(Uint i=0;i<len;i++){
    if(list[i]->finished == 0){
      return FALSE;
    }
  }
  return TRUE;
}
//checks if the message  has been printed or nothing
//checks the existence of the message in the history queue
Uint not_in_history(Node *node, Print_q *history){
  if(history->size != 0){
    Node *current = history->head;
    while(current){
      if(node->pid == current->pid && node->priority == current->priority && node->running == current->running && node->sub_id == current->sub_id){
        return FALSE;
      }
      current = current->next;
    }
    return TRUE;
  }
  return TRUE;
}
//treat the running messages, print out the message if it has not been printed out
//before
void treat_running(Print_q *queue,Print_q *historyr){
  Node *current = queue->head;
  while(current){
    if(current->running == RUN){
      if(current->sub_id != -1){
        if(not_in_history(current,historyr)){
          printf("%.0f,RUNNING,pid=%lu.%lu,remaining_time=%lu,cpu=%lu\n",current->priority,current->pid,current->sub_id,current->remainTime,current->cpu_num);
          Node *temp = dupNode(current);
          insert_node(temp,historyr);
        }
        current = current->next;
      }else{
        if(not_in_history(current,historyr)){
          printf("%.0f,RUNNING,pid=%lu,remaining_time=%lu,cpu=%lu\n",current->priority,current->pid,current->remainTime,current->cpu_num);
          Node *temp = dupNode(current);
          insert_node(temp,historyr);
        }
        current = current->next;
      }
    }else{
      current = current->next;
    }
  }
}
//calculate the remaining process and print out the finishing message
void treat_finishing(Print_q *queue, Uint *counter, Uint known, Process_list *subprocess,Uint *count,Print_q *historyf){
  Node *current = queue->head;
  Node *nodelist[known];
  Uint len = 0;
  Uint cant= 0;
  for(Uint i=0;i<known;i++){
    nodelist[i] = NULL;
  }
  while(current){
    if(current->running == 0){
      for(Uint i=0;i<known;i++){
        if(nodelist[i] != NULL && current->pid == nodelist[i]->pid){
          current = current->next;
          cant = 1;
          break;
        }
      }
      if(!cant){
        nodelist[len] = current;
        len += 1;
        *counter += 1;

        current = current->next;
      }
      cant = 0;
    }else{
      current = current->next;
    }
  }
  //calculate the process remaining by minusing the counter of number of processes finished by known processes that may be finished
  current = queue->head;
  Uint proc_remain = 0;
  double temp = (double)known - (double)*counter;
  if(temp <= 0){
    proc_remain = 0;
  }else{
    proc_remain = known - *counter;
  }
  while(current){
    if(current->running == FIN){
      if(current->sub_id != -1){
        if(lastin(current,subprocess)){
          if(not_in_history(current,historyf)){
            printf("%.0f,FINISHED,pid=%lu,proc_remaining=%lu\n",current->priority,current->pid,proc_remain);
            Node *temp = dupNode(current);
            insert_node(temp,historyf);
            *count += 1;
          }
          current = current->next;
        }else{
          current = current->next;
        }
      }else{
        if(not_in_history(current,historyf)){
          printf("%.0f,FINISHED,pid=%lu,proc_remaining=%lu\n",current->priority,current->pid, proc_remain);
          Node *temp = dupNode(current);
          insert_node(temp,historyf);
          *count += 1;
       }
        current = current->next;
      }
    }else{
      current = current->next;
    }
  }
}

//run the simulation multiple times until the lists are empty, then print the stats
void simulate_multi_wth_stat(Process_list **list, Uint len, Process_list *readin,Process_list *subprocess, Process_list *worksub){
  Print_q * historyr = new_priQue();
  Print_q *historyf = new_priQue();
  Stats *perform_stat = new_stats();
  Uint counter = 0;
  Uint count = 0;
  Uint known =0;
  //  Process_list *movesubprocess =   duplicateProcessList(subprocess);;
  Process *current;
  Print_q *ghast = new_priQue();
  Node *node;
  Node *prev = NULL;
  //if the scheduling hasnt been finished, then continue
  while(!finished(list, len)){
    //pop the earliest message from the printing queue
    node = popEarliest(list,len);
    for(Uint i=0;i<len;i++){
      //if all cpu lists has been exhausted and the print_queue is empty, then that
      //cpu list is finished, when all cpu lists are finished, scheduling is finished
      if(list[i]->size == 0 && isempty(list[i]->print_q)){
        list[i]->finished = 1;
      }
      //if the prinitng queue is empty. then continue scheduling and populate the printing queue
      if(isempty(list[i]->print_q)){
        if(list[i]-> current != NULL){
          schedule(list[i],list[i]->current,perform_stat,worksub);
        }
      }
    }
    //if the current message has the same time as the previous message, then waiting
    // if they have different time, then print all the previous messages that are stored in ghast
    if(prev == NULL && node != NULL){
      prev = node;
      Node *temp = dupNode(node);
      insert_node(temp,ghast);
    }else if(node != NULL){
      if(node->priority == prev->priority){
        Node *temp = dupNode(node);
        insert_node(temp,ghast);
      }else{
        Process *current = readin->head;
        while(current){
          if(current->time_arrived <= prev->priority){
            known += 1;
          }
          current = current->next;
        }
        prev = node;
        treat_finishing(ghast,&counter,known,subprocess,&count,historyf);
        treat_running(ghast,historyr);
        ghast = new_priQue();
        Node *temp = dupNode(node);
        insert_node(temp,ghast);
        known = 0;
      }
    }
  }
  //print the last of the messages
  treat_running(ghast,historyr);
  current = readin->head;
  while(current){
    if(current->time_arrived <= prev->priority){
      known += 1;
    }
    current = current->next;
  }
  treat_finishing(ghast, &counter, known,subprocess,&count,historyf);
  //calculating and printing statistics
  double total_over = 0;
  long long total_turn = 0;
  double max_over = 0;
  double max_makespan = 0;
  for(Uint i=0;i<len;i++){
    total_over += list[i]->stats->total_overhead;
    total_turn += list[i]->stats->total_turnaround;
    if(list[i]->stats->max_overhead > max_over){
      max_over = list[i]->stats->max_overhead;
    }
    if(list[i]->stats->makespan > max_makespan){
      max_makespan = list[i]->stats->makespan;
    }
  }
  printf("Turnaround time %.0f\n",(double)((total_turn/count) + ((total_turn % count)!=0)));
  printf("Time overhead %.2f %.2f\n",rounded(max_over),rounded(total_over/(double) count));
  printf("Makespan %.0f",max_makespan);
}

//run the simulation schedule once
void run (Process_list *list,Process *current, Process_list *subprocesslist, Uint exec){
  add_print(current,list,1,list->cpu_num);
  Node *ar;
  if(list->wait_que->size != 1){
    Process *next = current->next;
    remove_process(list->wait_que,current);
    while(next){
      Uint swi = allocation_case(list,current,next);
      if(swi == 0){
        next = next->next;
      }else if (swi == 2){
        exec = TRUE;
        break;
      }else{
        double temp = (double)current->remain_time + (double)list->stats->makespan - (double)next->time_arrived;
        if(temp < 0){
          current->remain_time = 0;
        }else{
          current->remain_time = current->remain_time + list->stats->makespan - next->time_arrived;
        }
        list->stats->makespan = next->time_arrived;
        add_print(next,list,1,list->cpu_num);
        insert_waitQue(current,list->wait_que);
        current = next;
        list->current = current;
        next = current->next;
      }
    }
    //if allowed execution, then finish the current process
    if(exec){
      list->stats->makespan += (double)current->remain_time;
      ar = add_print(current,list,0,list->cpu_num);
      remove_process_fromcpulist(list,current);
      remove_process(list->wait_que,current);
      if(current->parallelisable == 1){
        if(lastin(ar,subprocesslist)){
        calc_total_turnaround(list->stats->makespan,current,&list->stats->total_turnaround);
        calc_overhead(current,list->stats->makespan,&list->stats->max_overhead,&list->stats->total_overhead);
        }
      }else{
        calc_total_turnaround(list->stats->makespan,current,&list->stats->total_turnaround);
        calc_overhead(current,list->stats->makespan,&list->stats->max_overhead,&list->stats->total_overhead);
      }

      current = findcurrent(list,list->stats->makespan);
      if(list->stats->makespan < current->time_arrived){
        list->stats->makespan = (double)current->time_arrived;
      }
      list->current = current;
    }
  }
}
//find the k cpu lists with the shortest remain time, and arrange them in ascending
//order in a list, then return that list
void find_smallest_cpu(int *smallestlists, Process_list **list, Uint k, Uint len,Process *process){
  Uint current;
  Uint smallestnum[k];
  for(Uint i=0;i<k;i++){
    smallestlists[i] = -1;
  }
  for(Uint i=0;i<len;i++){
    current = calc_remain_in_list(list[i],process);
    for(Uint j=0;j<k;j++){
      if(smallestlists[j] == -1){
        smallestlists[j] = list[i]->cpu_num;
        smallestnum[j] = current;
        break;
      }else if(current < smallestnum[j]){
        for(Uint z=k-1;z>j;z--){
          smallestlists[z] = smallestlists[z-1];
          smallestnum[z] = smallestnum[z-1];
        }
        smallestlists[j] = list[i]->cpu_num;
        smallestnum[j] = current;
        break;
      }
    }
  }

}
//running the simulation based on the readin process list from the file input

void simulation(Uint len, Process_list *readin,Process_list *subprocess){
  assert(readin);
  Process *current = readin->head;
  Process_list *list[len];
  Process_list *gonelist[len];
  Uint listnum;
  Uint exec=FALSE;
  Process_list *worksub= new_processlist(NOTCPU);
  Uint k;
  Process_list *subprocesslist = new_processlist(NOTCPU);

  for(Uint i=0;i<len;i++){
    list[i] = new_processlist(i);
    gonelist[i] = new_processlist(i);
  }
  while(current){
    if(current->parallelisable == 0){
      listnum = assign_singular_process(list,current,len);
      processlist_insert(gonelist[listnum],current);
      Process *dup = dupProcess(current);
      insert_waitQue(dup,gonelist[listnum]->wait_que);
      //if the current arriving process has larger time arrived than the current working process's remain time + makespan
      //then execution is allowed
      double temp = gonelist[listnum]->stats->makespan + (double) gonelist[listnum]->head->remain_time;
      if(current->time_arrived > temp){
        exec = TRUE;
      }else{
        exec = FALSE;
      }
      if(gonelist[listnum]->current == NULL){
        gonelist[listnum]->current = gonelist[listnum]->wait_que->head;
      }
      //run the current cpu once;
      run(gonelist[listnum],gonelist[listnum]->current,worksub,exec);
    }else{
      //find k
      if(current->remain_time < len){
        k = current->remain_time;
      }else{
        k = len;
      }
      Process *process = dupProcess(current);
      //change the subprocesses remain time if the k isnot 1
      if(k!=1){
        process->remain_time = (process->remain_time/k) + ((process->remain_time% k)!=0) + 1;
      }
      int smallestlists[k];
      //find the cpus to assign the subprocesses to
      find_smallest_cpu(smallestlists,list,k,len,process);
      for(Uint i=0;i<k;i++){
        Uint cpunum = smallestlists[i];
        if(k==1){
          process->sub_id = -1;
          process->parallelisable = 0;
        }else{
          process->sub_id = i;
        }
        processlist_insert(subprocesslist,process);
        processlist_insert(worksub,process);
        processlist_insert(list[cpunum],process);
        processlist_insert(gonelist[cpunum],process);
        insert_waitQue(process,gonelist[cpunum]->wait_que);
      }
      for(Uint i=0;i<k;i++){
        Uint cpunum = smallestlists[i];
        double temper = gonelist[cpunum]->stats->makespan + (double)gonelist[cpunum]->head->remain_time;
        if(process->time_arrived > temper){
          exec = TRUE;
        }else{
          exec = FALSE;
        }
        if(gonelist[cpunum]->current == NULL){
          gonelist[cpunum]->current = gonelist[cpunum]->wait_que->head;
        }
        run(gonelist[cpunum],gonelist[cpunum]->current,worksub,exec);
      }

    }
    current = current->next;
  }
  simulate_multi_wth_stat(gonelist, len,readin,subprocesslist,worksub);
}
