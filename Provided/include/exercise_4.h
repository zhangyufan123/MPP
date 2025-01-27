/**
 * @file exercise_4.h
 * @author Ludovic Capelli (l.capelli@epcc.ed.ac.uk)
 **/

#ifndef EXERCISE_4_H_INCLUDED
#define EXERCISE_4_H_INCLUDED

#include "util.h"

/// The number of elements in the array of structures to send.
#define EPCC_ARRAY_SIZE 10

/**
 * @brief The structure that will be used in an array, so that you get an array
 * of structure. You have to create an MPI derived datatype representing this
 * structure.
 * @details The meaning of structure members is irrelevant, just know that they
 * are specifically tailored for educational purpose.
 **/
struct AoS
{
    /// Some double.
    double d1;
    /// Some integer.
    int i;
    /// Some double.
    double d2;
};

/**
 * @brief The structure containing arrays for each member, so that you have a
 * structure of arrays. You have to create a second MPI derived datatype, which
 * will representing this structure.
 **/
struct SoA
{
    /**
     * @brief The array containing all members d1 from each of the struct AoS
     * that will have been received.
     **/
    double d1[EPCC_ARRAY_SIZE];
    /**
     * @brief The array containing all members i from each of the struct AoS
     * that will have been received.
     **/
    int i[EPCC_ARRAY_SIZE];
    /**
     * @brief The array containing all members d2 from each of the struct AoS
     * that will have been received.
     **/
    double d2[EPCC_ARRAY_SIZE];
};

/// The MPI derived datatype representing the struct AoS.
MPI_Datatype EPCC_MPI_AOS;
/// The MPI derived datatype representing the structu SoA.
MPI_Datatype EPCC_MPI_SOA;

/**
 * @brief The function that defines the EPCC_MPI_AOS and EPCC_MPI_SOA derived
 * datatypes.
 * @details You must implement this function in exercise_4.c such that the
 * derived datatypes created corresponds to the datatypes EPCC_MPI_AOS and
 * EPCC_MPI_SOA.
 **/
void EPCC_create_MPI_datatypes(void);

#endif // EXERCISE_4_H_INCLUDED
