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

/* 文件队列的长度，大约可以存 18000个文件左右 */
#define FILENAME_QUEUE_SIZE (1024 * 1024 *10) 
/* 文件的起始生成器 */
#define FILE_START 100000
/* 文件名的缓存大小 */
#define FILENAME_SIZE 100

typedef struct{
char* queue_name;  //队列的名字
queue* head_cache;  //头队列
queue* tail_cache;   //尾队列
queue* file_queue;   //中间文件队列
unsigned int file_counter;     //文件个数
unsigned int  len;   //队列长度
unsigned int  filename_seed;
char file_path[FILENAME_SIZE];
pthread_mutex_t lock;
}file_queue;


/* 公共函数 */
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
