#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <time.h>

#include "afl-ijon-min.h"

ijon_input_info* new_ijon_input_info(char* max_dir, int i){
  ijon_input_info* self = malloc(sizeof(ijon_input_info));
  assert(asprintf(&self->filename,"%s/%d", max_dir, i)>0);
  self->slot_id = i;
	self->len = 0;
  return self;
}

ijon_min_state* new_ijon_min_state(char* max_dir) {
  ijon_min_state* self = malloc(sizeof(ijon_min_state));
  self->max_dir = max_dir;
  self->num_entries = 0;
	self->num_updates = 0;
  for(int i = 0; i< MAXMAP_SIZE; i++){
    self->max_map[i] = 0;
    self->infos[i]=new_ijon_input_info(max_dir, i);
  }
  return self;
}


// u8 ijon_should_schedule(ijon_min_state* self){
u8 ijon_should_schedule(int count){
  printf("[ijon_should_schedule]: now we have  %d\n", count);
  if(count > 0) {
    return random()%100 > 20;
  }
  return 0;
}

ijon_input_info* ijon_get_input(ijon_queue_t* shared){
  //printf("[ijon_get_input]: altogether %d\n", shared->count);
  ijon_input_info *info = malloc(sizeof(ijon_input_info));

  // fill in filename and len
  uint32_t rnd = random()%(shared->count); 
  //printf("[ijon_get_input]: altogether %d, rnd=%d\n", shared->count, rnd);
  Node *cur = shared->tscs_by_index;
  //printf("head node file: %s\n", cur->filename);
  for(int i = 0; i<rnd; i++){
    // if(rnd == 0){
    //   assert(asprintf(&info->filename, "%s", cur->filename)>0);
    //   info->len = cur->len;
    // }
    // rnd -= 1;
    cur = cur->next;
  }
  assert(asprintf(&info->filename, "%s", cur->filename)>0);
  info->len = cur->len;
  printf("[ijon_get_input]: altogether %d, pick %d, %s\n", shared->count, rnd, info->filename);
  return info;
  //printf("Altogether %d entries to pick from\n", self->num_entries);
  //uint32_t rnd = random()%self->num_entries; 
  // for(int i = 0; i<MAXMAP_SIZE; i++){
  //   if(self->max_map[i]>0){
  //     if(rnd==0){
  //       printf("schedule: %i %s\n",i, self->infos[i]->filename);
  //       return self->infos[i];
  //     }
  //     rnd-=1;
  //   }
  // }
  // printf("schedule: %i %s\n",rnd, self->infos[rnd]->filename);
  // return self->infos[rnd];

  //return 0;
}

void ijon_store_max_input(ijon_min_state* self, int i, uint8_t* data, size_t len, int index, int seed_id, uint64_t distance, Node **head_ref){
  ijon_input_info* inf = self->infos[i];
  inf->len = len;
  // int fd = open(inf->filename, O_CREAT|O_TRUNC|O_WRONLY,0600);
  // printf("write to file: %s\n", inf->filename);
  // assert(write(fd,data,len) == len);
  // close(fd);
	
	char* filename = NULL;
	//assert(asprintf(&filename, "%s/finding_%lu_%lu", self->max_dir, self->num_updates, time(0)) > 0);
  assert(asprintf(&filename, "%s/id:%06d_%d_%llx", self->max_dir, i, index, distance) > 0);
  assert(asprintf(&inf->filename, "%s", filename) > 0);

	self->num_updates+=1;
  int fd = open(filename, O_CREAT|O_TRUNC|O_WRONLY,0600);
  //printf("write to file: %s, attached: %s, count: %d\n", filename, inf->filename, i);
  assert(write(fd,data,len) == len);
  close(fd);

  // now we have a new file written to the ijon_max queue to schedule, update the linked list 
  Node *new_node = (Node *)malloc(sizeof(Node));
  assert(asprintf(&new_node->filename, "%s", filename)>0);
  new_node->index = index;
  new_node->len = len;

  // insert by index number
  Node *cur = *head_ref;
  //Node *tmp;

  // as head
  if (cur == NULL || index <= cur->index) {
    new_node->next = *head_ref;
    *head_ref = new_node;
  }  
  else {
    // locate the node before the new node
    while(cur->next != NULL && cur->next->index < index) {
      cur = cur->next;
    }  
    new_node->next = cur->next;
    cur->next = new_node;
  }
  FILE *fp = fopen("/data/debug.log", "a+");
  fprintf(fp, "[Node insertion - %d]: file: %s\n", i, filename);
  fclose(fp);
	free(filename);
}

