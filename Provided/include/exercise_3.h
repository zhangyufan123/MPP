/**
 * @file exercise_3.h
 * @author Ludovic Capelli (l.capelli@epcc.ed.ac.uk)
 **/

#ifndef EXERCISE_2_H_INCLUDED
#define EXERCISE_2_H_INCLUDED

#include "util.h"

/**
 * @brief The structure for which you have to create an MPI derived datatype.
 * @details The meaning of structure members is irrelevant, just know that they
 * are specifically tailored for educational purpose.
 **/
struct EPCC_struct
{
    /// Some integer.
    int i;
    /// Some character.
    char c;
    /// Some double.
    double d;
};

/**
 * @brief The MPI derived datatype representing a variable that is of type
 * EPCC_struct.
 * @details This MPI derived datatype is created in the function named
 * EPCC_create_MPI_datatype, whose content you must write.
 **/
MPI_Datatype EPCC_MPI_STRUCT;

/**
 * @brief The function that defines the EPCC_MPI_STRUCT derived datatype.
 * @details You must implement this function in exercise_3.c such that the
 * derived datatype created corresponds to the datatype EPCC_struct.
 **/
void EPCC_create_MPI_datatype(void);

#endif // EXERCISE_2_H_INCLUDED