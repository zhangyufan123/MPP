/**
 * @file util.h
 * @author Ludovic Capelli (l.capelli@epcc.ed.ac.uk)
 * @brief This file contains general convenience functions such as EPCC_printf.
 **/

#ifndef EPCC_UTIL_H
#define EPCC_UTIL_H

#include <mpi.h>

/**
 * @brief Represents the roles available in a classic 2-process example.
 **/
enum EPCC_rank
{
    /// The one sending the data.
    SENDER,
    /// The one receiving the data.
    RECEIVER
};

/**
 * @brief The MPI communicator to use during the application.
 * @details Behind the scenes, \p EPCC_DEFAULT_COMMUNICATOR translates to 
 * MPI_COMM_WORLD. However, it is usually a good practice to abstract the MPI
 * communicator used by default to improve modularity as it can be changed just
 * by modifying the corresponding variable rather that facing large rewritings.
 **/
extern const MPI_Comm EPCC_DEFAULT_COMMUNICATOR;

/**
 * @brief Convenience function used to automatically indicate the rank of the
 * calling MPI process.
 * @details This function is identical to the original printf. In other words,
 * you can just prepend "EPCC_" to your printf calls and your program will work
 * just as well. With EPCC_printf, it will simply print the rank of the calling
 * MPI process as well, which is an information we typically always want to
 * print anyway.
 * @param format The string describing the format of the argument passed.
 * @param ... The arguments.
 * @pre MPI_Initialize has been called.
 * @pre MPI_Finalize has not been called yet.
 **/
void EPCC_printf(const char* format, ...);

#endif // EPCC_UTIL_H defined