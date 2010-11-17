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

#ifndef BINARY_GRAPH_READER_H
#define BINARY_GRAPH_READER_H

#include "graph.h"

#include <map>
#include <vector>
#include <cstdio>

using namespace std;

class BinaryGraphReader
{
 public:
  BinaryGraphReader(const BinaryGraphReader& R);
  BinaryGraphReader(const char* filename);
  BinaryGraphReader(FILE* file);
  map< int, vector< Graph > >& Read(void);

 private:
  FILE* file;
  map< int, vector< Graph > > store;
};

#endif
