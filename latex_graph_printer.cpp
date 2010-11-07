#include "latex_graph_printer.h"

using namespace std;

LaTeXGraphPrinter::LaTeXGraphPrinter(FILE* file, int g, int n, int printOnlyCodim)
{
  this->file = file;
  this->G = g;
  this->M = n;
  this->printOnlyCodim = printOnlyCodim;
  rowLength = 4;
}

LaTeXGraphPrinter::LaTeXGraphPrinter(FILE* file, int g, int n)
{
  this->file = file;
  this->G = g;
  this->M = n;
  this->printOnlyCodim = -1;
  rowLength = 4;
}

LaTeXGraphPrinter::LaTeXGraphPrinter(const LaTeXGraphPrinter& L)
{
  file = L.file;
  G = L.G;
  M = L.M;
  rowLength = L.rowLength;
  printOnlyCodim = L.printOnlyCodim;
}

void
LaTeXGraphPrinter::BeginPrint(void)
{
  fprintf(file, "\\documentclass{article}\n");
  fprintf(file, "\n");
  fprintf(file, "\\usepackage{tikz}\n");
  fprintf(file, "\\usepackage{booktabs}\n");
  fprintf(file, "\\usepackage{longtable}\n");
  fprintf(file, "\\usepackage{geometry}\n");
  fprintf(file, "\\usetikzlibrary{trees}\n");
  fprintf(file, "\n");
  fprintf(file, "\\tikzstyle{every picture}=[scale=.5,inner sep=10]\n");
  fprintf(file, "\\tikzstyle{every node}=[draw,circle,inner sep=1]\n");
  fprintf(file, "\n");
  fprintf(file, "\\title{Strata of $\\overline{\\mathcal{M}}_{%d,%d}$}\n", G, M);
  fprintf(file, "\\author{strata.cpp}\n");
  fprintf(file, "\\date{\\today}\n");
  fprintf(file, "\n");
  fprintf(file, "\\begin{document}\n");
  fprintf(file, "\\maketitle\n");
  fprintf(file, "\n");
  fprintf(file, "Legend: Id = (components, unique id, codimension); G = Graph.\n");
  fprintf(file, "\n");
  fprintf(file, "  \\begin{longtable}{c@{}c");
  for (int i = 0; i < rowLength-1; i++)
    fprintf(file, "|c@{}c");
  fprintf(file, "}\n");
  fprintf(file, "    \\toprule\n");
  for (int i = 0; i < rowLength-1; i++)
    fprintf(file, "    Id & G &\n");
  fprintf(file, "    Id & G \\\\\n");
  fprintf(file, "    \\midrule\n");
  fprintf(file, "    \\endhead\n");
  fprintf(file, "\n");
  fprintf(file, "    \\bottomrule\n");
  fprintf(file, "    \\endfoot\n");
  fprintf(file, "\n");
}

void
LaTeXGraphPrinter::PrintSomeGraph(map< int, vector< Graph* > >& store)
{
  for (int i = 0; i <= 3*G-3+M; i++)
    for (map< int, vector< Graph* > >::iterator s = store.begin(); s != store.end(); ++s)
      for (vector< Graph* >::iterator t = s->second.begin(); t != s->second.end(); ++t)
        if ((*t)->total_edges == i)
          if (printOnlyCodim == -1 || (*t)->total_edges == printOnlyCodim)
            print_middle(*t, false);
}

void
LaTeXGraphPrinter::EndPrint(void)
{
  fprintf(file, "  \\end{longtable}\n");
  fprintf(file, "\\end{document}\n");
}

void
LaTeXGraphPrinter::print_middle(Graph *graph, bool level_by_vertices)
{
  static int prev_K = 1;
  static int counter = 0;
  counter ++;
  int c = graph->total_edges;
  if ((level_by_vertices && prev_K != graph->K) || (!level_by_vertices && prev_K != c))
    {
      if (counter % rowLength != 1)
        {
          for (; counter % rowLength != 1; counter ++)
            fprintf(file, " & ");
          fprintf(file, "\\\\\n\n\n");
        }
      if (level_by_vertices)
        prev_K = graph->K;
      else
        prev_K = c;
      counter = 1;
      fprintf(file, "\\begin{tikzpicture}[baseline]\\path(0,0) circle (1.5);\\end{tikzpicture}");
      for (int i = 0; i < 2*rowLength-1; i++)
        fprintf(file, " &");
      fprintf(file, " \\\\\n");
    }
  fprintf(file, "    $\\scriptstyle{(%d, %d, %d)}$ & \n", graph->K, counter, c);

  graph->PrintLaTeX(file);

  if (counter % rowLength == 0) fprintf(file, "\\\\\n\n");
  else fprintf(file, "    & \n");
}
