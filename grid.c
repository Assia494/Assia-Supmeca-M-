#include <grid.h>



_tile** cree_grid(int size_x,int size_y) {                       //crée un tableau case 2D   ,pour la grille de jeu

	_tile** new_grid = NULL;
	new_grid = malloc( size_y * sizeof(_tile*) );
	exit_if_null_pointer(new_grid);

	for(int dy=0 ; dy<size_y ; dy++) {
		new_grid[dy] = malloc( size_x * sizeof(_tile) );
		exit_if_null_pointer(new_grid[dy]);

		for(int dx=0 ; dx<size_x ; dx++) {
			new_grid[dy][dx] = cree_tile();
			new_grid[dy][dx].value = 0;
		}
	}
	return new_grid;

}


_tile cree_tile() {                                              //creer une case de la grille avec des données par défaut                                    
	_tile new_tile;  
	new_tile.value = 0;
	new_tile.player = 0;
	return new_tile;
}



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





_tile get_tile_from_pos(_tile** grid,int size_x,int size_y,int x,int y) {      // a partir d une position on veut recup la case
	exit_if_null_pointer(grid);

	if(!inter_check(x,0,size_x) + !inter_check(x,0,size_x)) {                                             // si la case à la position demander depasse les limites de la grille (sort de la grille)
		printf("Got incoherent value  size=(x=%d/y=%d) ,position=(x=%d/y=%d)",size_x,size_y,x,y);         // pr pas que la case choisie ne soit pas negative ou en dehors du tableau
		exit(1);
	}
	return grid[y][x];                                                                                    //retourn la case trouver

}



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


_coord get_player_pos_from_grid(_tile** grid,int size_x,int size_y);



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









