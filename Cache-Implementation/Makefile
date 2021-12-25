CFLAGS = -g -Wall -Wno-unused-function

all: timemask cache-ref cache-test

cache-ref: cache-ref.o cache-test.o
cache-test: cache-test.o cache.o
cache-test.o: cache-test.c cache.h
cache.o: cache.c cache.h


timemask.o: timemask.c mask.h
mask.o: mask.c mask.h
timemask: timemask.o mask.o

mask-%.o: mask.c mask.h
	gcc $(CFLAGS) -DMASK_VERSION=$* -c mask.c -o mask-$*.o
timemask-%: timemask.o mask-%.o
	gcc $(CFLAGS) $(LDFLAGS) -o timemask-$* timemask.o mask-$*.o

clean:
	-rm -f cache-test cache-test.o cache.o cache-ref
	-rm -f timemask timemask.o mask.o timemask-* mask-*.o
tidy: clean
	-rm -f *~
