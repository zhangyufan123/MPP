/**
 * @file exercise_1.c
 * @author Ludovic Capelli (l.capelli@epcc.ed.ac.uk)
 * @brief This application contains a 4-process scatter where the root MPI
 * process sends non-contiguous data to all MPI processes. In this instance,
 * columns of an 2-dimensional array.
 * MPI process sends a struct to the other one.
 * @param argc The number of arguments received.
 * @param argv The values of the arguments received, with argv[0] being the
 * string containing the invocation command you typed to execute the program
 * (e.g.: mpirun -n 4 ./bin/main).
 * @return The error code indicating the exit status of the program. It is
 * common understanding that 0 represents a successful execution, while negative
 * values represent erroneous executions.
 * @pre The application is run using 4 MPI processes.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "../include/exercise_1.h"


// Note: "number of rows" is equivalent to "length of a column"
#define EPCC_ROW_COUNT 5
// Note: "number of columns" is equivalent to "length of a row"
#define EPCC_COLUMN_COUNT 4

MPI_Datatype EPCC_MPI_COLUMN;
/*
void EPCC_create_MPI_datatype(int row_count, int column_count)
{
    // Define MPI_Type_vector for extracting columns
    (void)column_count;
    int displacements[5] = {0, 4, 4, 4, 4};
    MPI_Type_create_indexed_block(row_count, 1, displacements, MPI_INT, &EPCC_MPI_COLUMN);
    MPI_Type_commit(&EPCC_MPI_COLUMN);
}*/

void EPCC_create_MPI_datatype(int row_count, int column_count)
{
    // Use a temporary datatype for now, since we will have to resize it.
    MPI_Datatype temp_datatype;
    // Create a classic vector representing one column.
    MPI_Type_vector(row_count, 1, column_count, MPI_INT, &temp_datatype);
    /* Resize it such that the start of the next column is not after the end
     * of the current column, but the end of the first cell in the current
     * column. */
    MPI_Type_create_resized(temp_datatype, 0, sizeof(int), &EPCC_MPI_COLUMN);
    // Commit the new datatype such that it can be used in MPI routines.
    MPI_Type_commit(&EPCC_MPI_COLUMN);
}

int main(int argc, char* argv[])
{
    const int COMM_SIZE_EXPECTED = 4;
    const int ROOT_MPI_PROCESS_RANK = 0;

    MPI_Init(&argc, &argv);

    int comm_size;
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

    if (comm_size != COMM_SIZE_EXPECTED)
    {
        printf("This application is meant to be run with %d MPI processes, not %d.\n", COMM_SIZE_EXPECTED, comm_size);
    }
    else
    {
        int my_rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

        int column[EPCC_ROW_COUNT] = {0}; // Initialize receiving buffer
        EPCC_create_MPI_datatype(EPCC_ROW_COUNT, EPCC_COLUMN_COUNT);

        if (my_rank == ROOT_MPI_PROCESS_RANK)
        {
            int buffer[EPCC_ROW_COUNT][EPCC_COLUMN_COUNT] = {
                { 0,  1,  2,  3},
                { 4,  5,  6,  7},
                { 8,  9, 10, 11},
                {12, 13, 14, 15},
                {16, 17, 18, 19}
            };

            MPI_Scatter(buffer, 1, EPCC_MPI_COLUMN, column, EPCC_ROW_COUNT, MPI_INT, ROOT_MPI_PROCESS_RANK, MPI_COMM_WORLD);
        }
        else
        {
            MPI_Scatter(NULL, 1, EPCC_MPI_COLUMN, column, EPCC_ROW_COUNT, MPI_INT, ROOT_MPI_PROCESS_RANK, MPI_COMM_WORLD);
        }

        MPI_Barrier(MPI_COMM_WORLD); // Synchronize processes

        printf("[MPI process %d] The column I received contains elements: ", my_rank);
        for (int i = 0; i < EPCC_ROW_COUNT; i++)
        {
            printf("%d ", column[i]);
        }
        printf("\n");

        MPI_Type_free(&EPCC_MPI_COLUMN);
    }

    MPI_Finalize();
    return 0;
}
