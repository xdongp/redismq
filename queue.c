#include <string.h>
#include <strings.h>
#include <stdio.h>
#include "queue.h"

queue* create_queue(unsigned int max_size){
	char *p = NULL;
	queue* q = (queue *)malloc(sizeof(queue));
	if (NULL == q){
		qerror("can not malloc queue memory,exit");
		return NULL;
	}

	q->mem_total = max_size;
	q->mem_used = 0;
	p = (char *)malloc(q->mem_total);
	if (NULL == p){
		qerror("can not malloc node memory,exit");
		return NULL;
	}
	
	q->start = q->head = q->tail = p; /*初始队列时，头尾相同*/
	q->end = q->start + q->mem_total;
	
	bzero(q->start, q->mem_total);
    pthread_mutex_init(&q->lock, NULL);/*初始化队列锁*/
	q->len = 0;
	
	return q;
}/*返回队列指针*/

int queue_reset(queue * const q){
	if(!q){
		return -1;
	}
	
   	pthread_mutex_lock(&q->lock);
	q->mem_used = 0;
	q->len = 0;
	q->head = q->tail = q->start;
	bzero(q->start, q->mem_total);
	printf("reset ================= : %d\n", q->mem_total);
   	pthread_mutex_unlock(&q->lock);
    return 0;
}

void dumperror(char *info){
	FILE *fp;
	fp = fopen("error-dump.log", "w");
	if(fp == NULL){
		printf("error exit %s", info);
	}
	fprintf(fp, "[error info][%s]\n", info);
	fclose(fp);
}

int queue_init(queue * const q, unsigned int mem_used, unsigned int len){
	if(!q){
		return -1;
	}
	
   	pthread_mutex_lock(&q->lock);
	q->end = q->start + q->mem_total;
	
	q->head = q->start;
	q->tail = q->start + mem_used;
	q->mem_used = mem_used;
	q->len = len;
   	pthread_mutex_unlock(&q->lock);
    return 0;
}

int safe_write(FILE *fp, char * const buf, unsigned int size){
	char *p  = buf;
	int n = size, ret;
	while( n > 0){	
		ret = fwrite(p, sizeof(char),n, fp);
		n -= ret;
		p += ret;
	}
	if(n == 0 ){
		return size;
	}else{
		return -1;
	}
}

int safe_read(FILE *fp, char * const buf, unsigned int size){
	char *p  = buf;
	int n = size, ret;
	while( n > 0){	
		ret = fread(p, sizeof(char), n, fp);
		n -= ret;
		p += ret;
	}

	if(n == 0 ){
		return size;
	}else{
		return -1;
	}
}

/* 文件保存是不安全的，可能不会完整 */
int queue_save(queue * const q, char const * filename){
	/*将文件存入队列，并且将队列重置*/
	FILE *fp;
	unsigned int first, second;
	if( (fp = fopen(filename,  "w")) == NULL){
    	qerror("Open file  error!\n");
		return -1;
	}
	
	/*保存队列数据内存的长度*/
	if(safe_write(fp, (char *)&q->mem_used, sizeof(unsigned int)) < 0){
		qerror("can not save queue len!");
		return -1;
	}

	/*保存队列的长度*/
	if(safe_write(fp, (char *)&q->len, sizeof(unsigned int)) < 0){
		qerror("can not save queue len!");
		return -1;
	}

	if( q->tail > q-> head){
		#ifdef DEBUG
			printf("[sec 1] save len : %d start: %d end:%d  tail: %d head %d\n", 
			q->tail - q->head, q->start, q->end, q->tail, q->head);
		#endif

		if(safe_write(fp, q->head, q->mem_used) < 0){
			qerror("can not save!");
			return -1;
		}
	}else{
		first = q->end - q->head;
		second = q->tail - q->start;
		if(safe_write(fp, q->head, first) < 0){
			qerror("can not save!");
			return -1;
		}

		if(safe_write(fp, q->start, second) < 0){
			qerror("can not save!");
			return -1;
		}

		#ifdef DEBUG
			printf("[sec 2] save len : %d start: %d end:%d  tail: %d head %d\n", 
			first+second, q->start, q->end, q->tail, q->head);
		#endif
	}
	
	queue_reset(q);
	fclose(fp);
	printf("save file to %s ok!\n", filename);	
	return 0;
}

/* 从磁盘中读出来数据也有可能是不完整的 */
int queue_load(queue * const q, char const * filename){
	/*将文件读入队列，并且恢复队列*/
	
	FILE *fp;
	unsigned int len, mem_used;
	if((fp = fopen(filename, "r")) == NULL){
    	qerror("Open file error!\n");
		return -1;
	}else{
		printf("load file name: %s\n", filename);
	}
	/* 初始化内存 */
	bzero(q->start, q->mem_total);
	
	printf("file read ok, memset ok");

	if(safe_read(fp, (char *)&mem_used, sizeof(unsigned int)) < 0){
		qerror("can not read queue len!");
		return -1;
	}

	if(safe_read(fp, (char *)&len, sizeof(unsigned int)) < 0){
		qerror("can not read queue len!");
		return -1;
	}
	
   	pthread_mutex_lock(&q->lock);
	/*读取队列数据*/
	if(safe_read(fp, q->start, mem_used) < 0){
		qerror("can not read queue data!");
		dumperror("can not read queue data!fatal error, maybe exist error data!");
		return -1;
	}
   	pthread_mutex_unlock(&q->lock);

	/*设置队列*/
	queue_init(q, mem_used, len);
	remove(filename);
	fclose(fp);
	return 0;
}


