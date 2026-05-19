#include "sauvegarde.h"

void save_game(const _jeu *jeu, const char *filename)
{
    if (jeu == NULL || filename == NULL) {
        fprintf(stderr, "save_game : argument NULL\n");
        return;
    }

    FILE *file = fopen(filename, "w");

    if (file == NULL) {
        fprintf(stderr, "Erreur ouverture %s : %s\n", filename,strerror(errno));
        return;
    }

    printf("Saving game...\n");

    /* ---------- HEADER ---------- */

    if (fprintf(file, "SAVEFILE 1\n") < 0) {
        fprintf(stderr, "Erreur écriture header\n");
        fclose(file);
        return;
    }

    /* ---------- GAME STATE ---------- */

    if (fprintf(file, "happy_bar_len %d\n", jeu->happy_bar_len) < 0) {
        fclose(file);
        return;
    }

    if (fprintf(file, "patient_minimum_spawn_intervalle %d\n", jeu->patient_minimum_spawn_intervalle) < 0) {
        fclose(file);
        return;
    }

    if (fprintf(file, "patient_spawn_range %d\n", jeu->patient_spawn_range) < 0) {
        fclose(file);
        return;
    }

    if (fprintf(file,"patient_spawning_hapiness %d\n", jeu->patient_spawning_hapiness) < 0) {
        fclose(file);
        return;
    }

    if (fprintf(file, "patient_hapiness_range %d\n", jeu->patient_hapiness_range) < 0) {
        fclose(file);
        return;
    }

    if (fprintf(file, "next_patient_time %d\n", jeu->next_patient_time) < 0) {
        fclose(file);
        return;
    }

    if (fprintf(file, "hummeur_tab %d %d %d\n", jeu->hummeur_tab[0],jeu->hummeur_tab[1], jeu->hummeur_tab[2]) < 0) {
        fclose(file);
        return;
    }

    if (fprintf(file, "nb_step %d\n", jeu->nb_step) < 0) {
        fclose(file);
        return;
    }

    /* ---------- PLAYER ---------- */

    if (fprintf(file, "tool_type %d\n", jeu->player.tool.type) < 0) {
        fclose(file);
        return;
    }

    if (fprintf(file, "tool_clean %d\n", jeu->player.tool.clean) < 0) {
        fclose(file);
        return;
    }

    if (fprintf(file, "tool_used %d\n", jeu->player.tool.used) < 0) {
        fclose(file);
        return;
    }

    if (fprintf(file, "glove_type %d\n", jeu->player.glove.type) < 0) {
        fclose(file);
        return;
    }

    if (fprintf(file, "glove_used %d\n", jeu->player.glove.used) < 0) {
        fclose(file);
        return;
    }

    /* ---------- GRID ---------- */

    if (jeu->grid == NULL) {
        fprintf(stderr, "Erreur : grid NULL\n");
        fclose(file);
        return;
    }

    if (fprintf(file, "grid_size_x %d\n",jeu->grid_size_x) < 0) {
        fclose(file);
        return;
    }

    if (fprintf(file, "grid_size_y %d\n",jeu->grid_size_y) < 0) {
        fclose(file);
        return;
    }

    for (int y = 0; y < jeu->grid_size_y; y++) {

        for (int x = 0; x < jeu->grid_size_x; x++) {

            _tile tile = jeu->grid[y][x];
            char c = '?';

            if (tile.player == 1) {
                c = 'P';
            }
            else if (inter_check(tile.value, 'a', 'z') || inter_check(tile.value, 'A', 'Z')) {
                c = (char)tile.value;
            }
            else if (inter_check(tile.value, 0, 9)) {
                c = (char)(tile.value + '0');
            }
            else {
                fprintf(stderr,
                        "Valeur invalide grid (%d,%d)\n",
                        x, y);
                fclose(file);
                return;
            }

            if (fputc(c, file) == EOF) {
                fprintf(stderr, "Erreur écriture grille\n");
                fclose(file);
                return;
            }
        }

        if (fputc('\n', file) == EOF) {
            fprintf(stderr, "Erreur écriture newline grille\n");
            fclose(file);
            return;
        }
    }

    /* ---------- PLATEAUX ---------- */

    if (jeu->plateau_tab == NULL &&jeu->nb_plateau > 0) {

        fprintf(stderr, "plateau_tab NULL\n");
        fclose(file);
        return;
    }

    for (int p = 0; p < jeu->nb_plateau; p++) {

        _plateau *plateau = &jeu->plateau_tab[p];

        if (fprintf(file,
                    "plateau_id %d\n",
                    plateau->id) < 0) {
            fclose(file);
            return;
        }

        if (fprintf(file, "plateau_tool") < 0) {
            fclose(file);
            return;
        }

        for (int i = 0; i < NB_TOOLS; i++) {

            if (fprintf(file, " %d",plateau->tools[i]) < 0) {
                fclose(file);
                return;
            }
        }

        if (fputc('\n', file) == EOF) {
            fclose(file);
            return;
        }

        if (fprintf(file, "has_patient %d\n", plateau->patient != NULL) < 0) {
            fclose(file);
            return;
        }

        if (plateau->patient != NULL) {

            _patient *patient = plateau->patient;

            if (fprintf(file,  "patient_humeur %d\n", patient->hummeur) < 0) {
                fclose(file);
                return;
            }

            if (fprintf(file, "patient_etat %d\n", patient->etat) < 0) {
                fclose(file);
                return;
            }

            if (fprintf(file, "patient_maladie_type %d\n", patient->maladie.type) < 0) {
                fclose(file);
                return;
            }

            if (fprintf(file, "patient_profit %f\n", patient->maladie.profit) < 0) {
                fclose(file);
                return;
            }
        }
    }

    fclose(file);

    printf("Sauvegarde réussie !\n");
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
            exit_if_null_pointer(jeu->plateau_tab[i].void free_game(_jeu *jeu){
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
} patient);

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
