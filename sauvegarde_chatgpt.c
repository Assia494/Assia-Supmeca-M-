void save_game(_jeu *jeu, const char *filename)
{
    FILE *f = fopen(filename, "wb");

    if (!f) return;

    // sauvegarde des données simples
    fwrite(&jeu->nb_step, sizeof(int), 1, f);
    fwrite(&jeu->play, sizeof(int), 1, f);
    fwrite(&jeu->profit, sizeof(float), 1, f);

    fwrite(&jeu->grid_size_x, sizeof(int), 1, f);
    fwrite(&jeu->grid_size_y, sizeof(int), 1, f);

    // sauvegarde du joueur
    fwrite(&jeu->player, sizeof(_player), 1, f);

    // sauvegarde du tableau hummeur
    fwrite(jeu->hummeur_tab, sizeof(int), NB_hummeur, f);

    // ---------------- GRID ----------------

    for (int y = 0; y < jeu->grid_size_y; y++)
    {
        fwrite(jeu->grid[y],
               sizeof(_tile),
               jeu->grid_size_x,
               f);
    }

    // ---------------- PLATEAUX ----------------

    fwrite(&jeu->nb_plateau, sizeof(int), 1, f);

    for (int i = 0; i < jeu->nb_plateau; i++)
    {
        fwrite(&jeu->plateau_tab[i],
               sizeof(_plateau),
               1,
               f);

        // sauvegarde patient si existe
        int has_patient = (jeu->plateau_tab[i].patient != NULL);

        fwrite(&has_patient, sizeof(int), 1, f);

        if (has_patient)
        {
            fwrite(jeu->plateau_tab[i].patient,
                   sizeof(_patient),
                   1,
                   f);
        }
    }

    fclose(f);
}









void load_game(_jeu *jeu, const char *filename)
{
    FILE *f = fopen(filename, "rb");

    if (!f) return;

    fread(&jeu->nb_step, sizeof(int), 1, f);
    fread(&jeu->play, sizeof(int), 1, f);
    fread(&jeu->profit, sizeof(float), 1, f);

    fread(&jeu->grid_size_x, sizeof(int), 1, f);
    fread(&jeu->grid_size_y, sizeof(int), 1, f);

    fread(&jeu->player, sizeof(_player), 1, f);

    fread(jeu->hummeur_tab, sizeof(int), NB_hummeur, f);

    // ---------------- GRID ----------------

    jeu->grid = malloc(sizeof(_tile*) * jeu->grid_size_y);

    for (int y = 0; y < jeu->grid_size_y; y++)
    {
        jeu->grid[y] = malloc(sizeof(_tile) * jeu->grid_size_x);

        fread(jeu->grid[y],
              sizeof(_tile),
              jeu->grid_size_x,
              f);
    }

    // ---------------- PLATEAUX ----------------

    fread(&jeu->nb_plateau, sizeof(int), 1, f);

    jeu->plateau_tab = malloc(sizeof(_plateau) * jeu->nb_plateau);

    for (int i = 0; i < jeu->nb_plateau; i++)
    {
        fread(&jeu->plateau_tab[i],
              sizeof(_plateau),
              1,
              f);

        int has_patient;

        fread(&has_patient, sizeof(int), 1, f);

        if (has_patient)
        {
            jeu->plateau_tab[i].patient =
                malloc(sizeof(_patient));

            fread(jeu->plateau_tab[i].patient,
                  sizeof(_patient),
                  1,
                  f);
        }
        else
        {
            jeu->plateau_tab[i].patient = NULL;
        }
    }

    fclose(f);
}
