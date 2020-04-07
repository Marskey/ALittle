
#ifndef _ALITTLE_CSV_H_
#define _ALITTLE_CSV_H_

#include "../klib/kstring.h"
#include "../klib/kvec.h"

typedef kvec_t(kstring_t*) csvrow;
typedef kvec_t(csvrow*) csvdata;

typedef struct
{
	kstring_t* path;
	csvdata data;
	int row_count;
	int col_count;
} csv;

csv* csv_create();
void csv_init(csv* c);
void csv_destroy(csv* c);
void csv_clear(csv* c);

// ��������
int csv_load(csv* c, const char* path, kstring_t* error);
// ��ȡ·��
const char* csv_getpath(csv* c);
// ��ȡ��λ
const char* csv_readcell(csv* c, int row, int cell);
// ��ȡ������
int csv_rowcount(csv* c);
// ��ȡ������
int csv_colcount(csv* c);

#endif // _ALITTLE_CSV_H_
