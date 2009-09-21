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

int ParseCommandLine(int argc, char* argv[], int& g, int& n, int& codim,
                     enum Printer& printer, enum Statistics& statistics);
void PrintCompilationFlags(void);
void QuitWithUsage(char* name);

int
main(int argc, char* argv[])
{
  int g, n, codim;
  int ret;
  enum Printer printer;
  enum Statistics statistics;
    
  ParseCommandLine(argc, argv, g, n, codim, printer, statistics);
  
  PrintCompilationFlags();

  BoundaryComputer c(g, n);
  GraphPrinter* P;
  if (printer == LaTeX)
    P = new LaTeXGraphPrinter(stdout, g, n);
  else if (printer == Text)
    P = new TextGraphPrinter(stdout, g, n);
  c.Compute(*P, statistics, codim);

  return 0;
}

void
QuitWithUsage(char* name)
{
  fprintf(stderr, "Usage:\n\t%s [-P L|T] [-S F|T|N] [-C C] genus [marked_points]\n", name);
  fprintf(stderr, "\t\t-P T: print graphs in LaTeX (default)\n");
  fprintf(stderr, "\t\t-P L: print graphs in text format\n");
  fprintf(stderr, "\t\t\n");
  fprintf(stderr, "\t\t-S F: print full statistics (default)\n");
  fprintf(stderr, "\t\t-S T: print terse statistics\n");
  fprintf(stderr, "\t\t-S N: don't print statistics\n");
  fprintf(stderr, "\t\t\n");
  fprintf(stderr, "\t\t-C C: print only graphs with C edges\n");
  exit(1);
}

int
ParseCommandLine(int argc, char* argv[], int& g, int& n, int& codim, enum Printer& printer, enum Statistics& statistics)
{
  // Check command line arguments
  codim = -1;
  if (argc < 3)
    QuitWithUsage(argv[0]);
  else
    {
      bool def_g = false, def_n = false;
      printer = LaTeX; // default
      statistics = Full; // default
      n = 0; // default
      for (int i = 1; i < argc; ++i)
        {
          if (argv[i][0] == '-')
            {
              if (argv[i][1] == '\0' || argv[i][2] != '\0')
                QuitWithUsage(argv[0]);
              else if (argv[i][1] == 'C')
                {
                  if (i+1 >= argc)
                    QuitWithUsage(argv[0]);
                  else
                    codim = atoi(argv[i+1]);
                  ++i;
                }
              else if (argv[i][1] == 'P')
                {
                  if (i+1 >= argc || strlen(argv[i+1]) != 1)
                    QuitWithUsage(argv[0]);
                  else if (argv[i+1][0] == 'L')
                    printer = LaTeX;
                  else if (argv[i+1][0] == 'T')
                    printer = Text;
                  else
                    QuitWithUsage(argv[0]);
                  ++i;
                }
              else if (argv[i][1] == 'S')
                {
                  if (i+1 >= argc || strlen(argv[i+1]) != 1)
                    QuitWithUsage(argv[0]);
                  else if (argv[i+1][0] == 'F')
                    statistics = Full;
                  else if (argv[i+1][0] == 'T')
                    statistics = Terse;
                  else if (argv[i+1][0] == 'S')
                    statistics = No;
                  else
                    QuitWithUsage(argv[0]);
                  ++i;
                }
              else
                QuitWithUsage(argv[0]);
            }
          else
            {
              if (!def_g)
                {
                  def_g = true;
                  g = atoi(argv[i]);
                }
              else if (!def_n)
                {
                  def_n = true;
                  n = atoi(argv[i]);
                }
              else
                QuitWithUsage(argv[0]);
            }
        }
    }

  if (2*g-3+n < 0)
    {
      fprintf(stderr, "Stability condition not verified! Try with 2g+n-3 non-negative!\n");
      exit(1);
    }
  return 0;
}

void
PrintCompilationFlags(void)
{
  static char r[] = "$Rev: 6 $";
  int l = strlen(r);
  r[l-2] = '\0';
  fprintf(stderr, "Compilation flags: ");
  fprintf(stderr, "R=%s ", r+6);
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
#ifdef USE_NAUTY
  fprintf(stderr, "USE_NAUTY ");
#endif
  fprintf(stderr, "\n");
}
