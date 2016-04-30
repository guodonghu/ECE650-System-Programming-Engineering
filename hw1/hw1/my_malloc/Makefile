all: my_malloc.c my_malloc.h
	gcc -o libmymalloc.so -Wno-deprecated-declarations -fPIC -std=gnu99 -shared -Wall -Werror my_malloc.c
clean:
	rm -f my_malloc *~ *# *.so *.o
