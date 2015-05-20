CXXFLAGS = -std=c++11 -Wall -Wextra
TARGET = solver
LDFLAGS = -lm

${TARGET}: Solver.o Problem.o main.o
	${CXX} -o $@ $^ ${LDFLAGS}

.PHONY: clean mrproper

clean:
	rm -f *.o

mrproper: clean
	rm -f ${TARGET}
