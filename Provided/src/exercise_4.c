/**
 * @file exercise_4.c
 * @author Ludovic Capelli (l.capelli@epcc.ed.ac.uk)
 * @brief This application contains a simple 2-process configuration where one
 * MPI process sends an array of struct to the other one. However, the recipient
 * does not receive it as an array of structs, but as a struct of arrays.
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
#include "../include/exercise_4.h"

#define AOS_MEMBER_COUNT 3

void EPCC_create_MPI_datatypes(void)
{
    // First, we describe our structure, similarly to exercise 3.
    struct AoS s1;
    int array_of_block_lengths[AOS_MEMBER_COUNT] = {1, 1, 1};
    MPI_Aint array_of_displacements[AOS_MEMBER_COUNT];
    MPI_Aint start_address;
    MPI_Get_address(&s1, &start_address);
    MPI_Aint offset_address;
    MPI_Get_address(&s1.d1, &offset_address);
    array_of_displacements[0] = MPI_Aint_diff(offset_address, start_address);
    MPI_Get_address(&s1.i, &offset_address);
    array_of_displacements[1] = MPI_Aint_diff(offset_address, start_address);
    MPI_Get_address(&s1.d2, &offset_address);
    array_of_displacements[2] = MPI_Aint_diff(offset_address, start_address);
    MPI_Datatype array_of_types[AOS_MEMBER_COUNT] = {MPI_DOUBLE, MPI_INT, MPI_DOUBLE};
    MPI_Type_create_struct(AOS_MEMBER_COUNT, array_of_block_lengths, array_of_displacements, array_of_types, &EPCC_MPI_AOS);
    MPI_Type_commit(&EPCC_MPI_AOS);

    // When sending a structure, the data put in the buffer sent is:
    // d1, i, d2

    // When sending multiple such structures, the data in the buffer sent is:
    // d1, i, d2, d1, i, d2, d1, i, d2, etc...

    // For the structure of array, the idea is to calculate the displacement of
    // each array in the structure s2. That will give us the starting point for
    // the first round of d1, i, and d2.
    // Then, for each additional round or d1, i and d2, since we know where we
    // put the each member received in the first round, we just need to offset
    // each by the size of the type we store. The offset of d1 and d2 will move
    // by a double, and that of i by the size of an int.
    // We repeat until there are no more rounds of d1, i, d2 to read.
    // Trick, to read one full round of d1, i, and d2 in one iteration, we just
    // increment the iteration count by 3 every time, so that in each loop 
    // iteration, we actually process the equivalent of three iterations at once.
    struct SoA s2;
    const int SOA_EPCC_ARRAY_SIZE = AOS_MEMBER_COUNT * EPCC_ARRAY_SIZE;
    int soa_array_of_block_lengths[SOA_EPCC_ARRAY_SIZE];
    for(int i = 0; i < SOA_EPCC_ARRAY_SIZE; i++)
    {
        soa_array_of_block_lengths[i] = 1;
    }
    MPI_Aint soa_start_address;
    MPI_Get_address(&s2, &soa_start_address);
    MPI_Aint soa_first_member_start_address;
    MPI_Get_address(&s2.d1[0], &soa_first_member_start_address);
    MPI_Aint soa_second_member_start_address;
    MPI_Get_address(&s2.i[0], &soa_second_member_start_address);
    MPI_Aint soa_third_member_start_address;
    MPI_Get_address(&s2.d2[0], &soa_third_member_start_address);
    MPI_Aint soa_array_of_displacements[SOA_EPCC_ARRAY_SIZE];
    for(int i = 0; i < SOA_EPCC_ARRAY_SIZE; i+=3)
    {
        soa_array_of_displacements[i] = MPI_Aint_diff(soa_first_member_start_address, soa_start_address) + sizeof(double) * i / 3;
        soa_array_of_displacements[i+1] = MPI_Aint_diff(soa_second_member_start_address, soa_start_address) + sizeof(int) * i / 3;
        soa_array_of_displacements[i+2] = MPI_Aint_diff(soa_third_member_start_address, soa_start_address) + sizeof(double) * i / 3;
    }
    MPI_Datatype soa_array_of_types[SOA_EPCC_ARRAY_SIZE];
    for(int i = 0; i < SOA_EPCC_ARRAY_SIZE; i+=3)
    {
        soa_array_of_types[i] = MPI_DOUBLE;
        soa_array_of_types[i+1] = MPI_INT;
        soa_array_of_types[i+2] = MPI_DOUBLE;
    }

    MPI_Type_create_struct(SOA_EPCC_ARRAY_SIZE, soa_array_of_block_lengths, soa_array_of_displacements, soa_array_of_types, &EPCC_MPI_SOA);
    MPI_Type_commit(&EPCC_MPI_SOA);
}
/*
void EPCC_create_MPI_datatypes(void)
{
    int blocklengthsA[AOS_MEMBER_COUNT] = {1, 1, 1};
    MPI_Aint displacementsA[AOS_MEMBER_COUNT] = {0, sizeof(double), sizeof(double) + sizeof(int)};
    MPI_Datatype typesA[AOS_MEMBER_COUNT] = {MPI_DOUBLE, MPI_INT, MPI_DOUBLE};

    MPI_Type_create_struct(AOS_MEMBER_COUNT, blocklengthsA, displacementsA, typesA, &EPCC_MPI_AOS);
    MPI_Type_commit(&EPCC_MPI_AOS);

    int blocklengthsS[AOS_MEMBER_COUNT] = {EPCC_ARRAY_SIZE, EPCC_ARRAY_SIZE, EPCC_ARRAY_SIZE};
    MPI_Aint displacementsS[AOS_MEMBER_COUNT] = {0, EPCC_ARRAY_SIZE * sizeof(double), EPCC_ARRAY_SIZE * (sizeof(double) + sizeof(double))};
    MPI_Datatype typesS[AOS_MEMBER_COUNT] = {MPI_DOUBLE, MPI_INT, MPI_DOUBLE};

    MPI_Type_create_struct(AOS_MEMBER_COUNT, blocklengthsS, displacementsS, typesS, &EPCC_MPI_SOA);
    MPI_Type_commit(&EPCC_MPI_SOA);

    //EPCC_printf("You need to implement this function.\n");
    //MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
}
*/
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
        EPCC_create_MPI_datatypes();
        int my_rank;
        MPI_Comm_rank(EPCC_DEFAULT_COMMUNICATOR, &my_rank);
        switch(my_rank)
        {
            case SENDER:
            {
                struct AoS my_aos_variable[EPCC_ARRAY_SIZE];
                for(int i = 0; i < EPCC_ARRAY_SIZE; i++)
                {
                    my_aos_variable[i].d1 = (double)(i * 4);
                    my_aos_variable[i].i = i * 15;
                    my_aos_variable[i].d2 = (double)(i * 2 + 100);
                }
                EPCC_printf("To send:\n");
                for(int i = 0; i < EPCC_ARRAY_SIZE; i++)
                {
                    EPCC_printf("  - aos[%d] = %.1f, aos[%d].i = %d, aos[%d].d2 = %.1f\n", i, my_aos_variable[i].d1, i, my_aos_variable[i].i, i, my_aos_variable[i].d2);
                }
                MPI_Send(my_aos_variable, EPCC_ARRAY_SIZE, EPCC_MPI_AOS, RECEIVER, 0, EPCC_DEFAULT_COMMUNICATOR);
                break;
            }
            case RECEIVER:
            {
                struct SoA my_soa_variable;
                //struct AoS my_soa_variable[EPCC_ARRAY_SIZE];
                MPI_Recv(&my_soa_variable, 1, EPCC_MPI_SOA, SENDER, MPI_ANY_TAG, EPCC_DEFAULT_COMMUNICATOR, MPI_STATUS_IGNORE);
                //MPI_Recv(my_soa_variable, EPCC_ARRAY_SIZE, EPCC_MPI_AOS, SENDER, MPI_ANY_TAG, EPCC_DEFAULT_COMMUNICATOR, MPI_STATUS_IGNORE);
                EPCC_printf("Received:\n");
                
                for(int i = 0; i < EPCC_ARRAY_SIZE; i++)
                {
                    EPCC_printf("  - soa.d1[%d] = %.1f, soa.i[%d] = %d, soa.d2[%d] = %.1f\n", i, my_soa_variable.d1[i], i, my_soa_variable.i[i], i, my_soa_variable.d2[i]);
                }/*
               for (int i = 0; i < EPCC_ARRAY_SIZE; i++)
                {
                    EPCC_printf("  - aos[%d] = %.1f, aos[%d].i = %d, aos[%d].d2 = %.1f\n", i, my_soa_variable[i].d1, i, my_soa_variable[i].i, i, my_soa_variable[i].d2);
                }*/
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
