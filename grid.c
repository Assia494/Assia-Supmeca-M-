#include "grid.h"



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




void move_player(_tile** grid,int size_x,int size_y,_movement movement) {               //déplace le joueur dans la grille de jeu en fonction du mouvement demandé(stocké dans movement) 
	_coord initial_pos;
	_coord final_pos;

	initial_pos = get_player_pos_from_grid(grid,size_x,size_y);           //avoir la position du joueur
	if((initial_pos.x<0)||(initial_pos.y<0)){
        printf("player can't be found\n");
        exit(2);
    }
    
	if (inter_check(initial_pos.x,0,size_x)&&inter_check(initial_pos.y,0,size_y)) {
        //calcule la position final en fonction de la position du joueur et du mouvement demandé
		final_pos.x = initial_pos.x +(movement==RIGHT) -(movement==LEFT) ;              
		final_pos.y = initial_pos.y -(movement==UP) +(movement==DOWN) ;

		if (inter_check(final_pos.x,0,size_x)&&inter_check(final_pos.y,0,size_y)) { //vérifie si la position final est acceptable

			if (can_move_at_pos(grid,size_x,size_y,final_pos.x,final_pos.y)) {
			    //déplace le joueur
				grid[initial_pos.y][initial_pos.x].player = 0;
				grid[final_pos.y][final_pos.x].player = 1;

			}
			else{
			    //printf("Couldn't move\n");
			}


		}
	}

}



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



void get_grid_size_from_string(char map_string[] ,int* size_x ,int* size_y){    //avoir la taille de la grille à partir de sa chaine de caractere représentative
    int temp_size_x = 0;
    int new_size_x = 0;
    
    int new_size_y = 0;
    //for(int i=0;i<taille;i++){
    int i = 0;
    while(map_string[i] != '@'){ //tant que le caractere actuelle n'est pas le caractere de fin
        if(map_string[i] == '_'){//si le caractere est un caractere de changement de ligne (on passe à la prochaine ligne)
            if((temp_size_x > new_size_x)&&(new_size_y==0)){
                new_size_x = temp_size_x;
            }
            else if((temp_size_x != new_size_x)&&(new_size_y!=0)){//si le nombre de caractere entre chaque caractere de changement de ligne n'est pas tous la même que la premiere intervalle  (  10010ahbh_djzdj_..._@     1er intervalle = 9,2eme intervalle = 5 => 1er intervalle != 2eme intervalle => format de la chaine de caractère est incorrect               )
                printf("le string map a une forme irregulier x:%d!=%d && y=%d\n",temp_size_x,new_size_x,new_size_y);
                exit(0);
            }
            new_size_y ++;//compter les lignes
            temp_size_x=0;
        }
        else{
            temp_size_x++;//compter les colones
        }
        i++;
    }
    //noter la taille
    *size_x = new_size_x;
    *size_y = new_size_y;
}


_tile** make_grid_from_string(char string[] ,int max_size_x ,int max_size_y){ //à partir d'une d'une chaine de caractère ,on construit la grille de jeu
    _tile** new_grid = NULL;
    new_grid = cree_grid(max_size_x ,max_size_y);
    int string_ind = 0;
    char c ;
    char c2[2];
    c2[1] = '\0';
    for(int dy=0;dy<max_size_y;dy++){
        for(int dx=0 ;dx<1+max_size_x ;dx++){
            
            c = string[string_ind];
            if(c=='_'){
                dx = 0;
                dy ++;
                string_ind++;
            }
            //printf("c\n");
            c = string[string_ind];
            //printf("%c\n",c);
            if(string_ind < max_size_y*(1+max_size_x)){
                if(c!='P'){
                    if(inter_check(c,'0','9')){
                        c2[0] = c;
                        new_grid[dy][dx].value = atoi(c2);
                    }
                    else{
                        new_grid[dy][dx].value = c;
                    }
                }
                else{
                    new_grid[dy][dx].value  = 0;   
                    new_grid[dy][dx].player = 1;    
                }
                
                
                
            }
            string_ind++;
            
    }
    
    return new_grid;
}
}





