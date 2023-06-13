all: LIBMX 

LIBMX: 
	clang -c -std=c11 -Wall -Wextra -Werror -Wpedantic src/*.c
	ar -rc libmx.a *.o
	rm *.o

uninstall:
	rm libmx.a

clean:
	rm libmx.a

reinstall:
	make uninstall
	make
