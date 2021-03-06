#include "ltypes.h"
#include "lmem.h"
#include "larray.h"
#include "lsearch.h"
#include "ltricky.h"

#include <string.h>
#include <assert.h>

LArray *l_array_new(int count,int size)
{
	LArray *array;
	array=l_new0(LArray);
	array->count=count;
	array->size=size;
	if(size>0)
	{
		array->data=l_alloc(array->count*array->size);
	}
	return array;
}

void l_array_free(LArray *array,LFreeFunc func)
{
	int i;
	if(!array) return;
	if(func && array->data) for(i=0;i<array->len;i++)
	{
		func(array->data+i*array->size);
	}
	l_free(array->data);
	l_free(array);
}

static void l_array_expand(LArray *array)
{
	int v1,v2;
	if(array->count-array->len>=1)
		return;
	v1=array->len+1;
	v2=array->count*5/4;
	array->count=v1>v2?v1:v2;
	array->data=l_realloc(array->data,array->size*array->count);
}

void l_array_append(LArray *array,const void *val)
{
	l_array_expand(array);
	memcpy(array->data+array->len*array->size,val,array->size);
	array->len++;
}

void l_array_insert(LArray *array,int n,const void *val)
{
	char *orig;
	l_array_expand(array);
	orig=array->data+array->size*n;
	memmove(orig+array->size,orig,(array->len-n)*array->size);
	memcpy(orig,val,array->size);
	array->len++;
}

int l_array_remove(LArray *array,int n)
{
	char *orig;
	if(n<0 || n>=array->len)
		return -1;
	orig=array->data+array->size*n;
	memmove(orig,orig+array->size,(array->len-n-1)*array->size);
	array->len--;
	return 0;
}

#if 0
void l_array_insert_sorted(LArray *array,const void *val,LCmpFunc cmpar)
{
	int i;
	for(i=0;i<array->len;i++)
	{
		void *orig=l_array_nth(array,i);
		int ret=cmpar(orig,val);
		if(ret<=0) continue;
		l_array_insert(array,i,val);
		return;
	}
	l_array_append(array,val);
}
#else
void l_array_insert_sorted(LArray *array,const void *val,LCmpFunc cmpar)
{
	int pos;
	pos=l_bsearch_right(val,array->data,array->len,array->size,cmpar);
	if(pos==array->len)
		l_array_append(array,val);
	else
		l_array_insert(array,pos,val);
}
#endif

void l_ptr_array_append(LArray *array,const void *val)
{
	l_array_append(array,&val);
}

void l_ptr_array_insert(LArray *array,int n,const void *val)
{
	l_array_insert(array,n,&val);
}

void l_ptr_array_free(LArray *array,LFreeFunc func)
{
	int i;
	if(!array) return;
	if(func) for(i=0;i<array->len;i++)
	{
		func(*(void**)(array->data+i*array->size));
	}
	l_free(array->data);
	l_free(array);
}
