CXXFLAGS=-I/Library/Frameworks/SDL2.framework/Headers -std=c++11
LIBS=/Library/Frameworks/SDL2.framework/SDL2 -framework GLUT -framework OpenGL -llua
SRCS=app.cc sdl_context.cc main.cc
OBJS=app.o sdl_context.o main.o
BIN=sdl_sample

${BIN}: ${OBJS}
		clang++ ${CXX_HEADERS} ${LIBS} $^ -o $@

.PHONY: all
all: depend $(BIN)

.SUFFIXES: .cc .o

.PHONY: clean
clean:
	rm ${BIN} ${OBJS} depend.inc

.PHONY: depend
depend: $(OBJS:.o=.cc)
	-@ $(RM) depend.inc
	-@ for i in $^; do\
		cc ${CXXFLAGS} -MM $$i | sed "s/\ [_a-zA-Z0-9][_a-zA-Z0-9]*\.cc//g" >> depend.inc;\
	done

-include depend.inc
