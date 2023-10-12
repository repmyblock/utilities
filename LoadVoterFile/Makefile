IDIR =../include
CC=g++ -O3
CFLAGS=-I$(IDIR) -std=c++17  # Ensure this flag is here

ODIR=src
LDIR =../lib

LIBS=-lmysqlcppconn -lpthread

_DEPS = Voter.h DatabaseConnector.h DataCollector.h RawFilesInjest.h 
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = DatabaseConnector.o DataCollector.o RawFilesInjest.o Voter.o main.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

ProcessCD: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
