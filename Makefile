EXES = boundary #boundary2ordered
SRCS = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp, %.o, $(SRCS))
PRINTEROBJS = binary_graph_printer.o latex_graph_printer.o text_graph_printer.o
READEROBJS = binary_graph_reader.o
COMPUTEROBJS = boundary_computer.o #boundary_computer_2.o ct_computer.o
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
# BLADEBUG: For each value of K,g,n,l, prints the number of stable
# graphs with that values

CPPFLAGS = -O3 -DHAVE_GETRUSAGE=1 -DHAVE_MAXRSS=0 -DEMPTY_OFTEN #-DBLADEBUG
STRATAFLAGS = -DUSE_NAUTY

all: $(EXES)

-include depend

boundary: $(BOUNDARYOBJS) depend
	g++ -o boundary $(BOUNDARYOBJS) $(CPPFLAGS) $(STRATAFLAGS) $(NAUTYOBJS)

boundary2ordered: $(BOUNDARY2ORDEREDOBJS) depend
	g++ -o boundary2ordered $(BOUNDARY2ORDEREDOBJS) $(CPPFLAGS) $(STRATAFLAGS) $(NAUTYOBJS)

%.o: %.cpp $(HDRS)
	g++ -c -o $*.o $*.cpp $(CPPFLAGS) $(STRATAFLAGS)

depend: $(SRCS)
	g++ -MM $(CPPFLAGS) $(STRATAFLAGS) $^ > $@

clean:
	rm -f $(OBJS) $(EXES) strata2.tar.gz

clean_depend: clean
	rm -f depend

backup:
	tar cf strata2.tar.gz ./

.PHONY: clean clean_depend backup all
