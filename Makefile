CC=clang
DEFINES=-DPB_DEBUG -D_GNU_SOURCE
INCLUDES=-Iinclude -I../include
CFLAGS=-c -g -Wall $(DEFINES) $(INCLUDES)
LDFLAGS=-lpthread -levent
OBJDIR=build/
LIBDIR=lib/
SRCDIR=src/
EXECUTABLE="bin/pb"

SRCS=$(wildcard $(SRCDIR)*.c)
LSRCS+=$(wildcard $(LIBDIR)*.c)
OBJS=$(patsubst $(SRCDIR)%.c, $(OBJDIR)%.o, $(SRCS))
LOBJS=$(patsubst $(LIBDIR)%.c, $(OBJDIR)%.o, $(LSRCS))
OBJS+=$(LOBJS)

all: pb

pb: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $(EXECUTABLE)

clean:
	rm build/*.o

$(OBJDIR)%.o: $(SRCDIR)%.c
	$(CC) $(CFLAGS) $< -o $@

$(OBJDIR)%.o: $(LIBDIR)%.c
	$(CC) $(CFLAGS) $< -o $@
