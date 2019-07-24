//CXX = g++-8
CXXFLAG = -std=c++17 -O2 -Wall -Wextra -fPIC
GMAEINCLUDE = ./
BUILDDIR = ./build

all: gamerunner AI_1 AI_2

AI_1:
	$(CXX) $(CXXFLAG) -I$(GMAEINCLUDE) -shared AITemplate/Porting_1.cpp -o ${BUILDDIR}/AI.so
	cp ${BUILDDIR}/AI.so ${BUILDDIR}/a1.so
	
AI_2:	
	$(CXX) $(CXXFLAG) -I$(GMAEINCLUDE) -shared AITemplate/Porting_2.cpp -o ${BUILDDIR}/AI.so
	cp ${BUILDDIR}/AI.so ${BUILDDIR}/a2.so

gamerunner:
	$(CXX) $(CXXFLAG) -I$(GMAEINCLUDE) -pthread main.cpp -o ${BUILDDIR}/gamerunner -ldl

.PHONY: clean
clean:
	-rm gamerunner a1.so
