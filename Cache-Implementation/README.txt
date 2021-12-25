Put your answers to non-coding questions in this file as described in
the assignment description.

All answers are to be in plain text ASCII.

Predicting Cache Miss Rates:

a) Miss Rate = 8/64 = 0.125
  In sumA, we traverse the array a in row major order which makes the stride equal to 1
  - Stride = 1:
    a[0] -> cache miss, so we copy 64 bytes of data to a block
    a[1] -> cache hit (because the block contains a[1] with an offset of 8 bytes) 
    a[2] -> cache hit (because the block contains a[2] with an offset of 16 bytes) 
    ...
    a[7] -> cache hit (because the block contains a[7] with an offset of 56 bytes) 

    a[8] -> cache miss (now we reach a new cache line which is empty/cold)
    a[9] -> cache hit (because the block contains a[9] with an offset of 8 bytes) 
    ...
    a[15] -> cache hit (because the block contains a[15] with an offset of 56 bytes) 
    This cycle then repeats until we are looping through the array a.
    Miss Rate is 8/64 because 'a' is an array of longs (8 bytes) and the block size is 64 bytes.

b) Miss Rate = 1.0

  In sumB, we traverse the array a in column major order which results in worse spacial locality
  Since we have a direct mapped cache, we know that we will have 64 sets with 1 line in each set so we have 64 lines. 
  We use the following example to demonstrate why the cache will have Miss Rate = 1.0:
    When sumB function starts looping, we access the first few array indices as follows:
    - Note that the set number = (array index/8) % 64 since sizeof(long) = 8 and there are 64 sets
      a[0] goes in cache set 0 line 0 -> cache miss
      a[256] replaces a[0] in cache line 0 -> cache miss (because we use LRU, data in line 0 is overwritten)
      a[512] replaces a[256] in cache line 0 -> cache miss (the replacement here follows the same reasoning)
      a[768] replaces a[512] in cache line 0 -> cache miss ...
      a[1024] replaces a[768] in cache line 0 -> cache miss ...
      a[1280] replaces a[1024] in cache line 0 -> cache miss ...

      now we access:
      a[1] -> cache miss because the a[0:7] block is no longer cache (a[1] replaces a[1280] in line 0)
      a[257] -> cache miss becasue the a[256:263] block is no longer in cache (a[257] replaces a[1] in line 0)
      ...
      We notice that this cycle repeats for all cache accesses because we have 1 line per set 
      but we access the same set/line consecutively 6 times (6 rows) and keep overwriting the stored data.
      Hence we get a miss rate = 1.0

c) Miss Rate = 0.5
  
  In sum C, we traverse the array a in column major order. However, we access 4 different 
  addresses in the cache where pairs of 2 are located in the same cache line/block.
  This means that for every 2 accesses to the cache, we miss 1 and hit 1, giving us a miss rate of 1/2 = 0.5
  
  Assume we have just begun iterating over the cols and rows in Sum C:
  For example:
  When variable j = 0:
  - i = 0:  cache read at a[i * cols + j] = a[0] -> cache miss (data stored on line 0)            
            similarly, cache read at a[(i+1) * cols + j] = a[256] -> cache miss (data stored on line 32)

            cache read at a[i * cols + j+1] = a[1] -> cache hit (j+1 is adjacent to j and its data is
                                                        stored in same cache line at index j)
            cache read at a[(i+1) * cols + j+1] = a[257] -> cache hit
  
  - i = 2: the above pattern follows  
            cache read at a[512] -> cache miss (replaces data at line 0)
            cache read at a[768] -> cache miss (replaces data at line 32)

            cache read at a[513] -> cache hit
            cache read at a[769] -> cache hit

    This pattern follows until the loop has finished executing
    so we get 2 hits for every 2 misses. Hence the miss rate = 0.5 


