exec : base.o creer.o display.o fonctionutile.o game.o grid.o main.o map.o
	gcc functions.o affichage.o main.o -o exec
	
	
functions.o : functions.c functions.h affichage.h
	gcc -c functions.c
	
affichage.o : affichage.c affichage.h
	gcc -c affichage.c
	
main.o : main.c affichage.h functions.h
	gcc -c main.c
	
