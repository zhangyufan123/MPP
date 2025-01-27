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
    // ========== 1) 构造 "AoS" 类型 (Array of Structs) ==========
    {
        // 用一个“样例对象”来查询字段真实偏移
        struct AoS dummy_aos;
        MPI_Aint base, addr_d1, addr_i, addr_d2;

        MPI_Get_address(&dummy_aos,       &base);
        MPI_Get_address(&dummy_aos.d1,    &addr_d1);
        MPI_Get_address(&dummy_aos.i,     &addr_i);
        MPI_Get_address(&dummy_aos.d2,    &addr_d2);

        // 相对于 struct 起始的位移
        MPI_Aint dispA[3];
        dispA[0] = addr_d1 - base;  // 通常为0
        dispA[1] = addr_i  - base;  // 可能为8(或其它), 由编译器决定
        dispA[2] = addr_d2 - base;  // 可能为16(或其它)
        printf("dispA[0] = %ld, dispA[1] = %ld, dispA[2] = %ld\n", dispA[0], dispA[1], dispA[2]);
        int blocklengthsA[3] = {1, 1, 1};
        MPI_Datatype typesA[3] = {MPI_DOUBLE, MPI_INT, MPI_DOUBLE};

        MPI_Type_create_struct(
            3, blocklengthsA, dispA, typesA, &EPCC_MPI_AOS
        );
        MPI_Type_commit(&EPCC_MPI_AOS);
    }

    // ========== 2) 构造 "SoA" 类型 (Struct of Arrays) ==========
    {
        // 同样用一个“样例对象”查询真实偏移
        struct SoA dummy_soa;
        MPI_Aint base, addr_d1, addr_i, addr_d2;

        MPI_Get_address(&dummy_soa,          &base);
        MPI_Get_address(&dummy_soa.d1,       &addr_d1);
        MPI_Get_address(&dummy_soa.i,        &addr_i);
        MPI_Get_address(&dummy_soa.d2,       &addr_d2);

        // 计算相对于 struct SoA 起始的位移
        MPI_Aint dispS[3];
        dispS[0] = addr_d1 - base + 8;  // 数组 d1[] 的起点
        dispS[1] = addr_i  - base;  // 数组 i[]  的起点
        dispS[2] = addr_d2 - base;  // 数组 d2[] 的起点
        printf("dispS[0] = %ld, dispS[1] = %ld, dispS[2] = %ld\n", dispS[0], dispS[1], dispS[2]);
        // 每个数组里有 EPCC_ARRAY_SIZE 个元素
        int blocklengthsS[3] = {
            EPCC_ARRAY_SIZE,  // d1[] 有多少 double
            EPCC_ARRAY_SIZE,  // i[]  有多少 int
            EPCC_ARRAY_SIZE   // d2[] 有多少 double
        };

        // 对应数组中元素的类型
        MPI_Datatype typesS[3] = {
            MPI_DOUBLE, // d1[] 是 double
            MPI_INT,    // i[]  是 int
            MPI_DOUBLE  // d2[] 是 double
        };

        MPI_Type_create_struct(
            3, blocklengthsS, dispS, typesS, &EPCC_MPI_SOA
        );
        MPI_Type_commit(&EPCC_MPI_SOA);
    }
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
