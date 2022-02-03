# Thread-Scheduler

*	Featured with 3-level non-preemptive priority feedback queue and Round Robin(RR) scheduling with different Time Quantum(TQ).

*	Implemented context switch using ucontext and related APIs in Linux.

* Utilized signal handler in Linux such as SIGTSTP and SIGALRM to solve time calculating and threads information monitoring issues.

* Configurated the thread using JSON.

## Directories
* function_library.h, function_library.c -> Thread function
* os_2021_thread_api.h, os_2021_thread_api.c -> Thread application interface function
* simulator.c -> Function to parse init_threads.json file, create thread, and the main source
* init_threads.json -> Thread configuration file

## Building the Project
* Install json-c library
```
sudo apt-get install libjson-c-dev
```
* Git Hook install
```
make
```
* Compile simulator
```
make simulator
```
* Delete all output file
```
make clean
```

## Usage
* Execute simulator
```
./simulator
```
* Use ```Ctrl+Z``` to reports thread information
* Use ```Ctrl+C``` to terminate the process
```
f1 wants to waiting for event 3
The priority of thread f1 is changed from M to H
f3 wants to waiting for event 3
The priority of thread f3 is changed from M to H
^Z
**************************************************************************************************
*	TID	Name		State		B_Priority	C_Priority	Q_Time	W_time	 *
*	0	reclaimer 	RUNNING		L		L		120	0	 *
*	3	random_1  	READY		L		L		148	0	 *
*	4	random_2  	READY		L		L		148	0	 *
*	1	f1        	WAITING		M		H		0	208	 *
*	2	f3        	WAITING		M		H		0	208	 *
**************************************************************************************************
I found 65409.
random_1 changes the status of f1 to READY.
f1 wants to cancel thread random_1
f1 wants to cancel thread random_2
The priority of thread f1 is changed from H to M
^Z
**************************************************************************************************
*	TID	Name		State		B_Priority	C_Priority	Q_Time	W_time	 *
*	1	f1        	RUNNING		M		M		30	660	 *
*	4	random_2  	READY		L		L		508	0	 *
*	0	reclaimer 	READY		L		L		478	0	 *
*	3	random_1  	READY		L		L		478	0	 *
*	2	f3        	WAITING		M		H		0	718	 *
**************************************************************************************************
The priority of thread f1 is changed from M to L
The memory space by random_2 has been released.
The memory space by random_1 has been released.
^Z
**************************************************************************************************
*	TID	Name		State		B_Priority	C_Priority	Q_Time	W_time	 *
*	0	reclaimer 	RUNNING		L		L		540	0	 *
*	1	f1        	READY		M		L		112	660	 *
*	2	f3        	WAITING		M		H		0	862	 *
**************************************************************************************************
^C
```

## Implementation

### Multilevel Feedback Queue
The two thing we considerate in our queue is: 1.Thread's priority 2. Enter order

If their priority is different, high priority serve first
Else, first come first serve
If the input order is: 1.L 2.H 3.M 4.M 5.H 6.L, the thread in queue be like:
Head -> 2 -> 5 -> 3 -> 4 -> 1 -> 6
