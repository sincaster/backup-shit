all: freeglut.cc
	g++ -o freeglut freeglut.cc -L/usr/local/lib/ -I/usr/local/include -lglut -lGLEW -lGL -lSDL2 -lOSMesa -lXext -std=c++20 -O3 -march=core2 -msse2 -mfpmath=sse -fassociative-math -ffast-math -DSHM 
	
clean:
	rm -f *~ 
	
asm: freeglut.cc
	g++ -S -o freeglut.asm freeglut.cc -lglut -lGLEW -lGL -lSDL2 -std=c++20 -O3 -msse2 -mfpmath=sse -ftree-vectorize -ftree-vectorizer-verbose=5 -march=core2


asm1: freeglut.cc
	g++ -S -o freeglut1.asm freeglut.cc -lglut -lGLEW -lGL -lSDL2 -std=c++20 -O3 -msse2 -mfpmath=sse -ftree-vectorize -ftree-vectorizer-verbose=5 -march=core2
