﻿// MPITest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <stdio.h> 
#include <stdlib.h> 
#include <mpi.h>
#include <time.h>
#define  MASTER 0
#define NRTASKS 4
#define SIZE 40


using namespace std;

int getSum(int lower, int upper,int rank);

int numbers[SIZE];

void main(int argc, char *argv[])
{
	int size, myrank, mysum = 0, finalSum = 0, sumToCompareTo = 0;
	int source; 

	int chunksize = SIZE / NRTASKS;

	int tag = 1;

	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int lower = myrank* chunksize;
	int upper = lower + chunksize;

	/*         The Master initializes the values of the array and then works with the results of the other processes */
	if (myrank == MASTER) 
	{
		srand(time(NULL));
		printf("The master process is initializing the numbers!\n");
		for (int index = 0; index < SIZE; ++index)
		{
			numbers[index] = rand()%100+1;
			sumToCompareTo += numbers[index];
			printf("Number %d to be initialized is % d!\n",index,numbers[index]);
		}
		
		int offset = lower;

		for (int processIndex = 1; processIndex < NRTASKS; ++processIndex) 
		{
			lower += chunksize;
			MPI_Send(&numbers[lower], chunksize, MPI_INT, processIndex, tag, MPI_COMM_WORLD);
			printf("Master sent elements to process %d\n", processIndex);
		}

		mysum = getSum(offset, upper,myrank);

		for (int processIndex = 1; processIndex < NRTASKS; ++processIndex) 
		{
			source = processIndex;
			MPI_Recv(&numbers[lower], chunksize, MPI_INT, source,tag, MPI_COMM_WORLD, &status);
		}

		MPI_Reduce(&mysum, &finalSum, 1, MPI_INT, MPI_SUM, MASTER, MPI_COMM_WORLD);

		printf("Correct sum is %d \n", sumToCompareTo);
		printf("Gotten sum is %d \n", finalSum);

	}
	/*       The other processes that work with the data*/
	else {
		source = MASTER;
		MPI_Recv(&numbers[lower],chunksize,MPI_INT,source, tag,MPI_COMM_WORLD,&status);

		mysum = getSum(lower, upper, myrank);

		MPI_Reduce(&mysum, &finalSum, 1, MPI_INT, MPI_SUM, MASTER, MPI_COMM_WORLD);

		MPI_Send(&numbers[lower], chunksize, MPI_INT, MASTER, tag, MPI_COMM_WORLD);
	}


	MPI_Finalize();
}

int getSum(int lower, int upper,int rank) 
{
	int mysum = 0;

	for (int index = lower; index < upper; ++index)
	{
		mysum += numbers[index];
		printf("Index is %d and value is %d\n", index, numbers[index]);
	}

	printf("Sum gotten by the process with rank %d is %d\n", rank, mysum);

	return mysum;
}

