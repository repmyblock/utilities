#include "RecordHash.h"
#include <stdlib.h>
#include <string.h>
#include <unicode/ustring.h>
#include <unicode/unorm2.h>
#include <unicode/uchar.h>
#include <unicode/utf16.h>

static char *NormalizeRecordText(const char *text, size_t capacity) {
	const char *end = memchr(text, '\0', capacity);
	if (!end) return NULL;
	UChar decoded[4096], decomposed[4096], folded[4096], normalized[4096];
	int32_t length = 0;
	UErrorCode error = U_ZERO_ERROR;
	u_strFromUTF8(decoded, 4096, &length, text, (int32_t)(end - text), &error);
	if (U_FAILURE(error)) return NULL;

	int32_t first = 0, last = length;
	while (first < last) {
	  int32_t next = first;
	  UChar32 cp;
	  U16_NEXT(decoded, next, last, cp);
	  if (!u_isUWhiteSpace(cp)) break;
	  first = next;
	}
	while (last > first) {
	  int32_t previous = last;
	  UChar32 cp;
	  U16_PREV(decoded, first, previous, cp);
	  if (!u_isUWhiteSpace(cp)) break;
	  last = previous;
	}
	const UNormalizer2 *nfd = unorm2_getNFDInstance(&error);
	const UNormalizer2 *nfc = unorm2_getNFCInstance(&error);
	if (U_FAILURE(error)) return NULL;
	length = unorm2_normalize(nfd, decoded + first, last - first, decomposed, 4096, &error);
	if (U_FAILURE(error)) return NULL;
	length = u_strFoldCase(folded, 4096, decomposed, length, U_FOLD_CASE_DEFAULT, &error);
	if (U_FAILURE(error)) return NULL;
	length = unorm2_normalize(nfc, folded, length, normalized, 4096, &error);
	if (U_FAILURE(error)) return NULL;

	int32_t bytes = 0;
	u_strToUTF8(NULL, 0, &bytes, normalized, length, &error);
	if (error != U_BUFFER_OVERFLOW_ERROR && U_FAILURE(error)) return NULL;
	error = U_ZERO_ERROR;
	char *result = malloc((size_t)bytes + 1);
	if (!result) return NULL;
	u_strToUTF8(result, bytes + 1, NULL, normalized, length, &error);
	if (U_FAILURE(error)) { free(result); return NULL; }
	result[bytes] = '\0';
	return result;
}

typedef struct { unsigned char *key; size_t length, next; uint64_t hash; uint32_t ID; } Entry;

struct RecordHashTable {
	Entry *entries;
	size_t *buckets;
	size_t count, bucketCount, stride, nullOffset, fieldCount;
	RecordField *fields;
};

static int Append(unsigned char **key, size_t *length, const void *bytes, size_t n) {
  if (n > SIZE_MAX - *length) return 0;
  unsigned char *p = realloc(*key, *length + n);
  if (!p) return 0;
  memcpy(p + *length, bytes, n); *key = p; *length += n; return 1;
}

static int MakeKey(const RecordHashTable *t, const void *record,
                   unsigned char **key, size_t *length, uint64_t *hash) {
	*key=NULL; *length=0;
	const unsigned char *row=record;
	uint32_t nulls; memcpy(&nulls,row+t->nullOffset,sizeof nulls);
	for (size_t i=0;i<t->fieldCount;i++) {
		unsigned char present = !(nulls & (UINT32_C(1) << (i+1)));
		if (!Append(key,length,&present,1)) goto fail;
		if (!present) continue;
		const RecordField *f=&t->fields[i];
		if (f->text) {
		  char *text=NormalizeRecordText((const char *)row+f->offset,f->size);
		  if (!text) goto fail;
		  size_t n=strlen(text);
		  int ok=Append(key,length,&n,sizeof n) && Append(key,length,text,n+1);
		  free(text); if (!ok) goto fail;
		} else if (!Append(key,length,row+f->offset,f->size)) goto fail;
	}
	*hash=UINT64_C(14695981039346656037);
	for (size_t i=0;i<*length;i++) { 
		*hash^=(*key)[i]; *hash*=UINT64_C(1099511628211); 
	}
	return 1;
fail:
  free(*key); *key=NULL; *length=0; return 0;
}

