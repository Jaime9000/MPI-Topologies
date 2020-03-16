//
//  pingPong.c
//  
//
//  Created by James Schermer on 4/22/19.
//

#include <stdio.h>
#include <mpi.h>
#include <string.h>
#include <crypter.h>
#include <sys/time.h>
#include <stdlib.h>


/*--------Function Prototypes----------------------*/
void block(int size, int rank);
void nonblock(int size, int rank);

double one[32768];

void block(int size, int rank){
    double start, end;
    
    if(rank == 0){
        start = MPI_Wtime();
        printf("PING!!!!!!! Sending data from Processor %d to processor 1)\n", rank);fflush(stdout);
        MPI_Send(&one, size, MPI_DOUBLE, 1, 1, MPI_COMM_WORLD);
        
        MPI_Recv(&one, size, MPI_DOUBLE, 1, 1, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
    }
    
    else if(rank == 1){
        
        MPI_Recv(&one, size, MPI_DOUBLE,0,1, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        MPI_SEND(&one, size, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
        printf("PONG!!!!!!! Processor %d recieved and then sent data to Proccessor 0\n", rank);fflush(stdout);
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
    
    if(rank == 0){
        end = MPI_Wtime();
        printf("Time............... for size %d: %fms\n", size, (end-start)*1000)fflush(stdout);
    }
    
}

void nonblock(int size, int rank){
    MPI_Request send_req, recv_req;
    
    double start, end;
    
    if(rank ==0){
        start = MPI_Wtime();
        printf("PING!!!!!!! Sending data to processor 1, from processor %d\n", rank);fflush(stdout);
        MPI_Isend(&one, size, MPI_DOUBLE, 1, 1, MPI_COMM_WORLD, &send_request);
        
        MPI_Irecv(&one, size, MPI_DOUBLE, 1,1,MPI_COMM_WORLD, &recv_request );
        
        MPI_Wait(&send_request, MPI_STATUS_IGNORE);
        
        MPI_Wait(&recv_request, MPI_STATUS_IGNORE);
    }
    
    else if(rank == 1){
        MPI_Irecv(&one, size, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &recv_request);
        
        MPI_Isend(&one, size, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &send_request);
        
        MPI_Wait(&recv_request, MPI_STATUS_IGNORE);
        
        MPI_Wait(&send_request, MPI_STATUS_IGNORE);
        
        printf("PONG!!!!!!! processor %d recieved data and sent data back to processor 0\n", rank);fflush(stdout);
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
    
    if(rank == 0){
        end  = MPI_Wtime();
        printf("Time............... for size %d: %fms\n", size, (end-start)*1000)fflush(stdout);
    }
}

int main(int argc, char **argv){
    char proc_name[MPI_MAX_PROCESSOR_NAME];
    int my_size; //world_size;
    int my_rank; //rank
    int name_len;
    
    double start, end;
    double one[32768];
    
    ///Intialize envi
    MPI_Init(NULL,NULL);
    
    MPI_Status stat;
    
    ///num procs
    MPI_Comm_size(MPI_COMM_WORLD, &my_size)
    
    //rank of cur proc
    MPI_Comm_size(MPI_COMM_WORLD, &my_rank);
    
    //get current proc name
    MPI_Get_processor_name(proc_name, &namelen);
    
    if(my_rank==0)
        printf("BLOCKING!!!!!\n");fflush(stdout);
    
    block(1,my_rank);
    block(8,my_rank);
    block(512,my_rank);
    block(4096,my_rank);
    block(32768,my_rank);
    
    if(my_rank == 0)
        printf("NON_BLOCKING!!!!!\n");fflush(stdout);
    
    nonblock(1,my_rank);
    nonblock(8,my_rank);
    nonblock(64, my_rank);
    nonblock(512, my_rank);
    nonblock(4096, my_rank);
    nonblock(32768,my_rank);
    
    MPI_Finalize();
    
    return 0;
}
    
