#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>

#include "Struct_manipulation.h"
#include "Schedule_sim.h"

/*=================================================================================*/
//Object initialisation functions
/*=================================================================================*/
//creates an empty waiting queue and return a pointer of it
Wait_que *new_waitque(){
  Wait_que *que = malloc(sizeof(*que));
  assert(que);
  que->head = NULL;
  que->tail = NULL;
  que->size = 0;
  return que;
}
//creates a node with input as content, and return a pointer of it
Node * new_node(double makespan, Uint running, Uint pid, Uint sub_id, Uint remainTime,  Uint cpu_num){
  Node *node = malloc(sizeof(*node));
  assert(node);
  node->priority = makespan;
  node->next = NULL;
  node->prev = NULL;
  node->pid = pid;
  node->sub_id = sub_id;
  node->cpu_num = cpu_num;
  node->remainTime = remainTime;
  node->running = running;
  return node;
}
//creates an empty queue and return a pointer to it
Print_q *new_priQue(){
  Print_q *que = malloc(sizeof(*que));
  assert(que);
  que->head = NULL;
  que->tail = NULL;
  que->size = 0;
  return que;
}
Stats *new_stats(){
  Stats *stats = malloc(sizeof(*stats));
  assert(stats);
  stats->total_turnaround = 0;
  stats->total_overhead = 0;
  stats->max_overhead = 0;
  stats->makespan = 0;
  return stats;
}
//creates an empty Process list and return a pointer to it
Process_list *new_processlist(Uint cpu_num){
  Process_list *list = malloc(sizeof(*list));
  assert(list);
  list->head = NULL;
  list->current = NULL;
  list->stats = new_stats();
//  list->sec = NULL;
  list->tail = NULL;
  list->print_q = new_priQue();
  list->size = 0;
  list->finished = 0;
  list->cpu_num = cpu_num;
  list->wait_que = new_waitque();
  return list;
}
// creates a process with input as its data content and return a pointer to it
Process *new_process(char *str, Uint parallelisable, Uint k){
  Uint input_counter = 0;
  Process *process = malloc(sizeof(*process));
  char delim[] = " ";
  char temp[strlen(str)];
  strcpy(temp,str);
  //breaks string into tokens, 'splitting' it by deliminator of " "
  char * ptr = strtok(temp,delim);
  process->next = NULL;
  process->prev = NULL;
  process->k = k;
  char *endptr;
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
        process->exec_time =  process->remain_time;
        input_counter += 1;
      }else{
        if(k == 1){
          Uint temp_num =strtoul(ptr, &endptr, 10);
          process->remain_time =temp_num;
          process->exec_time = temp_num;
          input_counter += 1;
        }else if(k==-1){
          process->remain_time = strtoul(ptr, &endptr, 10);
          process->exec_time = process->remain_time ;
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
//===============================================================================//
//inserting or manipulating index of structures
//==========================================================//
//place process infront of current in the deque
void Place_infront(Process_list *list, Process *current, Process *process){
  if(current->p_id == list->head->p_id){
    process->next = current;
    current->prev = process;
    list->head = process;
  }else{
    if(current->prev != NULL){
      process->prev = current->prev;
    }
    current->prev->next = process;
    process->next = current;
    current->prev = process;
  }
}
//insert a process into the waiting queue
void insert_waitQue(Process *process, Wait_que *queue){
  Process *temp = dupProcess(process);
  if(queue->size > 0){
    Process * current = queue->head;
    while(current){
      if(current->p_id == temp->p_id){
        return;
      }
      current = current->next;
    }
    temp->prev = queue->tail;
    queue->tail->next = temp;
    temp->next = NULL;
  }else{
    queue->head =temp;
    temp->next = NULL;
  }
  queue->tail = temp;
  queue->size ++;
}
//inserting a content string into a process list
void processlist_insert(Process_list *list, Process *original){
  bool found = false;
  Process *process = dupProcess(original);

  //if list is empty, then assign it as head and tail
  if(list->size == 0){
    list->head = process;
    list->tail = process;
    list->stats->makespan = (double)process->time_arrived;
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
  //list->current = list->head;
  list->size++;
}
//inserting a node to the bottom of the queue
void insert_node(Node * node, Print_q *queue){
  if(queue->size > 0){
    node->prev = queue->tail;
    queue->tail->next = node;
  }else{
    queue->head = node;
  }
  queue->tail = node;
  queue->size ++;
}
Node * add_print(Process *current, Process_list* list, Uint running, Uint cpu_num){
  Node *node;
  if(current->time_arrived > list->stats->makespan){
    node = new_node(current->time_arrived,running,current->p_id,current->sub_id,current->remain_time, cpu_num);
  }else{
    node = new_node(list->stats->makespan,running,current->p_id,current->sub_id,current->remain_time, cpu_num);
  }
  insert_node(node,list->print_q);
  return node;
}
//============================================================================
//Printing functions for testing
//-------------------------------------------------------------------------
//print the contents of the print queue (for testing)
void print_priQue(Print_q *queue){
  assert(queue);
  if(queue->size != 0){
    Node *current = queue->head;
    while(current){
      printf("[priority: %.0f   pid:%lu  remainTime: %lu, running %lu]    ",current->priority, current->pid, current->remainTime,current->running);
      current = current->next;
    }
    printf("\n");
  }else{
    printf("nothing");
  }
}
//print the contents of the waiting queue
void print_waitque(Wait_que *queue){
  assert(queue);
  if(queue->size != 0){
    Process *current = queue->head;
    while(current){
      printf("[arrived %lu, pid:%lu  remainTime: %lu]    ", current->time_arrived, current->p_id, current->remain_time);
      current = current->next;
    }
    printf("\n");
  }else{
    printf("nothing");
  }
}
//printing the linked list out.. Code from project conducted in DOA
void print_list(Process_list *list) {
  Process *current = list->head;
  Uint i = 0;

  printf("[");

  while (current) {
        printf("%lu", current->p_id);
    // Print a comma unless we just printed the final element
    if (i < list->size - 1) {
      printf(", ");
    }
    current = current->next;
    i++;
  }

  printf("]\n");
}

/*============================================================================*/
//functions to duplicate and object
/*============================================================================*/
//duplicates a process with prev and next points to null
Process *dupProcess(Process *process){
  Process *temp = malloc(sizeof(*temp));
  temp->time_arrived = process->time_arrived;
  temp->p_id = process->p_id;
  temp-> remain_time= process->remain_time;
  temp-> exec_time= process->exec_time;
  temp-> parallelisable= process->parallelisable;
  temp->sub_id = process->sub_id;
  temp ->next = NULL;
  temp->prev = NULL;
  temp->k = process->k;
  return temp;
}

//duplicates a Node
Node *dupNode(Node *node){
  Node *temp = new_node(node->priority,node->running,node->pid,node->sub_id, node->remainTime,node->cpu_num);
  return temp;
}

Process_list * duplicateProcessList(Process_list * list){
  Process_list *temp = new_processlist(list->cpu_num);
  Process *current = list->head;
  while(current){
    processlist_insert(temp,current);
    current = current->next;
  }
  return temp;
}
//======================================================================================//
//calculations
//======================================================================================//
//division rounding with both positive integer,mathmatical equasion idea from Stack overflow
//https://stackoverflow.com/questions/2422712/rounding-integer-division-instead-of-truncating
Uint div_round( Uint A, Uint B ){
  return ((A -( B+1)/2) / B + 1);
}
//calculating total and max overhead after the current process
void calc_overhead(Process *process, double makespan, double *max_overhead, double *total){
  float this_overhead = ((makespan - process->time_arrived) /(double)process->exec_time);
  if(this_overhead > *max_overhead){
    *max_overhead = this_overhead;
  }
  *total += this_overhead;
}
//calculates total turnaround after this process
void calc_total_turnaround(double makespan, Process *process, double *turnaround){
  double temp = makespan - (double) process->time_arrived;
  if (temp < 0){
    *turnaround += 0;
  }else{
    *turnaround += makespan - (double)process->time_arrived;
  }
}
//rounds to nearest
double rounded(double num){
  double out = (double)(num * 100 + .51);
  return (out / 100);
}

//=======================================//
//removeing element in structure
//=========================================//
void remove_process_fromcpulist(Process_list *list, Process * process){
  assert(list);
  Process * current = list->head;
  while(current){
    if(current->p_id == process->p_id && current->sub_id == process->sub_id){
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
    current = current->next;
  }
}

//removes the input process from the queue
Process * remove_process(Wait_que *queue, Process *current){
  assert(queue);
  Process *temp = queue->head;
  if (temp == NULL){
    return NULL;
  }
  while(temp){
    if (current->p_id == temp->p_id){
      if(temp->prev == NULL && temp->next != NULL){
        temp->next->prev = NULL;
        queue->head = temp->next;
      }else if (temp->next == NULL && temp->prev != NULL) {
        temp->prev->next = NULL;
        queue->tail = temp->prev;
      }else if (temp->prev != NULL && temp->next != NULL){
        temp->prev->next = temp->next;
        temp->next->prev = temp->prev;
      }else{
        queue->head = NULL;
        queue->tail = NULL;
        return current;
      }
      current->next = queue->head;
      return current;
    }else{
      temp = temp->next;
    }
  }
  current->next = queue->head;
  return current;
}
//remove the head of a queue
Node * remove_head(Print_q *queue){
  Node *temp = queue->head;
  if(queue->head->next != NULL){
    queue->head->next->prev = NULL;
    queue->head = queue->head->next;
  }else{
    queue->head = NULL;
  }
  queue->size -= 1;
  return temp;
}


//check if queue is empty
Uint isempty(Print_q *queue){
  if(queue->size == 0){
    return TRUE;
  }else{
    return FALSE;
  }
}
void free_list(Process_list *list){
  Process *temp = list->tail;
  while(temp){
    Process *another = temp;
    temp = temp->prev;
    free(another);
  }
  free(list);
}
