#include <stdio.h>
#include <stdlib.h>
#include <structprojet.h>
#include <fonctionutile.h>


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



_patient* cree_patient(){ //cree un patient avec des paramètre par defaut
    _patient* patient = NULL;
    patient = malloc(sizeof(_patient));
    exit_if_null_pointer(patient);
    
    patient->hummeur = 100;
    patient->etat = satisfait;
    
    int desease_index = randint(1,NB_MALADIE);//choisir une maladie aleratoire
    switch(desease_index){//avoir une maladie (les outils necessaire ,type de maladie ,le profit pour soigner ce maladie)
        //parametre de chaque maladie
        default:
            patient->maladie.type = TEST_DESEASE;
            make_tool_tab(patient->maladie.tool_needed,0,1,0,0,1,1,0);
            patient->maladie.profit = 1.23f;
            break;
        case 1:
            patient->maladie.type = DESEASE_A;
            make_tool_tab(patient->maladie.tool_needed,0,1,0,0,1,0,1);
            patient->maladie.profit = 3.20f;
            break;
        case 2:
            patient->maladie.type = DESEASE_B;
            make_tool_tab(patient->maladie.tool_needed,1,0,0,1,1,0,0);
            patient->maladie.profit = 2.90f;
            break;
        case 3:
            patient->maladie.type = DESEASE_C;
            make_tool_tab(patient->maladie.tool_needed,1,0,0,1,0,1,0);
            patient->maladie.profit = 4.14f;
            break;
        case 4:
            patient->maladie.type = DESEASE_D;
            make_tool_tab(patient->maladie.tool_needed,0,0,1,1,0,1,0);
            patient->maladie.profit = 3.49f;
            break;
        case 5:
            patient->maladie.type = DESEASE_E;
            make_tool_tab(patient->maladie.tool_needed,0,1,1,0,0,0,1);
            patient->maladie.profit = 5.65f;
            break;
    }
    
    return patient;
}
