CC = gcc
FLAGS = -Wall

all:myshell 


myshell:stshell.o
	$(CC) $(FLAGS) stshell.o -o myshell -lreadline

stshell.o:stshell.c stshell.h
	$(CC) $(FLAGS) -c stshell.c


clean:
	rm -f *.o   myshell