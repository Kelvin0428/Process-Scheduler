# Process-Scheduler
This is a project I worked on during my studies at the University of Melbourne for the subject Computer systems.

## Background
This project is a basic simulator that allocates CPU (or processor) to the running processes. The program will be invoked via the command line. It will be given a description of arriving processesincluding their arrival time, execution time in seconds, their id, and whether they are parallelisable or not.
 <br /> There are some test files included in the repositary to test the functions of my code.

## Functionality
Processes will be allocated CPU time according to shortest-time-remaining algorithm, a preemptivescheduling algorithm. When a new process arrives, it is scheduled if there is no other process running.If, when process jarrives, there is a process irunning, then iis postponed and added to a queue if andonly if jhas a shorter execution time than the remaining time of i. Process iis resumed where it leftoff, if it is the process with the shortest remaining time left among all other processes in the queue
 <br /> <br />  In the case where there is more than 1 processors running, indicatd by N > 1. A non-parallelisable process is assigned to a CPU that has the shortest remaining time to complete all processesand subprocesses assigned to it so far.
  <br /> <br /> In other words, a parallelisable process is split into k≤N subproceses, where k is the largest value for which x/k ≥ 1.Each subprocess is then assigned execution time of ⌈x/k⌉+ 1. Subprocesses follow a similar namingconvention as above: a process with id iis split into subprocesses with id’s i.0, i.1, . . .,i.k′, wherek′= k−1. Subprocesses are then assigned to k processors with shortest time left to complete such that subprocess i.0is assigned to the CPU that can finish its processes the fastest, i.1is assigned to the second fastest processor and so on. Use CPU ids to break ties, giving preference to smaller processor numbers.  <br />Example: consider a scenario with 4 CPUs, each with one process left to complete; processes on CPU 0,1,2,3 have remaining time of 30,20,20,10, respectively. Then for a process i with execution time of 2,k is set to 2. Its subprocess i.0 and i.1 will be assigned to CPU 3 and 1 respectively

 ### Challenge mode
 There is a challenge incorporated within this project, which calls for an improvement in performance (Optimisation), where the algorithm is allowed to see what processes will be arriving and optimise accordingly

 ## Usage
The program is called by _allocate_ and take the following command line arguments, the arguments can be passed in any order.
 <br /> -f filename will specify the name of the file describing the processes. <br /> -p processors where processors is one of {1,2,N}, N≤1024. <br /> -c an optional parameter
  <br />  <br /> The filename contains the processes to be executed and has the following format. Each line of the file corresponds to a process. The first line refers to the first process that needs to be executed, and the last line refers to the last process to be executed. Each line consists of a space-separated tuple (time-arrived,process-id,execution-time,parallelisable). 
   <br />  <br /> Example: ./allocate -f processes.txt -p 1. <br /><br />The allocation program is required to simulate execution of processes in the file processes.txt on a single CPU.
