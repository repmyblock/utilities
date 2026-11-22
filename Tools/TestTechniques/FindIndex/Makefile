CC = gcc

SRC_DIR := src
MODULE_DIR := $(SRC_DIR)/datamodules
GENERATED_DIR := $(SRC_DIR)/autocreated

GENERATOR := scripts/GenerateTest.pl
SCHEMA := scripts/2026_10_14_RepMyBlock.sql

CPPFLAGS += -I$(SRC_DIR) -I$(MODULE_DIR) $(shell mysql_config --cflags)
CFLAGS = -O0 -g3 -std=c11 -Wall -Wextra -MMD -MP -fno-omit-frame-pointer
LDLIBS += $(shell mysql_config --libs) -licuuc -licudata

COMMON_OBJECTS := \
	$(SRC_DIR)/CommonFiles.o \
	$(SRC_DIR)/RecordHash.o \
	$(SRC_DIR)/RecordLoader.o

MODULE_SOURCES := $(wildcard $(MODULE_DIR)/*.c)
MODULE_OBJECTS := $(MODULE_SOURCES:.c=.o)
MODULE_HEADERS := $(wildcard $(MODULE_DIR)/*.h)

CHECK_SOURCES := $(wildcard $(GENERATED_DIR)/Check*.c)
CHECK_OBJECTS := $(CHECK_SOURCES:.c=.o)
CHECK_TARGETS := $(patsubst $(GENERATED_DIR)/%.c,%,$(CHECK_SOURCES))

MULTI_OBJECTS := \
	$(MODULE_OBJECTS) \
	$(SRC_DIR)/FindMultiIndex.o \
	$(SRC_DIR)/SaveMemory.o \
	$(COMMON_OBJECTS)

INDEX_OBJECTS := $(SRC_DIR)/FindIndexCode.o
TARGETS := FindMultiIndex FindIndexCode $(CHECK_TARGETS)
OBJECTS := $(MULTI_OBJECTS) $(INDEX_OBJECTS) $(CHECK_OBJECTS)
DEPS := $(OBJECTS:.o=.d)

GENERATED_HEADER := $(MODULE_DIR)/DataModules.h

.PHONY: all clean superclean rebuild generate FORCE
all: generate $(TARGETS)

# Generate the module and checker source files.
generate:
	./$(GENERATOR) --root ./$(GENERATED_DIR) --schema $(SCHEMA) --force

.PHONY: FORCE
FORCE:

# Create src/DataModules.h with paths relative to src/.
$(GENERATED_HEADER): FORCE $(MODULE_HEADERS)
	@{ \
	    printf '%s\n' '#ifndef DATA_MODULES_H' '#define DATA_MODULES_H'; \
	    for header in $(notdir $(MODULE_HEADERS)); do \
	        printf '#include "datamodules/%s"\n' "$$header"; \
	    done; \
	    printf '%s\n' '#endif'; \
	} > $@.tmp
	@cmp -s $@.tmp $@ && rm -f $@.tmp || mv -f $@.tmp $@

$(SRC_DIR)/FindMultiIndex.o: $(SRC_DIR)/FindMultiIndex.h $(GENERATED_HEADER)
$(CHECK_OBJECTS): $(GENERATED_HEADER)

# Main executables are created in the project root.
FindMultiIndex: $(MULTI_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

FindIndexCode: $(INDEX_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# Checker executables are created in the project root.
Check%Module: $(COMMON_OBJECTS) $(GENERATED_DIR)/Check%Module.o $(MODULE_DIR)/%.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

Check%: $(COMMON_OBJECTS) $(GENERATED_DIR)/Check%.o $(MODULE_DIR)/%.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# Compile sources under src/.
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Compile generated checker sources.
$(GENERATED_DIR)/%.o: $(GENERATED_DIR)/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Compile module sources.
$(MODULE_DIR)/%.o: $(MODULE_DIR)/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGETS) $(OBJECTS) $(DEPS)
	rm -f $(SRC_DIR)/*.o $(SRC_DIR)/*.d
	rm -f $(GENERATED_DIR)/*.o $(GENERATED_DIR)/*.d
	rm -f $(MODULE_DIR)/*.o $(MODULE_DIR)/*.d
	rm -f $(GENERATED_HEADER) $(GENERATED_HEADER).tmp

# Remove executables, build files, and generated sources.
.PHONY: superclean
superclean: clean
	rm -f $(MODULE_DIR)/*
	rm -rf $(GENERATED_DIR)/*

rebuild:
	$(MAKE) clean
	$(MAKE) all

-include $(DEPS)