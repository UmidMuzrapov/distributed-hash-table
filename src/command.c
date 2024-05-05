#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "dht.h"

void command_node(){
  int i, dummy;
  int keyval[2];
  int addArgs[2];
  int answer[2];
  int key;

  for (i = 1; i <= 100; i++) {
    keyval[0] = i;
    keyval[1] = i+100;
    MPI_Send(keyval, 2, MPI_INT, 0, PUT, MPI_COMM_WORLD);
    MPI_Recv(&dummy, 1, MPI_INT, 0, ACK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  addArgs[0] = 1;
  addArgs[1] = 10;
  MPI_Send(addArgs, 2, MPI_INT, 0, ADD, MPI_COMM_WORLD);
  MPI_Recv(&dummy, 1, MPI_INT, 0, ACK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  addArgs[0] = 2;
  addArgs[1] = 20;
  MPI_Send(addArgs, 2, MPI_INT, 0, ADD, MPI_COMM_WORLD);
  MPI_Recv(&dummy, 1, MPI_INT, 0, ACK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  addArgs[0] = 3;
  addArgs[1] = 30;
  MPI_Send(addArgs, 2, MPI_INT, 0, ADD, MPI_COMM_WORLD);
  MPI_Recv(&dummy, 1, MPI_INT, 0, ACK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  for (i = 1; i <= 100; i++) {
    key = i;
    MPI_Send(&key, 1, MPI_INT, 0, GET, MPI_COMM_WORLD);
    MPI_Recv(answer, 2, MPI_INT, 0, RETVAL, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("val is %d, storage id is %d\n", answer[0], answer[1]);
  }

  addArgs[0] = 20;
  MPI_Send(addArgs, 1, MPI_INT, 0, REMOVE, MPI_COMM_WORLD);
  MPI_Recv(&dummy, 1, MPI_INT, 0, ACK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  for (i = 1; i <= 100; i++) {
    key = i;
    MPI_Send(&key, 1, MPI_INT, 0, GET, MPI_COMM_WORLD);
    MPI_Recv(answer, 2, MPI_INT, 0, RETVAL, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("val is %d, storage id is %d\n", answer[0], answer[1]);
  }

  addArgs[0] = 10;
  MPI_Send(addArgs, 1, MPI_INT, 0, REMOVE, MPI_COMM_WORLD);
  MPI_Recv(&dummy, 1, MPI_INT, 0, ACK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  for (i = 1; i <= 100; i++) {
    key = i;
    MPI_Send(&key, 1, MPI_INT, 0, GET, MPI_COMM_WORLD);
    MPI_Recv(answer, 2, MPI_INT, 0, RETVAL, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("val is %d, storage id is %d\n", answer[0], answer[1]);
  }

  addArgs[0] = 30;
  MPI_Send(addArgs, 1, MPI_INT, 0, REMOVE, MPI_COMM_WORLD);
  MPI_Recv(&dummy, 1, MPI_INT, 0, ACK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  for (i = 1; i <= 100; i++) {
    key = i;
    MPI_Send(&key, 1, MPI_INT, 0, GET, MPI_COMM_WORLD);
    MPI_Recv(answer, 2, MPI_INT, 0, RETVAL, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("val is %d, storage id is %d\n", answer[0], answer[1]);
  }

  MPI_Send(&dummy, 1, MPI_INT, 0, END, MPI_COMM_WORLD);
  printf("command finalizing\n");
  MPI_Finalize();
  exit(0);
}

