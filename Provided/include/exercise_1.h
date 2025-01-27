/**
 * @file exercise_1.h
 * @author Ludovic Capelli (l.capelli@epcc.ed.ac.uk)
 **/

#ifndef EXERCISE_1_H_INCLUDED
#define EXERCISE_1_H_INCLUDED

#include "util.h"

/**
 * @brief The MPI derived datatype representing a variable that is of type
 * EPCC_MPI_COLUMN.
 * @details This MPI derived datatype is created in the function named
 * EPCC_create_particle_MPI_datatype, whose content you must write.
 **/
MPI_Datatype EPCC_MPI_COLUMN;

/**
 * @brief The function that defines the EPCC_MPI_COLUMN_DATATYPE derived
 * datatype.
 * You must implement this function in util.c using MPI_Type_vector such that
 * the vector datatype created describes one full column of an array of
 * \p row_count x \p column_count.
 * @param row_count The number of rows in the array.
 * @param column_count The number of columns in the array.
 **/
void EPCC_create_MPI_datatype(int row_count, int column_count);

#endif // EXERCISE_1_H_INCLUDED