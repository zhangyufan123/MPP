#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include "automaton.h"

void setRC(int r, int c) {
    PROC_ROWS = r;
    PROC_COLS = c;
    NPROC = PROC_ROWS*PROC_COLS;
}

/*
 * Divide L evenly. If it is not divisible, the last segment is longer than the others.
 */
void setXY() {
    int row = (int)floor(L/PROC_ROWS);
    int col = (int)floor(L/PROC_COLS);
    LXX = (int*)malloc(sizeof(int)*PROC_ROWS);
    LYY = (int*)malloc(sizeof(int)*PROC_COLS);
    if(PROC_ROWS>1) {
        for(int i=0;i<PROC_ROWS-1;i++) {
            LXX[i] = row;
        }
        LXX[PROC_ROWS-1] = L - (PROC_ROWS-1) * row;
    }
    else {
        LXX[0] = row;
    }
    if(PROC_COLS>1) {
        for(int i=0;i<PROC_COLS-1;i++) {
            LYY[i] = col;
        }
        LYY[PROC_COLS-1] = L - (PROC_COLS-1) * col;
    }
    else {
        LYY[0] = col;
    }
}

void freeLXY() {
    free(LXX);
    free(LYY);
}