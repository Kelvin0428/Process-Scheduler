#ifndef QUESTIONONE_H
#define QUESTIONONE_H
// a node within print queue, consists of data needed to print the message
typedef struct node Node;
// a doubly linked list consisting of messages to print
typedef struct print_q Print_q;
// process list is implemented with a doubly linked list
typedef struct process_list Process_list;
//process includes all the necesary information
typedef struct process Process;
// a structure consisting of the stats needed to print
typedef struct stats Stats;
// a queue of processes waiting to be scheduled
typedef struct wait_que Wait_que;
//using unsigned int to beable to store int of 2^32
typedef unsigned long Uint;
struct process_list {
  Process *head;
  Process *tail;
  Process *current;
  Print_q *print_q;
  Wait_que *wait_que;
  Stats *stats;
  Uint cpu_num;
  Uint size;
  Uint finished;
};

struct process {
  Uint time_arrived;
  Uint p_id;
  Uint remain_time;
  Uint exec_time;
  Uint parallelisable;
  Uint sub_id;
  Uint k;
  Process *next;
  Process *prev;
};

struct node{
  double priority;
  Uint pid;
  Uint sub_id;
  Uint remainTime;
  Uint running;
  Uint cpu_num;
  Node *next;
  Node *prev;
};

struct print_q{
  Node *head;
  Node *tail;
  Uint size;
};
struct wait_que{
  Process *head;
  Process *tail;
  Uint size;
};
struct stats{
  double total_turnaround;
  double total_overhead;
  double max_overhead;
  double makespan;
};
void free_list(Process_list *list);
//rounding a float to nearest
double rounded(double num);
//create an empty priority queue and return a pointer of it
Print_q *new_priQue();
Stats *new_stats();
//create an empty process list and return a pointer of it
Process_list *new_processlist(Uint cpu_num);
//create a new process containing the input as its content, and reutnr a pointer of it
Process *new_process(char *str, Uint parallelisable, Uint k);
//places the process infront of the current process in the list
void Place_infront(Process_list *list, Process *current, Process *process);
//inserting the process into the process list based on its content
void processlist_insert(Process_list *list, Process *original);
//printing the list pids to standard output
void print_list(Process_list *list);
Process_list * duplicateProcessList(Process_list * list);
//division rounding, mathematical function idea from stackoverflow
Uint div_round( Uint A, Uint B );
//calculates the overhead infos
void calc_overhead(Process *process, double makespan, double *max_overhead, double *total);
//calculate the total turnaround, not the average
void calc_total_turnaround(double makespan, Process *process, double *turnaround);
//removes a process from a list
void remove_process_fromcpulist(Process_list *list, Process * process);
//duplicates a process
Process *dupProcess(Process *process);
//duplicates a process list
Process_list * duplicateProcessList(Process_list * list);
//creates an empty wait queue and reutnrs a pointer of it
Wait_que *new_waitque();
//creates a new node and returns a pointer of it
Node * new_node(double makespan, Uint running, Uint pid, Uint sub_id, Uint remainTime,  Uint cpu_num);
//print waituqe content to  output
void print_waitque(Wait_que *queue);
//print printque content to output
void print_priQue(Print_q *queue);
//insert a node in a print queue
void insert_node(Node * node, Print_q *queue);
//create a message node and store it in the processlist's print queue
Node * add_print(Process *current, Process_list* list, Uint running, Uint cpu_num);
//insert a proecss into the waitque
void insert_waitQue(Process *process, Wait_que *queue);
//removes a process from the waitque
Process * remove_process(Wait_que *queue, Process *current);
//removes the head of the print queue
Node * remove_head(Print_q *queue);
//checks if the print queue is empty
Uint isempty(Print_q *queue);
//duplicates a node
Node *dupNode(Node *node);;
#endif
