_tile cree_tile() {                                              //creer une case de la grille avec des données par défaut                                    
	_tile new_tile;  
	new_tile.value = 0;
	new_tile.player = 0;
	return new_tile;
}



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



