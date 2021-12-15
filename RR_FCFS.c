#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#define INACTIVE 0 // a flag in the array of i/p which indecates if the process is inactive or not
#define READY 1 // a flag in the array of i/p which indecates if the process is in Ready queue or not
#define RUNNING 2 // a flag in the array of i/p which indecates if the process is Runnung on processor or not
#define BLOCKED 3 // a flag in the array of i/p which indecates if the process is Blocked or not
#define ID 0 // the ID of process
#define CPU 1 //the 1st cpu time
#define IO 2 // the IO time 
#define ARRIVAL 3 // the arrival time 
#define STATE 4 // indicates the state of process Inactive or Ready or blocked or running 
#define TERMINATION 5 // a flag in the array of i/p which indecates if the process is terminated or not
#define CPU_2ND 6 // the time of 2nd running after IO blocking time 
#define ID_ORDER 7 // the order of process id in the file 


/////////////////////////////////////////Queue Functions//////////////////////////////////////////////////

int Q[100]; //inetiate a queue with a 100 element 
int size;

void enQ(int data){  //push data in the queue
Q[size]=data;
size++;
}

int deQ(){ // pull the data from the queue 
int i;
int data=Q[0]; 
if (size==0)
	return 0;
for (i=0; i<(size-1); i++)
	Q[i]=Q[i+1];
size--;
return data;
}

int front(){
return Q[0];
}

int isEmpty(){
if(size==0) //if the queue is empty return 1 else return 0 
	return 1;
else
	return 0;
}


/////////////////////////////////////////Sort Function//////////////////////////////////////////////////

void sort(int arr[100][2] , int incomings) //sort function takes the array of processes and the number of proccesses 
{
int i,j,tmp; 

     for(i=0;i<incomings-1;i++)
     {
           for(j=i;j<incomings-1;j++)
           {
                 if(arr[j][1]>arr[j+1][1])
                 {
                       tmp=arr[j][1];
                       arr[j][1]=arr[j+1][1];
                       arr[j+1][1]=tmp;
                 }
           }
     }

}


/////////////////////////////////////////Read Function//////////////////////////////////////////////////

int readFile(int status[100][8], char* inputFileName){ // reads the input file 
    int i , j;
    int tmp ;
    FILE *in_file= fopen(inputFileName, "r") ; //open the file
    if(!in_file) //checking if the file opening has a problem 
        {
        printf("something wrong with reading file") ;
        return 1;
        }
        for (i=0;feof(in_file)==0;i++){ //save file contents in array
            for (j=0;j<4;j++){
                fscanf(in_file ,"%d", &tmp ) ;
                if (feof(in_file))
                    break ;
                status[i][j] = tmp ;
            }
	    status[i][ID_ORDER] = i;
        }
    fclose(in_file); //close the file 
    return(i-1) ; //return the no. of processes 
}


/////////////////////////////////////////Write Function//////////////////////////////////////////////////

void write(int limit, int cycle,int status[100][8],FILE *ptr_file) //write func where status is the array and cycle is the current cycle and limit is the no of processes 
{
char *states[] = {"Inactive", "ready","running", "blocked"};
int i;

fprintf(ptr_file,"%d",cycle);
for (i=0; i<limit; i++)
	if(status[i][4]!=0)
		fprintf(ptr_file," %d:%s",status[i][0],states[status[i][4]]);
fprintf(ptr_file,"\n");
}



/////////////////////////////////////////First Come First Served//////////////////////////////////////////////////


