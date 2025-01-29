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
    // Use a dummy struct instance, so that we can calculate the displacement
    // of its members.
    struct EPCC_struct s;

    // Each block will contain one element: the corresponding member.
    int array_of_block_lengths[EPCC_MEMBER_COUNT] = {1, 1, 1};

    // [Arbitrary addresses taken for illustration]
    // If our dummy struct s is located at address 0x50, its members i may be
    // located at address 0x50, or 0x54, or 0x58 etc... So we fetch the address
    // of both, then calculate the difference and know for the sure the 
    // displacement. We repeat for the other two structure members.
    MPI_Aint array_of_displacements[EPCC_MEMBER_COUNT];
    MPI_Aint start_address;
    MPI_Get_address(&s, &start_address);
    MPI_Aint offset_address;
    MPI_Get_address(&s.i, &offset_address);
    array_of_displacements[0] = MPI_Aint_diff(offset_address, start_address);
    MPI_Get_address(&s.c, &offset_address);
    array_of_displacements[1] = MPI_Aint_diff(offset_address, start_address);
    MPI_Get_address(&s.d, &offset_address);
    array_of_displacements[2] = MPI_Aint_diff(offset_address, start_address);

    // The first block contains "i", so an int, the second block contains "c" so
    // a char, and the third block contains "d" so a double.
    MPI_Datatype array_of_types[EPCC_MEMBER_COUNT] = {MPI_INT, MPI_CHAR, MPI_DOUBLE};

    // Block lengths, displacements and types calculated, we can now construct
    // the MPI derived datatype.
    MPI_Type_create_struct(EPCC_MEMBER_COUNT, array_of_block_lengths, array_of_displacements, array_of_types, &EPCC_MPI_STRUCT);

    // We commit it so that we can use it in MPI routines.
    MPI_Type_commit(&EPCC_MPI_STRUCT);
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
