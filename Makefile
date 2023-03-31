# imglabeler makefile

NAME = imglabeler

CC = gcc
STD = -std=c99
WFLAGS = -Wall -Wextra -pedantic
OPT = -O2
INC = -I/opt/homebrew/Cellar/glfw/3.3.8/include
DLIB = -L/opt/homebrew/Cellar/libpng/1.6.39/lib -L/opt/homebrew/Cellar/jpeg/9e/lib -L/opt/homebrew/Cellar/glfw/3.3.8/lib
LIB = imgtool

SRCDIR = src
TMPDIR = tmp
LIBDIR = lib

SCRIPT = build.sh

SRC = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c,$(TMPDIR)/%.o,$(SRC))
LIBS = $(patsubst %,$(LIBDIR)/lib%.a,$(LIB))
DLIB += $(patsubst %,-L%, $(LIBDIR))
DLIB += $(patsubst %,-l%, $(LIB))
INC += $(patsubst %,-I%,$(LIB))
INC += -Ispxe -Iutopia

DLIB += -lz -lpng -ljpeg -lglfw

OS=$(shell uname -s)
ifeq ($(OS),Darwin)
	OPNGL += -framework OpenGL
else
	DLIB += -lm -lGL -lGLEW
endif

CFLAGS = $(STD) $(WFLAGS) $(OPT) $(INC)

$(NAME): $(OBJS) $(LIBS)
	$(CC) $(OBJS) -o $@ $(CFLAGS) $(DLIB) $(OPNGL)

.PHONY: clean

$(LIBDIR)/lib%.a: %
	cd $^ && $(MAKE) && mv bin/*.a ../$(LIBDIR)

$(LIBS): | $(LIBDIR)

$(TMPDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJS): | $(TMPDIR)

$(TMPDIR):
	mkdir -p $@

$(LIBDIR):
	mkdir -p $@

clean: $(SCRIPT)
	./$^ $@
