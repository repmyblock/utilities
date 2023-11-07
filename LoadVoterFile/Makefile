IDIR =src
CC=g++
CFLAGS= -std=c++17 -g  -I$(IDIR)

ODIR=src
LDIR =../lib

LIBS=-lmysqlcppconn -lpthread

_DEPS = Voter.h DataConnector.h DatabaseConnector.h DataCollector.h RawFilesInjest.h 
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = DatabaseConnector.o DataCollector.o RawFilesInjest.o Voter.o ProcessCD.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

ProcessCD: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ ProcessCD
