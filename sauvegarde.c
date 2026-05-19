#include "sauvegarde.h"


void save_game(_jeu jeu, const char* filename){
	//printf("%d octet \n",sizeof(jeu));
	
	FILE *file = fopen(filename, "w");
	exit_if_null_pointer(file);
	printf("saving game\n");
	fprintf(file ,"savefile 1\n");

	
	// les paramètres du jeu
	fprintf(file ,"happy_bar_len %d\n",jeu.happy_bar_len);
	fprintf(file ,"patient_minimum_spawn_intervalle %d\n",jeu.patient_minimum_spawn_intervalle);
	fprintf(file ,"patient_spawn_range %d\n",jeu.patient_spawn_range);
	fprintf(file ,"patient_spawning_hapiness %d\n",jeu.patient_spawning_hapiness);
	fprintf(file ,"patient_hapiness_range %d\n",jeu.patient_hapiness_range);
	fprintf(file ,"next_patient_time %d\n",jeu.next_patient_time);
	fprintf(file ,"hummeur_tab %d %d %d\n",jeu.hummeur_tab[0],jeu.hummeur_tab[1],jeu.hummeur_tab[2]);
	fprintf(file ,"nb_step %d\n",jeu.nb_step);
	
	//fprintf(file ,"\n\n");
	// les informations du joueur
	fprintf(file ,"tool_type %d\n",jeu.player.tool.type);
	fprintf(file ,"tool_clean %d\n",jeu.player.tool.clean);
	fprintf(file ,"tool_used %d\n",jeu.player.tool.used);
	fprintf(file ,"glove_type %d\n",jeu.player.glove.type);
	fprintf(file ,"glove_used %d\n",jeu.player.glove.used);
	
	//fprintf(file ,"\n\n");
	// les informations du joueur
	fprintf(file ,"tool_type %d\n",jeu.player.tool.type);
	fprintf(file ,"tool_clean %d\n",jeu.player.tool.clean);
	fprintf(file ,"tool_used %d\n",jeu.player.tool.used);
	fprintf(file ,"glove_type %d\n",jeu.player.glove.type);
	fprintf(file ,"glove_used %d\n",jeu.player.glove.used);
	
	//fprintf(file ,"\n\n");
	// les informations de la grille de jeu
	fprintf(file ,"grid_size_x %d\n",jeu.grid_size_x);
	fprintf(file ,"grid_size_y %d\n",jeu.grid_size_y);
	_tile tile;
	for(int dy=0;dy<jeu.grid_size_y;dy++){
		for(int dx=0;dx<jeu.grid_size_x;dx++){
			tile = jeu.grid[dy][dx];
			//printf("grid(%d,%d)   max(%d,%d)\n",dy,dx,jeu.grid_size_y,jeu.grid_size_x);
			if(tile.player == 1){
				fputc('P',file);
			}
			else if(inter_check(tile.value,'a','z')||inter_check(tile.value,'A','Z')){
				fputc(tile.value ,file);
			}
			else if(inter_check(tile.value,0,9)){
				fputc(tile.value + '0' ,file);
			}
			else{
				printf("Il y a une erreur dans la sauvegarde de la grille du jeu");
			}
		}
		fputc('_',file);
	}
	fputc('@',file);
	
	//fprintf(file ,"\n\n");
	// les informations sur les plateaux
	for(int plateau_ind=0;plateau_ind<jeu.nb_plateau;plateau_ind++){
		fprintf(file ,"plateau_id %d\n",jeu.plateau_tab[plateau_ind].id);
		fprintf(file ,"plateau_tool");
		for(int i=0;i<NB_TOOLS;i++){
			fprintf(file ," %d",jeu.plateau_tab[plateau_ind].tools[i]);
		}
		fputc('\n',file);
		fprintf(file ,"plateau_used_tool");
		for(int i=0;i<NB_TOOLS;i++){
			fprintf(file ," %d",jeu.plateau_tab[plateau_ind].used_tools[i]);
		}
		fputc('\n',file);
		fprintf(file ,"has_patient %d\n",jeu.plateau_tab[plateau_ind].patient != NULL);
		if(jeu.plateau_tab[plateau_ind].patient != NULL){
			fprintf(file ,"|hummeur %d\n",jeu.plateau_tab[plateau_ind].patient->hummeur);
			fprintf(file ,"|etat %d\n",jeu.plateau_tab[plateau_ind].patient->etat);
			fprintf(file ,"|maladie_type %d\n",jeu.plateau_tab[plateau_ind].patient->maladie.type);
			fprintf(file ,"|profit %f\n",jeu.plateau_tab[plateau_ind].patient->maladie.profit);
			fprintf(file ,"|tool_needed");
			for(int i=0;i<NB_TOOLS;i++){
				fprintf(file ," %d",jeu.plateau_tab[plateau_ind].patient->maladie.tool_needed[i]);
			}
			fputc('\n',file);
		}
	}

	new_jeu.play = 0;
     //initialisation du joueur
	new_jeu.player.tool.type = 0;
	new_jeu.player.glove.type = 0;
    new_jeu.profit = 0.00f;
    
    //initialisation de la grille de jeu
	new_jeu.grid_size_x = 0;
    new_jeu.grid_size_y = 0;
	new_jeu.grid = NULL;
    char* map_string = get_random_map_string();
    get_grid_size_from_string(map_string ,&(new_jeu.grid_size_x) ,&(new_jeu.grid_size_y));
    new_jeu.grid = make_grid_from_string(map_string   ,new_jeu.grid_size_x    ,new_jeu.grid_size_y);

    //initialisation des plateaux
    new_jeu.happy_bar_len = 32;
    new_jeu.nb_plateau = 0;
    new_jeu.plateau_tab = NULL ;
    new_jeu.plateau_tab = get_plateau_tab(new_jeu.grid ,new_jeu.grid_size_x ,new_jeu.grid_size_y ,&(new_jeu.nb_plateau));
    
    //initialisation des paramètre des patients
    new_jeu.hummeur_tab[0] = 0;
    new_jeu.hummeur_tab[1] = 0;
    new_jeu.hummeur_tab[2] = 0;
    new_jeu.patient_minimum_spawn_intervalle = 4*(new_jeu.nb_plateau);
    new_jeu.patient_spawn_range = 10 ;
    new_jeu.patient_spawning_hapiness = 37*(new_jeu.nb_plateau);
    new_jeu.patient_hapiness_range = 7*(new_jeu.nb_plateau);
    new_jeu.next_patient_time = 13 + 0*(new_jeu.patient_minimum_spawn_intervalle + randint(0 ,new_jeu.patient_spawn_range));
    
    new_jeu.nb_step = -1;
    
    fclose(file);
}
