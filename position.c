#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include<unistd.h>

#include <string.h>
#include <errno.h> 

#define GRID_SIZE_X 20
#define GRID_SIZE_Y 10

#define NB_TOOLS 7  //sans le gant
#define NB_MALADIE 5 //sans le test_desease
#define NB_hummeur 3
#define MAX_map_string 5000
#define username_SIZE 50


_coord get_player_pos_from_grid(_tile** grid,int size_x,int size_y) {  //deplacer dans chaque case de la grille si la case player est 1 : retrourner la position -> utile pr en cas de reinitialisation. 
	_coord pos;
	pos.x = -1;
	pos.y = -1;
	exit_if_null_pointer(grid);
	for(int dy=0 ; dy<size_y ; dy++) {                                  //parcours ligne  de en haut vers le bas
		for(int dx=0 ; dx<size_x ; dx++) {                              //parcours colonne  de gauche vers la droite
			if(grid[dy][dx].player == 1) {
				pos.x = dx;
				pos.y = dy;
				return pos;                //retourner la position du joueur si trouvé
			}
		}
	}
	printf("Player not found\n");   
	return pos;                            //retourner la position (-1,-1) si le joueur n'est pas trouvé
}
//-----------------------------------------------------------
_tile get_tile_from_pos(_tile** grid,int size_x,int size_y,int x,int y) {      // a partir d une position on veut recup la case
	exit_if_null_pointer(grid);

	if(!inter_check(x,0,size_x) + !inter_check(x,0,size_x)) {                                             // si la case à la position demander depasse les limites de la grille (sort de la grille)
		printf("Got incoherent value  size=(x=%d/y=%d) ,position=(x=%d/y=%d)",size_x,size_y,x,y);         // pr pas que la case choisie ne soit pas negative ou en dehors du tableau
		exit(1);
	}
	return grid[y][x];                                                                                    //retourn la case trouver

}
//-----------------------------------------------------------
_coord get_element_pos_from_grid(_tile** grid,int size_x,int size_y,int element) { //avoir la position d'une case avec une valeur spécifique dans la grille de jeu(par exemple la position des cases d'actions)
	_coord pos;
	pos.x = -1;
	pos.y = -1;
	exit_if_null_pointer(grid);
	for(int dy=0 ; dy<size_y ; dy++) {
		for(int dx=0 ; dx<size_x ; dx++) {
			if(grid[dy][dx].value == element) {
				pos.x = dx;
				pos.y = dy;
				return pos;
			}
		}
	}
	return pos;
}
//-----------------------------------------------------------
_plateau cree_plateau(int new_id){ //crée un variable plateau
    _plateau new_plateau;
    for(int i=0;i<NB_TOOLS;i++){
        new_plateau.tools[i] = 0;       //pas d'outils propre
        new_plateau.used_tools[i] = 0;  //pas d'outils usée
    }
    
    new_plateau.patient = NULL;     //pas de patient <=> plateau.patient == NULL
    new_plateau.id = new_id;        // le nom du plateau qui est dans l'intervalle [t,z]
    return new_plateau;
}
//-----------------------------------------------------------
int can_move_at_pos(_tile** grid,int size_x,int size_y,int x,int y) {        //(x,y) est la position d'arriver du déplacement
	int tile_value = 0;
	_tile tile;

	tile = get_tile_from_pos(grid,size_x,size_y,x,y) ;                          // a partir du type de case est ce que  le joueur peut sy rendre
	tile_value = tile.value;
	if(inter_check(tile_value,'a','z')){                                        // les cases de valeurs 'a' à 'z' sont des cases d'actions  ,de plus ces case le joueur peut se déplacer sur cette case donc return 1
	    return 1;
	}
	switch(tile_value) {                                                   
    	default: //si la case n'est pas du vide
    		return 0; 
    	case 0: //si la case est du vide
    		return 1;
	}
}
