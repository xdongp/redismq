#ifndef QUEUE_H_
#define QUEUE_H_
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include <string.h>
#define CACHE_LIMIT 2
#define FILE_LIMIT 2


// the data queue and node
typedef struct node{
  char* data;
  struct node* next;
}node;

struct cache_queue{
  int node_num;
  struct node* head;
  struct node* tail;
};

// the queue and node for filenames.
struct file_node{
  char* name;
  struct file_node* next;
};

struct file_queue{
  int file_num;
  struct file_node* head;
  struct file_node* tail;
};

struct queue{
  char * queue_name;
  struct cache_queue* head_cache;
  struct cache_queue* tail_cache;
  struct file_queue* file_queue;
  int file_counter;
};


struct cache_queue* new_cache_queue();
struct queue* new_queue(char *name);
struct queue* init(char *file_path);
char* get_a_new_name(struct queue* q,char *file_path);
void save_cache_queue_to_file(char* filename, struct cache_queue* cq);
struct cache_queue* read_cache_queue_from_file(char* filename);
struct file_queue* new_file_queue();
void add_file(struct file_queue* q, char* filename);
char* get_file_name(struct file_queue* q);
void push(struct queue* q, char* dt,char *file_path);
char* pop(struct queue* q);
void pushhead(struct queue* q, char* dt);
void pushtail(struct queue* q, char* dt);
#endif /*QUEUE_H_*/