void FCFS(int status[100][8], int limit, FILE *out_ptr){
int cycle=0; //counter indicates the clock cycles 
int i,j;
int empty=0; //the total CPU time of the process
int CPU_total=0;
int blockingCycles=0;// the no of IDLE cycles in the processor itself  
int currentOrderedID=0;
int racing[100][2];// an array which includes the process which in race condition (all wants to enter the queue in the same cycle ) 
int quitFlag=0;
int systemBlocked=0;
float CPU_Util;
while(!quitFlag){
	systemBlocked=0;
	j=0;
	for(i=0; i<limit; i++){ //checking if the process is come at the same cycle  to put them in the racing array 
		if(status[i][ARRIVAL] == cycle){
			racing[j][0]=i;
			racing[j][1]=status[i][ID];
			j++;
		}
	}

	for(i=0; i<limit; i++){
		if((status[i][STATE] == BLOCKED) && (status[i][IO] == 0)){ // enter the orderd id of the process to racing array 
			racing[j][0]=i;
			racing[j][1]=status[i][ID];
			j++;
		}
	}

	sort(racing ,j);
	for (;j>0;j--){ //sort racing array due to their ids 
		enQ(racing[j-1][0]);
		status[racing[j-1][0]][STATE] = READY;
	}


	//check for current working process
	if(status[currentOrderedID][STATE] == RUNNING){
		if(status[currentOrderedID][CPU] == 0){ //if the cpu time ==0 
			if(status[currentOrderedID][IO] != 0){ // and the io isn't zero 
				status[currentOrderedID][STATE] = BLOCKED; //so make the proc blocked 
				if(empty=(isEmpty() ==1)){ // if the q is empty 
						blockingCycles++; // increase the # of idle cycles 
						systemBlocked=1;// and flag that these cycle is idle cycle 
				}
				else{
					currentOrderedID=deQ(); //if not deq the ready q 
					status[currentOrderedID][STATE] = RUNNING; // and make the proc run 
				}
			}
			else if(status[currentOrderedID][CPU_2ND] == 0){ // if the 2nd cpu time =0 
					status[currentOrderedID][STATE] = INACTIVE; // so proc is inactive 
					status[currentOrderedID][TERMINATION] = cycle; // and save the termination time 
					if(empty=(isEmpty() ==1)){ //if the q is empty 
						blockingCycles++; // increas idle cycle 
						systemBlocked=1;// and flag that sys is blocked 
					}
					else{
						currentOrderedID=deQ(); //else dq  proc from ready q 
						status[currentOrderedID][STATE] = RUNNING; // and make it run 
					}
				}
		}

	}
	else
		if(empty=(isEmpty() ==1)){ // if the q is empty 
			blockingCycles++; // increase # of idles 
			systemBlocked=1; // flag that idle cycle 
		}
		else{
			currentOrderedID=deQ(); // else dq 
			status[currentOrderedID][STATE] = RUNNING; // make it runs 
		}

	//call writing func
	write(limit, cycle,status, out_ptr);

	if (!systemBlocked) //if system isnot blocked 
		if(status[currentOrderedID][CPU] == 0){ //if cpu1 ==0 so we are in the 2nd exec 
			status[currentOrderedID][CPU_2ND]--;//dec the 2nd exec 
		}
		else{
			status[currentOrderedID][CPU]--; //else dec the 1st cpu 
		}

	for(i=0; i<limit; i++) 
		if(status[i][STATE] == BLOCKED)
			status[i][IO]--; // check if the state is blocked dec the io time of proc 

	cycle++; // increase the no of cycles 



	CPU_total=0; 
	for(i=0; i<limit; i++){ // the tot cpu time is sum betn 1st and 2nd cpu time 
		CPU_total=CPU_total + status[i][CPU_2ND];
		if (CPU_total != 0) 
			break;
	}
	if (CPU_total == 0){ // if the tot summ of process cpu time ==0 
		quitFlag=1; //so quit the loop 
		status[currentOrderedID][TERMINATION]=cycle; //and save the termination time of process
	}
}

fprintf(out_ptr,"\nFinishing time: %d",cycle-1); //print the total no of cycles 
CPU_Util=1.0*(cycle-blockingCycles)/(cycle); 
fprintf(out_ptr,"\nCPU utilization: %.2f",CPU_Util); //print cpu utilization 
for(i=0; i<limit; i++)
	fprintf(out_ptr,"\nTurnaround process %d: %d",status[i][ID],(status[i][TERMINATION]-status[i][ARRIVAL])); //print the round time for each process 


}


