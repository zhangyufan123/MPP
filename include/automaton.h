/*
 *  Main header file for percolation exercise.
 */

/*
 *  System size L
 */
#include <math.h>

#define L 960 // Change L here

/*
 *  Use 1D decomposition over NPROC processes across first dimension
 *  For an LxL simulation, the local arrays are of size LX x LY
 */

int PROC_ROWS, PROC_COLS; // The number of rows and columns of the grid
int LX, LY; // The size of each block
int LLX, LLY; // The size of the first block
int NPROC; // The number of blocks
int *LXX; // Array of LX of every block
int *LYY; // Array of LY of every block

/*
 *  Prototypes for supplied functions
 */

/*
 *  Visualisation
 */

void autowrite(char *cellfile, int cell[L][L]);
void autowritedynamic(char *cellfile, int **cell, int l);

/*
 *  Calculate and set LX, LY for every process
 */

void setRC(int r, int c);
void setXY();
void freeLXY(); // Free memory

/*
 *  Random numbers
 */

void rinit(int ijkl);
float uni(void);
