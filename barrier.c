//  Created by James Schermer on 4/23/19.
//
/*///
 /Syntax For Refrence.....
 /
 /
 /MPI_Send(
 void* data,
 int count,
 MPI_Datatype datatype,
 int destination,
 int tag,
 MPI_Comm communicator)
 /
 /
 /
 /MPI_Recv(
 void* data,
 int count,
 MPI_Datatype datatype,
 int source,
 int tag,
 MPI_Comm communicator,
 MPI_Status* status)
 /
 /
 /
 /
 /
 */
#include <stdio.h>
#include <mpi.h>
#include <string.h>
#include <crypt.h>
#include <sys/time.h>
#include <stdlib.h>

/*-------------PROTOTYPES--------------------*/
void barrier_R(int world_size, int rank);
void barrier_T(int world_size, int rank);

double one[32768];



void ring(int my_size, int rank){
    
    MPI_Request send_req, recv_req;
    
    if(rank == 0){
        //Ring: SEND -- my_rank + 1(neighbor); RECV -- from last processor (my_size - 1)
        MPI_Isend(&one, 1, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD, &send_request);
     
        MPI_Irecv(&one, 1, MPI_DOUBLE, world_size - 1,1,MPI_COMM_WORLD, &recv_request );
 
        
        MPI_Wait(&send_request, MPI_STATUS_IGNORE);
    
        MPI_Wait(&recv_request, MPI_STATUS_IGNORE);
        //But we also have to send data from 0 to continue to its neighbor /because ring
        MPI_Isend(&one, 1, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD, &send_request);
        MPI_Wait(&send_request, MPI_STATUS_IGNORE);
    }
    
    if(rank != 0){
        //if not 0 we must recieve and then send
        MPI_Irecv(&one, 1, MPI_DOUBLE, (rank - 1), 1, MPI_COMM_WORLD, &recv_request);
        MPI_Wait(&recv_request, MPI_STATUS_IGNORE);
        
        
        MPI_Isend(&one, 1, MPI_DOUBLE, (rank + 1) % world_size, 1, MPI_COMM_WORLD, &send_request);
        
        
        MPI_Irecv(&one, 1, MPI_DOUBLE, (rank-1), 1, MPI_COMM_WORLD, &recv_request);
        MPI_Wait(&recv_request, MPI_STATUS_IGNORE);
        
        MPI_Isend(&one, 1, MPI_DOUBLE, (rank  +1) % world_size, 1, MPI_COMM_WORLD, &send_request);
    }
        
}

