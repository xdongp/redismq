#include "queue.h"

struct queue* new_queue(char *name){
  struct queue* q;
  struct cache_queue* cq;

  q = (struct queue*)malloc(sizeof(struct queue));
  cq = new_cache_queue();
  q->queue_name=name;
  q -> head_cache = q -> tail_cache = cq;
  q -> file_queue = new_file_queue();
  q -> file_counter = 0;
  return q;
}

struct cache_queue* new_cache_queue(){
  struct cache_queue* cq;
  cq = (struct cache_queue*)malloc(sizeof(struct cache_queue));
  cq -> head = cq -> tail = NULL;
  cq -> node_num = 0;
  return cq;
}

void cache_push(struct cache_queue* cq, char* dt){
  struct node* tmp;
  struct node* p;

  tmp = (struct node*)malloc(sizeof(struct node));
  tmp -> data = dt;
  tmp -> next = NULL;

  if((cq -> head == cq -> tail) && (cq -> head == NULL)){
    cq -> head = cq -> tail = tmp;
    cq -> node_num = 1;
  }
  else{
    p = cq -> tail;
    p -> next = tmp;
    cq -> tail = tmp;
    cq -> node_num ++;
  }
}
void pushhead(struct queue* q, char* dt)
{
	cache_push(q ->head_cache, dt);
}

void pushtail(struct queue* q, char* dt)
{
	cache_push(q ->tail_cache, dt);
}

void push(struct queue* q, char* dt,char *file_path){
  if(q -> file_counter == 0){
    if(q -> tail_cache -> node_num == CACHE_LIMIT) // we need to save
      {
	char* name = get_a_new_name(q,file_path);
	add_file(q -> file_queue, name);
	q -> file_counter ++;
	save_cache_queue_to_file(name,q->tail_cache);
	q -> tail_cache = new_cache_queue();
	q -> head_cache = new_cache_queue();
	cache_push(q -> tail_cache, dt);
      }
    else cache_push(q -> tail_cache, dt);
  }
  else{
    if(q -> tail_cache -> node_num == CACHE_LIMIT) // we need to save
      {
	char* name = get_a_new_name(q,file_path);
	add_file(q -> file_queue, name);
	q -> file_counter ++;
	save_cache_queue_to_file(name,q->tail_cache);
	q -> tail_cache = new_cache_queue();
	q -> head_cache = new_cache_queue();
	cache_push(q -> tail_cache, dt);
      }
    else cache_push(q -> tail_cache, dt);
  }    
}

char* cache_pop(struct cache_queue* cq){
  struct node* p;
  char *res = NULL;

  	if(cq -> head == cq -> tail)     // only one node
    {
      p = cq -> head;
      res = p -> data;
      cq -> head = cq -> tail = NULL;
      cq -> node_num = 0;
      free(p);
    }
  	else{
    	p = cq -> head;
    	cq -> head = p -> next;
    	cq -> node_num --;
    	res = p -> data;
    	free(p);
  	}
  return res;
}

char* pop(struct queue* q){
  char *res = NULL;
  if(q -> file_counter == 0){
    if(q -> head_cache -> node_num > 0)
    {
      res = cache_pop(q -> head_cache);
    }
    else if(q ->tail_cache->node_num > 0)
    {
       res = cache_pop(q ->tail_cache);
       printf("start pop tail::::%s",res);
    }  
    else res = NULL;
  }
  else{
    if(q -> head_cache -> node_num > 0)
      res = cache_pop(q -> head_cache);
    else{
      char* name = get_file_name(q -> file_queue);
      	//printf("%s+++++++\n",q ->tail_cache);
      	q -> head_cache = read_cache_queue_from_file(name);
      	remove(name); // remove the file from disk.
      	q ->file_counter--;
      	//printf("%s\n",name);
      	res = cache_pop(q -> head_cache);

      //printf("%s\n",name);
    }
  }
  return res;
}


char* get_a_new_name(struct queue* q,char *file_path){
  char* name = (char*)malloc(sizeof(char)*100);
  int dir_num=q->file_queue->file_num/FILE_LIMIT;
  sprintf(name,"%s%i/",file_path,dir_num);
  if(q->file_queue->file_num%FILE_LIMIT == 0)
  {
  	 if(mkdir(name, 0777) == -1){
    	perror("Couldn't create the directory.\n");
  	}
  }
  sprintf(name,"%s%i/queue_%i",file_path,dir_num,q->file_counter);
  return name;
}

void save_cache_queue_to_file(char* filename, struct cache_queue* cq){
  struct node* p;

/*
  std::ofstream outfile(filename,std::ios::out|std::ios::app);
  for(p = cq -> head; p; p = p -> next){
    outfile<<p -> data<<std::endl;
    free(p);
  }
  free(cq);
  cq = NULL;
  outfile.close();
  */

  FILE* outfile = fopen(filename, "w");

  for(p = cq -> head; p; p = p -> next){
    fprintf(outfile,"%s\n", p -> data);
    free(p);
  }
  free(cq);
  cq = NULL;
  
  fclose(outfile);//此处。。。。。。
    printf("%s\n",filename);
}

struct cache_queue* read_cache_queue_from_file(char* filename){
  struct cache_queue* res;

	

  //printf("%s.....\n",filename);
  FILE* infile = fopen(filename, "r");


  res = new_cache_queue();

  while(!feof(infile)){
  	char* dt=NULL;
  	dt=(char*)malloc(sizeof(char)*65536);
  	//memset(dt,'\n',65536);
    fscanf(infile,"%s\n",dt);
    cache_push(res,dt);
  }
  //printf("%s\n",filename);
  return res;
}

struct file_queue* new_file_queue(){
  struct file_queue* q;
  
  q = (struct file_queue*)malloc(sizeof(struct file_queue));
  q -> file_num = 0;
  q -> head = q -> tail = NULL;
  return q;
}

void add_file(struct file_queue* q, char* filename){
  struct file_node* tmp;
  struct file_node* p;
  
  tmp = (struct file_node*)malloc(sizeof(struct file_node));
  tmp -> name = filename;
  tmp -> next = NULL;
  
  if((q -> head == q -> tail) && (q -> head == NULL))
    q -> head = q -> tail = tmp;
  else{
    p = q -> tail;
    p -> next = tmp;
    q -> tail = tmp;
  }
  q->file_num+=1;
}

char* get_file_name(struct file_queue* q){
  struct file_node* p;
  char* res = NULL;
  
  if((q -> head == q -> tail) && (q -> head == NULL))
    res = NULL;
  else if (q -> head == q -> tail)
    {
      p = q -> head;
      res = p -> name;
      q -> head = q -> tail = NULL;
      q->file_num--;
      free(p);
    }
  else{
    p = q -> head;
    q -> head = p -> next;
    q->file_num--;
    res = p -> name;
    free(p);
  }
  return res;
}


