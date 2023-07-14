#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>

#include "Schedule_sim.h"
#include "Struct_manipulation.h"
#include "Challenge.h"
void processlist_insert_challenge(Process_list *list, Process *original){
  bool found = false;
  Process *process = dupProcess(original);
  //if list is empty, then assign it as head and tail
  if(list->size == 0){
    list->head = process;
    list->tail = process;
    list->stats->makespan = process->time_arrived;
    found = true;
  }else{
    Process *current = list->head;
    //loop through the linked list
    while(current){
      if(process->p_id == current->p_id && current->parallelisable){
        if(process->sub_id < current->sub_id){
          Place_infront(list,current,process);
          found = true;
          break;
        }else{
          if(current->next != NULL){
            current->next->prev = process;
            process->next = current->next;
            process->prev = current;
            current->next = process;
          }else{
            current->next = process;
            process->prev = current;
            list->tail = process;
          }
          found = true;
          break;
        }
      }
      //reordering the process list by 1.time arrived 2.remain_time 3.p_id
      if(process->time_arrived == current->time_arrived){
        if(process->remain_time == current->remain_time){
          if(process->p_id < current->p_id){
            Place_infront(list,current,process);
            found = true;
            break;
          }else{
            current = current->next;
          }
        }else if(process->remain_time < current->remain_time){
          Place_infront(list,current,process);
          found = true;
          break;
        }else{
          current = current->next;
        }
      }else if(process->time_arrived < current->time_arrived){
        Place_infront(list,current,process);
        found = true;
        break;
      }else{
        current = current->next;
      }
    }
    //if the need insert process has not found its place, then place it at the end
    if(!found){
      list->tail->next = process;
      process->prev = list->tail;
      list->tail = process;
    }
  }
  list->current = list->head;
  list->size++;
}
void assign_process(Process_list ** list, Process_list * readin,Uint len){
  assert(readin);
  Process *process = readin->head;
  Uint pList_num;
  while(process){
    Process *dup = dupProcess(process);
    //find the lowest time amongst all cpu list
    pList_num = find_lowest_time(list, len, dup);
    //assign that process to that cpu list
    processlist_insert_challenge(list[pList_num],dup);
    process = process->next;
  }
}
double find_makespan(Process_list * list){
  assert(list);
  double output = 0;
  Process *current = list->head;
  while(current){
    if(current->time_arrived <= output){
      output += (double)current->remain_time;
      current = current->next;
    }else{
      output = (double)current->time_arrived;
    }
  }
  return output;
}
void remove_process_fromlist(Process_list *list, Process * process, Uint wantsub){
  assert(list);
  Process * current = list->head;
  while(current){
    if(current->p_id == process->p_id){
      if(wantsub || (!wantsub && current->sub_id == process->sub_id)){
        if(current->next != NULL && current->prev != NULL){
          current->next->prev = current->prev;
          current->prev->next = current->next;
        }else if (current->next != NULL && current->prev == NULL){
          current->next->prev = NULL;
          list->head = current->next;
        }else if (current->next == NULL && current->prev != NULL){
          current->prev->next = NULL;
          list->tail = current->prev;
        }else{
          list->head = NULL;
          list->tail = NULL;
        }
        list->size -= 1;
        break;
      }
    }
      current = current->next;
    }
}
Uint improve(Uint index, Process_list **list, Uint len,Uint k){
  Process_list * theone = list[index];
  double maxmakespan = find_makespan(list[index]);
  assert(theone);
  Process * current = theone->tail;
  Uint exiting = 0;

  while(current){
    if(current->parallelisable == 1){
      current = current->prev;
    }else{
      for(Uint i=0;i<len;i++){
        if(i!=index){
          Process_list *test = duplicateProcessList(list[i]);
          processlist_insert_challenge(test,current);
          if(find_makespan(test) < maxmakespan){
            remove_process_fromlist(theone, current,0);
            processlist_insert_challenge(list[i],current);
            exiting = 1;
            break;
          }
        }
      }
      if(exiting == 1){
        break;
      }else{
        current= current->prev;
      }
    }
  }
  if(exiting == 1){
    return 1;
  }else{
    return 0;
  }
}
void check_for_improvement(Process_list **list,Uint len,Uint k){
  Uint max = find_makespan(list[0]);
  Uint index = 0;
  Uint current;
  for(Uint i=1;i<len;i++){
    current = find_makespan(list[i]);
    if(current > max){
      max = current;
      index = i;
    }
  }
  improve(index,list,len,k);
  return;

}

