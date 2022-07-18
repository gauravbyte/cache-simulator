
#include "cache.h"

int main(int argc, char** argv) {
  parameters *params = parse_args(argc, argv);
  if (!params) {
    fprintf(stderr, "invalid parameters \n");
    return 1;
  }

  Cache *c = cache_init(params->setcount, params->countblock);
  int o_bits = find_pow(params->block_size); 
  int i_bits = find_pow(params->setcount);  
  char type;
  char hex[9];
  int nothing;
  FILE* ptr = fopen(argv[5],"r");
  while(fscanf(ptr,"%c 0x%s %d\n", &type, hex, &nothing) == 3) {
    c->ts++;

    
    uint32_t adr = strtoul(hex, NULL, 16);
    uint32_t tag = bitmask(adr, 32 - i_bits - o_bits, i_bits + o_bits);
    uint32_t index = bitmask(adr, i_bits, o_bits);
    Set *curr_set = c->setcount + index;
    Block *curr_block = is_hit(c, curr_set, params->countblock, tag);

    if (type == 'l') {
      handle_load(params, c, curr_set, tag, curr_block != NULL);
    }
    else if (type == 's') {
      handle_store(params, c, curr_set, tag, curr_block != NULL);
    }
  }

  summary(c,argv[6]);
  free_cache(c);
  free(params);
  return 0;
}
