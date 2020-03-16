//
//  ring.c
//  
//
//  Created by James Schermer on 4/22/19.
//

#include <stdio.h>
#include <mpi.h>
#include <string.h>
#include <crypt.h>
#include <sys/time.h>
#include <stdlib.h>

void fals(int rank);
void ring(int size, int rank, int my_size);
double one[32768];


void ring(int size, int rank, int my_size){
   
    double start, end;
    
    MPI_Request send_req, recv_req;
    
    if(rank == 0){
        start = MPI_Wtime();
        for(int i = 0; i < my_size; i++){
            
            MPI_Isend(&one, size, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD, &send_request);
            
            MPI_Irecv(&one, size, MPI_DOUBLE, my_size - 1,1,MPI_COMM_WORLD, &recv_request);
            
            MPI_Wait(&send_req, MPI_STATUS_IGNORE);
            
            MPI_Wait(&recv_req, MPI_STATUS_IGNORE);
        }
    }
    
    if(rank != 0){
        for(int i = 0; i < my_size; i++){
            
            MPI_Isend(&one, size, MPI_DOUBLE, (rank + 1) % my_size, 1, MPI_COMM_WORLD, &send_req);
            
            MPI_Irecv(&one, size, MPI_DOUBLE, (rank - 1), 1, MPI_COMM_WORLD, &recv_req);
            
            MPI_Wait(&send_req, MPI_STATUS_IGNORE);
            
            MPI_Wait(&recv_req, MPI_STATUS_IGNORE);
        }
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
    
    if(rank == 0){
        end = MPI_Wtime();
        printf("Time............ for %d processors for message of size %d:  %fms\n",my_size, size, (end-start) * 1000);
    }
}
    
int main(int argc, char **argv){
    
    proc_name[MPI_MAX_PROCESSOR_NAME];
    int my_size;
    int my_rank;
    int name_len;
    
    double start, end;
    double one[32768];
    
    MPI_Init(NULL,NULL);
    
    MPI_status stat;
    //num of procs
    MPI_Comm_size(MPI_COMM_WORLD, &my_size);
    
    //rank of procs
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    
    //Get name of proc
    MPI_Get_processor_name(proc_name, &name_len);
    
    ring(1, my_rank, my_size);
    ring(8, my_rank, my_size);
    ring(64, my_rank, my_size);
    ring(512, my_rank, my_size);
    ring(4096, my_rank, my_size);
    ring(32768, my_rank, my_size);
    
    MPI_Finalize();
    
    return 0;
}

