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
TextGraphPrinter::PrintSomeGraph(map< int, vector< Graph > >& store)
{
  for (int i = 0; i <= 3*G-3+M; i++)
    for (map< int, vector< Graph > >::iterator s = store.begin(); s != store.end(); ++s)
      for (vector< Graph >::iterator t = s->second.begin(); t != s->second.end(); ++t)
        if (t->total_edges == i)
          if (printOnlyCodim == -1 || t->total_edges == printOnlyCodim)
            print_middle(&(*t), false);
}

void
TextGraphPrinter::EndPrint(void)
{
}

void
TextGraphPrinter::print_middle(Graph *graph, bool level_by_vertices)
{
  int c = 0;
  for (int i = 0; i < graph->K; i++)
    for (int j = i; j < graph->K; j++)
      c += graph->a[i][j];
  fprintf(file, "Genus: %d,\n", graph->G);
  fprintf(file, "  codimension: %d,\n", c);
  fprintf(file, "  irreducible components: %d,\n", graph->K);
  fprintf(file, "  with genus ");
  for (int i = 0; i < graph->K; i++)
    fprintf(file, "%d ", graph->g[i]);
  fprintf(file, "\n");
  fprintf(file, "  with marked points ");
  for (int i = 0; i < graph->K; i++)
    fprintf(file, "%d ", graph->m[i]);
  fprintf(file, "\n");
  fprintf(file, "  with loops ");
  for (int i = 0; i < graph->K; i++)
    fprintf(file, "%d ", graph->l[i]);
  fprintf(file, "\n");
  fprintf(file, "  with divisions ");
  for (int i = 0; i < graph->simple_divisions.size(); i++)
    fprintf(file, "%d ", graph->simple_divisions[i]);
  fprintf(file, "\n");
  fprintf(file, "  with adjacency matrix\n");
  for (int i = 0; i < graph->K; i++)
    {
      fprintf(file, "    ");
      for (int j = 0; j < graph->K; j++)
        fprintf(file, "%d ", graph->a[i][j]);
      fprintf(file, "\n");
    }
  fprintf(file, "\n");

}
