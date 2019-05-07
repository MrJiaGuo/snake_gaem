project:main.o init.o aboutbmp.o linklist.o
	arm-linux-gcc main.o init.o aboutbmp.o linklist.o -pthread -o project
main.o:main.c
	arm-linux-gcc -c main.c
init.o:init.c
	arm-linux-gcc -c init.c
aboutbmp.o:aboutbmp.c
	arm-linux-gcc -c aboutbmp.c
linklist.o:linklist.c
	arm-linux-gcc -c linklist.c
clean:
	rm -r *.o project
