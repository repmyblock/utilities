#ifndef RECORD_LOADER_H
#define RECORD_LOADER_H
#include "RecordHash.h"


int LoadRecordArray(MYSQL *, const char *, size_t, size_t, size_t,
                    const RecordField *, size_t, void **, size_t *);
#endif
