

PROJECT_DIR = $(shell pwd)/src


all:
	gcc $(PROJECT_DIR)/server.c -o server 

clean:
	rm -f server