/*
Factor: <minimum> distance of all watch point.
TODO: change mini to mean(), max(), etc. 
*/
int ijon_update_max(ijon_min_state* self, ijon_queue_t* ijon_q, shared_data_t* shared, uint8_t* data, size_t len, int seed_id){
  int num_kept = 0;
	int should_min = (len>512) ;
  //uint64_t minidist = 0xffffffffffffffff;
  int minindex = -2;
  //int savei = 0;
  // find the minimum of all local distances and save by that index ?
  for(int i = 0; i < MAXMAP_SIZE; i++){ 
    // a progress at index i, the distance becomes smaller globally as well. 
    if(shared->afl_max[i] > self->max_map[i]) {// || shared->afl_max[i] == 0xffffffffffffffff){
      self->max_map[i] = shared->afl_max[i];
      ijon_store_max_input(self, ijon_q->count, data, len, i, seed_id, 0xffffffffffffffff - shared->afl_max[i], &ijon_q->tscs_by_index);
      num_kept += 1;
      ijon_q->count += 1;

      // // small test here!
      // Node *cur = ijon_q->tscs_by_index;
      // Node *temp;

      // if (cur != NULL) {
      // //   temp = cur -> next;
      //   char *fname = basename(cur->filename);
      //   printf( "[check]: %s, %s\n", cur->filename, fname);

      //   // assert(asprintf(&inf->filename, "%s", filename) > 0);
      //   char newcpy[20];
      //   memcpy(newcpy, fname, strlen(fname));
      //   char *ptr = strtok(newcpy, "_");
      //   ptr = strtok(NULL, "_");
      //   printf( "[cont-sens-id]: %s\n", ptr);
      // }

      
      // if(0xffffffffffffffff - shared->afl_max[i] < minidist && shared->afl_max[i] != 0xffffffffffffffff) {
      //   minidist = 0xffffffffffffffff - shared->afl_max[i];
      //   minindex = shared->aif_index[i];
      //   //savei = i;
      // }
      // else if(shared->afl_max[i] == 0xffffffffffffffff) {
      //   // a within range case!
      //   ijon_store_max_input(self, ijon_q->count, data, len, shared->aif_index[i], seed_id, shared->afl_max[i], &ijon_q->tscs_by_index);
      //   num_kept += 1;
      //   // self->num_entries++;
      //   return num_kept;
      // }
    }
/*
    else if(should_min && (shared->afl_max[i] == self->max_map[i] ) && ( len < self->infos[i]->len) ){
      printf("minimized maxmap %d: %lx (len: %ld)\n", i, self->max_map[i], len);
			// ijon_store_max_input(self, i, data, len, shared->aif_index[i], seed_id, shared->afl_max[i]);
      // num_kept += 1;
      if(0xffffffffffffffff - shared->afl_max[i] < minidist && shared->afl_max[i] != 0xffffffffffffffff) {
        minidist = 0xffffffffffffffff - shared->afl_max[i];
        minindex = shared->aif_index[i];
        //savei = i;
      }
      else if(shared->afl_max[i] != 0xffffffffffffffff) {
        // a within range case!
    //     printf("[before]%d,%p",shared->count, &(shared->count));
    // shared->count += 1;
    // printf("[after]%d,%p",shared->count, &(shared->count));
        ijon_store_max_input(self, ijon_q->count, data, len, shared->aif_index[i], seed_id, shared->afl_max[i], &ijon_q->tscs_by_index);
        num_kept += 1;
        // self->num_entries++;
        return num_kept;
      }
		} */
  }

  // if (minindex != -2) {
  //   // printf("[before]%d,%p",shared->count, &(shared->count));
  //   // shared->count += 1;
  //   // printf("[after]%d,%p",shared->count, &(shared->count));
  //   ijon_store_max_input(self, ijon_q->count, data, len, minindex, seed_id, minidist, &ijon_q->tscs_by_index);
  //   num_kept += 1;
  //   // self->num_entries++;
  // }


  return num_kept;
}
