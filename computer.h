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

#ifndef COMPUTER_H
#define COMPUTER_H

#include "graph.h"
#include "graph_printer.h"

#include <map>
#include <vector>
#include <cstdio>

enum Statistics { Full, Terse, No };

using namespace std;

class Computer
{
 public:
  virtual vector< Graph* >& GetAllResults(void) = 0;
  virtual map< uchar, vector< Graph* > >& GetAllResultsByCodimension(void) = 0;
  virtual void Compute(GraphPrinter &printer, enum Statistics stat, int computeOnlyCodim) = 0;
  virtual void Compute(GraphPrinter &printer, enum Statistics stat) = 0;
  virtual void Compute(GraphPrinter &printer) = 0;
  virtual void Statistics(FILE* file) = 0;
  virtual void TerseStatistics(FILE* file) = 0;
};

#endif

