#ifndef QUEUE_H_
#define QUEUE_H_
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include <string.h>
#include <pthread.h>

/*#define DEBUG  */
#define qerror(str) do{ fprintf(stderr, "ERROR:%s\n", str); }while(0);
#define qwarn(str) do{  /*	printf("WARN:%s\n", str)*/; }while(0);

typedef struct queue{
	char* start; /*���е��ڴ���ʼ��ַ*/
	char* end; /*���е��ڴ���ֹ��ַ*/
	char* head; /*����ͷ*/
	char* tail;   /*����β*/
	unsigned int  mem_total; /*���е�������Ҳ���Ƕ��е��ڴ��С*/
	unsigned int  mem_used; /*�������ڴ��ʹ����*/
	unsigned int  len;     /*���г���*/
    pthread_mutex_t lock;
}queue;


void dumperror(char *info);
queue* create_queue(unsigned int max_size);
int cache_push(queue* const q, char* const src);
int cache_pop(queue* const  q, char* const dst);
int cache_read(queue* const q, char* const dst);
int destroy_queue(queue* q);
int empty(queue* const q);
char* pmoven(queue* const q, char *p, unsigned int n);
unsigned int queue_len(queue* const q);
unsigned int queue_mem_free(queue* const q);
unsigned int queue_mem_total(queue* const q);
int safe_write(FILE *fp, char * const buf, unsigned int size);
int safe_read(FILE *fp, char * const buf, unsigned int size);
int queue_save(queue * const q, char const *filename);
int queue_load(queue * const q, char const *filename);

#endif /*QUEUE_H_*/
