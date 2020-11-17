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


u8 ijon_should_schedule(ijon_min_state* self){
  if(self->num_entries > 0){
    return random()%100 > 20;
  }
  return 0;
}

ijon_input_info* ijon_get_input(ijon_min_state* self){
  uint32_t rnd = random()%self->num_entries; 
  for(int i = 0; i<MAXMAP_SIZE; i++){
    if(self->max_map[i]>0){
      if(rnd==0){
        printf("schedule: %i %s\n",i, self->infos[i]->filename);
        return self->infos[i];
      }
      rnd-=1;
    }
  }
  return 0;
}

void ijon_store_max_input(ijon_min_state* self, int i, uint8_t* data, size_t len, int index, int seed_id, int distance){
  ijon_input_info* inf = self->infos[i];
  inf->len = len;
  int fd = open(inf->filename, O_CREAT|O_TRUNC|O_WRONLY,0600);
  //printf("write to file: %s\n", inf->filename);
  assert(write(fd,data,len) == len);
  close(fd);
	
	char* filename = NULL;
	//assert(asprintf(&filename, "%s/finding_%lu_%lu", self->max_dir, self->num_updates, time(0)) > 0);
  assert(asprintf(&filename, "%s/id:%06d_%d_%d", self->max_dir, seed_id, index, distance) > 0);
	self->num_updates+=1;
  fd = open(filename, O_CREAT|O_TRUNC|O_WRONLY,0600);
  //printf("write to file: %s\n", filename);
  assert(write(fd,data,len) == len);
  close(fd);
	free(filename);
}

int ijon_update_max(ijon_min_state* self, shared_data_t* shared, uint8_t* data, size_t len, int seed_id){
  int num_kept = 0;
	int should_min = (len>512) ;
  for(int i=0; i<MAXMAP_SIZE; i++){ 
    if(shared->afl_max[i] > self->max_map[i] || shared->afl_max[i] == 0xffffffffffffffff){
      if(self->max_map[i]==0){ // found an input that triggers a new slot
        self->num_entries++;
      }
      self->max_map[i] = shared->afl_max[i];
      printf("updated maxmap %d: %lx (len: %ld), for watch point: %d\n", i, self->max_map[i], len, shared->aif_index[i]);
      ijon_store_max_input(self, i, data, len, shared->aif_index[i], seed_id, shared->afl_max[i]);
      num_kept += 1;
    }
    else if(should_min && (shared->afl_max[i] == self->max_map[i] ) && ( len < self->infos[i]->len) ){
      printf("minimized maxmap %d: %lx (len: %ld)\n", i, self->max_map[i], len);
			ijon_store_max_input(self, i, data, len, shared->aif_index[i], seed_id, shared->afl_max[i]);
      num_kept += 1;
		}
  }
  return num_kept;
}
