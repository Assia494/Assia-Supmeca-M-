exec : base.o creer.o display.o fonctionutile.o game.o grid.o main.o map.o patient.o plateau.o sauvegarde.o scoreboard.o structprojet.o action_player.o
	gcc base.o creer.o display.o fonctionutile.o game.o grid.o main.o map.o patient.o plateau.o sauvegarde.o scoreboard.o structprojet.o 
	
	
base.o : base.c base.o structprojet.o
	gcc -c structprojet.c
	
creer.o : creer.o creer.c structprojet.o
	gcc -c creer.c
	
display.o : display.c display.o structprojet.h fonctionutile.h
	gcc -c display.c
	

fonctionutile.o : fonctionutile.c fonctionutile.h structprojet.h 
	gcc -c fonctionutile.c 

game.o : game.c game.h fonctionutile.h structprojet.h 
	gcc -c game.c
grid.o :grid.c grid.h fonctionutile.h structprojet.h 
	gcc -c creer.c

main.o : main.c main.h fonctionutile.h structprojet.h  grid.h map.h patient.h plateau.h sauvegarde.h scoreboard.h structprojet.h action_player.h
	gcc -c main.c

map.o : map.c map.h structprojet.h fonctionutile.h
	gcc -c map.c

patient.o : patient.c patient.h  structprojet.h game.h fonctionutile.h
	gcc -c patient.c

plateau.o : plateau.c plateau.h structprojet.h
	gcc -c plateau.c

sauvegarde.o : sauvegarde.c sauvegarde.h structprojet.h fonctionutile.h 
	gcc -c sauvegarde.c

scoreboard.o : sauvegarde.h sauvegarde.c scoreboard.h
	gcc -c scoreboard.c

structprojet.o : structprojet.h structprojet.c stdio.h stdlib.h time.h string.h errno.h unistd.h 
	gcc -c structprojet.c

action_player.o  : action_player.c action_player.h structprojet.h 
	gcc -c action_player.c
