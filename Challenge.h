
#include "Struct_manipulation.h"
double find_makespan(Process_list * list);
void remove_process_fromlist(Process_list *list, Process * process, Uint wantsub);
Uint improve(Uint index, Process_list **list, Uint len,Uint k);
void check_for_improvement(Process_list **list,Uint len,Uint k);
Process_list * c_create_array(Uint value, char *filename, Process_list **list, Process_list *subprocess);
void check_for_parallel_improvement(Process_list ** list,Uint len, Uint k, Process_list *subprocess,Process_list *minisub);
void simulate_scheduler(Process_list **list, Uint len, Process_list *readin,Process_list *subprocess);
