CC = gcc
#CFLAGS = -Wall -Wextra -std=c99
CFLAGS= -Wall
LFLAGS = 
LIBS = -lm
SOURCES = myshell.c utility.c
OBJECTS = $(subst .c,.o,$(SOURCES))
EXE = myshell
.PHONY: clean help

%.exe : %.o
	$(CC) $(CFLAGS) $< $(LIBS) -o $@ 

%.o : %.c
	$(CC) $(CFLAGS) -c $< 

all : $(EXE)

clean:
	rm -f $(OBJECTS) $(EXE) *~

help:
	@echo "Valid targets:"
	@echo "  clean:  removes .o and .exe files"
