CC = gcc

CPPFLAGS += -Idatamodules $(shell mysql_config --cflags)
#CFLAGS = -O2 -std=c11 -Wall -Wextra -MMD -MP
CFLAGS = -O0 -g3 -std=c11 -Wall -Wextra -MMD -MP -fno-omit-frame-pointer
LDLIBS += $(shell mysql_config --libs) -licuuc -licudata

MODULE_SOURCES = $(wildcard datamodules/*.c)
MODULE_OBJECTS = $(MODULE_SOURCES:.c=.o)
MODULE_HEADERS = $(wildcard datamodules/*.h)

COMMON_OBJECTS = CommonFiles.o RecordHash.o RecordLoader.o
MULTI_OBJECTS = $(MODULE_OBJECTS) FindMultiIndex.o SaveMemory.o $(COMMON_OBJECTS)
INDEX_OBJECTS = FindIndexCode.o

# Build a test executable for each generated Check*.c source.
CHECK_SOURCES = $(wildcard Check*.c)
CHECK_TARGETS = $(CHECK_SOURCES:.c=)
CHECK_OBJECTS = $(CHECK_SOURCES:.c=.o)

TARGETS = FindMultiIndex FindIndexCode $(CHECK_TARGETS)
OBJECTS = $(MULTI_OBJECTS) $(INDEX_OBJECTS) $(CHECK_OBJECTS)
DEPS = $(OBJECTS:.o=.d)

.PHONY: all clean rebuild
all: $(TARGETS)

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
$(CHECK_OBJECTS): $(GENERATED_HEADER)

FindMultiIndex: $(MULTI_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

FindIndexCode: $(INDEX_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# The Module-suffixed checker sources use the same-named module without the
# suffix; the regular Check<Type>.c sources use the same rule without stripping.
Check%Module: $(COMMON_OBJECTS) Check%Module.o datamodules/%.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

Check%: $(COMMON_OBJECTS) Check%.o datamodules/%.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGETS) $(OBJECTS) $(DEPS) Check*.o Check*.d \
	      datamodules/*.o datamodules/*.d $(GENERATED_HEADER) $(GENERATED_HEADER).tmp

superclean:
	rm -f $(TARGETS) $(OBJECTS) $(DEPS) Check*.? \
	      datamodules/*.? $(GENERATED_HEADER) $(GENERATED_HEADER).tmp

rebuild:
	$(MAKE) clean
	$(MAKE) all

-include $(DEPS)

