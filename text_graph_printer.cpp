#include "text_graph_printer.h"

using namespace std;

TextGraphPrinter::TextGraphPrinter(FILE* file, int g, int n, int printOnlyCodim)
{
  this->file = file;
  this->G = g;
  this->M = n;
  this->printOnlyCodim = printOnlyCodim;
}

TextGraphPrinter::TextGraphPrinter(FILE* file, int g, int n)
{
  this->file = file;
  this->G = g;
  this->M = n;
  this->printOnlyCodim = -1;
}

TextGraphPrinter::TextGraphPrinter(const TextGraphPrinter& L)
{
  file = L.file;
  G = L.G;
  M = L.M;
  printOnlyCodim = L.printOnlyCodim;
}

void
TextGraphPrinter::BeginPrint(void)
{
  fprintf(file, "Strata of \\bar{M}_{%d, %d}\n\n", G, M);
}

void
TextGraphPrinter::PrintSomeGraph(map< uchar, vector< Graph* > >& store)
{
  for (map< uchar, vector< Graph* > >::iterator s = store.begin(); s != store.end(); ++s)
    for (vector< Graph* >::iterator t = s->second.begin(); t != s->second.end(); ++t)
      if (printOnlyCodim == -1 || (*t)->total_edges == printOnlyCodim)
        (*t)->PrintNormal(file);
}

void
TextGraphPrinter::EndPrint(void)
{
}
