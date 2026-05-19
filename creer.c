#include "structprojet.h"
#include "fonctionutile.h"


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


void make_tool_tab(int tab[] ,int a,int b,int c,int d,int e,int f,int g){
    tab[0] = a;
    tab[1] = b;
    tab[2] = c;
    tab[3] = d;
    tab[4] = e;
    tab[5] = f;
    tab[6] = g;
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



_jeu creer_jeu(){
    _jeu new_game;
    new_game.play = 0;
    new_game.player.tool.type = 0;
    new_game.player.glove.type = 0;
    new_game.profit = 0.00f;

    new_game.grid_size_x = 0;
    new_game.grid_size_y = 0;
    new_game.grid = NULL;
    char* map_string = get_random_map_string();
    get_grid_size_from_string(map_string ,&(new_game.grid_size_x) ,&(new_game.grid_size_y));
    new_game.grid = make_grid_from_string(map_string ,new_game.grid_size_x ,new_game.grid_size_y);

    new_game.happy_bar_len = 32;
    new_game.nb_plateau = 0;
    new_game.plateau_tab = NULL;
    new_game.plateau_tab = get_plateau_tab(new_game.grid ,new_game.grid_size_x ,new_game.grid_size_y ,&(new_game.nb_plateau));

    new_game.hummeur_tab[0] = 0;
    new_game.hummeur_tab[1] = 0;
    new_game.hummeur_tab[2] = 0;
    new_game.patient_minimum_spawn_intervalle = 4*(new_game.nb_plateau);
    new_game.patient_spawn_range = 10;
    new_game.patient_spawning_hapiness = 37*(new_game.nb_plateau);
    new_game.patient_hapiness_range = 7*(new_game.nb_plateau);
    new_game.next_patient_time = 13 + 0*(new_game.patient_minimum_spawn_intervalle + randint(0 ,new_game.patient_spawn_range));
    new_game.nb_step = -1;

    return new_game;
}



int* cree_tab(int size){//cree un tableau 
    //(temporaire)
    int* tab =NULL;
    tab = malloc(size*sizeof(int));
    exit_if_null_pointer(tab);
    for(int i=0;i<size;i++){
        tab[i] = randint(100,10000);  //initialisation des valeurs du tableau cree 
    }
    
    return tab;
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
