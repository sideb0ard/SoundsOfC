# Makefile
CC = gcc
RM = rm
LDFLAGS = -lncurses -lportaudio

SRC = pong.c pa_sin.c pa_saw.c drawing_library.c

PROG = bin/pong
SRCDIR = src/
OBJDIR = obj/

###### SHIZZLE ##########
OBJ=$(SRC:.c=.o)  
CSRC=$(addprefix $(SRCDIR),$(SRC))
COBJ=$(addprefix $(OBJDIR),$(OBJ))

all: $(CSRC) $(PROG)

$(PROG): $(COBJ)
	$(CC) $(LDFLAGS) $(COBJ) -o $(PROG)

$(OBJDIR)%.o: $(SRCDIR)%.c
	$(CC) $< -c -o $@

clean:
	$(RM) $(COBJ) $(PROG)
