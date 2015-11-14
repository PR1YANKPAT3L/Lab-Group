CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pthread
LFLAGS = 
LIBS = -lm
SOURCES = sudoku.c
OBJECTS = $(subst .c,.o,$(SOURCES))
EXE = sudoku
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
