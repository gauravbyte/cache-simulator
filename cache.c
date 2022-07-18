
#include "cache.h"

parameters * parse_args(int argc, char** argv) {
   if (argc != 7) { return NULL; }

  int num_sets = atoi(argv[1]);
  int num_blocks = atoi(argv[2]);
  int sizeblock = atoi(argv[3]);
  
  if (!check2power(num_sets) || !check2power(num_blocks) ||
      !check2power(sizeblock) || sizeblock < 4) { return NULL; }

  int policy =1;



  if (strcmp("LRU", argv[4]) == 0) { policy = 0; }
  else if (strcmp("FIFO", argv[4]) != 0) { policy =1 ;}
  else if (strcmp("LFU",argv[4])!= 0){ policy = 2 ;}

  parameters * sim = malloc(sizeof(parameters));
  sim->setcount = num_sets;
  sim->countblock = num_blocks;
  sim->block_size = sizeblock;

  sim->evict_mode = policy;
  return sim;
}

Cache *cache_init(int num_sets, int numblocks) {
  Cache *cache = malloc(sizeof(Cache));  
  cache->setcount = malloc(sizeof(Set) * num_sets);
  cache->num_sets = num_sets;
  cache->blocks_in_set = numblocks;

  for (int i = 0; i < num_sets; i++) {
    Set *s = cache->setcount + i;
    s->countblock = malloc(sizeof(Block) * numblocks); 
    for (int j = 0; j < numblocks; j++) {
      Block *b = s->countblock + j;
      b->tag = 0;
      b->valid = false;
      b->count_access =0;
      b->load_ts = 0;
      b->access_ts = 0;
    }
  }

  cache->loads = 0;
  cache->stores = 0;
  cache->load_hit = 0;
  cache->load_miss = 0;
  cache->store_hit = 0;
  cache->store_miss = 0;
  cache->cycles = 0;
  cache->ts = 0;
  return cache;
}

void free_cache(Cache *c) {
  int n_s = c->num_sets;
  Set *set_ptr = c->setcount;
  
  for (int i = 0; i < n_s; i++) {
    free((set_ptr + i)->countblock);
  }
  free(c->setcount);
  free(c);
}

void summary(Cache *c,char* out_file) {

  FILE *fptr = fopen(out_file, "w");
  fprintf(fptr,"Total loads: %d\nTotal stores: %d\nLoad hits: %d\n",
	 c->loads, c->stores, c->load_hit);
  fprintf(fptr,"Load misses: %d\nStore hits: %d\nStore misses: %d\n",
	 c->load_miss, c->store_hit, c->store_miss);
  fprintf(fptr,"Total cycles: %d\n", c->cycles);
}

void load_block(Block *b, uint32_t t, unsigned int ts) {
  b->tag = t;
  b->valid = true;

  b->load_ts = ts;   
  b->access_ts = ts; 
}

bool check2power (int num) { return ((num > 0) && ((num & (num - 1)) == 0)); }

int find_pow(int num) {
  int count = 0;
  while (num > 1) {
    count++;
    num = num >> 1;
  }
  return count;
}

uint32_t bitmask(uint32_t source, int length, int lower) {
  uint32_t mask = (((uint32_t) 1) << length) - 1;
  mask <<= lower;
  mask &= source;
  mask >>= lower;
  return mask;
}

Block *is_hit(Cache *c, Set *s, int slots, uint32_t t) {
  Block *curr_block = s->countblock;
  for (int i = 0; i < slots; i++) {
    curr_block = s->countblock + i;

    if (curr_block->valid && curr_block->tag == t) {
      curr_block->access_ts = c->ts;
      return curr_block;
    }
  }
  return NULL;
}

Block *replace_block(int MODE, Set *s, int slots) {
  int lru_index = -1;
  unsigned int lru_timestamp = 0;
  int fifo_index = -1;
  int lfu_index = -1;
  unsigned int fifo_timestamp = 0;
  Block *b = s->countblock;
  unsigned int minfreq = INT_MAX;

  for (int i = 0; i < slots; i++) {
    b = s->countblock + i;

    if (!(b->valid)) {
      return b;
    }
    else {
      if (MODE==0) {

	if (lru_index == -1 || b->access_ts < lru_timestamp) {
	  lru_timestamp = b->access_ts;
	  lru_index = i;
	}
      }
      else if (MODE==1){

	if (fifo_index == -1 || b->load_ts < fifo_timestamp) {
	  fifo_timestamp = b->load_ts;
	  fifo_index = i;
	}
      }
      else if (MODE==2)
      {
        if (lfu_index==-1 || b->count_access < minfreq){
          minfreq = b->count_access;
          lfu_index = i;
        }
      }
    }
  }


  if (MODE==0) { b = s->countblock + lru_index; }
  else if (MODE==1){ b = s->countblock + fifo_index; }
  else if (MODE==2) {b = s->countblock + lfu_index;}
  b->count_access++;
  return b;
}

Block *handle_write_back(parameters *p, Cache *c,
			 Set *s, uint32_t t) {
  Block *b = replace_block(p->evict_mode, s, p->countblock);


  load_block(b, t, c->ts);
  c->cycles += 100 * (p->block_size / 4);
  return b;
}

void handle_load(parameters *p, Cache *c,
		 Set *s, uint32_t t, bool hit) {
  c->loads++;
  c->cycles++;
  if (hit) { c->load_hit++; }
  else {
    c->load_miss++;
    // Evict or fill invalid block
    handle_write_back(p, c, s, t);
  }
}

void handle_store(parameters *p, Cache *cptr,Set *s, uint32_t t, bool hit) {
  cptr->stores++;
  if (hit) {
    cptr->store_hit++;
    cptr->cycles++;
            // write-back
      
  }
  else {
    cptr->store_miss++;

     handle_write_back(p, cptr, s, t);

  
      cptr->cycles++;
    }

  
}
