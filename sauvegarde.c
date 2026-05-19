#include "sauvegarde.h"


void save_game(_jeu jeu, const char* filename){
	//printf("%d octet \n",sizeof(jeu));
	
	FILE *file = fopen(filename, "w+");
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



void load_game(_jeu *jeu, const char *filename)
{
    FILE *f = fopen(filename, "rb");
    if (f == NULL) {
        printf("Impossible d'ouvrir %s\n", filename);
        return;
    }

    free_game(jeu);
    *jeu = (_jeu){0};

    int ok = 1;

    /* ---------------- GAME STATE ---------------- */

    ok &= (fread(&jeu->nb_step, sizeof(int), 1, f) == 1);
    ok &= (fread(&jeu->play, sizeof(int), 1, f) == 1);
    ok &= (fread(&jeu->profit, sizeof(float), 1, f) == 1);

    ok &= (fread(&jeu->patient_minimum_spawn_intervalle, sizeof(int), 1, f) == 1);
    ok &= (fread(&jeu->patient_spawn_range, sizeof(int), 1, f) == 1);
    ok &= (fread(&jeu->patient_spawning_hapiness, sizeof(int), 1, f) == 1);
    ok &= (fread(&jeu->patient_hapiness_range, sizeof(int), 1, f) == 1);
    ok &= (fread(&jeu->next_patient_time, sizeof(int), 1, f) == 1);

    ok &= (fread(&jeu->happy_bar_len, sizeof(int), 1, f) == 1);
	ok &= (fread(&jeu->nb_plateau, sizeof(int), 1, f) == 1);

    if (!ok || jeu->nb_plateau < 0 || jeu->nb_plateau > 100) {
        printf("Erreur lecture (game state)\n");
        fclose(f);
        return;
    }

    /* ---------------- USERNAME ---------------- */

    if (fread(jeu->username, sizeof(char), username_SIZE, f) != username_SIZE) {
        printf("Erreur lecture username\n");
        fclose(f);
        return;
    }
    jeu->username[username_SIZE - 1] = '\0';

    /* ---------------- GRID SIZE ---------------- */

    if (fread(&jeu->grid_size_x, sizeof(int), 1, f) != 1 || fread(&jeu->grid_size_y, sizeof(int), 1, f) != 1)
    {
        printf("Erreur lecture grid size\n");
        fclose(f);
        return;
    }

    if (jeu->grid_size_x <= 0 || jeu->grid_size_x > 500 || jeu->grid_size_y <= 0 || jeu->grid_size_y > 500)
    {
        printf("Taille grille invalide\n");
        fclose(f);
        return;
    }

    /* ---------------- PLAYER ---------------- */

    if (fread(&jeu->player, sizeof(_player), 1, f) != 1) {
        printf("Erreur lecture player\n");
        fclose(f);
        return;
    }

    /* ---------------- HUMEUR ---------------- */

    if (fread(jeu->hummeur_tab, sizeof(int), NB_hummeur, f) != NB_hummeur) {
        printf("Erreur lecture humeur\n");
        fclose(f);
        return;
    }

    /* ---------------- GRID ---------------- */

    jeu->grid = malloc(sizeof(_tile*) * jeu->grid_size_y);
    exit_if_null_pointer(jeu->grid);

    for (int y = 0; y < jeu->grid_size_y; y++) {

        jeu->grid[y] = malloc(sizeof(_tile) * jeu->grid_size_x);
        exit_if_null_pointer(jeu->grid[y]);

        if (fread(jeu->grid[y], sizeof(_tile),
                  jeu->grid_size_x, f) != jeu->grid_size_x)
        {
            printf("Erreur lecture grid\n");
            fclose(f);
            free_game(jeu);
            return;
        }
    }

    /* ---------------- PLATEAUX ---------------- */

    jeu->plateau_tab = malloc(sizeof(_plateau) * jeu->nb_plateau);
    exit_if_null_pointer(jeu->plateau_tab);

    for (int i = 0; i < jeu->nb_plateau; i++) {

        if (fread(jeu->plateau_tab[i].tools, sizeof(int), NB_TOOLS, f) != NB_TOOLS)
        {
            printf("Erreur lecture tools\n");
            fclose(f);
            free_game(jeu);
            return;
        }

        if (fread(jeu->plateau_tab[i].used_tools, sizeof(int), NB_TOOLS, f) != NB_TOOLS)
        {
            printf("Erreur lecture used_tools\n");
            fclose(f);
            free_game(jeu);
            return;
        }

        if (fread(&jeu->plateau_tab[i].id, sizeof(int), 1, f) != 1) {
            printf("Erreur lecture id\n");
            fclose(f);
            free_game(jeu);
            return;
        }

        int has_patient = 0;
        if (fread(&has_patient, sizeof(int), 1, f) != 1) {
            printf("Erreur lecture has_patient\n");
            fclose(f);
            free_game(jeu);
            return;
        }

        if (has_patient) {
            jeu->plateau_tab[i].patient = malloc(sizeof(_patient));
            exit_if_null_pointer(jeu->plateau_tab[i].patient);

            if (fread(jeu->plateau_tab[i].patient,
                      sizeof(_patient), 1, f) != 1)
            {
                printf("Erreur lecture patient\n");
                fclose(f);
                free_game(jeu);
                return;
            }
        } else {
            jeu->plateau_tab[i].patient = NULL;
        }
    }

    fclose(f);

    jeu->player.pos = get_player_pos_from_grid(
        jeu->grid,
        jeu->grid_size_x,
        jeu->grid_size_y
    );

    printf("Partie chargee avec succes !\n");
}
