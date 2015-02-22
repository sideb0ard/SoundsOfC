# Makefile
CC = gcc
RM = rm
LDFLAGS = -lncurses 
LIBDIR = /usr/local/lib
LIBRARY = $(LIBDIR)/libportaudio.dylib
CFLAGS=-c -Wall
#LDFLAGS = -lncurses

#SRC = pong.c pa_sin.c pa_saw.c drawing_library.c
SRC = main.c player.c game.c field.c ball.c drawing_library.c pa_sin.c

PROG = bin/pong
SRCDIR = src/
OBJDIR = obj/

###### SHIZZLE ##########
OBJ=$(SRC:.c=.o)  
CSRC=$(addprefix $(SRCDIR),$(SRC))
COBJ=$(addprefix $(OBJDIR),$(OBJ))

all: $(CSRC) $(PROG)

$(PROG): $(COBJ)
	$(CC) $(LDFLAGS) $(COBJ) -o $(PROG) $(LIBRARY)

$(OBJDIR)%.o: $(SRCDIR)%.c
	$(CC) -o $@ $< $(CFLAGS) 

clean:
	$(RM) $(COBJ) $(PROG)
