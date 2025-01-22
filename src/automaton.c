#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include "automaton.h"
#include "arraymalloc.h"

/*
 * Parallel program to simulate a simple 2D cellular automaton
 */

int main(int argc, char *argv[])
{
  /*
   *  Define the main arrays for the simulation
   */

  int **cell; // Store the cells in each process with halos
  int **neigh; // Store the number of living neighors of each cell

  /*
   *  Additional array WITHOUT halos for initialisation and IO. This
   *  is of size LxL because, even in our parallel program, we do
   *  these two steps in serial
   */

  int **allcell; // store all the cell
  int **tmpcell; // Temporarily store cells
  /*
   *  Array to store local part of allcell
   */

  int **smallcell; // Store the cells in each process without halos

  /*
   *  Variables that define the automaton behaviour
   */

  int seed;
  int incells; // Initial number of living cells
  double rho;
  double tstart, tend; // Store and calculate the execution time
  /*
   *  Local variables
   */

  int i, j, ncell, localncell, step, maxstep, printfreq;
  double r;

  /*
   *  Variables needed by MPI
   */

  MPI_Comm comm = MPI_COMM_WORLD;
  MPI_Comm cart_comm;

  int size, rank;
  int tag = 1;

  int dims[2] = {0, 0};
  int periods[2] = {1, 0};
  int coords[2];
  int up, down, left, right;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(comm, &size);
  MPI_Comm_rank(comm, &rank);
  
  MPI_Dims_create(size, 2, dims);
  MPI_Cart_create(comm, 2, dims, periods, 0, &cart_comm);
  MPI_Cart_coords(cart_comm, rank, 2, coords);
  MPI_Cart_shift(cart_comm, 0, 1, &up, &down);
  MPI_Cart_shift(cart_comm, 1, 1, &left, &right);
  MPI_Barrier(comm);
  
  /*
   *  Set LX, LY for each process, allocate memory
   */
    
  setRC(dims[0], dims[1]);
  setXY();

  LX = LXX[coords[0]];
  LY = LYY[coords[1]];

  LLX = LXX[0];
  LLY = LYY[0];
  
  cell = (int **) arraymalloc2d(LX+2, LY+2, sizeof(int));
  neigh = (int **) arraymalloc2d(LX+2, LY+2, sizeof(int));
  allcell = (int **) arraymalloc2d(L, L, sizeof(int));
  tmpcell = (int **) arraymalloc2d(L, L, sizeof(int));
  smallcell = (int **) arraymalloc2d(LX, LY, sizeof(int));
  
  /*
   * Non-periodic boundary conditions
   *
   * Note that the special rank of MPI_PROC_NULL is a "black hole" for
   * communications. Using this value for processes off the edges of
   * the grid means there is no additional logic needed to ensure
   * processes at the edges do not attempt to send to or receive from
   * invalid ranks (i.e. rank = -1 and rank = NPROC).
   *
   * A full solution would compute neighbours in a Cartesian topology
   * via MPI_Cart_shift where MPI_PROC_NULL is assigned automatically.
   */

  if (dims[0] * dims[1] != size)
    {
      if (rank == 0)
        {
          printf("automaton: ERROR, NPROC = %d running on %d process(es)\n",
                 NPROC, size);
        }

      MPI_Finalize();
      return 1;
    }

  if (argc != 2)
    {
      if (rank == 0)
        {
          printf("Usage: automaton <seed>\n");
        }

      MPI_Finalize();
      return 1;
    }

  /*
   *  Update for a fixed number of steps and periodically report progress
   */
  maxstep = 10*L; // Change max step here
  printfreq = 500;
  
  if (rank == 0)
    {
      printf("automaton: running on %d process(es)\n", size);

      /*
       *  Set the cell density rho (between 0 and 1)
       */

      rho = 0.52; // Change rho here

      /*
       *  Set the random number seed and initialise the generator
       */

      seed = atoi(argv[1]);

      printf("automaton: L = %d, rho = %f, seed = %d, maxstep = %d\n",
             L, rho, seed, maxstep);

      rinit(seed);

      /*
       *  Initialise with the fraction of filled cells equal to rho
       */

      ncell = 0;

      for (i=0; i < L; i++)
        {
          for (j=0; j < L; j++)
            {
              r=uni();

              if(r < rho)
                {
                  allcell[i][j] = 1;
                  ncell++;
                }
              else
                {
                  allcell[i][j] = 0;
                }
            }
        }

      printf("automaton: rho = %f, living cells = %d, actual density = %f\n",
              rho, ncell, ((double) ncell)/((double) L*L) );
      incells = ncell;
    }
  /*
   *   Now broadcast allcell and incells to the every process
   */

  MPI_Bcast(&incells, 1, MPI_INT, 0, comm);
  MPI_Bcast(&allcell[0][0], L*L, MPI_INT, 0, comm);
  
  /*
   * Initialise the cell array: copy the array smallcell to the
   * centre of the array cell; set the halo values to zero.
   */
   
  for (i=1; i <= LX; i++)
    {
      for (j=1; j <= LY; j++)
        {
          cell[i][j] = allcell[coords[0]*LLX+i-1][coords[1]*LLY+j-1];
        }
    }
    
  /*
   * Set normal boundary condition
   */
   
  for (i=0; i <= LX+1; i++)
    {
      cell[i][0]    = 0;
      cell[i][LY+1] = 0;
    }

  for (j=0; j <= LY+1; j++)
    {
      cell[0][j]    = 0;
      cell[LX+1][j] = 0;
    }

  int ru = coords[0]*LLX+1; // Lower bound
  int rb = coords[0]*LLX+LX; // Upper bound

  /*
   * Set new boundary condition
   */

  if (size==1) {
      for (i=L/6; i <= (5*L)/6; i++){
        cell[i][L+1] = 1;
        cell[i][0]   = 1;
      }
  }
  else {
      if(coords[1]==0){
          if(ru<L/6&&rb>=L/6){
              for (j=L/6-ru; j < LX; j++)
              {
                cell[j+1][0] = 1;
              }
          }
          if(ru>L/6&&rb<=(5*L)/6){
              for (j=0; j < LX; j++)
              {
                cell[j+1][0] = 1;
              }
          }
          if(ru<(5*L)/6&&rb>(5*L)/6){
              for (j=0; j < ((5*L)/6)-ru+1; j++)
              {
                cell[j+1][0] = 1;
              }
          }
      }
      if(coords[1]==PROC_COLS-1){
          if(ru<L/6&&rb>=L/6){
              for (j=L/6-ru; j < LX; j++)
              {
                cell[j+1][LY+1] = 1;
              }
          }
          if(ru>L/6&&rb<=(5*L)/6){
              for (j=0; j < LX; j++)
              {
                cell[j+1][LY+1] = 1;
              }
          }
          if(ru<(5*L)/6&&rb>(5*L)/6){
              for (j=0; j < ((5*L)/6)-ru+1; j++)
              {
                cell[j+1][LY+1] = 1;
              }
          }
      }
  }
  MPI_Barrier(comm);
  
  MPI_Request requests[8]; // Requests for Non-blocking communication
  MPI_Status statuses[8]; // Statuses for Non-blocking communication
  
  // Start timing
  if (rank==0){
      tstart = MPI_Wtime();
  }
  
  int step_count = 0; // The number of steps
  
  for (step = 1; step <= maxstep; step++)
    {
      /*
       *  Swap halos up and down
       */

      /*
       * Communications is done using the sendrecv routine; a full
       * solution would use non-blocking communications (e.g. using
       * some suitable combination of issend and/or irecv)
       */
        MPI_Datatype column_type; // New data type for halo sweap
        MPI_Type_vector(LX, 1, LY + 2, MPI_INT, &column_type);
        MPI_Type_commit(&column_type);
       
       // Communications between neighbor blocks
        MPI_Issend(&cell[1][1], LY, MPI_INT, up, tag, comm, &requests[0]);
        MPI_Irecv(&cell[LX+1][1], LY, MPI_INT, down, tag, comm, &requests[1]);
        MPI_Issend(&cell[LX][1], LY, MPI_INT, down, tag, comm, &requests[2]);
        MPI_Irecv(&cell[0][1], LY, MPI_INT, up, tag, comm, &requests[3]);
        
        MPI_Issend(&cell[1][1], 1, column_type, left, tag, comm, &requests[4]);
        MPI_Irecv(&cell[1][LY+1], 1, column_type, right, tag, comm, &requests[5]);
        MPI_Issend(&cell[1][LY], 1, column_type, right, tag, comm, &requests[6]);
        MPI_Irecv(&cell[1][0], 1, column_type, left, tag, comm, &requests[7]);
        
        MPI_Waitall(8, requests, statuses);
        
      for (i=1; i<=LX; i++)
        {
          for (j=1; j<=LY; j++)
            {
              /*
               * Set neigh[i][j] to be the sum of cell[i][j] plus its
               * four nearest neighbours
               */

              neigh[i][j] =   cell[i][j] 
                            + cell[i][j+1]
                            + cell[i][j-1]
                            + cell[i+1][j]
                            + cell[i-1][j];
            }
        }

      localncell = 0;

      for (i=1; i<=LX; i++)
        {
          for (j=1; j<=LY; j++)
            {
              /*
               * Udate based on number of neighbours
               */

              if (neigh[i][j] == 5 || neigh[i][j] == 4 || neigh[i][j] == 2)
                {
                  cell[i][j] = 1;
                  localncell++;
                }
              else
                {
                  cell[i][j] = 0;
                }
            }
        }

      /*
       *  Compute the global changes on rank 0
       */

      MPI_Reduce(&localncell, &ncell, 1, MPI_INT, MPI_SUM, 0, comm);
      /*
       *  Report progress every now and then
       */
      MPI_Bcast(&ncell, 1, MPI_INT, 0, comm);
      
      if (step % printfreq == 0)
        {
          if (rank == 0)
            {
              printf("automaton: number of living cells on step %d is %d\n",
                     step, ncell);
            }
        }
        
      // Special termination conditions
      if (ncell<(3*incells)/4||ncell>(4*incells)/3) {
          if (rank==0) {
              printf("Terminate at step %d with %d living cells!\n", step, ncell);
              step_count = step;
          }
          break;
      }
      step_count = step;
    }
    
  MPI_Barrier(comm);
  
  // End timing
  if(rank==0){
    tend = MPI_Wtime();
    printf("L=%d, rho=%f, T=%d, ms=%d, seed=%d\n", L, rho, size, maxstep, seed);
    printf("Time cost each step: %f ms, total step: %d\n", 1000*(tend-tstart)/step_count, step_count);
  }

  // I would recommend stopping the MPI timer here - remember to
  // synchronise the processes as described in the MPP exercise sheet.
  //
  // If you use the time per step, rather than the total time, for
  // your performance measurements then the performance should be
  // independent of maxstep because each step takes the same
  // time. This will enable you to adjust the number of steps up or
  // down to achieve a sensible overall runtime without changing the
  // performance measurement.
  //
  // If you quit the loop early, e.g. due to the number of live cells
  // reaching some threshold, then remember to divide by the actual
  // number of steps and not by maxstep.

  /*
   *  Copy the centre of cell, excluding the halos, into allcell
   */
  
  for (i=1; i<=LX; i++)
    {
      for (j=1; j<=LY; j++)
        {
          smallcell[i-1][j-1] = cell[i][j];
        }
    }

  for (i=0; i < L; i++)
    {
      for (j=0; j < L; j++)
        {
          tmpcell[i][j] = 0;
        }
    }
    
  for (i=1; i <= LX; i++)
    {
      for (j=1; j <= LY; j++)
        {
          tmpcell[coords[0]*LLX+i-1][coords[1]*LLY+j-1] = smallcell[i-1][j-1];
        }
    }

  /*
   *  Now gather the array smallcell back to allcell
   */
  MPI_Reduce(&tmpcell[0][0], &allcell[0][0], L*L, MPI_INT, MPI_SUM, 0, comm); 

  /*
   *  Write the cells to the file "cell.pbm" from rank 0
   */

  if (rank == 0)
    {
      //autowrite("cell.pbm", allcell);
      autowritedynamic("cell.pbm", allcell, L);
      
    }
    
  // Free all the memory
  free(cell);
  free(neigh);
  free(allcell);
  free(tmpcell);
  free(smallcell);
  freeLXY();
  /*
   * Finalise MPI before finishing
   */

  MPI_Finalize();

  return 0;
}
