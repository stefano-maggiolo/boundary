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

#include "binary_graph_printer.h"

using namespace std;

BinaryGraphPrinter::BinaryGraphPrinter(FILE* file, int g, int n, int printOnlyCodim)
{
  this->file = file;
  this->G = g;
  this->M = n;
  this->printOnlyCodim = printOnlyCodim;
}

BinaryGraphPrinter::BinaryGraphPrinter(FILE* file, int g, int n)
{
  this->file = file;
  this->G = g;
  this->M = n;
  this->printOnlyCodim = -1;
}

BinaryGraphPrinter::BinaryGraphPrinter(const BinaryGraphPrinter& L)
{
  file = L.file;
  G = L.G;
  M = L.M;
  printOnlyCodim = L.printOnlyCodim;
}

void
BinaryGraphPrinter::BeginPrint(void)
{
  unsigned char magic = 0;
  fwrite(&magic, sizeof(unsigned char), 1, file);
}

void
BinaryGraphPrinter::PrintSomeGraph(map< uchar, vector< Graph* > >& store)
{
  for (map< uchar, vector< Graph* > >::iterator s = store.begin(); s != store.end(); ++s)
    for (vector< Graph* >::iterator t = s->second.begin(); t != s->second.end(); ++t)
      if (printOnlyCodim == -1 || (*t)->total_edges == printOnlyCodim)
        (*t)->PrintBinary(file);
}

void
BinaryGraphPrinter::EndPrint(void)
{
  unsigned char eof = 0;
  fwrite(&eof, sizeof(unsigned char), 1, file);
}