d) Miss Rate = 1.0
  
  NOTE: This case is similar to part (b)

  Since we have a 2-way set associative cache, we know that we will have 32 sets with 2 lines in each set. 
  We use the following example to demonstrate why the cache will have Miss Rate = 1.0:
    When sumB function starts looping, we access the first few array indices as follows:
    - Note that the set number = (array index/8) % 32 since sizeof(long) = 8 and there are 32 sets
      a[0] goes in cache set 0 line 0 -> cache miss
      a[256] goes in cache set 0 line 1 -> cache miss
      a[512] replaces a[0] in cache set 0 line 0 -> cache miss (because we use LRU, data in line 0 is overwritten)
      a[768] replaces a[256] in cache set 0 line 1 -> cache miss (the replacement here follows the same reasoning)
      a[1024] replaces a[512] in cache set 0 line 0 -> cache miss
      a[1280] replaces a[1024] in cache set 0 line 1 -> cache miss

      now we access:
      a[1] -> cache miss because the a[0:7] block is no longer cache (a[1] replaces a[1024] in set 0 line 0)
      a[257] -> cache miss becasue the a[256:263] block is no longer in cache (a[257] replaces a[1280] in set 0 line 1)
      ...
      We notice that this cycle repeats for all cache accesses because we have 2 lines per set 
      but we access the same set consecutively 6 times (6 rows) and keep overwriting the stored data.
      Hence we get a miss rate = 1.0

e) Miss Rate = 1.0
  
  NOTE: This case is similar to part (b)

  Since we have a 4-way set associative cache, we know that we will have 16 sets with 4 lines in each set. 
  We use the following example to demonstrate why the cache will have Miss Rate = 1.0:
    When sumB function starts looping, we access the first few array indices as follows:
    - Note that the set number = (array index/8) % 16 since sizeof(long) = 8 and there are 16 sets
      a[0] goes in cache set 0 line 0 -> cache miss
      a[256] goes in cache set 0 line 1 -> cache miss
      a[512] goes in cache set 0 line 2 -> cache miss
      a[768] goes in cache set 0 line 3 -> cache miss
      a[1024] replaces a[0] in cache set 0 line 0 -> cache miss (because we use LRU, data in line 0 is overwritten)
      a[1280] replaces a[1024] in cache set 0 line 1 -> cache miss (the replacement here follows the same reasoning)

      now we access:
      a[1] -> cache miss because the a[0:7] block is no longer cache (a[1] replaces a[512] in set 0 line 2)
      a[257] -> cache miss because the a[256:263] block is no longer in cache (a[257] replaces a[768] in set 0 line 3)
      a[513] -> cache miss because the a[512:519] block is no longer in cache (a[513] replaces a[1024] in set 0 line 0)
      a[769] -> cache miss because the a[768:775] block is no longer in cache (a[769] replaces a[1280] in set 0 line 1)
      ...
      We notice that this cycle repeats for all cache accesses because we have 4 lines per set 
      but we access the same set consecutively 6 times (6 rows) and keep overwriting the stored data.
      Hence we get a miss rate = 1.0

f) Worst Case miss rate = 1.0 (exact same as if the random policy performs the same operation as LRU (part e))
   Best Case miss rate = 13/24 = 0.5417 (explained below)
   Miss Rate Range = 0.5417 to 1.0000
  
  Since we have a 4-way set associative cache, we know that we will have 16 sets with 4 lines in each set. 
  We use the following example to demonstrate the best case miss rate:
    When sumB function starts looping, we access the first few array indices as follows:
    - Note that the set number = (array index/8) % 16 since sizeof(long) = 8 and there are 16 sets
    Suppose we start with a[0]:
    a[0] -> store the block in cache set 0 line 0 -> cache miss
    a[256] -> store the block in cache set 0 line 1 -> cache miss
    a[512] -> store the block in cache set 0 line 2 -> cache miss
    a[768] -> store the block in cache set 0 line 3 -> cache miss

    Now we consider that for any cache miss, we will replace the contents of cache set 0 line 3. So:
    a[1024] replaces a[768] in cache set 0 line 3 -> cache miss
    a[1280] replaces a[1024] in cache set 0 line 3 -> cache miss

    In the next iteration, we see 3 cache hits followed by 3 misses:
    a[1] -> cache hit (set 0 line 0)
    a[257] -> cache hit (set 0 line 1)
    a[513] -> cache hit (set 0 line 2)
    a[769] -> cache miss (now we replace a[1280] with a[769], set 0 line 3)
    a[1025] -> cache miss (now we replace a[769] with a[1025], set 0 line 3)
    a[1281] -> cache miss (now we replace a[1025] with a[1281], set 0 line 3)
    ...

    This repeats until we get to the 8th iteration (the final 8 byte long in a 64 byte block)
    This time, for a cache miss, we replace line 0 to get an additional cache hit for line 3:
    a[7] -> cache hit (set 0 line 0)
    a[263] -> cache hit (set 0 line 1)
    a[519] -> cache hit (set 0 line 2)
    a[775] -> cache miss (now we replace a[7] with a[775], set 0 line 0)
    a[1031] -> cache miss (now we replace a[775] with a[1031], set 0 line 0)
    a[1287] -> cache hit 

    This concludes the usage of one single set, the whole process repeats for the remaining array
    indices and the remaining sets

    Best case miss rate = (6 + 3*7 - 1 misses) / (6*8 accesses) = 13/24 = 0.5417

