#include "../include/util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

const MPI_Comm EPCC_DEFAULT_COMMUNICATOR = MPI_COMM_WORLD;
const size_t EPCC_MAX_BUFFER_SIZE = 1024;
static bool EPCC_has_read_rank = false;
int EPCC_my_rank;

void EPCC_printf(const char* format, ...)
{
    if(!EPCC_has_read_rank)
    {
        MPI_Comm_rank(EPCC_DEFAULT_COMMUNICATOR, &EPCC_my_rank);
        EPCC_has_read_rank = true;
    }
    const int BUFFER_SIZE = EPCC_MAX_BUFFER_SIZE;
	char buffer[BUFFER_SIZE];
	snprintf(buffer, BUFFER_SIZE, "[MPI process %d] ", EPCC_my_rank);

	int buffer_size_remaining = BUFFER_SIZE - strlen(buffer);
	va_list arguments;
	va_start(arguments, format);
	vsnprintf(buffer + strlen(buffer), buffer_size_remaining, format, arguments);
	va_end(arguments);

	printf("%s", buffer);
}