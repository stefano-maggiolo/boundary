/*
  Copyright (c) 2009, Stefano Maggiolo and Nicola Pagani.
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.
  */

#include "strata2.h"

using namespace std;

int ParseCommandLine(int argc, char* argv[], int& g, int& n, int& codim);
void PrintCompilationFlags(void);

int
main(int argc, char* argv[])
{
  int g, n, codim;
  int ret;

  ret = ParseCommandLine(argc, argv, g, n, codim);
  if (ret) return ret;
  
  PrintCompilationFlags();

  BoundaryComputer c(g, n);
  c.Compute(true, codim);

  // And some statistics.
  c.Statistics(stderr);
  
  return 0;
}

int
ParseCommandLine(int argc, char* argv[], int& g, int& n, int& codim)
{
  // Check command line arguments
  codim = -1;
  if (argc < 3)
    {
      printf("Usage:\n\t%s genus marked_points\n", argv[0]);
      return 1;
    }
  else if (argc == 4)
    codim = atoi(argv[3]);
  
  g = atoi(argv[1]);
  n = atoi(argv[2]);

  if (2*g-3+n < 0)
    {
      printf("Stability condition not verified! Try with 2g+m-3 non-negative!\n");
      return 1;
    }
  return 0;
}

void
PrintCompilationFlags(void)
{
  fprintf(stderr, "Compilation flags: ");
#ifdef USE_DEGREES_MAP
  fprintf(stderr, "USE_DEGREES_MAP ");
#endif
#ifdef USE_DEGREES_NO_MAP
  fprintf(stderr, "USE_DEGREES_NO_MAP ");
#endif
#ifdef USE_LINES_MAP
  fprintf(stderr, "USE_LINES_MAP ");
#endif
#ifdef USE_LINES_NO_MAP
  fprintf(stderr, "USE_LINES_NO_MAP ");
#endif
#ifdef START_LAPACK_COMPUTATION
  fprintf(stderr, "START_LAPACK_COMPUTATION=%d ", START_LAPACK_COMPUTATION);
#endif
  fprintf(stderr, "\n");
}
