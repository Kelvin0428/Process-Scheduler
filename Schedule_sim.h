#ifndef QUESTIONTWO_H
#define QUESTIONTWO_H
#define RUN 1
#define FIN 0
#define TRUE 1
#define FALSE 0
#define NULLEXIST -1
#define FIRSTELEMENT 0
#define GIVECURRENT 0
#define GIVENEXT 1
#define GAP 2
#include "Struct_manipulation.h"


double calc_remain_in_list(Process_list *list,Process * process);
Uint find_lowest_time(Process_list **list, Uint len, Process *process);
//finding k for a process, if process has remain time lower than the number
//of cpu availble then k is the remain time
Uint findk(char * str, Uint value);
/*creates an array of process list, of size of input value -p*/
Process_list * create_array_processList(Uint value, char *filename, Process_list **list, Process_list *subprocess);


//determing whether to give processor to current process or next, or if special case
Uint allocation_case(Process_list *list, Process *current, Process *next);

//find the current process within the process list to process
Process *findcurrent(Process_list *Plist, double makespan);

//check if the input of a process states the process is parallelisable or not
Uint parallelisable(char *str);

//check if a sub process is the last of the subprocess that finished executing
Uint lastin(Node *current,Process_list *subprocess);

//simulating allocation of processes
Process *schedule(Process_list *list, Process *current, Stats *perform_stat,Process_list *movesubprocess);

//pop the element with lowest priority
Node *popEarliest(Process_list ** list, Uint len);

//removes an element from the list
void remove_list(Process_list ** list, Uint index, Uint len);

//check if print queue is empty
Uint finished(Process_list ** list,Uint len);

//checks if the message  has been printed or nothing
//checks the existence of the message in the history queue
Uint not_in_history(Node *node, Print_q *history);

//treat the running messages, print out the message if it has not been printed out before
void treat_running(Print_q *queue,Print_q *historyr);

//calculate the remaining process and print out the finishing message
void treat_finishing(Print_q *queue, Uint *counter, Uint known,Process_list *subprocess, Uint *count,Print_q *historyf);

//run the simulation multiple times until the lists are empty, then print the stats
void simulate_multi_wth_stat(Process_list **list, Uint len, Process_list *readin,Process_list *subprocess, Process_list *worksub);

//find the k cpu lists with the shortest remain time, and arrange them in ascending
//order in a list, then return that list
void find_smallest_cpu(int *smallestlists, Process_list **list, Uint k, Uint len,Process *process);

//run the simulation schedule once
void run (Process_list *list,Process *current, Process_list *subprocesslist, Uint exec);

/*running the simulation based on the readin process list from the file input
reading in the processes one at a time, assign cpu, then schedule it*/
void simulation(Uint len, Process_list * readin,Process_list *subprocess);

Uint assign_singular_process(Process_list ** list, Process *process,Uint len);
#endif
