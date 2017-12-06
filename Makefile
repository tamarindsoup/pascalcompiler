CC = cc
LEX = lex
YACC = yacc
YFLAGS = -d

EXE = parser
SRCS = stack.c stack.h gen.c gen.h scanner.l parser.y
OBJC1 = $(SRCS:.l=.c)
OBJC = $(OBJC1:.y=.c)

.SUFFIXES:
.SUFFIXES: .c .h .l .y

.Phony:all clean
all:$(EXE)

$(EXE):$(OBJC)
	$(CC) $(CFLAGS) $(filter-out %.h,$^) -ll -o $@

clean:
	$(RM) $(EXE)
	$(RM) $(filter-out $(SRCS),$(OBJC))
	$(RM) y.tab.h
	$(RM) result.code
