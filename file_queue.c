#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include "queue.h"
#include "file_queue.h"


/* 生成队列 */
file_queue* new_file_queue(char *name, unsigned int size, char *filepath){
	char cmd[120];
    int ret;
	file_queue *q = (file_queue *)malloc(sizeof(file_queue));
	if( !q){
		qerror("can not alloc memory for create queue!\n");
		return NULL;
	}

	q->queue_name = name;
	q->head_cache = create_queue(size);
	if( !q->head_cache){
		qerror("can not alloc memory for create queue!\n");
		return NULL;
	}

	q->tail_cache = create_queue(size);
	if( !q->tail_cache){
		qerror("can not alloc memory for create queue!\n");
		return NULL;
	}

	q->file_queue = create_queue(FILENAME_QUEUE_SIZE);
	if( !q->file_queue){
		qerror("can not alloc memory for create queue!\n");
		return NULL;
	}

	/*初始化文件个数*/
	q->file_counter = 0;
	/*初始化队列长度*/
	q->len = 0;
	strcpy(q->file_path, filepath);
	strcat(q->file_path, "/");
	strcat(q->file_path, name);
	strcat(q->file_path, "/");

	/* 创建队列目录,在创建之前，先删除目录*/
  	if(access(q->file_path, R_OK|W_OK|X_OK) == 0){
		rmdir(q->file_path);
		strcpy(cmd, "rm -rf ");
		strcat(cmd, q->file_path);
		ret = system(cmd);
        if(ret < 0) qerror("remove dir error!\n");
	}
  	
    if(mkdir(q->file_path,  0755) == -1){     
         	qerror("mkdir error\n");   
            return  NULL;   
      	}else{
			printf("create dir: %s ok\n", q->file_path);
	}
	
	/*初始化文件名的起始*/
	q->filename_seed = FILE_START;
    pthread_mutex_init(&q->lock, NULL);/*初始化队列锁*/
	return q;	

}

unsigned  int file_queue_len(file_queue * const q){
	return q->len;
}

unsigned int file_queue_mem(file_queue * const q){
	return q->head_cache->mem_used + q->tail_cache->mem_used;
}

unsigned int file_number_in_queue(file_queue * const q){
	return q->file_counter;
}

/* 整数到字符转换 */
char* itoa(int i, char *s){
	char n, *p = s; 
	while(i){
		n = i % 10;
		*p++ = n + '0';
		i /= 10;
	}
	*p = '\0';
	
	strrev(s);
	return s;
}

/* 字符串反转 */
char* strrev(char *s){
    char temp, *end = s + strlen(s) - 1;
    while( end > s){
        temp = *s;
        *s = *end;
        *end = temp;
        --end;
        ++s;
    }
	return s;
}

/* 获取一个文件名 */
char* get_a_new_name(file_queue * const q, char * const fullpath){
	unsigned int i;
	char filename[10];
	while(1){
		bzero(filename, 10);
		i = q->filename_seed;
		q->filename_seed++;
		itoa(i, filename);
		if( index(filename, '\n') == NULL)
			break;
	}
	strcpy(fullpath, q->file_path);
	strcat(fullpath, filename);
	#ifdef DEBUG
		printf("gen file name: %s \n", fullpath);
	#endif
	return fullpath;
}

/* 原子加 */
inline void autoadd(file_queue * const q, unsigned int *v){
		pthread_mutex_lock(&q->lock);
		*v = *v + 1;
		pthread_mutex_unlock(&q->lock);
}

/* 原子减 */
inline void autosub(file_queue * const q, unsigned int *v){
		pthread_mutex_lock(&q->lock);
		*v = *v - 1;
		pthread_mutex_unlock(&q->lock);
}

int push(file_queue* const q, char * const src){
	char fullpath[FILENAME_SIZE+10];
	
	/* 队尾未满， 直接插入 */
	if(cache_push(q->tail_cache, src) >= 0){
		autoadd(q, &q->len);
		#ifdef DEBUG
   			printf("push ok, not save, len: %d\n", q->len);
		#endif

		return 0;
	}else{

		/* 存入文件 */
		get_a_new_name(q, fullpath);
		/* 先将文件名放入队列 */

		/* 考虑到保存文件时，出错可能大于 push 文件队列,因此先 save file */
		if(queue_save(q->tail_cache, fullpath) < 0){
			qerror(" can not save queue to file!\n")
			dumperror(" can not save queue to file, but filename have push to filename queue!");
			return -1;
		}else{
			printf("save file ok, filename :%s\n", fullpath);
			if(cache_push(q->file_queue, fullpath) <0 ){
				qerror("can not put file name to file_name queue, may file queue buffer is not enough, fatal error!\n");
				dumperror("can not put file name to file_name queue, fatal error!");
				return -1;
			}else{
				autoadd(q, &q->file_counter);
			}
		}
		

		/* 再次插入 */
		if( cache_push(q->tail_cache, src) >= 0){
			
			autoadd(q, &q->len);
			#ifdef DEBUG
   				printf("push ok, have save, len: %d\n", q->len);
			#endif
			return 0;
		}else{
			return -1;
		} 
	}
}

int pop(file_queue* const q, char *const dst){
	char fullpath[FILENAME_SIZE+10];

	/*头队列中有数据，直接读取 */
	if(cache_pop(q->head_cache, dst) >= 0){
		autosub(q, &q->len);
		#ifdef DEBUG
   			printf("pop ok, not load, len: %d\n", q->len);
		#endif
		return 0;
	}else{
		/* 如果有文件，从文件中读取*/
		if(q->file_counter > 0){
	
			/* 从队列中进入的buf一定要初始化为0 */
			bzero(fullpath, FILENAME_SIZE);
			if( cache_read(q->file_queue, fullpath) < 0){
				qerror("can not pop filename, but file_count > 0, fatal error!\n");
				dumperror("can not pop filename, but file_count > 0, fatal error!");
				#ifdef DEBUG
					printf("filename queue len: %d, mem_used: %d\n", q->file_queue->len, q->file_queue->mem_used);
				#endif
				return -1;
			}

			/* 为了实现事务操作 */
			if(queue_load(q->head_cache, fullpath) < 0){
				printf("can not load file [%s] to queue!\n", fullpath);
				dumperror("can not load file to queue!");
				return -1;
			}else{
				if( cache_pop(q->file_queue, fullpath) < 0){
					dumperror("file have load, but can not pop filename, fatal error!\n");
				}else{
					autosub(q, &q->file_counter);
				}

			}
		
			if(cache_pop(q->head_cache, dst) >= 0){
				autosub(q, &q->len);
   				printf("pop ok, load file, len: %d\n", q->len);
				return 0;
			}else{
				return -1;
			}
		}else{
			/* 从尾部获取 */
			if(cache_pop(q->tail_cache, dst) >= 0){
				autosub(q, &q->len);
				#ifdef DEBUG
   					printf("pop ok, read tail, len: %d\n", q->len);
				#endif
				return 0;
			}else{
				#ifdef DEBUG
					printf("tail cache pop error\n");
				#endif
				return -1;
			}
		}
	}	
}

int del_file_queue(file_queue *q){
	if( !q ){
		return -1;
	}
	
	destroy_queue(q->head_cache);
	destroy_queue(q->file_queue);
	destroy_queue(q->tail_cache);
	free(q);
	return 0;
}
