
my_project: convertor.o tools.o
	gcc convertor.o tools.o -o my_project

convertor.o: convertor.c main_header.h
	gcc -c convertor.c

tools.o: tools.c main_header.h
	gcc -c tools.c

clean:
	rm my_project output.BMP *.o