void check_for_parallel_improvement(Process_list ** list,Uint len, Uint k, Process_list *subprocess,Process_list *minisub){
  Uint baseline[len];
  Uint max=find_makespan(list[0]);
  Uint min = max;
  Uint minindex = 0;
  Uint found=0;
  for (Uint i=1;i<len;i++){
    baseline[i] = find_makespan(list[i]);
    if (baseline[i] > max){
      max = baseline[i];
    }
    if(baseline[i] < min){
      min = baseline[i];
      minindex = i;
    }
  }
  if(minisub->size > 0){
    Process *current =minisub->head;
    while(current){
      if(current->time_arrived == 0 && current->p_id != minisub->tail->p_id){
        current->parallelisable = 0;
        assign_singular_process(list,current,len);
        current = current->next;
      }else{
        max=find_makespan(list[0]);
        min = max;
        minindex = 0;
        for (Uint i=1;i<len;i++){
          if (find_makespan(list[i]) > max){
            max = find_makespan(list[i]);
          }
          if(find_makespan(list[i]) < min){
            min = find_makespan(list[i]);
            minindex = i;
          }
        }
        k=len;
        if(current->remain_time<len){
          k = current->remain_time;
        }
        Process_list *dup = duplicateProcessList(list[minindex]);
        processlist_insert_challenge(dup,current);
        if(find_makespan(dup) < max){
          current->parallelisable = 0;
          current->sub_id = -1;
          processlist_insert_challenge(list[minindex],current);
          found = 1;
        }
        if(found == 1){
          current = current->next;
          found = 0;
        }else{
          if(current->remain_time != 1){
            current->remain_time = (current->remain_time /k) + ((current->remain_time  % k)!=0) + 1;
          }
          for(Uint i=0;i<k;i++){
            current->parallelisable = 1;
            current->sub_id = i;
            processlist_insert_challenge(list[i],current);
            processlist_insert_challenge(subprocess,current);
          }
          current = current->next;
        }
      }
    }
  }
}
Process *new_process_challenge(char *str, Uint parallelisable, Uint k){
  Uint input_counter = 0;
  Process *process = malloc(sizeof(*process));
  char delim[] = " ";
  char temp[strlen(str)];
  strcpy(temp,str);
  //breaks string into tokens, 'splitting' it by deliminator of " "
  char * ptr = strtok(temp,delim);
  char * endptr;
  process->next = NULL;
  process->prev = NULL;
  while(ptr!= NULL){
    if(input_counter == 0){
      process->time_arrived = strtoul(ptr, &endptr, 10);
      input_counter += 1;
    }
    else if(input_counter == 1){
      process->p_id = strtoul(ptr, &endptr, 10);
      input_counter += 1;
    }
    else if(input_counter == 2){
      if(parallelisable == -1){
        process->remain_time = strtoul(ptr, &endptr, 10);
        process->exec_time = process->remain_time;
        input_counter += 1;
      }else{
        if(k == 1){
          Uint temp_num = strtoul(ptr, &endptr, 10);
          process->remain_time =temp_num;
          process->exec_time = temp_num;
          input_counter += 1;
        }
        else{
          Uint temp_num = strtoul(ptr, &endptr, 10);
          temp_num =(temp_num/k) + ((temp_num % k)!=0) + 1;
          process->remain_time =temp_num;
          process->exec_time = temp_num;
          input_counter += 1;
        }
      }
    }
    ptr = strtok(NULL,delim);
  }
  if(parallelisable != -1 && k != 1){
    process->parallelisable = 1;
    process->sub_id = parallelisable;
  }else{
    process->parallelisable = 0;
    process->sub_id = -1;
  }
  return process;
}
Process_list * c_create_array(Uint value, char *filename, Process_list **list, Process_list *subprocess){
  FILE * fp;
  char * content = NULL;
  size_t len = 0;
  size_t read;
  Uint k;
  Process *process;
  Process_list * readin = new_processlist(-1);
  Process_list *addin = new_processlist(-1);
  Process_list *minisub = new_processlist(-1);
    fp = fopen(filename, "r");
    if (fp == NULL){
        exit(EXIT_FAILURE);
    }
    for(Uint i=0; i<value+1;i++){
      list[i] = new_processlist(i);
    }

    while ((read = getline(&content, &len, fp)) != -1){
      k = findk(content,value);
      if(!parallelisable(content)){
        process = new_process_challenge(content,-1,k);
        processlist_insert_challenge(addin,process);
        processlist_insert_challenge(readin,process);
      }else{
        process = new_process_challenge(content,1,1);
        processlist_insert_challenge(readin,process);
        processlist_insert_challenge(minisub,process);
      }
    }
    assign_process(list,addin,value);
    check_for_improvement(list,value,k);
    check_for_parallel_improvement(list,value,k,subprocess,minisub);
    fclose(fp);
    if (content){
      free(content);
    }

    for(Uint i=0;i<value;i++){
      if(list[i]->head != NULL){
        list[i]->stats->makespan = list[i]->head->time_arrived;
      }else{
        list[i]->stats->makespan = 0;
      }
      list[i]->current = list[i]->head;
    }
    return readin;
}


