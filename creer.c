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



