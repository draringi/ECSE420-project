.PHONY: all clean check

LDFLAGS:=-lfl -pipe -Wall -g
CFLAGS:=-c -pipe -Wall -g -fstack-protector -I.
GENFILES:= common/parser.yy.h common/parser.yy.c common/parser.tab.h common/parser.tab.c
COMMON_OBJ:=  common/parser.yy.o common/parser.tab.o common/matrix.o
PRINT_OBJ:= common/printer.o
SEQ_OBJ:= sequential/main.o sequential/dlx.o
OBJFILES:= $(COMMON_OBJ)
BINFILES:= sparse-print cover-seq

all: $(BINFILES)

sparse-print: $(COMMON_OBJ) $(PRINT_OBJ)
	@$(CC) $(LDFLAGS) $(COMMON_OBJ) $(PRINT_OBJ) -o sparse-print

cover-seq: $(COMMON_OBJ) $(SEQ_OBJ)
	@$(CC) $(LDFLAGS) $(COMMON_OBJ) $(SEQ_OBJ) -o cover-seq

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

clean:
	-@$(RM) $(GENFILES) $(OBJFILES) $(BINFILES)
