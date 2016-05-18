% : %.c
	gcc -g -std=c99 $? -o $@ -lpthread
