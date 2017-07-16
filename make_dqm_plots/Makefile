CC = g++

CFLAGS = $(shell root-config --cflags)
LDFLAGS = $(shell root-config --libs) -lboost_system -lboost_filesystem

_OBJ = make_dqm_plots.o
OBJ = $(patsubst %,%,$(_OBJ))

%.o: %.cc
	$(CC) -c $(CFLAGS) -o $@ $<

build: $(OBJ)
	$(CC) -o make_dqm_plots.exe $(LDFLAGS) make_dqm_plots.o
	rm -f *.o
