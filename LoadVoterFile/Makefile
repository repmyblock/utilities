IDIR =src
CC=g++
CFLAGS= -std=c++17 -O3 -I$(IDIR)

ODIR=src
LDIR =../lib

LIBS=-lmysqlcppconn -lpthread

_DEPS = Voter.h DataConnector.h DatabaseConnector.h DataCollector.h RawFilesInjest.h 
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

# Add the object files from the StatesModules directory
_STATES_OBJ = OH_RawFilesInjest.o MN_RawFilesInjest.o WA_RawFilesInjest.o NV_RawFilesInjest.o NY_RawFilesInjest.o
STATES_OBJ = $(patsubst %,$(ODIR)/StatesModules/%,$(_STATES_OBJ))

_OBJ = DatabaseConnector.o DataCollector.o RawFilesInjest.o Voter.o ProcessCD.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(ODIR)/%.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(ODIR)/StatesModules/%.o: $(ODIR)/StatesModules/%.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

ProcessCD: $(OBJ) $(STATES_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o $(ODIR)/StatesModules/*.o *~ core $(INCDIR)/*~ ProcessCD

git:
	expand -t 2 $($ODIR)/*.cpp 
