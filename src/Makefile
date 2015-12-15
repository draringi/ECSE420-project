.PHONY: all clean check

LDFLAGS:= --linker-options -lfl -g
CFLAGS:= -c -g --compiler-options -fstack-protector,-Wall,-pipe -I.
GENFILES:= common/parser.yy.h common/parser.yy.c common/parser.tab.h common/parser.tab.c
COMMON_OBJ:=  common/parser.yy.o common/parser.tab.o common/matrix.o
PRINT_OBJ:= common/printer.o
SEQ_OBJ:= sequential/main.o sequential/dlx.o
FORK_OBJ:= fork/main.o fork/forkx.o
CUDA_OBJ:= cuda/main.o cuda/cudax.o
OBJFILES:= $(COMMON_OBJ) $(SEQ_OBJ) $(FORK_OBJ) $(CUDA_OBJ)
BINFILES:= sparse-print cover-seq cover-fork cover-cuda
CC:=nvcc

all: $(BINFILES)

sparse-print: $(COMMON_OBJ) $(PRINT_OBJ)
	@$(CC) $(LDFLAGS) $(COMMON_OBJ) $(PRINT_OBJ) -o sparse-print

cover-seq: $(COMMON_OBJ) $(SEQ_OBJ)
	@$(CC) $(LDFLAGS) $(COMMON_OBJ) $(SEQ_OBJ) -o cover-seq

cover-fork: $(COMMON_OBJ) $(FORK_OBJ)
	@$(CC) $(LDFLAGS) $(COMMON_OBJ) $(FORK_OBJ) -o cover-fork

cover-cuda: $(COMMON_OBJ) $(CUDA_OBJ)
	@$(CC) $(LDFLAGS) $(COMMON_OBJ) $(CUDA_OBJ) -o cover-cuda

common/printer.o: common/printer.c common/matrix.h common/parser.yy.h
	@$(CC) $(CFLAGS) common/printer.c -o common/printer.o

common/matrix.o: common/matrix.c common/matrix.h
	@$(CC) $(CFLAGS) common/matrix.c -o common/matrix.o

common/parser.tab.o: common/parser.tab.c common/parser.tab.h common/parser.yy.h
	@$(CC) $(CFLAGS) common/parser.tab.c -o common/parser.tab.o

common/parser.yy.o: common/parser.yy.c common/parser.tab.h
	@$(CC) $(CFLAGS) common/parser.yy.c -o common/parser.yy.o

common/parser.yy.c: common/parser.l
	@flex common/parser.l

common/parser.yy.h: common/parser.yy.c

common/parser.tab.c: common/parser.y
	@bison --defines=common/parser.tab.h -o common/parser.tab.c common/parser.y

common/parser.tab.h: common/parser.tab.c

sequential/main.o: sequential/main.c common/parser.yy.h common/matrix.h sequential/dlx.h
	@$(CC) $(CFLAGS) sequential/main.c -o sequential/main.o

sequential/dlx.o: sequential/dlx.c sequential/dlx.h common/matrix.h
	@$(CC) $(CFLAGS) sequential/dlx.c -o sequential/dlx.o

fork/main.o: fork/main.c common/parser.yy.h common/matrix.h fork/forkx.h
	@$(CC) $(CFLAGS) fork/main.c -o fork/main.o

fork/forkx.o: fork/forkx.c fork/forkx.h common/matrix.h
	@$(CC) $(CFLAGS) fork/forkx.c -o fork/forkx.o

cuda/main.o: cuda/main.c common/parser.yy.h common/matrix.h
	@$(CC) $(CFLAGS) cuda/main.c -o cuda/main.o

cuda/cudax.o: cuda/cudax.cu cuda/cudax.h
	@$(CC) $(CFLAGS) cuda/cudax.cu -o cuda/cudax.o

clean:
	-@$(RM) $(GENFILES) $(OBJFILES) $(BINFILES)
