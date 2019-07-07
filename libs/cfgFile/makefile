all: clean
	cc -o sample sample.c cfgFile.c ../linkedlist/linkedlist.c -I. -I../linkedlist/ -Wall -Wno-unused-value -g

clean:
	-rm sample *.o core
