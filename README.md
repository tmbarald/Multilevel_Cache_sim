# Multilevel_Cache_sim

The following project is a simple multilevel cache simulator written in C++. Currently, the simulator supports two levesl of hierarchy (L1 and L2). The cache uses a LRU (Least recently used) replacement policy and a Write-Back Write Allocate write-back policy.

# Inputs

Currently, the cache simulator only accepts command line inputs. The inputs are the following:

• BLOCKSIZE: Number of bytes in a block. 
• L1_SIZE: Total bytes of data storage for L1 Cache.
• L1_ASSOC: Associativity of L1 cache (ASSOC=1 is a direct-mapped cache) 
• L2_SIZE: Total bytes of data storage for L2 Cache 
• L2_ASSOC: Associativity of L2 cache  
• L2_DATA_BLOCKS: Number of data blocks in a sector (Currently, only accepted value is '1') 
• L2_ADDR_TAGS: Number of address tags pointing to a sector (Currently, only accepted value is '1')
• Trace_file: Input Trace file (gcc_trace, etc.) 
 
The command line input is the the following:

./sim_cache <Blocsize int> <L1_size> <L1_Assoc> <L2_size> <L2_assoc> <<L2_data_blocks> <L2_Addr_tags> <Tracefile>
  
  # Outputs
  
  The output of the program can be separated into three parts:  Input configuration, Cache tag store, and raw measurements.
  
  ## Input configuration
  
  The input configurations output is simply a formatted display of the input arguments. This was added as a source of debugging the program during its initial stages and for posterity.
  
  ## Cache tag store
  
  The cache tag store output is a display of all of the tags at the end of the program. The tags are printed in the order of most recently used. Also, the dirty bit of the tag is also printed in conjunction with the cache's physical tag.
  
  ## Raw Measurements
  
  The raw measurements are displayed as followed.
  
  a. number of L1 reads
  b. number of L1 read misses 
  c. number of L1 writes
  d. number of L1 write misses 
  e. L1 miss rate = MRL1 = (L1 read misses + L1 write misses)/(L1 reads + L1 writes)  
  f. number of L2 read
  g. number of L2 read miss h. number of L2 write 
  i. number of L2 write miss 
  j. number of L2 sector misses 
  k. number of L2 cache block misses 
  l. L2 miss rate = MRL2 = (from standpoint of stalling the CPU) = L2 read misses / L2 reads
  m. number of l2 writeback to main memory
  n. total memory access (with L2, should match: L2 read misses + L2 write misses + writeback from L2) (without L2, should match: L1 read misses + L1 write misses) 
 
