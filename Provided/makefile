SRC_DIRECTORY=src
INCLUDE_DIRECTORY=include
BIN_DIRECTORY=bin
CFLAGS=-O2 -Wall -Wextra
CC=mpicc

default: \
	all

all: \
	create_directory \
	$(BIN_DIRECTORY)/exercise_1 \
	$(BIN_DIRECTORY)/exercise_2 \
	$(BIN_DIRECTORY)/exercise_3 \
	$(BIN_DIRECTORY)/exercise_4

create_directory:
	@mkdir -p $(BIN_DIRECTORY)

$(BIN_DIRECTORY)/exercise_1: $(SRC_DIRECTORY)/exercise_1.c \
							 $(INCLUDE_DIRECTORY)/exercise_1.h \
							 $(SRC_DIRECTORY)/util.c \
							 $(INCLUDE_DIRECTORY)/util.h
	$(CC) -o $@ $(SRC_DIRECTORY)/exercise_1.c $(SRC_DIRECTORY)/util.c $(CFLAGS)

$(BIN_DIRECTORY)/exercise_2: $(SRC_DIRECTORY)/exercise_2.c \
					   		 $(INCLUDE_DIRECTORY)/exercise_2.h \
					   		 $(SRC_DIRECTORY)/util.c \
					   		 $(INCLUDE_DIRECTORY)/util.h
	$(CC) -o $@ $(SRC_DIRECTORY)/exercise_2.c $(SRC_DIRECTORY)/util.c $(CFLAGS)

$(BIN_DIRECTORY)/exercise_3: $(SRC_DIRECTORY)/exercise_3.c \
					   		 $(INCLUDE_DIRECTORY)/exercise_3.h \
					   		 $(SRC_DIRECTORY)/util.c \
					   		 $(INCLUDE_DIRECTORY)/util.h
	$(CC) -o $@ $(SRC_DIRECTORY)/exercise_3.c $(SRC_DIRECTORY)/util.c $(CFLAGS)

$(BIN_DIRECTORY)/exercise_4: $(SRC_DIRECTORY)/exercise_4.c \
					   		 $(INCLUDE_DIRECTORY)/exercise_4.h \
					   		 $(SRC_DIRECTORY)/util.c \
					   		 $(INCLUDE_DIRECTORY)/util.h
	$(CC) -o $@ $(SRC_DIRECTORY)/exercise_4.c $(SRC_DIRECTORY)/util.c $(CFLAGS)

clean:
	rm -rf $(BIN_DIRECTORY)
