CC = gcc

CPPFLAGS += -Idatamodules $(shell mysql_config --cflags)
CFLAGS = -O2 -std=c11 -Wall -Wextra -MMD -MP
LDLIBS += $(shell mysql_config --libs) -licuuc -licudata

TARGETS = FindMultiIndex FindIndexCode

MODULE_SOURCES = $(wildcard datamodules/*.c)
MODULE_OBJECTS = $(MODULE_SOURCES:.c=.o)

MULTI_OBJECTS = FindMultiIndex.o $(MODULE_OBJECTS)
INDEX_OBJECTS = FindIndexCode.o

OBJECTS = $(MULTI_OBJECTS) $(INDEX_OBJECTS)
DEPS = $(OBJECTS:.o=.d)

.PHONY: all clean rebuild

all: $(TARGETS)

MODULE_HEADERS = $(wildcard datamodules/*.h)
GENERATED_HEADER = DataModules.h

.PHONY: FORCE
FORCE:

$(GENERATED_HEADER): FORCE $(MODULE_HEADERS)
	@{ \
	    printf '%s\n' '#ifndef DATA_MODULES_H' '#define DATA_MODULES_H'; \
	    for header in $(MODULE_HEADERS); do \
	        printf '#include "%s"\n' "$$header"; \
	    done; \
	    printf '%s\n' '#endif'; \
	} > $@.tmp
	@cmp -s $@.tmp $@ && rm -f $@.tmp || mv -f $@.tmp $@

FindMultiIndex.o: FindMultiIndex.h $(GENERATED_HEADER)

FindMultiIndex: $(MULTI_OBJECTS) 
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

FindIndexCode: $(INDEX_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGETS) $(OBJECTS) $(DEPS) $(GENERATED_HEADER)

rebuild:
	$(MAKE) clean
	$(MAKE) all

-include $(DEPS)


