#include <stdio.h>
#include <stdlib.h>

void save_game(_jeu *jeu, const char *filename){
	printf("%d octet \n",sizeof(*jeu));
	FILE *f = fopen(filename, "wb");

	if (f == NULL) {
	printf("Erreur ouverture %s\n", filename);
	return;
	}

	// --- GAME STATE ---
	if (fwrite(&jeu->nb_step, sizeof(int), 1, f) != 1) {
		printf("Erreur sauvegarde nb_step\n");
		fclose(f);
		return;
	}

	if (fwrite(&jeu->play, sizeof(int), 1, f) != 1) {
		printf("Erreur sauvegarde play\n");
		fclose(f);
		return;
	}

	if (fwrite(&jeu->profit, sizeof(float), 1, f) != 1) {
		printf("Erreur sauvegarde profit\n");
		fclose(f);
		return;
	}

	if (fwrite(&jeu->patient_minimum_spawn_intervalle, sizeof(int),1, f) != 1) {
		printf("Erreur sauvegarde patient_minimum_spawn_intervalle\n");
		fclose(f);
		return;
	}

	if (fwrite(&jeu->patient_spawn_range, sizeof(int), 1, f) != 1) {
		printf("Erreur sauvegarde patient_spawn_range\n");
		fclose(f);
		return;
	}

	if (fwrite(&jeu->patient_spawning_hapiness, sizeof(int), 1, f) != 1) {
		printf("Erreur sauvegarde patient_spawning_hapiness\n");
		fclose(f);
		return;
	}

	if (fwrite(&jeu->patient_hapiness_range, sizeof(int), 1, f) != 1) {
		printf("Erreur sauvegarde patient_hapiness_range\n");
		fclose(f);
		return;
	}

	if (fwrite(&jeu->next_patient_time, sizeof(int), 1, f) != 1) {
		printf("Erreur sauvegarde next_patient_time\n");
		fclose(f);
		return;
	}

	if (fwrite(&jeu->nb_plateau, sizeof(int), 1, f) != 1) {
		printf("Erreur sauvegarde nb_plateau\n");
		fclose(f);
		return;
	}

	// --- USERNAME ---
/*
	if (fwrite(jeu->username, sizeof(char), username_SIZE,f) != username_SIZE) {
		printf("Erreur sauvegarde username\n");
		fclose(f);
		return;
	}
*/
	// --- GRID SIZE ---
	if (fwrite(&jeu->grid_size_x, sizeof(int), 1, f) != 1) {
		printf("Erreur sauvegarde grid_size_x\n");
		fclose(f);
		return;
	}

	if (fwrite(&jeu->grid_size_y, sizeof(int), 1, f) != 1) {
		printf("Erreur sauvegarde grid_size_y\n");
		fclose(f);
		return;
	}

	// --- PLAYER ---
	if (fwrite(&jeu->player, sizeof(_player), 1, f) != 1) {
		printf("Erreur sauvegarde player\n");
		fclose(f);
		return;
	}

	// --- HUMEUR ---
	if (fwrite(jeu->hummeur_tab, sizeof(int), NB_hummeur,f) != NB_hummeur) {
		printf("Erreur sauvegarde hummeur_tab\n");
		fclose(f);
		return;
	}

	// --- GRID ---
	for (int y = 0; y < jeu->grid_size_y; y++) {
	if (fwrite(jeu->grid[y], sizeof(_tile), jeu->grid_size_x,f) != jeu->grid_size_x) {
		printf("Erreur sauvegarde grid\n");
		fclose(f);
		return;
	}
	}

	// --- PLATEAUX ---
	for (int i = 0; i < jeu->nb_plateau; i++) {

	if (fwrite(jeu->plateau_tab[i].tools, sizeof(int), NB_TOOLS, f) != NB_TOOLS) {
		printf("Erreur sauvegarde tools\n");
		fclose(f);
		return;
	}

	if (fwrite(jeu->plateau_tab[i].used_tools, sizeof(int), NB_TOOLS, f) != NB_TOOLS) {
		printf("Erreur sauvegarde used_tools\n");
		fclose(f);
		return;
	}

	if (fwrite(&jeu->plateau_tab[i].id, sizeof(int), 1, f) != 1) {
		printf("Erreur sauvegarde id\n");
		fclose(f);
		return;
	}

	int has_patient = (jeu->plateau_tab[i].patient != NULL);

	if (fwrite(&has_patient,sizeof(int), 1, f) != 1) {
		printf("Erreur sauvegarde has_patient\n");
		fclose(f);
		return;
	}

	if (has_patient) {
		if (fwrite(jeu->plateau_tab[i].patient,sizeof(_patient), 1, f) != 1) {
			printf("Erreur sauvegarde patient\n");
			fclose(f);
			return;
			}
		}
	}

	fclose(f);
	printf("Partie sauvegardee avec succes !\n");
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
	