g) Miss Rate = 0.125

  - Note that the set number = (array index/8) % 64 since sizeof(long) = 8 and there are 64 sets
  => since this cache is direct mapped, we have 64 lines

    When sumB function starts looping, we access the first few array indices as follows:
    a[0] -> goes in cache line 0 -> cache miss
    a[240] -> goes in cache line 30 ((240/8) % 64) -> cache miss
    a[480] -> goes in cache line 60 -> cache miss
    a[720] -> goes in cache line 26 -> cache miss
    a[960] -> goes in cache line 56 -> cache miss
    a[1200] -> goes in cache line 22 -> cache miss

    Now, variable j increments and i resets to 0:
    a[1] -> cache hit (line 0)
    a[241] -> cache hit (line 30)
    a[481] -> cache hit (line 60)
    a[721] -> cache hit (line 26)
    a[961] -> cache hit (line 56)
    a[1201] -> cache hit (line 22)
    ...

    We will continue to get hits until the 8th iteration (j = 7) because of 64 byte blocks
    When j = 8, we see that new 64 byte blocks will either replace current lines or store data in new lines
    And so we will get 6 misses follow by 6*7 hits

    This gives us a miss rate of 6/(6*8) = 0.125

_____________________________________________________________________________________________________________________________________________________________________________

The log and table from the mask execution:

Machine Used: lin05

mask1 Log: 
  Change Intended:
    - Swap order of loops iterating over rows and columns (make rows the outer loop, columns the inner loop).
    - Swap indices [j][i] to correspond to changed order of loops.
  Rationale: 
    - This change will allow the program to be more cache friendly by exploiting spatial locality as C uses 
      row major order to store arrays. Hence we decrease the stride between every memory access and make the 
      program more efficient.

mask2 Log:
  Change Intended:
    - Replace all instances of the local variables row, col in terms of i, j.
  Rationale:
    - This change will make the program more cache friendly by decreasing the memory footprint. This is because
      we can get rid of the variables row, col completely and only use i, j for array accesses in every loop.

mask3 Log:
  Change Intended:
   - Combine all loop iterations (from accessing separate regions of the image) into one loop (excluding the last loop)
  Rationale:
   - This changes the runtime by a constant factor which decreases the average runtime for the program. 

mask4 Log:
  Change Intended:
   - Use the register keyword before variables that are frequently accessed.
  Rationale:
   - This makes accessing the variables much faster since they are stored in registers instead of memory.
     As a result the program has a lower memory footprint and is much more cache-friendly and efficient.

mask5 Log:
  Change Intended:
  - Combined the number of if statements from mask4 version.
  Rationale:
  - Reducing the number of comparisons would result in lesser assembly branch/jump instructions 
    and the code would run faster.

mask6 Log: 
  Change Intended:
  - Separated the image into specific regions (eg. middle, corners, sides) and removed conditional statements.
  Rationale:
  - This would eliminate the need to check if the neighbours of the corner and side pixels are within the bound or not
    since these parts are separated. (Ex. for the top side/edge pixels, the neighbours would be left, bottom left, bottom
    bottom right, and the right pixels.)
  - Reducing comparison operations would allow the loops to execute faster because there are lower assembly instructions
    per loop.  


Table of Results:

maskX       Average Time (us)   Best Time (us)    Ratio (For Average Time)      
0 (base)    1871295             1863226           1.001509
1           256348              256166            0.140327
2           249956              249652            0.136669
3           219309              219083            0.117489
4           156799              156766            0.085111
5           154617              154451            0.083906
6           91275               91168             0.049531