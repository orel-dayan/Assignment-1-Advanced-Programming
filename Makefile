AR=ar
CC=gcc
FLAGS= -Wall -g
SRC_DIR=source
OBJ_DIR=objects

all: stshell 

clean:
	rm -rf $(OBJ_DIR)/*.o myshell

stshell: $(OBJ_DIR)/stshell.o 
	$(CC) $(FLAGS) -o myshell $(OBJ_DIR)/stshell.o -lreadline


$(OBJ_DIR)/stshell.o: $(SRC_DIR)/stshell.c $(SRC_DIR)/stshell.h
	$(CC) $(FLAGS) -c $(SRC_DIR)/stshell.c -o $(OBJ_DIR)/stshell.o
