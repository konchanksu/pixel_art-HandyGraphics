# 参考資料
# http://www.ie.u-ryukyu.ac.jp/~e085739/c.makefile.tuts.html
# http://www.gnu.org/software/make/manual/make.pdf
# http://urin.github.io/posts/2013/simple-makefile-for-clang

PROGRAM = pixel
OBJS = keyboard.o map.o rgbcolor.o opening.o 
FILE = pixel.c

.PHONY: clean
.PHONY: all

.SUFFIXES: .c .o

CC = hgcc
CFLAGS = -Wall -Qunused-arguments
# SRCS = $(OBJS:%.o=%.c)

all: $(PROGRAM)

# 実行ファイルの作成
$(PROGRAM): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(FILE) -o $@; ./$(PROGRAM)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm $(OBJS) $(PROGRAM)
