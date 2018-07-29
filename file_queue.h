#ifndef FILE_QUEUE_H_
#define FILE_QUEUE_H_
/*#define DEBUG */
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include <string.h>
#include <linux/types.h>
#include <pthread.h>
#include "queue.h"

/* �ļ����еĳ��ȣ���Լ���Դ� 18000���ļ����� */
#define FILENAME_QUEUE_SIZE (1024 * 1024 *10) 
/* �ļ�����ʼ������ */
#define FILE_START 100000
/* �ļ����Ļ����С */
#define FILENAME_SIZE 100

typedef struct{
char* queue_name;  //���е�����
queue* head_cache;  //ͷ����
queue* tail_cache;   //β����
queue* file_queue;   //�м��ļ�����
unsigned int file_counter;     //�ļ�����
unsigned int  len;   //���г���
unsigned int  filename_seed;
char file_path[FILENAME_SIZE];
pthread_mutex_t lock;
}file_queue;


/* �������� */
char* itoa(int i, char *s);
char* strrev(char *s);
char* get_a_new_name(file_queue * const q, char * const fullpath);
void autoadd(file_queue * const q, unsigned int *v);
void autosub(file_queue * const q, unsigned int *v);
/* end */

file_queue* new_file_queue(char *name, unsigned int size, char *file_path);
int del_file_queue(file_queue *q);
int push(file_queue* const  q, char * const src);
int pop(file_queue* const q, char *const dst);
unsigned int file_queue_mem(file_queue * const q);

#endif /*FILE_QUEUE_H_*/
