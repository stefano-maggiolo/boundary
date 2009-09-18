SOURCES = $(wildcard *.cpp)
CSOURCES = $(wildcard *.c)
OBJECTS = $(patsubst %.cpp, %.o, $(SOURCES))
COBJECTS = $(patsubst %.c, %.o, $(CSOURCES))
HEADERS = $(wildcard *.h)

# Flags available:

#   USE_LINES_MAP (default=no)

#     Let simple_divisions be the vector with positions i such that
#     g[i-1] != g[i] or m[i-1] != m[i] or l[i-1] != l[i]; then with
#     this flag, before comparing two graphs g1 and g2 the program
#     compare the number of x's in the strip [i...j-1, 1...n] (where i
#     and j are successive elements of simple_divisions, and x are
#     natural numbers. If they differ, the graphs cannot be
#     isomorphic.

#   USE_LINES_NO_MAP (default=yes)

#     For every strip [i...j-1, 1...K] as before, sorts each row of
#     the graph g1 and g2, and then sorts the rows
#     lexicographically. If the strips obtained differ, then g1 cannot
#     be isomorphic to g2. More refined than USE_LINES_MAP, and faster
#     for low number of nodes K.

#   USE_DEGREES_MAP (defaul=no)

#     For every rectangle [i...j-1, i'...j'-1], with i and j, and i'
#     and j', successive elements in simple_divisions, compares the
#     number of x's in the rectangles of the two graphs. If these
#     numbers differ, g1 cannot be isomorphic to g2.

#   USE_DEGREES_NO_MAP (default=yes)

#     For every rectangle [i...j-1, i'...j'-1], as before, sorts each
#     row of the rectangles, and the sorts the rows
#     lexicographically. If the rectangles differ, then g1 cannot be
#     isomorphic to g2. More refined than USE_DEGREES_MAP and faster
#     for low number of nodes K.

#   START_LAPACK_COMPUTATION (default=9)

#     Before testing isomorphism of two graphs with number of nodes at least START_LAPACK_COMPUTATION, compute the eigenvalues of the adjacency matrices. If the sorted vectors of eigenvalues differ, the graphs are not isomorphic. If there is an eigenvalue with one-dimensional eigenspace and such that the coefficient of the corresponding eigenvector are all distinct, than the only possibility for g1 and g2 to be isomorphic is that one is the permutation of the other via the permutation matrix obtained by the permutation of the coefficients of the eigenvector. If this permutation failed to provide an isomorphism, or it sends a node to a node with different genus, number of marked points, number of loops, then the graphs cannot be isomorphic.

CPPFLAGS = -O2 -llapackpp -DUSE_NAUTY -DUSE_LINES_NO_MAP -DUSE_DEGREES_NO_MAP 

#CFLAGS = -O2

all: strata2

%.o: %.cpp $(HEADERS)
	g++ -c -o $*.o $*.cpp $(CPPFLAGS)

#%.o: %.c $(HEADERS)
#	gcc -c -o $*.o $*.c $(CFLAGS)

clean:
	rm -f $(OBJECTS) strata2

strata2: $(OBJECTS) $(HEADERS)
	g++ -o strata2 $(OBJECTS) $(CPPFLAGS) ./nauty/nauty.o ./nauty/nautil.o ./nauty/naugraph.o ./nauty/naututil.o ./nauty/rng.o

backup:
	tar cf strata2.tar.gz ./
