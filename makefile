exec : base.o creer.o display.o fonctionutile.o  grid.o main.o map.o patient.o plateau.o sauvegarde.o scoreboard.o action_player.o economy.o
	gcc base.o creer.o display.o fonctionutile.o grid.o main.o map.o patient.o plateau.o sauvegarde.o scoreboard.o economy.o -o exec
	
economy.o : economy.c economy.h structprojet.h sauvegarde.h fonctionutile.h 
	gcc -c economy.c 

base.o : base.c base.h structprojet.h fonctionutile.h map.h grid.h plateau.h patient.h sauvegarde.h scoreboard.h 
	gcc -c base.c
	
creer.o : creer.h creer.c structprojet.h
	gcc -c creer.c
	
display.o : display.c display.h structprojet.h fonctionutile.h  
	gcc -c display.c
	

fonctionutile.o : fonctionutile.c fonctionutile.h structprojet.h 
	gcc -c fonctionutile.c 
	
grid.o :grid.c grid.h fonctionutile.h structprojet.h 
	gcc -c creer.c

main.o : main.c main.h fonctionutile.h economy.h structprojet.h  grid.h map.h patient.h plateau.h sauvegarde.h scoreboard.h structprojet.h action_player.h
	gcc -c main.c

map.o : map.c map.h structprojet.h fonctionutile.h
	gcc -c map.c

patient.o : patient.c patient.h  structprojet.h game.h fonctionutile.h grid.h plateau.h sauvegarde.h 
	gcc -c patient.c

plateau.o : plateau.c plateau.h structprojet.h  grid.h
	gcc -c plateau.c

sauvegarde.o : sauvegarde.c sauvegarde.h structprojet.h fonctionutile.h grid.h plateau.h patient.h economy.h 
	gcc -c sauvegarde.c

scoreboard.o : sauvegarde.h sauvegarde.c scoreboard.h 
	gcc -c scoreboard.c

action_player.o  : action_player.c action_player.h structprojet.h grid.h plateau.h sauvegarde.h fonctionutile.h economy.h patient.h base.h creer.h display.h
	gcc -c action_player.c
