#
#  A Simple Makefile for View3D
#
CC=gcc
CFLAGS= -Wunused
#
obj   = ctrans.o \
	getdat.o \
	heap.o \
	misc.o \
	polygn.o \
	readvf.o \
	savevf.o \
	test3d.o \
	v3main.o \
	view3d.o \
	viewobs.o \
	viewpp.o \
	viewunob.o
#
progs = View3D
#
headers = prtyp.h types.h vglob.h view3d.h vxtrn.h
#
all: $(progs)
#
$(obj): $(headers)
#
$(progs): $(obj)
	$(CC) $(CFLAGS) -o $@ $(obj) -lm
#
.c.o:
	$(CC) $(CFLAGS) -c $*.c
#
.c:
	$(CC) $(CFLAGS) -o $@ $*.c $(obj) -lm
#
clean:
	rm -f $(progs) $(obj)