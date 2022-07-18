

EXES = sim
num_blocks =
num_sets = 
size_block =
write_allocate= write_allocate
write_back = write_back
eviction_policy = LRU
trace_file = 
output_file =

# Source file
C_SRCS = cache.c

# Compiler flags
CFLAGS = -g -Og -Wall -Wextra -pedantic -std=gnu11

%.o : %.c
	gcc $(CFLAGS) -c $*.c -o $*.o

# Default target: build all executables
all : $(EXES)

# C version of postfix calculatoro
sim : cache.o main.o
	gcc -o $@ $^

# Clean up generated files (such as object files and executables)
clean :
	rm -f *~ *.o $(EXES) depend.mak solution.zip


# num_sets=256	 num_blocks=1024 size_block=32 eviction_policy=LFU
# trace_file=read01.trace output_file=output.txt

run : 
	./sim  $(num_sets) $(num_blocks) $(size_block) $(eviction_policy) $(trace_file) $(output_file)


	