/////////////////////////////////////////Round Robin//////////////////////////////////////////////////
// round robin algorith is the same of FCFS but with quantum time so difference only will be commented
void RR(int status[100][8], int limit,int quantumTime, FILE *out_ptr){
int cycle=0;
int i,j;
int empty=0;
int CPU_total=0;
int blockingCycles=0;
int currentOrderedID=0;
int racing[100][2];// an array which includes the process which in race condition (all wants to enter the queue in the same cycle ) 
int quitFlag=0;
int systemBlocked;
float CPU_Util;
int currentIDQuantum=quantumTime;
while(!quitFlag){
	j=0;
	systemBlocked=0;
	for(i=0; i<limit; i++){
		if(status[i][ARRIVAL] == cycle){
			racing[j][0]=i;
			racing[j][1]=status[i][ID];
			j++;
		}
	}

	for(i=0; i<limit; i++){
		if((status[i][STATE] == BLOCKED) && (status[i][IO] == 0)){
			racing[j][0]=i;
			racing[j][1]=status[i][ID];
			j++;
		}
	}

	sort(racing ,j);
	for (;j>0;j--){
		enQ(racing[j-1][0]);
		status[racing[j-1][0]][STATE] = READY;
	}

	//check for current working process
	if(status[currentOrderedID][STATE] == RUNNING){
		if(status[currentOrderedID][CPU] == 0){
			if(status[currentOrderedID][IO] != 0){
				status[currentOrderedID][STATE] = BLOCKED;
				if(empty=(isEmpty() ==1)){
						blockingCycles++;
						systemBlocked=1;
				}
				else{
					currentOrderedID=deQ();
					currentIDQuantum=quantumTime;
					status[currentOrderedID][STATE] = RUNNING;
				}
			}
			else if(status[currentOrderedID][CPU_2ND] == 0){
					status[currentOrderedID][STATE] = INACTIVE;
					status[currentOrderedID][TERMINATION] = cycle;
					if(empty=(isEmpty() ==1)){
						blockingCycles++;
						systemBlocked=1;
					}
					else{
						currentOrderedID=deQ();
						currentIDQuantum=quantumTime;
						status[currentOrderedID][STATE] = RUNNING;
					}
				}
				else if(currentIDQuantum==0){ // if the quantum ==0 
                    status[currentOrderedID][STATE] = READY; // make the process in ready state 
                    enQ(status[currentOrderedID][ID_ORDER]); // and put it into reaady q 
                    currentOrderedID=deQ(); // and pull another proc from ready q 
                    currentIDQuantum=quantumTime; // and reset the quantum time 
                    status[currentOrderedID][STATE] = RUNNING;// and make it in run mode
				}
		}
        else if(currentIDQuantum==0){ // if the quantum ==0 
                status[currentOrderedID][STATE] = READY;// make the proc in ready state 
                enQ(status[currentOrderedID][ID_ORDER]); // put it into q 
                currentOrderedID=deQ(); // pull another one from the q 
                currentIDQuantum=quantumTime;// reset the quantum time 
                status[currentOrderedID][STATE] = RUNNING; // andm ake the proc run state 
        }
	}
	else
		if(empty=(isEmpty() ==1)){ 
			blockingCycles++;
			systemBlocked=1;
		}
		else{
			currentOrderedID=deQ();
			currentIDQuantum=quantumTime;
			status[currentOrderedID][STATE] = RUNNING;
			//currentIDQuantum--;
		}


    
	//call writing func
	write(limit, cycle,status, out_ptr);

	if (!systemBlocked){ // if the cycle is idle  state 
		currentIDQuantum--; // minus the quantum time 
		if(status[currentOrderedID][CPU] == 0){ 
			status[currentOrderedID][CPU_2ND]--;
		}
		else{
			status[currentOrderedID][CPU]--;
		}
    }
	for(i=0; i<limit; i++)
		if(status[i][STATE] == BLOCKED)
			status[i][IO]--;

	cycle++;

	CPU_total=0;
	for(i=0; i<limit; i++){
		CPU_total=CPU_total + status[i][CPU_2ND];
		if (CPU_total != 0)
			break;
	}
	if (CPU_total == 0){
		quitFlag=1;
		status[currentOrderedID][TERMINATION]=cycle;
	}
}

fprintf(out_ptr,"\nFinishing time: %d",cycle-1);
CPU_Util=1.0*(cycle-blockingCycles)/(cycle);
fprintf(out_ptr,"\nCPU utilization: %.2f",CPU_Util);
for(i=0; i<limit; i++)
	fprintf(out_ptr,"\nTurnaround process %d: %d",status[i][ID],(status[i][TERMINATION]-status[i][ARRIVAL]));


}


/////////////////////////////////////////Main Function//////////////////////////////////////////////////


int main (int argc, char *argv[])
{
int schedulAlg, quantumTime;
char* inputFileName;
int status[100][8]={0};
int processNo,i;
FILE *out_file;


//reading arguments from main

schedulAlg=atoi(argv[1]);
if(schedulAlg==0){ //check the arguments taken by the users 
	if(argc!=3){
		printf("Error in arguements/n");
		return 1;
	}
	else
		inputFileName=argv[2];
}
else{
	if(argc!=4){
		printf("Error in arguements\n");
		return 1;
	}
	else{
		quantumTime=atoi(argv[2]);
		inputFileName=argv[3];
	}
}

//call reading function
processNo = readFile(status, inputFileName) ;

for(i=0;  i<processNo; i++)
	status[i][CPU_2ND]=status[i][CPU];


//call algorithms

if (schedulAlg==0){
//open file for output
strcat(inputFileName,"FCFS.out");
out_file =fopen(inputFileName, "w");
FCFS(status,processNo, out_file);
}
else
{
//open file for output
strcat(inputFileName,"RR.out");
out_file =fopen(inputFileName, "w");
RR(status,processNo,quantumTime, out_file);
}



//close output file
fclose(out_file);

return 0;

}













