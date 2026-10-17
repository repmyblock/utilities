#ifndef RECORD_HASH_H
#define RECORD_HASH_H
#include <stdint.h>
#include <stddef.h>
typedef enum { RECORD_ERROR=-1, RECORD_NOT_FOUND=0, RECORD_FOUND=1, RECORD_DUPLICATE=2 } RecordLookupResult;
typedef struct { RecordLookupResult status; uint32_t *IDs; size_t count; } RecordMatch;
typedef struct RecordHashTable RecordHashTable;
typedef struct { size_t offset, size; int text; } RecordField;
RecordHashTable *BuildRecordHashTable(const void *, size_t, size_t, size_t, size_t, const RecordField *, size_t);
RecordMatch FindRecordID(const RecordHashTable *, const void *);
void FreeRecordMatch(RecordMatch *);
void FreeRecordHashTable(RecordHashTable *);
#endif
