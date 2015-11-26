.PHONY: all clean check

LDFLAGS:=-lfl -pipe -Wall -g
CFLAGS:=-c -pipe -Wall -g -fstack-protector
GENFILES:= common/parser.yy.h common/parser.yy.c common/parser.tab.h common/parser.tab.c
COMMON_OBJ:=  common/parser.yy.o common/parser.tab.o common/matrix.o common/printer.o
OBJFILES:= $(COMMON_OBJ)
BINFILES:= sparse-print

all: $(BINFILES)

sparse-print: $(OBJFILES)
	@$(CC) $(LDFLAGS) $(COMMON_OBJ) -o sparse-print

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

clean:
	-@$(RM) $(GENFILES) $(OBJFILES)
