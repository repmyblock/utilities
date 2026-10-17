#include "RecordLoader.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <errno.h>

static int ReadNumber(const char *s, unsigned char *out, size_t size) {
  if (!s || !*s || strspn(s,"0123456789")!=strlen(s)) return 0;
  errno=0; char *end; uintmax_t value=strtoumax(s,&end,10);
  if (errno || *end) return 0;
  if (size==sizeof(uint8_t) && value<=UINT8_MAX) {
	  uint8_t n=(uint8_t)value; memcpy(out,&n,sizeof n);
  } else if (size==sizeof(uint16_t) && value<=UINT16_MAX) {
    uint16_t n=(uint16_t)value; memcpy(out,&n,sizeof n);
  } else if (size==sizeof(uint32_t) && value<=UINT32_MAX) {
    uint32_t n=(uint32_t)value; memcpy(out,&n,sizeof n);
  } else return 0;
  return 1;
}

int LoadRecordArray(MYSQL *db,const char *query,size_t stride,size_t idOffset,
 										size_t nullOffset,const RecordField *fields,size_t fieldCount,
 										void **output,size_t *count) {
  if (!output || !count) return EXIT_FAILURE;
  *output=NULL; *count=0;
  if (!db || !query || !fields || !stride || fieldCount>31) return EXIT_FAILURE;
  
  if (mysql_set_character_set(db,"utf8mb4") || mysql_query(db,query)) {
  	fprintf(stderr,"Module query failed: %s\n",mysql_error(db)); return EXIT_FAILURE;
  }
  
  MYSQL_RES *result=mysql_use_result(db);
  if (!result) { fprintf(stderr,"Module result failed: %s\n",mysql_error(db)); return EXIT_FAILURE; }
  unsigned char *data=NULL; size_t used=0,capacity=0;
  if (mysql_num_fields(result)!=fieldCount+1) goto fail;
  MYSQL_ROW row;
  
  while ((row=mysql_fetch_row(result))) {
		if (used==capacity) {
		  size_t max=SIZE_MAX/stride;
		  if (capacity==max) goto fail;
		  size_t next=capacity?capacity+capacity/2:1024;
		  if (next<capacity || next>max) next=max;
		  void *grown=realloc(data,next*stride); if (!grown) goto fail;
		  data=grown; capacity=next;
		}
		unsigned long *lengths=mysql_fetch_lengths(result); if (!lengths) goto fail;
		unsigned char *record=data+used*stride; memset(record,0,stride);
		if (!ReadNumber(row[0],record+idOffset,sizeof(uint32_t))) goto fail;
		uint32_t nulls=0;
		for (size_t i=0;i<fieldCount;i++) {
		  const RecordField *f=&fields[i]; const char *value=row[i+1];
		  if (!value) { nulls|=UINT32_C(1)<<(i+1); continue; }
		  if (f->text) {
	      if (lengths[i+1]>=f->size || memchr(value,'\0',lengths[i+1])) goto fail;
	      memcpy(record+f->offset,value,lengths[i+1]);
	  	} else if (!ReadNumber(value,record+f->offset,f->size)) goto fail;
		}
		memcpy(record+nullOffset,&nulls,sizeof nulls); used++;
	}
	
  if (mysql_errno(db)) goto fail;
  mysql_free_result(result);
  if (used && used<capacity) { void *p=realloc(data,used*stride); if(p) data=p; }
  *output=data; *count=used; return EXIT_SUCCESS;
  
fail:
  fprintf(stderr,"Module load failed at row %zu (invalid data or allocation/read failure): %s\n",used+1,mysql_error(db));
  mysql_free_result(result); 
  free(data); 
  return EXIT_FAILURE;
}