void tree(int world_size, int rank){
    
    MPI_Request left_recv, right_recv, left_send, right_send, parent_send, parent_recv;
    
    if(rank == 0){
        //Process zero waits to recive from its children, if they exist
        if(2*rank + 1 < world_size)
            MPI_Irecv(&one, 1, MPI_DOUBLE, 2*rank+1, 1, MPI_COMM_WORLD, &left_recv);
        if(2*rank + 2 < world_size)
            MPI_Irecv(&one, 1, MPI_DOUBLE, 2*rank+2, 1, MPI_COMM_WORLD, &right_recv);
        
        if(2*rank + 1 < world_size){
            MPI_Wait(&left_recv, MPI_STATUS_IGNORE);
            
        }
        if(2*rank + 2 < world_size){
            MPI_Wait(&right_recv, MPI_STATUS_IGNORE);
           
        }
        
        //Process zero sends to it's children the continue call, if they exist
        if(2*rank + 1 < world_size)
            MPI_Isend(&one, 1, MPI_DOUBLE, (2*rank + 1), 1, MPI_COMM_WORLD, &left_send);
        if(2*rank + 2 < world_size)
            MPI_Isend(&one, 1, MPI_DOUBLE, (2*rank + 2), 1, MPI_COMM_WORLD, &right_send);
        
        
    }
    
    if(rank != 0){
        //Process waits to recive from its children, if they exist
        if(2*rank +  1 < world_size)
            MPI_Irecv(&one, 1, MPI_DOUBLE, 2*rank+1, 1, MPI_COMM_WORLD, &left_recv);
        
        if(2*rank + 2 < world_size)
            MPI_Irecv(&one, 1, MPI_DOUBLE, 2*rank+2, 1, MPI_COMM_WORLD, &right_recv);
        
        if(2*rank + 1 < world_size){
            MPI_Wait(&left_recv, MPI_STATUS_IGNORE);
        }
        
        if(2*rank + 2 < world_size){
            MPI_Wait(&right_recv, MPI_STATUS_IGNORE);
        }
        
        //Next the processes, having recieved from their children send that data to their parent
        if(rank % 2 == 0 && (rank / 2) -1 >= 0){//In this case we subtract one from rank/2 so that we send to our parent node
            MPI_Isend(&one, 1, MPI_DOUBLE, (rank/2)-1,1,MPI_COMM_WORLD, &parent_send);
        }
        if(rank % 2 != 0 && (rank / 2) >= 0){
            MPI_Isend(&one, 1, MPI_DOUBLE, rank/2,1, MPI_COMM_WORLD, &parent_send);
            //printf("Processor %d: attempting send to parent processor %d\n", rank, rank/2);
        }
        
        // The processes wait to recieve  the continue from their parent
        if(rank % 2 == 0 && (rank /2) -1 >= 0){
            MPI_Irecv(&one, 1, MPI_DOUBLE, (rank/2)-1, 1, MPI_COMM_WORLD, &parent_recv);
        }
        
        if(rank % 2 != 0 && (rank /2) >= 0){
            MPI_Irecv(&one, 1, MPI_DOUBLE, (rank/2),1, MPI_COMM_WORLD, &parent_recv);
        }
        
        if(rank % 2 == 0 && (rank /2)-1 >= 0){
            MPI_Wait(&parent_recv, MPI_STATUS_IGNORE);
        }
        
        if(rank % 2 != 0 && rank /2 >= 0){
            MPI_Wait(&parent_recv, MPI_STATUS_IGNORE);
        }
        
        //Having recieved the continue the processes send their data to their child, if it exists, and leaves
        
        if(2*rank + 1 < world_size){
            MPI_Isend(&one, 1, MPI_DOUBLE, 2*rank+1, 1, MPI_COMM_WORLD, &left_send);
          
        }
        if(2*rank + 2 < world_size){
            MPI_Isend(&one, 1, MPI_DOUBLE, 2*rank+2, 1, MPI_COMM_WORLD, &right_send);
            
        }
    }
    
}

/*---------------MAIN------------------------------*/
int main(int argc, char **argv){
    char proc_name[MPI_MAX_PROCESSOSR_NAME];
    int my_size;
    int my_rank;
    int name_len;
    double one[32768];
    
    int i;
    double start, end;
    
    //Initialize envi
    MPI_Init(NULL,NULL);
    
    MPI_Status stat;
    
    //Num procs
    MPI_Comm_size(MPI_COMM_WORLD, &my_size);
    //rank of proc
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    
    //Get procs name
    MPI_Get_processor_name(proc_name, &name_len);
    
    if(my_rank == 0)
        start = MPI_Wtime();
    
    for(i = 0; i <1000; i++){
        ring(my_size, my_rank);
    }
    
    if(my_rank == 0){
        end = MPI_Wtime();
        printf("Time..............for ring barrier: %fms\n", ((end-start)/1000)*1000);fflush(stdout);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    
    if(my_rank == 0)
        start = MPI_Wtime();
    
    for(int i = 0; i < 1000; i++){
        tree(my_size,my_rank);
        
    }
    
    if(my_rank == 0){
        end = MPI_Wtime();
        printf("Time..............for tree barrier: %fms\n", ((end-start)/1000)*1000);fflush(stdout);
    }
    
    MPI_Finalize();
    
    return 0;
}

