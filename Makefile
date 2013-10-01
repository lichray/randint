# ccconf example CXX=clang++ CXXFLAGS=-std=c++1y -stdlib=libc++ -Wall -g -I. -fsanitize=undefined-trap -fsanitize-undefined-trap-on-error LDFLAGS+=-stdlib=libc++
LDFLAGS  = -stdlib=libc++  
CXXFLAGS = -std=c++1y -stdlib=libc++ -Wall -g -I. -fsanitize=undefined-trap -fsanitize-undefined-trap-on-error  
CXX      = clang++  

.PHONY : all clean
all : example
clean :
	rm -f example example.o tags

tags : *.h example.cc 
	ctags *.h example.cc 

example : example.o
	${CXX} ${LDFLAGS} -o example example.o
example.o : example.cc random.h
