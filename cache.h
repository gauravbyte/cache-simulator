#ifndef SIM_H
#define SIM_H
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>


typedef struct {
  int setcount;
  int countblock;
  int block_size;
  int evict_mode;
} parameters;

typedef struct {
  uint32_t tag;           
  bool valid;             
  unsigned int load_ts;   
  unsigned int access_ts; 
  unsigned int count_access;
} Block;

typedef struct {
  Block *countblock;        
} Set;

typedef struct {
  Set *setcount;              
  int num_sets;           
  int blocks_in_set;          
  int loads;              
  int stores;             
  int load_hit;           
  int load_miss;          
  int store_hit;          
  int store_miss;         
  int cycles;             
  int ts;                 
} Cache;

parameters *parse_args(int argc, char** argv);

Cache *cache_init(int num_sets, int blocks_in_set);

void free_cache(Cache *c);

void summary(Cache *c,char* outputfile);


void load_block(Block *b, uint32_t t, unsigned int ts);

bool check2power (int num);

int find_pow(int num);

uint32_t bitmask(uint32_t source, int length, int lower);

Block *is_hit(Cache *c, Set *s, int slots, uint32_t t);

Block *replace_block(int lru, Set *s, int slots);

Block *handle_write_back(parameters *p, Cache *c,
			 Set *s, uint32_t t);

void handle_load(parameters *p, Cache *c,
		  Set *s, uint32_t t, bool hit);

void handle_store(parameters *p, Cache *c,
		  Set *s, uint32_t t, bool hit);

#endif 
