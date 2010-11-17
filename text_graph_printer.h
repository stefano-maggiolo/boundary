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

#ifndef TEXT_GRAPH_PRINTER_H
#define TEXT_GRAPH_PRINTER_H

#include "graph.h"
#include "graph_printer.h"

#include <map>
#include <vector>
#include <cstdio>

using namespace std;

class TextGraphPrinter : public GraphPrinter
{
 public:
  TextGraphPrinter(FILE* file, int g, int n, int printOnlyCodim);
  TextGraphPrinter(FILE* file, int g, int n);
  TextGraphPrinter(const TextGraphPrinter& L);
  void BeginPrint(void);
  void PrintSomeGraph(map< uchar, vector< Graph* > >& store);
  void EndPrint(void);

 private:
  FILE* file;
  int printOnlyCodim;
  int G, M;
};

#endif