void FreeRecordHashTable(RecordHashTable *t) {
  if (!t) return;
  for (size_t i=0;i<t->count;i++) free(t->entries[i].key);
  free(t->entries); free(t->buckets); free(t->fields); free(t);
}

RecordHashTable *BuildRecordHashTable(const void *rows,size_t count,size_t stride,
 																				size_t idOffset,size_t nullOffset,const RecordField *fields,
 																				size_t fieldCount) {
  if ((!rows && count) || !stride || !fields || !fieldCount || 
  	fieldCount>31 || count>SIZE_MAX/stride || count>(SIZE_MAX-1)/2 ||
    idOffset>stride || sizeof(uint32_t)>stride-idOffset ||
    nullOffset>stride || sizeof(uint32_t)>stride-nullOffset) return NULL;
  for (size_t i=0;i<fieldCount;i++)
  	if (!fields[i].size || fields[i].offset>stride || fields[i].size>stride-fields[i].offset)
    	return NULL;
  size_t buckets=count*2+1;
  if (buckets>SIZE_MAX/sizeof(size_t) || count>SIZE_MAX/sizeof(Entry)) return NULL;
  RecordHashTable *t=calloc(1,sizeof(*t)); if (!t) return NULL;
  t->stride=stride; t->nullOffset=nullOffset; t->fieldCount=fieldCount; t->bucketCount=buckets;
  t->fields=malloc(fieldCount*sizeof(*fields));
  t->buckets=malloc(buckets*sizeof(*t->buckets));
  if (!t->fields || !t->buckets) goto fail;
  memcpy(t->fields,fields,fieldCount*sizeof(*fields));
  for (size_t i=0;i<buckets;i++) t->buckets[i]=SIZE_MAX;
  if (count) { t->entries=calloc(count,sizeof(*t->entries)); if (!t->entries) goto fail; }
  for (size_t i=0;i<count;i++) {
    const unsigned char *row=(const unsigned char *)rows+i*stride;
    Entry *e=&t->entries[i];
    if (!MakeKey(t,row,&e->key,&e->length,&e->hash)) goto fail;
    t->count++;
    memcpy(&e->ID,row+idOffset,sizeof(e->ID));
    size_t b=e->hash%buckets; e->next=t->buckets[b]; t->buckets[b]=i;
  }
  return t;
fail: 
	FreeRecordHashTable(t); return NULL;
}

RecordMatch FindRecordID(const RecordHashTable *t,const void *query) {
  RecordMatch result={.status=RECORD_ERROR};
  if (!t || !query) return result;
  unsigned char *key; size_t length; uint64_t hash;
  if (!MakeKey(t,query,&key,&length,&hash)) return result;
  size_t b=hash%t->bucketCount, matches=0;

  for (size_t i=t->buckets[b];i!=SIZE_MAX;i=t->entries[i].next) {
    Entry *e=&t->entries[i];
    if (e->hash==hash && e->length==length && !memcmp(e->key,key,length)) matches++;
  }

  if (matches) {
		if (matches>SIZE_MAX/sizeof(*result.IDs)) { free(key); return result; }
		result.IDs=malloc(matches*sizeof(*result.IDs));
		if (!result.IDs) { free(key); return result; }
		for (size_t i=t->buckets[b];i!=SIZE_MAX;i=t->entries[i].next) {
		  Entry *e=&t->entries[i];
		  if (e->hash==hash && e->length==length && !memcmp(e->key,key,length))
		  	result.IDs[result.count++]=e->ID;
		}
	}
  result.status=matches>1?RECORD_DUPLICATE:matches?RECORD_FOUND:RECORD_NOT_FOUND;
  free(key); return result;
}

void FreeRecordMatch(RecordMatch *match) {
  if (!match) return;
  free(match->IDs); *match=(RecordMatch){.status=RECORD_NOT_FOUND};
}

