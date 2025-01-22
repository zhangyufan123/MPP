#include <stdio.h>
#include <stdlib.h>

#include "automaton.h"

/*
 *  Function to write the cells in black and white (Portable Bit Map)
 *  (PBM) format.
 */

void autowrite(char *cellfile, int cell[L][L])
{
  FILE *fp;

  int i, j, npix, col;
  static int pixperline = 32; // the PBM format limits to 70 characters per line

  /*
   *  Write the file
   */

  printf("autowrite: opening file <%s>\n", cellfile);

  fp = fopen(cellfile, "w");

  printf("autowrite: writing data ...\n");

  /*
   *  Start with the PBM header
   */

  fprintf(fp, "P1\n");
  fprintf(fp, "# Written by autowrite\n");
  fprintf(fp, "%d %d\n", L, L) ;

  /*
   *  Now write the cells to file so that cell[0][0] is in the
   *  bottom-left-hand corner and cell[L-1][L-1] is in the
   *  top-right-hand corner
   */

  npix = 0;

  for (j=L-1; j >= 0; j--)
    {
      for (i=0; i < L; i++)
	{
	  npix++;

          // Strangely, PBM files have 1 for black and 0 for white
          
          col = 1;
          if (cell[i][j] == 1) col = 0;

	  // Make sure lines wrap after "npix" pixels

	  if (npix == 1)
	    {
	      fprintf(fp, "%1d", col);
	    }
	  else if (npix < pixperline)
	    {
	      fprintf(fp, " %1d", col);
	    }
	  else
	    {
	      fprintf(fp, " %1d\n", col);
	      npix = 0;
	    }
	}
    }

  if (npix != 0) fprintf(fp, "\n");

  printf("autowrite: ... done\n");

  fclose(fp);
  printf("autowrite: file closed\n");
}


/*
 *  Function to write a percolation map in black and white Portable
 *  Bit Map (PBM) format.
 *
 *  Note that this version expects the map array to have been
 *  dynamically allocated, e.g. using the arralloc() routine:
 *
 *  int **cell;
 *  cell = (int **) arralloc(sizeof(int), 2, L, L);
 *  ...
 *  autowritedynamic("cell.pbm", cell, L);
 */

void autowritedynamic(char *cellfile, int **cell, int l)
{
  FILE *fp;

  int i, j, npix, col;
  static int pixperline = 32; // the PGM format limits to 70 characters per line

  /*
   *  Write the file
   */

  printf("autowritedynamic: opening file <%s>\n", cellfile);

  fp = fopen(cellfile, "w");

  printf("autowritedynamic: writing data ...\n");

  /*
   *  Start with the PBM header
   */

  fprintf(fp, "P1\n");
  fprintf(fp, "# Written by autowritedynamic\n");
  fprintf(fp, "%d %d\n", l, l) ;

  /*
   *  Now write the cells to file so that cell[0][0] is in the
   *  bottom-left-hand corner and cell[l-1][l-1] is in the
   *  top-right-hand corner
   */

  npix = 0;

  for (j=l-1; j >= 0; j--)
    {
      for (i=0; i < l; i++)
	{
	  npix++;

          // Strangely, PBM files have 1 for black and 0 for white
          
          col = 1;
          if (cell[i][j] == 1) col = 0;

	  // Make sure lines wrap after "npix" pixels

	  if (npix == 1)
	    {
	      fprintf(fp, "%1d", col);
	    }
	  else if (npix < pixperline)
	    {
	      fprintf(fp, " %1d", col);
	    }
	  else
	    {
	      fprintf(fp, " %1d\n", col);
	      npix = 0;
	    }
	}
    }

  if (npix != 0) fprintf(fp, "\n");

  printf("autowritedynamic: ... done\n");

  fclose(fp);
  printf("autowritedynamic: file closed\n");
}

