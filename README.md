# OS-Scheduling-Simulator
Round Robin and FCFS Scheduling Simulation


## Code organization:
- At first the main function read its arguments, parses it and prints errors if arguments are not as expected.
- It then calls readFile function which takes the filename, opens it, parses it and fills a two dimensional array for each process with its status.
- The main function makes another copy from the CPU time for the second run after the process is blocked.
- The main then choose from calling RR or FCFS functions according to its argument.
- FCFS function firstly checks for the arrived processes that match the current cycle and put them in an array called racing.
- It also checks for the blocked processes that are now ready and put them in the same array.
- Next it sorts the processes in that array according to their ID and en-queues them in the ready queue.
- It then checks for the current working ID if it didn’t finish its CPU time yet it leaves everything as it existed except it decreases its CPU time and decreases the blocked processes’ IO time.
- If the current working process is blocked or finished its time the function de-queues the process in top of the queue.
- The FCFS function calls write function each cycle to print a line containing the status of each process in this cycle.
- The RR function is the same structure as FCFS function except that it checks for the quantum time too to decide if the current working process is going to proceed or not. 


## Main functions:
- Queue Functions (enQ, deQ, isEmpty)

These functions are created to abstract the queue functions using the simple array shift method.

- Sort function

This function sorts the processes which arrive in the same time along with processes returned from blocking state according to their ID. 

- readFile

This function takes a string with file name and fills a multidimensional array with the status of each process.

- write

This function writes on a file the status of each process each cycle. It should be called each cycle to print a line.

- FCFS

A function that simulates the first come first served scheduling algorithm.

- RR

A function that simulates the round robin scheduling algorithm.

##How to run:
You only have to change your directory to the directory of the deliverables, put your files in it and type make to compile.

- To run the simulation using FCFS just type

./osSchedulingSimulator 0 [inputFileName] 

- To run the simulation using RR just type

./osSchedulingSimulator 1 [quantumTime] [inputFileName] 