int cache_push(queue * const q, char * const s){
	/*从tail入队*/
	unsigned int first_len;	
	char *src = s;
	unsigned int len = strlen(s);

	/*1,判断队列能否放入此节� */
	if((q->mem_total - q->mem_used) < len + 1){
		qwarn("memory not enough,can not push!");
		return -1;
	}

	/*2,判断是否到了内存的末端*/
	first_len = q->end - q->tail;
	if(first_len < len){
		memcpy(q->tail, src, first_len);
		memcpy(q->start, (char *)(src + first_len), len  - first_len);
	}else{
		memcpy(q->tail, src, len);
	}
	
	q->tail = pmoven(q, q->tail, len);
	*q->tail = '\n';

	/* thread safe modify queue value*/
   	pthread_mutex_lock(&q->lock);
	q->mem_used += (len+1);
	q->len += 1;
   	pthread_mutex_unlock(&q->lock);

	q->tail = pmoven(q, q->tail, 1);
	#ifdef DEBUG
		printf("push ok, queue_len:%d, elem-len: %d, free: %d, used: %d,total:%d\n", 
		q->len, len, queue_mem_free(q), q->mem_used, q->mem_total);
	#endif
	return 0;
}/*入队，成功返回值大于0*/


int cache_pop(queue* const q, char *const dst){
	/*炒觝ead中出对*/
	char *p = q->head;	
	char *d = dst;
	int len = 0;

	/*1,判读队列是否为空*/	
	if( empty(q) < 0){
		qwarn("queue is empty!");
		//printf("queue is empty!\n");
		return -1;
	}
	
	/*2,从head开始，寻找'\n',并将'\n'的数据复制到ds   */
	while( *p != '\n'){
		/* 插入时不会出现 \0, 读出是也应该不出现 */		
		if( *p == 0 )
			goto fatal_error;

		*d = *p;
		d++;
		len++;
		p = pmoven(q, p, 1);
	}
		/* 将数组的尾部设置成'\0' */
		*++d = '\0';
		q->head = pmoven(q, p, 1);
	
		/* thread safe modify queue value*/
   		pthread_mutex_lock(&q->lock);
		q->mem_used = q->mem_used - len - 1;
		q->len -= 1;
   		pthread_mutex_unlock(&q->lock);
		
		#ifdef DEBUG
			printf("pop ok, queue_len:%d, elem-len: %d, free: %d, used: %d,total:%d\n", 
			q->len, len, queue_mem_free(q), q->mem_used, q->mem_total);
		#endif
		return (len != 0) ? len : -1;

fatal_error:
		dumperror("pop error, exist '\0', init queue!\n");
		queue_init(q, 0, 0);
		return -1;
}

int cache_read(queue* const q, char *const dst){
	char *p = q->head;	
	char *d = dst;
	int len = 0;

	/*1,判读队列是否为空*/	
	if( empty(q) < 0){
		qwarn("queue is empty!");
		printf("queue is empty!\n");
		return -1;
	}
	
	/*2,从head开始，寻找'\n',并将'\n'的数据复制到ds   */
	while( *p != '\n'){
		if( *p == 0 )
			goto fatal_error;

		*d = *p;
		d++;
		len++;
		p = pmoven(q, p, 1);
	}
		/* 将数组的尾部设置成'\0' */
		*++d = '\0';

		//q->head = pmoven(q, p, 1);
	
		/* thread safe modify queue value*/
   		//pthread_mutex_lock(&q->lock);
		//q->mem_used = q->mem_used - len - 1;
		//q->len -= 1;
   		//pthread_mutex_unlock(&q->lock);
		
		#ifdef DEBUG
			printf("read ok, not change queue data!\n");
		#endif
		return (len != 0) ? len : -1;

fatal_error:
		dumperror("pop error, exist '\0', init queue!\n");
		queue_init(q, 0, 0);
		return -1;
}


inline int empty(queue* const q){
	if(q->len <= 0){
		return -1;
	}else{
		return 0;
	}
}


inline char* pmoven(queue* const q, char *p, unsigned int n){
	p = q->start + (p - q->start + n) % q->mem_total;
	return p; 
}


inline unsigned int queue_len(queue* const q){
	return q->len;
}


inline unsigned int queue_mem_free(queue* const q){
	return q->mem_total - q->mem_used;
}


inline unsigned int queue_mem_total(queue* const q){
	return q->mem_total;
}


int destroy_queue(queue* q){
	if(q){
		if(q->start){
			free(q->start);
		}
		free(q);
		q = NULL;
	}
	return 0;
}

/*
int print_node(node *node)
{
    int i;
	char *src = node->data;
	unsigned int len = node->len;
    printf("--------c ------:");
    for(i = 0; i < len; i++)
        printf("%c", src[i]);
    printf("\n");

}


node *create_node(int size){
	node *p;
	p = (node *)malloc(sizeof(node));
	p->data = (char *)malloc(size);
	p->len = size;
	return p;
}

int main(){
	queue * q;
	int i, len;
	node *src, *dst;
	char *p;
	printf("00\n");
	src = create_node(10);
	dst = create_node(10);
	

	printf("11\n");	
	q = create_queue(100);
	printf("aa\n");	

	p = src->data;
	for(i=0; i< 10; i++)
	{
		*p++ = i + 'a';
		
	}

	printf("bb\n");	

	for(i=0; i< 11; i++)
	{
		*src->data = i + 'A';
		push(q, src);
      
	}

	printf("cc\n");	
	
	for(i=0; i< 15; i++)
	{
		len = pop(q, dst);
		print_node(dst);
	}
} */
