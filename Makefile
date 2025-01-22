MF=	Makefile

CC=	mpicc
CFLAGS=	-cc=icc -O3 -Wall -Iinclude

LFLAGS= $(CFLAGS)

EXE=	automaton

INC= \
	include/automaton.h \
	include/arraymalloc.h

SRC= \
	src/automaton.c \
	src/autoio.c \
	src/unirand.c \
	src/arraymalloc.c \
	src/function.c

#
# No need to edit below this line
#

.SUFFIXES:
.SUFFIXES: .c .o

OBJ=	$(SRC:.c=.o)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

all:	$(EXE)

$(OBJ):	$(INC)

$(EXE):	$(OBJ)
	$(CC) $(LFLAGS) -o $@ $(OBJ)

$(OBJ):	$(MF)

clean:
	rm -f $(EXE) $(OBJ) core
