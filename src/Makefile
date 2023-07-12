all: compile link

compile:
	g++ -I/usr/include -c main.cpp

link:
	g++ main.o -o main -L/usr/include/sfml -lsfml-graphics -lsfml-window -lsfml-system