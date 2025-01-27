/**
 * @file exercise_3.c
 * @author Ludovic Capelli (l.capelli@epcc.ed.ac.uk)
 * @brief This application contains a simple 2-process configuration where one
 * MPI process sends a struct to the other one.
 * @param argc The number of arguments received.
 * @param argv The values of the arguments received, with argv[0] being the
 * string containing the invocation command you typed to execute the program
 * (e.g.: mpirun -n 2 ./bin/main).
 * @return The error code indicating the exit status of the program. It is
 * common understanding that 0 represents a successful execution, while negative
 * values represent erroneous executions.
 * @pre The application is run using 2 MPI processes.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "../include/exercise_3.h"

#define EPCC_MEMBER_COUNT 3

void EPCC_create_MPI_datatype(void)
{
    EPCC_printf("You need to implement this function.\n");
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
    // The number of MPI processes that this application must spawn.
    const int COMM_SIZE_EXPECTED = 2;

    MPI_Init(&argc, &argv);

    int comm_size;
    MPI_Comm_size(EPCC_DEFAULT_COMMUNICATOR, &comm_size);
    if(comm_size != COMM_SIZE_EXPECTED)
    {
        EPCC_printf("This application is meant to be run with %d MPI processes, not %d.\n", COMM_SIZE_EXPECTED, comm_size);
    }
    else
    {
        struct EPCC_struct s;
        EPCC_create_MPI_datatype();
        int my_rank;
        MPI_Comm_rank(EPCC_DEFAULT_COMMUNICATOR, &my_rank);
        switch(my_rank)
        {
            case SENDER:
            {
                s.i = 232;
                s.c = 'c';
                s.d = 12345.6;
                EPCC_printf("To send:\n");
                EPCC_printf("  - s.i = %d\n", s.i);
                EPCC_printf("  - s.c = \'%c\'\n", s.c);
                EPCC_printf("  - s.d = %f\n", s.d); 
                MPI_Send(&s, 1, EPCC_MPI_STRUCT, RECEIVER, 0, EPCC_DEFAULT_COMMUNICATOR);
                break;
            }
            case RECEIVER:
            {
                MPI_Recv(&s, 1, EPCC_MPI_STRUCT, SENDER, MPI_ANY_TAG, EPCC_DEFAULT_COMMUNICATOR, MPI_STATUS_IGNORE);
                EPCC_printf("Received:\n");
                EPCC_printf("  - s.i = %d\n", s.i);
                EPCC_printf("  - s.c = \'%c\'\n", s.c);
                EPCC_printf("  - s.d = %f\n", s.d); 
                break;
            }
            default:
            {
                EPCC_printf("This line is not supposed to be reached. This program contains a bug.");
                MPI_Abort(EPCC_DEFAULT_COMMUNICATOR, EXIT_FAILURE);
                break;
            }
        }
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}
