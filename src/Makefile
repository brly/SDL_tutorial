CXX_HEADERS=-I/Library/Frameworks/SDL2.framework/Headers -std=c++11
LIBS=/Library/Frameworks/SDL2.framework/SDL2 -framework GLUT -framework OpenGL -llua

a.out: app.o
		clang++ ${CXX_HEADERS} ${LIBS} app.o -o a.out

app.o: app.cc
	clang++ ${CXX_HEADERS} -c app.cc

.PHONY: clean
clean:
	rm *.o a.out
