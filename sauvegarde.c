#include "sauvegarde.h"

void free_game(_jeu *jeu){
	if (jeu == NULL) {
		return;
	}

	// Libération des patients
	if (jeu->plateau_tab != NULL){

		for (int i = 0; i < jeu->nb_plateau; i++){

			if (jeu->plateau_tab[i].patient != NULL){
				free(jeu->plateau_tab[i].patient);
				jeu->plateau_tab[i].patient = NULL;
			}
		}

		free(jeu->plateau_tab);
		jeu->plateau_tab = NULL;
	}

	// LibÃ©ration de la grille
	if (jeu->grid != NULL) {

	for (int y = 0; y < jeu->grid_size_y; y++) {

		if (jeu->grid[y] != NULL) {
			free(jeu->grid[y]);
			}
		}

		free(jeu->grid);
		jeu->grid = NULL;
	}

	jeu->nb_plateau = 0;
	jeu->grid_size_x = 0;
	jeu->grid_size_y = 0;
} 
//-----------------------------------------------------------
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
//-----------------------------------------------------------
void save_game(const _jeu *jeu, const char* filename){
	//printf("%d octet \n",sizeof(jeu));
	
	FILE *file = fopen(filename, "w+");
	exit_if_null_pointer(file);
	printf("saving game\n");
	fprintf(file ,"savefile 1\n");

	
	// les paramètres du jeu
	fprintf(file, "happy_bar_len %d\n", jeu->happy_bar_len);
    fprintf(file, "patient_minimum_spawn_intervalle %d\n", jeu->patient_minimum_spawn_intervalle);
    fprintf(file, "patient_spawn_range %d\n", jeu->patient_spawn_range);
    fprintf(file, "patient_spawning_hapiness %d\n", jeu->patient_spawning_hapiness);
    fprintf(file, "patient_hapiness_range %d\n", jeu->patient_hapiness_range);
    fprintf(file, "next_patient_time %d\n", jeu->next_patient_time);
    fprintf(file, "hummeur_tab %d %d %d\n", jeu->hummeur_tab[0], jeu->hummeur_tab[1], jeu->hummeur_tab[2]);
    fprintf(file, "nb_step %d\n", jeu->nb_step);
    fprintf(file, "profit %.2f\n", jeu->profit);
	
	//fprintf(file ,"\n\n");
	// les informations du joueur
	fprintf(file ,"tool_type %d\n",jeu->player.tool.type);
	fprintf(file ,"tool_clean %d\n",jeu->player.tool.clean);
	fprintf(file ,"tool_used %d\n",jeu->player.tool.used);
	fprintf(file ,"glove_type %d\n",jeu->player.glove.type);
	fprintf(file ,"glove_used %d\n",jeu->player.glove.used);
	jeu->player.pos = get_player_pos_from_grid(jeu->grid,jeu->grid_size_x,jeu->grid_size_y);
	fprintf(file ,"player_pos_x %d\n",jeu->player.pos.x);
	fprintf(file ,"player_pos_y %d\n",jeu->player.pos.y);
	
	//fprintf(file ,"\n\n");
	// les informations de la grille de jeu
	fprintf(file ,"grid_size_x %d\n",jeu->grid_size_x);
	fprintf(file ,"grid_size_y %d\n",jeu->grid_size_y);
	_tile tile;
	for(int dy=0;dy<jeu->grid_size_y;dy++){
		for(int dx=0;dx<jeu->grid_size_x;dx++){
			tile = jeu->grid[dy][dx];
			if(inter_check(tile.value,'a','z')||inter_check(tile.value,'A','Z')){
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
	fputs("@\n",file);
	
	//fprintf(file ,"\n\n");
	// les informations sur les plateaux
	fprintf(file ,"nb_plateau %d\n",jeu->nb_plateau);
	
	for(int plateau_ind=0;plateau_ind<jeu->nb_plateau;plateau_ind++){
		fprintf(file ,"plateau_id %d\n",jeu->plateau_tab[plateau_ind].id);
		fprintf(file ,"plateau_tool");
		for(int i=0;i<NB_TOOLS;i++){
			fprintf(file ," %d",jeu->plateau_tab[plateau_ind].tools[i]);
		}
		fputc('\n',file);
		fprintf(file ,"plateau_used_tool");
		for(int i=0;i<NB_TOOLS;i++){
			fprintf(file ," %d",jeu->plateau_tab[plateau_ind].used_tools[i]);
		}
		fputc('\n',file);
		fprintf(file ,"has_patient %d\n",jeu->plateau_tab[plateau_ind].patient != NULL);
		if(jeu->plateau_tab[plateau_ind].patient != NULL){
			fprintf(file ,"|hummeur %d\n",jeu->plateau_tab[plateau_ind].patient->hummeur);
			fprintf(file ,"|etat %d\n",jeu->plateau_tab[plateau_ind].patient->etat);
			fprintf(file ,"|maladie_type %d\n",jeu->plateau_tab[plateau_ind].patient->maladie.type);
			fprintf(file ,"|profit %f\n",jeu->plateau_tab[plateau_ind].patient->maladie.profit);
			fprintf(file ,"|tool_needed");
			for(int i=0;i<NB_TOOLS;i++){
				fprintf(file ," %d",jeu->plateau_tab[plateau_ind].patient->maladie.tool_needed[i]);
			}
			fputc('\n',file);
		}
	}
	printf("saving game done\n");
    fclose(file);
}

void load_game(_jeu *jeu, const char *filename){


	

    FILE *file = fopen(filename, "r");
	exit_if_null_pointer(file);
	printf("loading game\n");
	
	free_game(jeu);
	
	int savefile = 0;
	fscanf(file,"savefile %d\n",&savefile);
	if(!savefile){
		printf("il n'y a pas de partie sauvegardé\n");
	}
	//printf("il y a une partie sauvegardé\n");
	jeu->play = 1;
	// les paramètres du jeu
	fscanf(file,"happy_bar_len %d\n",&jeu->happy_bar_len);
	fscanf(file,"patient_minimum_spawn_intervalle %d\n",&jeu->patient_minimum_spawn_intervalle);
	fscanf(file,"patient_spawn_range %d\n",&jeu->patient_spawn_range);
	fscanf(file,"patient_spawning_hapiness %d\n",&jeu->patient_spawning_hapiness);
	fscanf(file,"patient_hapiness_range %d\n",&jeu->patient_hapiness_range);
	fscanf(file,"next_patient_time %d\n",&jeu->next_patient_time);
	fscanf(file,"hummeur_tab");
	for(int i=0;i<NB_hummeur;i++){
		fscanf(file," %d",&jeu->hummeur_tab[i]);
	}
	fscanf(file,"\n");
	fscanf(file,"nb_step %d\n",&jeu->nb_step);
	fscanf(file,"profit %f\n",&jeu->profit);
	
	// les informations du joueur
	fscanf(file,"tool_type %d\n",&jeu->player.tool.type);
	fscanf(file,"tool_clean %d\n",&jeu->player.tool.clean);
	fscanf(file,"tool_used %d\n",&jeu->player.tool.used);
	fscanf(file,"glove_type %d\n",&jeu->player.glove.type);
	fscanf(file,"glove_used %d\n",&jeu->player.glove.used);
	fscanf(file,"player_pos_x %d\n",&jeu->player.pos.x);
	fscanf(file,"player_pos_y %d\n",&jeu->player.pos.y);
	
	
	// les informations de la grille de jeu
	fscanf(file,"grid_size_x %d\n",&(jeu->grid_size_x));
	fscanf(file,"grid_size_y %d\n",&(jeu->grid_size_y));
	char map_string[MAX_map_string];
	char c;
	int i = 0;
	do{
		c = fgetc(file);
		map_string[i] = c;
		i++;
	}while(c != EOF && c != '@');
	map_string[i] = '\0';
	fscanf(file,"\n");
    jeu->grid = make_grid_from_string(map_string   ,jeu->grid_size_x    ,jeu->grid_size_y);
	jeu->grid[jeu->player.pos.y][jeu->player.pos.x].player = 1;


	//initialisation des plateaux
	fscanf(file,"nb_plateau %d\n",&jeu->nb_plateau);
	jeu->plateau_tab = NULL;
	jeu->plateau_tab = malloc((jeu->nb_plateau)*sizeof(_plateau));
	exit_if_null_pointer(jeu->plateau_tab);
	int has_patient = 0;
	for(int plateau_ind=0;plateau_ind<jeu->nb_plateau;plateau_ind++){
		fscanf(file,"plateau_id %d\n",&jeu->plateau_tab[plateau_ind].id);	
		fscanf(file,"plateau_tool");
		for(int i=0;i<NB_TOOLS;i++){
			fscanf(file," %d",&jeu->plateau_tab[plateau_ind].tools[i]);
		}
		fscanf(file,"\n");
		fscanf(file,"plateau_used_tool");
		for(int i=0;i<NB_TOOLS;i++){
			fscanf(file," %d",&jeu->plateau_tab[plateau_ind].used_tools[i]);
		}
		fscanf(file,"\n");
		has_patient = 0;
		fscanf(file,"has_patient %d\n",&has_patient);
		if(has_patient){
			jeu->plateau_tab[plateau_ind].patient = cree_patient();
			fscanf(file,"|hummeur %d\n",&jeu->plateau_tab[plateau_ind].patient->hummeur);
			fscanf(file,"|etat %d\n",&jeu->plateau_tab[plateau_ind].patient->etat);
			fscanf(file,"|maladie_type %d\n",&jeu->plateau_tab[plateau_ind].patient->maladie.type);
			fscanf(file,"|profit %f\n",&jeu->plateau_tab[plateau_ind].patient->maladie.profit);
			
			
			fscanf(file,"|tool_needed ");
			for(int i=0;i<NB_TOOLS;i++){
				fscanf(file," %d",&jeu->plateau_tab[plateau_ind].patient->maladie.tool_needed[i]);
			}
			fscanf(file,"\n");
			//fscanf(file,"tool_needed %d\n",&jeu->plateau_tab[plateau_ind].patient.maladie.tool_needed);
		}
		else{
			jeu->plateau_tab[plateau_ind].patient = NULL;
		}
		
	}
	printf("loading game done\n");
	fclose(file);
	
}