//print the running messages
void treat_running_challenge(Print_q *queue){
  Node *current = queue->head;
  while(current){
    if(current->running == 1){
      if(current->sub_id != -1){
        printf("%.0f,RUNNING,pid=%ld.%ld,remaining_time=%ld,cpu=%ld\n",current->priority,current->pid,current->sub_id,current->remainTime,current->cpu_num);
        current = current->next;
      }else{
        printf("%.0f,RUNNING,pid=%ld,remaining_time=%ld,cpu=%ld\n",current->priority,current->pid,current->remainTime,current->cpu_num);
        current = current->next;
      }
    }else{
      current = current->next;
    }
  }
}
void treat_finishing_challenge(Print_q *queue, Uint *counter, Uint known, Process_list *subprocess,Uint *count){
  Node *current = queue->head;
  Node *nodelist[known];
  Uint len = 0;
  Uint cant= 0;
  for(Uint i=0;i<known;i++){
    nodelist[i] = NULL;
  }
  //adding counter of how many processes has been completed
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
      //printf("%ld,FINISHED,pid=%ld,proc_remaining=%ld D\n",current->priority,current->pid,1);
        current = current->next;
      }
      cant = 0;
    }else{
      current = current->next;
    }
  }
  current = queue->head;
  Uint proc_remain = 0;
  //calcualte how many processes are remaining
  double temp = (double)known - (double)*counter;
  if(temp <= 0){
    proc_remain = 0;
  }else{
    proc_remain = known - *counter;
  }
  while(current){
    if(current->running == 0){
      if(current->sub_id != -1){
        //check if the process that is parallelisable is the last of the sub processes that is finished, if not, then do not
        //print the message
        if(lastin(current,subprocess)){
          printf("%.0f,FINISHED,pid=%ld,proc_remaining=%ld\n",current->priority,current->pid,proc_remain);
          current = current->next;
          *count += 1;
        }else{
          current = current->next;
        }
      }else{
        printf("%.0f,FINISHED,pid=%ld,proc_remaining=%ld\n",current->priority,current->pid, proc_remain);
        current = current->next;
        *count += 1;
      }
    }else{
      current = current->next;
    }
  }
}

void simulate_scheduler(Process_list **list, Uint len, Process_list *readin,Process_list *subprocess){
  Stats *perform_stat = new_stats();
  Uint counter = 0;
  Uint count = 0;
  Uint known =0;
  Process_list *movesubprocess =   duplicateProcessList(subprocess);;
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
          schedule(list[i],list[i]->current,perform_stat,movesubprocess);
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
        treat_finishing_challenge(ghast,&counter,known,subprocess,&count);
        treat_running_challenge(ghast);
        ghast = new_priQue();
        Node *temp = dupNode(node);
        insert_node(temp,ghast);
        known = 0;
      }
    }
  }
  //print the last of the messages
  treat_running_challenge(ghast);
  current = readin->head;
  while(current){
    if(current->time_arrived <= prev->priority){
      known += 1;
    }
    current = current->next;
  }
  treat_finishing_challenge(ghast, &counter, known,subprocess,&count);
  //calculating and printing statistics
  double total_over = 0;
  long long  total_turn = 0;
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
  printf("Turnaround time %.0f\n",(double)(total_turn/count) + ((total_turn % count)!=0));
  printf("Time overhead %.2f %.2f\n",rounded(max_over),rounded(total_over/(double) count));
  printf("Makespan %.0f",max_makespan);
}
