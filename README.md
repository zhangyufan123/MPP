### Project structure

src:

```
/* The main program includes meshing, initialization, iteration,
 * data distribution and summary
 */
automaton.c
```

---

```
autoio.c //Write to file
/*Function to write the cells in black and white to cell.pbm*/
void autowritedynamic(char *cellfile, int **cell, int l)
```

---

```
unirand.c 
/*Functions together initialize each cell*/
void rinit(int ijkl)
void rstart(int i, int j, int k, int l)
float uni(void)
```

---

```
arraymalloc.c
/*Create two-dimensional dynamic memory arrays*/
void **arraymalloc2d(int nx, int ny, size_t typesize)
```

---

```
function.c
/*Determine the length and width of each grid based on the number of rows and columns*/
void setRC(int r, int c)
void setXY()
```

include:

```
/*Declare L, PROC_ROWS, PROC_COLS, LX, LY
 */
automaton.h
```

---

```
arraymalloc.h
```

### Compile command

Compile by Makefile in the project:

```
module load intel-20.4/compilers
module load mpt // mpt/2.25 will be loaded
make
```

### Run command

Run by script:

```
sbatch test.job
```

Run by hand:

```
mpirun -n P ./automaton seed
```

P is the number of processes, seed is an integer

### Parameters

To change L:

```
automaton.h
#define L 960
```

To change maxstep

```
automaton.c
maxstep = 10*L;
```

To change rho:

```
automaton.c
rho = 0.52;
```

