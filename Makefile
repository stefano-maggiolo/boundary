EXES = boundary #boundary2ordered
SRCS = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp, %.o, $(SRCS))
PRINTEROBJS = binary_graph_printer.o latex_graph_printer.o text_graph_printer.o
READEROBJS = binary_graph_reader.o
COMPUTEROBJS = boundary_computer.o
BOUNDARYOBJS = boundary.o graph.o $(PRINTEROBJS) $(READEROBJS) $(COMPUTEROBJS)
BOUNDARY2ORDEREDOBJS = boundary2ordered.o graph.o $(PRINTEROBJS) $(READEROBJS) $(COMPUTEROBJS)
NAUTYOBJS = ./nauty/nauty.o ./nauty/nautil.o ./nauty/naugraph.o ./nauty/naututil.o ./nauty/rng.o
HDRS = $(wildcard *.h)

# EMPTY_OFTEN: this flag makes boundary to push to output the
# graphs after each possibility for the data K,g,n,l (instead of
# after each possibility for K). This is really useful to save
# memory, but makes the computation a little slower and orders the
# graph in output in a different way.
#
# DEBUG_GNL_COUNT: For each value of K,g,n,l, prints the number of
# stable graphs with that values

BOUNDARYFLAGS = -DUSE_NAUTY -DEMPTY_OFTEN #-DDEBUG_GNL_COUNT
CPPFLAGS = -O3 -DHAVE_GETRUSAGE=1 -DHAVE_MAXRSS=0

all: $(EXES)

-include depend

boundary: $(BOUNDARYOBJS) depend
	g++ -o boundary $(BOUNDARYOBJS) $(CPPFLAGS) $(BOUNDARYFLAGS) $(NAUTYOBJS)

boundary2ordered: $(BOUNDARY2ORDEREDOBJS) depend
	g++ -o boundary2ordered $(BOUNDARY2ORDEREDOBJS) $(CPPFLAGS) $(BOUNDARYFLAGS) $(NAUTYOBJS)

%.o: %.cpp $(HDRS)
	g++ -c -o $*.o $*.cpp $(CPPFLAGS) $(BOUNDARYFLAGS)

depend: $(SRCS)
	g++ -MM $(CPPFLAGS) $(BOUNDARYFLAGS) $^ > $@

clean:
	rm -f $(OBJS) $(EXES) boundary.tar.gz

clean_depend: clean
	rm -f depend

backup:
	tar cf boundary.tar.gz ./

.PHONY: clean clean_depend backup all
