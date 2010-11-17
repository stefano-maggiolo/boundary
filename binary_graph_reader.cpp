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

#include "binary_graph_reader.h"

using namespace std;

BinaryGraphReader::BinaryGraphReader(FILE* f)
  :
  file(f)
{
}

BinaryGraphReader::BinaryGraphReader(const char* filename)
{
  file = fopen(filename, "rb");
}

BinaryGraphReader::BinaryGraphReader(const BinaryGraphReader& R)
  :
  file(R.file),
  store(R.store)
{
}

map< int, vector< Graph > >&
BinaryGraphReader::Read(void)
{
  unsigned char tmp;
  store.clear();
  assert(fread(&tmp, sizeof(unsigned char), 1, file) == 1);
  if (tmp != 0) return store;
  Graph* g;
  g = new Graph(file);
  while (g->K > 0)
    {
      g = new Graph(file);
      store[g->K].push_back(*g);
    }
  fclose(file);
  return store;
}

