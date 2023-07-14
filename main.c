#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>
#include "Struct_manipulation.h"
#include "Schedule_sim.h"
#include "Challenge.h"
/*main file */
/* Reads the file and process the information within the file based on how many
Processors there are*/
#define MAXCPULIMIT 1024
#define MINCPULIMIT 0
#define NOTCPU -1

void doreadfile(int value, char *filename, int c){
  Process_list *list[value];
  Process_list *readin;
  Process_list *subprocess = new_processlist(NOTCPU);
  // if the input -p is larger than 0 and lower than 1024, then process with simulation
  if(value < MAXCPULIMIT && value > MINCPULIMIT){
    for(int i=0;i<value;i++){
      list[i] = new_processlist(i);
    }
    //if challenge, then use the challenge create array and proceed with different algorithm
    if(c){
      //read in the input and populate cpu list before simulating
      readin = c_create_array(value,filename,list,subprocess);
      simulate_scheduler(list,value, readin,subprocess);
    }else{
      //read in input file into a list and simulate the arrival process
      readin = create_array_processList(value,filename,list,subprocess);
      simulation(value, readin,subprocess);
    }
    for(int i=0;i<value;i++){
      free_list(list[i]);
    }
    free_list(readin);
  }
  else{
    exit(EXIT_FAILURE);
  }
}
//Reading input, reading the file
void processdata(int argc, char *argv[]){
  int c = 0;
  if( argc == 5 ) {
    if(strcmp(argv[1], "-f") == 0){
      doreadfile(atoi(argv[4]), argv[2],c);
    }else{
      doreadfile(atoi(argv[2]),argv[4],c);
    }
  }else if(argc == 6){
    c = 1;
    if(strcmp(argv[1],"-f") == 0){
      if(strcmp(argv[3],"-p") == 0){
        doreadfile(atoi(argv[4]),argv[2],c);
      }
      else{
        doreadfile(atoi(argv[5]),argv[2],c);
      }
    }else if(strcmp(argv[1],"-p")==0){
      if(strcmp(argv[3],"-f") == 0){
        doreadfile(atoi(argv[2]),argv[4],c);
      }else{
        doreadfile(atoi(argv[2]),argv[5],c);
      }
    }else{
      if(strcmp(argv[2],"-p") == 0){
        doreadfile(atoi(argv[3]),argv[5],c);
      }else{
        doreadfile(atoi(argv[5]),argv[3],c);
      }
    }
  }
  else{
    printf("INVALID");
  }

  //printf("%d",makespan);
}


//main function
int main( int argc, char *argv[] )  {
   processdata(argc, argv);
   return 0;
}
