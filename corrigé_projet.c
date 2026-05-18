#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define NB_TOOLS 7
#define NB_MALADIE 5
#define NB_hummeur 3
#define MAX_map_string 5000
#define USERNAME_SIZE 50
#define MAX_GRID_SIZE 200
#define MAX_PLATEAUX 20

typedef enum {UP=0, RIGHT=1, DOWN=2, LEFT=3} _movement;
typedef enum {TEST_DESEASE=0, DESEASE_A=1, DESEASE_B=2, DESEASE_C=3, DESEASE_D=4, DESEASE_E=5} _maladie_type;
typedef enum {satisfait=0, mecontant=1, furieux=2} _etat;
typedef enum {new_jeu=0, select_menu=1, scoreboard=2, save=3, continu=4, quit=5} _menu;

typedef struct {
    int value;
    int player;
} _tile;

typedef struct {
    int x;
    int y;
} _coord;

typedef struct {
    int clean;
    int used;
    int type;
} _tool;

typedef struct {
    _coord pos;
    _tool tool;
    _tool glove;
} _player;

typedef struct {
    _maladie_type type;
    int tool_needed[NB_TOOLS];
    float profit;
} _maladie;

typedef struct {
    int hummeur;
    _maladie maladie;
    _etat etat;
} _patient;

typedef struct {
    int tools[NB_TOOLS];
    int used_tools[NB_TOOLS];
    _patient* patient;
    int id;
} _plateau;

typedef struct {
    int nb_step;
    int play;
    int hummeur_tab[NB_hummeur];
    _player player;
    float profit;
    int grid_size_x;
    int grid_size_y;
    _tile** grid;
    int happy_bar_len;
    int nb_plateau;
    _plateau* plateau_tab;
    int patient_minimum_spawn_intervalle;
    int patient_spawn_range;
    int patient_spawning_hapiness;
    int patient_hapiness_range;
    int next_patient_time;
    char username[USERNAME_SIZE];
} _jeu;

typedef struct {
    char username[USERNAME_SIZE];
    int nb_step;
    float profit;
    int hummeur_tab[NB_hummeur];
} _score;

void color(unsigned char r, unsigned char g, unsigned char b) {
    printf("\x1B[38;2;%d;%d;%dm", r, g, b);
}

void reset_color() {
    printf("\x1B[0m");
}

int randint(int a, int b) {
    if (b < a) return a;
    return rand() % (b - a + 1) + a;
}

int inter_check(int nb, int a, int b) {
    return (a <= nb) && (nb <= b);
}

void exit_if_null_pointer(void* pointer) {
    if (pointer == NULL) {
        printf(">>Got a NULL pointer\n");
        exit(1);
    }
}

void free_game(_jeu *jeu) {
    if (jeu == NULL) {
        return;
    }

    if (jeu->plateau_tab != NULL) {
        for (int i = 0; i < jeu->nb_plateau; i++) {
            if (jeu->plateau_tab[i].patient != NULL) {
                free(jeu->plateau_tab[i].patient);
                jeu->plateau_tab[i].patient = NULL;
            }
        }
        free(jeu->plateau_tab);
        jeu->plateau_tab = NULL;
    }

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

_tile cree_tile() {
    _tile new_tile;
    new_tile.value = 0;
    new_tile.player = 0;
    return new_tile;
}

_tile** cree_grid(int size_x, int size_y) {
    _tile** new_grid = malloc(size_y * sizeof(_tile*));
    exit_if_null_pointer(new_grid);

    for (int dy = 0; dy < size_y; dy++) {
        new_grid[dy] = malloc(size_x * sizeof(_tile));
        exit_if_null_pointer(new_grid[dy]);

        for (int dx = 0; dx < size_x; dx++) {
            new_grid[dy][dx] = cree_tile();
        }
    }
    return new_grid;
}

_coord get_player_pos_from_grid(_tile** grid, int size_x, int size_y) {
    _coord pos = {-1, -1};
    if (grid == NULL) return pos;
    
    for (int dy = 0; dy < size_y; dy++) {
        for (int dx = 0; dx < size_x; dx++) {
            if (grid[dy][dx].player == 1) {
                pos.x = dx;
                pos.y = dy;
                return pos;
            }
        }
    }
    return pos;
}

_tile get_tile_from_pos(_tile** grid, int size_x, int size_y, int x, int y) {
    _tile empty = {0, 0};
    if (grid == NULL) return empty;
    
    if (!inter_check(x, 0, size_x - 1) || !inter_check(y, 0, size_y - 1)) {
        printf("Got incoherent value size=(x=%d/y=%d), position=(x=%d/y=%d)\n", size_x, size_y, x, y);
        return empty;
    }
    return grid[y][x];
}

void save_game(_jeu *jeu, const char *filename) {
    FILE *f = fopen(filename, "wb");
    if (f == NULL) {
        printf("Erreur ouverture %s: %s\n", filename, strerror(errno));
        return;
    }

    int success = 1;
    
    success = success && (fwrite(&jeu->nb_step, sizeof(int), 1, f) == 1);
    success = success && (fwrite(&jeu->play, sizeof(int), 1, f) == 1);
    success = success && (fwrite(&jeu->profit, sizeof(float), 1, f) == 1);
    success = success && (fwrite(&jeu->patient_minimum_spawn_intervalle, sizeof(int), 1, f) == 1);
    success = success && (fwrite(&jeu->patient_spawn_range, sizeof(int), 1, f) == 1);
    success = success && (fwrite(&jeu->patient_spawning_hapiness, sizeof(int), 1, f) == 1);
    success = success && (fwrite(&jeu->patient_hapiness_range, sizeof(int), 1, f) == 1);
    success = success && (fwrite(&jeu->next_patient_time, sizeof(int), 1, f) == 1);
    success = success && (fwrite(&jeu->nb_plateau, sizeof(int), 1, f) == 1);
    success = success && (fwrite(jeu->username, sizeof(char), USERNAME_SIZE, f) == USERNAME_SIZE);
    success = success && (fwrite(&jeu->grid_size_x, sizeof(int), 1, f) == 1);
    success = success && (fwrite(&jeu->grid_size_y, sizeof(int), 1, f) == 1);
    success = success && (fwrite(&jeu->player, sizeof(_player), 1, f) == 1);
    success = success && (fwrite(jeu->hummeur_tab, sizeof(int), NB_hummeur, f) == NB_hummeur);

    if (!success) {
        printf("Erreur lors de la sauvegarde des données de base\n");
        fclose(f);
        return;
    }

    for (int y = 0; y < jeu->grid_size_y && success; y++) {
        success = (fwrite(jeu->grid[y], sizeof(_tile), jeu->grid_size_x, f) == (size_t)jeu->grid_size_x);
    }

    for (int i = 0; i < jeu->nb_plateau && success; i++) {
        success = success && (fwrite(jeu->plateau_tab[i].tools, sizeof(int), NB_TOOLS, f) == NB_TOOLS);
        success = success && (fwrite(jeu->plateau_tab[i].used_tools, sizeof(int), NB_TOOLS, f) == NB_TOOLS);
        success = success && (fwrite(&jeu->plateau_tab[i].id, sizeof(int), 1, f) == 1);

        int has_patient = (jeu->plateau_tab[i].patient != NULL);
        success = success && (fwrite(&has_patient, sizeof(int), 1, f) == 1);

        if (has_patient) {
            success = success && (fwrite(jeu->plateau_tab[i].patient, sizeof(_patient), 1, f) == 1);
        }
    }

    fclose(f);
    
    if (success) {
        printf("Partie sauvegardee avec succes!\n");
    } else {
        printf("Erreur lors de la sauvegarde\n");
    }
}

int load_game(_jeu *jeu, const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        printf("Impossible d'ouvrir %s: %s\n", filename, strerror(errno));
        return 0;
    }

    free_game(jeu);
    memset(jeu, 0, sizeof(_jeu));

    int success = 1;
    
    success = success && (fread(&jeu->nb_step, sizeof(int), 1, f) == 1);
    success = success && (fread(&jeu->play, sizeof(int), 1, f) == 1);
    success = success && (fread(&jeu->profit, sizeof(float), 1, f) == 1);
    success = success && (fread(&jeu->patient_minimum_spawn_intervalle, sizeof(int), 1, f) == 1);
    success = success && (fread(&jeu->patient_spawn_range, sizeof(int), 1, f) == 1);
    success = success && (fread(&jeu->patient_spawning_hapiness, sizeof(int), 1, f) == 1);
    success = success && (fread(&jeu->patient_hapiness_range, sizeof(int), 1, f) == 1);
    success = success && (fread(&jeu->next_patient_time, sizeof(int), 1, f) == 1);
    success = success && (fread(&jeu->nb_plateau, sizeof(int), 1, f) == 1);
    success = success && (fread(jeu->username, sizeof(char), USERNAME_SIZE, f) == USERNAME_SIZE);
    jeu->username[USERNAME_SIZE - 1] = '\0';
    success = success && (fread(&jeu->grid_size_x, sizeof(int), 1, f) == 1);
    success = success && (fread(&jeu->grid_size_y, sizeof(int), 1, f) == 1);
    success = success && (fread(&jeu->player, sizeof(_player), 1, f) == 1);
    success = success && (fread(jeu->hummeur_tab, sizeof(int), NB_hummeur, f) == NB_hummeur);

    if (!success) {
        printf("Erreur lecture des données de base\n");
        fclose(f);
        memset(jeu, 0, sizeof(_jeu));
        return 0;
    }

    // Validation des limites
    if (jeu->grid_size_x <= 0 || jeu->grid_size_x > MAX_GRID_SIZE ||
        jeu->grid_size_y <= 0 || jeu->grid_size_y > MAX_GRID_SIZE ||
        jeu->nb_plateau < 0 || jeu->nb_plateau > MAX_PLATEAUX) {
        printf("Données de sauvegarde invalides\n");
        fclose(f);
        memset(jeu, 0, sizeof(_jeu));
        return 0;
    }

    jeu->grid = malloc(sizeof(_tile*) * jeu->grid_size_y);
    if (jeu->grid == NULL) {
        printf("Erreur allocation grille\n");
        fclose(f);
        memset(jeu, 0, sizeof(_jeu));
        return 0;
    }
    
    for (int y = 0; y < jeu->grid_size_y; y++) {
        jeu->grid[y] = malloc(sizeof(_tile) * jeu->grid_size_x);
        if (jeu->grid[y] == NULL) {
            for (int i = 0; i < y; i++) free(jeu->grid[i]);
            free(jeu->grid);
            jeu->grid = NULL;
            fclose(f);
            memset(jeu, 0, sizeof(_jeu));
            return 0;
        }
        if (fread(jeu->grid[y], sizeof(_tile), jeu->grid_size_x, f) != (size_t)jeu->grid_size_x) {
            for (int i = 0; i <= y; i++) free(jeu->grid[i]);
            free(jeu->grid);
            jeu->grid = NULL;
            fclose(f);
            memset(jeu, 0, sizeof(_jeu));
            return 0;
        }
    }

    jeu->plateau_tab = malloc(sizeof(_plateau) * jeu->nb_plateau);
    if (jeu->plateau_tab == NULL) {
        free_game(jeu);
        fclose(f);
        memset(jeu, 0, sizeof(_jeu));
        return 0;
    }

    for (int i = 0; i < jeu->nb_plateau; i++) {
        jeu->plateau_tab[i].patient = NULL;
        
        if (fread(jeu->plateau_tab[i].tools, sizeof(int), NB_TOOLS, f) != NB_TOOLS ||
            fread(jeu->plateau_tab[i].used_tools, sizeof(int), NB_TOOLS, f) != NB_TOOLS ||
            fread(&jeu->plateau_tab[i].id, sizeof(int), 1, f) != 1) {
            free_game(jeu);
            fclose(f);
            memset(jeu, 0, sizeof(_jeu));
            return 0;
        }

        int has_patient = 0;
        if (fread(&has_patient, sizeof(int), 1, f) != 1) {
            free_game(jeu);
            fclose(f);
            memset(jeu, 0, sizeof(_jeu));
            return 0;
        }

        if (has_patient) {
            jeu->plateau_tab[i].patient = malloc(sizeof(_patient));
            if (jeu->plateau_tab[i].patient == NULL ||
                fread(jeu->plateau_tab[i].patient, sizeof(_patient), 1, f) != 1) {
                free_game(jeu);
                fclose(f);
                memset(jeu, 0, sizeof(_jeu));
                return 0;
            }
        }
    }

    fclose(f);

    jeu->player.pos = get_player_pos_from_grid(jeu->grid, jeu->grid_size_x, jeu->grid_size_y);
    
    if (jeu->player.pos.x < 0 || jeu->player.pos.y < 0) {
        printf("Position du joueur invalide dans la sauvegarde\n");
        free_game(jeu);
        memset(jeu, 0, sizeof(_jeu));
        return 0;
    }
    
    printf("Partie chargee avec succes!\n");
    return 1;
}

_coord get_element_pos_from_grid(_tile** grid, int size_x, int size_y, int element) {
    _coord pos = {-1, -1};
    if (grid == NULL) return pos;
    
    for (int dy = 0; dy < size_y; dy++) {
        for (int dx = 0; dx < size_x; dx++) {
            if (grid[dy][dx].value == element) {
                pos.x = dx;
                pos.y = dy;
                return pos;
            }
        }
    }
    return pos;
}

_plateau cree_plateau(int new_id) {
    _plateau new_plateau;
    for (int i = 0; i < NB_TOOLS; i++) {
        new_plateau.tools[i] = 0;
        new_plateau.used_tools[i] = 0;
    }
    new_plateau.patient = NULL;
    new_plateau.id = new_id;
    return new_plateau;
}

int can_move_at_pos(_tile** grid, int size_x, int size_y, int x, int y) {
    if (grid == NULL) return 0;
    
    _tile tile = get_tile_from_pos(grid, size_x, size_y, x, y);
    int tile_value = tile.value;
    
    if (inter_check(tile_value, 'a', 'z')) {
        return 1;
    }
    return (tile_value == 0);
}

void tile_print(_tile tile, _plateau* plateau_tab, int taille, _player player) {
    if (tile.player == 1) {
        if (player.glove.type == 'h') {
            printf("😷");
        } else {
            printf("😐");
        }
    } else {
        if (inter_check(tile.value, 'T', 'Z')) {
            char c = tile.value - 'T' + 't';
            for (int i = 0; i < taille; i++) {
                if (plateau_tab[i].id == c) {
                    if (plateau_tab[i].patient != NULL) {
                        if (plateau_tab[i].patient->etat == satisfait) {
                            printf("🤒");
                        } else {
                            printf("😥");
                        }
                    } else {
                        printf("🪑");
                    }
                    break;
                }
            }
        } else {
            switch (tile.value) {
                case 0:  
                    printf("  "); 
                    break;
                case 1:  
                    printf("⬛"); 
                    break;
                case 2:  
                    printf("🧊"); 
                    break;
                case 3:  
                    printf("🚪️"); 
                    break;
                case 4:  
                    printf("🔲️"); 
                    break;
                
                case 'A': 
                    printf("🪛"); 
                    break;
                case 'B': 
                    printf("⚙️ "); 
                    break;
                case 'C': 
                    printf("🔩"); 
                    break;
                case 'D': 
                    printf("🔬"); 
                    break;
                case 'E': 
                    printf("💉"); 
                    break;
                case 'F': 
                    printf("🩹"); 
                    break;
                case 'G': 
                    printf("💭"); 
                    break;
                case 'H': 
                    printf("🧤"); 
                    break;
                case 'I': 
                    printf("♻️ "); 
                    break;
                case 'J': 
                    printf("☣️ "); 
                    break;
                
                default:
                    if (inter_check(tile.value, 't', 'z')) {
                        printf(" %c", tile.value);
                    } else if (inter_check(tile.value, 'a', 'j')) {
                        printf(" .");
                    } else {
                        printf("  ");
                    }
                    break;
            }
        }
    }
}

void print_grid(_tile** grid, int size_x, int size_y, _plateau* plateau_tab, int taille, _player player) {
    if (grid == NULL){ return;}
    
    printf("\n\n");
    for (int dy = 0; dy < size_y; dy++) {
        printf("       ");
        for (int dx = 0; dx < size_x; dx++) {
            tile_print(grid[dy][dx], plateau_tab, taille, player);
        }
        printf("\n");
    }
    printf("\n");
}

void move_player(_tile** grid, int size_x, int size_y, _movement movement) {
    if (grid == NULL) return;
    
    _coord initial_pos = get_player_pos_from_grid(grid, size_x, size_y);
    if (initial_pos.x < 0 || initial_pos.y < 0) {
        printf("player can't be found\n");
        return;
    }

    _coord final_pos;
    final_pos.x = initial_pos.x + (movement == RIGHT) - (movement == LEFT);
    final_pos.y = initial_pos.y - (movement == UP) + (movement == DOWN);

    if (inter_check(final_pos.x, 0, size_x - 1) && inter_check(final_pos.y, 0, size_y - 1)) {
        if (can_move_at_pos(grid, size_x, size_y, final_pos.x, final_pos.y)) {
            grid[initial_pos.y][initial_pos.x].player = 0;
            grid[final_pos.y][final_pos.x].player = 1;
        }
    }
}

int cure_if_got_tools(_plateau* plateau, float* profit, int* hummeur_tab) {
    if (plateau == NULL || plateau->patient == NULL) return 0;
    
    int can_cure = 1;
    for (int i = 0; i < NB_TOOLS; i++) {
        if (plateau->tools[i] < plateau->patient->maladie.tool_needed[i]) {
            can_cure = 0;
            break;
        }
    }
    
    if (can_cure) {
        for (int i = 0; i < NB_TOOLS; i++) {
            plateau->tools[i] -= plateau->patient->maladie.tool_needed[i];
            plateau->used_tools[i] += plateau->patient->maladie.tool_needed[i];
        }

        if (plateau->patient->etat == satisfait) {
            printf("Un patient est parti satisfait car il a été soigné, il a donné %.2f$\n", plateau->patient->maladie.profit);
            hummeur_tab[0]++;
            *profit += plateau->patient->maladie.profit;
        } else {
            if (plateau->patient->maladie.profit != 0) {
                printf("Un patient est parti mécontent car il a été soigné et il a attendu longtemps, il a donné %.2f$\n", 0.5f * plateau->patient->maladie.profit);
                hummeur_tab[1]++;
                *profit += 0.5f * plateau->patient->maladie.profit;
            } else {
                printf("Un patient est parti mécontent car il a été soigné et le plateau a été sale lors de son arrivée, il a donné 0.0$\n");
                hummeur_tab[1]++;
            }
        }
        
        free(plateau->patient);
        plateau->patient = NULL;
        return 1;
    }
    return 0;
}

int try_cure_patient(_plateau* plateau, float* profit, int* hummeur_tab) {
    if (plateau != NULL && plateau->patient != NULL) {
        return cure_if_got_tools(plateau, profit, hummeur_tab);
    }
    return 0;
}

char try_do_action(_tile** grid, int size_x, int size_y, _player* player, _plateau* plateau_tab, int taille, float* profit, int* hummeur_tab) {
    if (grid == NULL || player == NULL) return 0;
    
    player->pos = get_player_pos_from_grid(grid, size_x, size_y);
    if (player->pos.x < 0 || player->pos.y < 0) {
        printf("player can't be found\n");
        return 0;
    }
    
    _tile current_tile = get_tile_from_pos(grid, size_x, size_y, player->pos.x, player->pos.y);
    int tile_value = current_tile.value;

    if (tile_value == 'h' && player->glove.type == 0) {
        player->glove.type = 'h';
        player->glove.clean = 0;
        player->glove.used = 0;
        *profit -= 1.2f;
        printf("1.2$ du profit est dépensé pour avoir des gants\n");
    }
    
    if (inter_check(tile_value, 'a', 'g') && player->tool.type == 0) {
        player->tool.type = tile_value;
        player->tool.clean = (player->glove.type == 'h') && (player->glove.used == 0);
        player->tool.used = 0;
        *profit -= 1.2f;
        printf("1.2$ du profit est dépensé pour avoir un outil\n");
    } else if (tile_value == 'i') {
        if (player->tool.type != 0 && player->tool.used != 1) {
            player->tool.type = 0;
        } else if (player->glove.type == 'h' && player->glove.used != 1) {
            player->glove.type = 0;
        }
    } else if (tile_value == 'j') {
        if (player->tool.type != 0 && player->tool.used == 1) {
            player->tool.type = 0;
        } else if (player->glove.type == 'h' && player->glove.used == 1) {
            player->glove.type = 0;
        }
    } else if (inter_check(tile_value, 't', 'z')) {
        for (int i = 0; i < taille; i++) {
            if (plateau_tab[i].id == tile_value) {
                if (player->tool.type != 0 && player->tool.clean == 1 && 
                    (plateau_tab[i].tools[(player->tool.type) - 'a'] + plateau_tab[i].used_tools[(player->tool.type) - 'a']) == 0) {
                    plateau_tab[i].tools[(player->tool.type) - 'a'] = 1;
                    player->tool.type = 0;
                    break;
                } else if (player->glove.used == 0 && player->tool.type == 0 && 
                           player->glove.type == 'h' && plateau_tab[i].patient != NULL) {
                    if (try_cure_patient(&plateau_tab[i], profit, hummeur_tab)) {
                        player->glove.used = 1;
                    }
                    break;
                } else if ((player->glove.used == 1 || player->glove.type != 'h') && 
                           plateau_tab[i].patient != NULL && player->tool.type == 0) {
                    free(plateau_tab[i].patient);
                    plateau_tab[i].patient = NULL;
                    printf("Un patient est parti furieux par peur de contamination,\nil a contaminé les outils présents sur son plateau, il n'a rien donné\n");
                    hummeur_tab[2]++;
                    for (int ind = 0; ind < NB_TOOLS; ind++) {
                        if (plateau_tab[i].tools[ind] == 1) {
                            plateau_tab[i].tools[ind] = 0;
                            plateau_tab[i].used_tools[ind] = 1;
                        }
                    }
                } else {
                    for (int j = 0; j < NB_TOOLS; j++) {
                        if (plateau_tab[i].used_tools[j] == 1) {
                            player->tool.type = 'a' + j;
                            player->tool.clean = 0;
                            player->tool.used = 1;
                            plateau_tab[i].used_tools[j] = 0;
                            break;
                        }
                    }
                }
                break;
            }
        }
    }
    return 0;
}

void ask_to_do_player_action(_tile** grid, int size_x, int size_y, _player* player, _plateau* plateau_tab, int taille, float* profit, int* hummeur_tab, int* playing) {
    char move;
    int valid = 0;
    
    printf("Veuillez saisir votre action-----\n\n");
    printf("- type de déplacement\n");
    printf(" |  z:vers le haut\n");
    printf(" |  d:vers la droite\n");
    printf(" |  s:vers le bas\n");
    printf(" |  q:vers la gauche\n");
    printf("- action\n");
    printf(" |  g:faire une action (si aucune action est possible rien ne se passe)\n");
    printf(" |  h:retour au menu principal (la partie n'est pas perdue)\n");
    
    do {
        if (scanf(" %c", &move) != 1) {
            printf("Erreur de saisie, veuillez recommencer\n");
            while (getchar() != '\n');
            continue;
        }
        
        valid = (move == 'z' || move == 'd' || move == 's' || move == 'q' || move == 'g' || move == 'h');
        if (!valid) {
            printf("L'action saisie est incorrecte, veuillez recommencer\n");
        }
    } while (!valid);
    
    printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    
    switch (move) {
        case 'z': move_player(grid, size_x, size_y, UP); break;
        case 'd': move_player(grid, size_x, size_y, RIGHT); break;
        case 's': move_player(grid, size_x, size_y, DOWN); break;
        case 'q': move_player(grid, size_x, size_y, LEFT); break;
        case 'g': try_do_action(grid, size_x, size_y, player, plateau_tab, taille, profit, hummeur_tab); break;
        case 'h': *playing = 0; break;
        default: break;
    }
}

void print_player_status(_player player, float profit) {
    color(190, 175, 30);
    printf("\n----------------Le joueur-------------------\n");
    printf("    |💵 %.2f$", profit);
    
    if (player.glove.type == 'h') {
        printf("    |🧤 ");
        printf(player.glove.used == 0 ? "🟩" : "🟫");
    }
    
    if (player.tool.type != 0) {
        printf("    |");
        switch (player.tool.type) {
            case 'a': 
                printf("🪛"); 
                break;
            case 'b': 
                printf("⚙️ "); 
                break;
            case 'c': 
                printf("🔩"); 
                break;
            case 'd': 
                printf("🔬"); 
                break;
            case 'e': 
                printf("💉"); 
                break;
            case 'f': 
                printf("🩹"); 
                break;
            case 'g': 
                printf("💭"); 
                break;
            default: 
                printf("..."); 
                break;
        }
        printf(player.tool.used == 0 ? " 🟩" : " 🟫");
    }
    printf("\n");
    reset_color();
}

void print_total_patient(int* hummeur_tab) {
    color(190, 175, 30);
    printf("    |patient score:  |%d 🟩    |%d 🟨    |%d 🟥  \n\n", hummeur_tab[0], hummeur_tab[1], hummeur_tab[2]);
    reset_color();
}

_tile** make_grid_from_string(char string[], int max_size_x, int max_size_y) {
    _tile** new_grid = cree_grid(max_size_x, max_size_y);
    int string_ind = 0;
    
    for (int dy = 0; dy < max_size_y; dy++) {
        for (int dx = 0; dx < max_size_x; dx++) {
            char c = string[string_ind];
            if (c == '_') {
                string_ind++;
                c = string[string_ind];
            }
            if (c == '@') {
                return new_grid;
            }
            if (c != 'P') {
                if (inter_check(c, '0', '9')) {
                    new_grid[dy][dx].value = c - '0';
                } else {
                    new_grid[dy][dx].value = c;
                }
            } else {
                new_grid[dy][dx].value = 0;
                new_grid[dy][dx].player = 1;
            }
            string_ind++;
        }
    }
    return new_grid;
}

_plateau* get_plateau_tab(_tile** grid, int size_x, int size_y, int* taille) {
    *taille = 0;
    
    for (int id = 't'; id <= 'z'; id++) {
        _coord plateau_pos = get_element_pos_from_grid(grid, size_x, size_y, id);
        if (inter_check(plateau_pos.x, 0, size_x - 1) && inter_check(plateau_pos.y, 0, size_y - 1)) {
            (*taille)++;
        }
    }
    
    if (*taille == 0) return NULL;
    
    _plateau* plateau_tab = malloc((*taille) * sizeof(_plateau));
    exit_if_null_pointer(plateau_tab);
    
    int index = 0;
    for (int id = 't'; id <= 'z'; id++) {
        _coord plateau_pos = get_element_pos_from_grid(grid, size_x, size_y, id);
        if (inter_check(plateau_pos.x, 0, size_x - 1) && inter_check(plateau_pos.y, 0, size_y - 1)) {
            plateau_tab[index] = cree_plateau(id);
            index++;
        }
    }
    return plateau_tab;
}

void print_plateau(_plateau plateau, int max_happiness, int happy_bar_len) {
    color(190, 175, 30);
    printf("  <plateau %c > ", plateau.id);
    
    const char* tool_icons[] = {"🪛", "⚙️ ", "🔩", "🔬", "💉", "🩹", "💭"};
    
    for (int i = 0; i < NB_TOOLS; i++) {
        printf("%s", tool_icons[i]);
        
        if (plateau.tools[i]) {
            if (plateau.patient != NULL && plateau.patient->maladie.tool_needed[i]) {
                printf(" ✅");
            } else {
                printf(" 🟩");
            }
        } else if (plateau.used_tools[i]) {
            if (plateau.patient != NULL && plateau.patient->maladie.tool_needed[i]) {
                printf(" 🚫");
            } else {
                printf(" 🟫");
            }
        } else if (plateau.patient != NULL) {
            printf(plateau.patient->maladie.tool_needed[i] ? " ⬜" : " 🟥");
        } else {
            printf(" 🟥");
        }
        printf("    ");
    }

    if (plateau.patient != NULL && max_happiness > 0) {
        float percentage = (float)plateau.patient->hummeur / max_happiness;
        if (percentage < 0) percentage = 0;
        if (percentage > 1) percentage = 1;
        
        int nb_box = (int)(percentage * happy_bar_len);
        printf("|hummeur  ");
        
        for (int i = 0; i < happy_bar_len; i++) {
            if (nb_box < 1) {
                printf("⬛");
            } else if (i <= happy_bar_len * 0.075) {
                printf("🟥");
            } else if (i <= happy_bar_len * 0.22) {
                printf("🟧");
            } else if (i <= happy_bar_len * 0.45) {
                printf("🟨");
            } else {
                printf("🟩");
            }
            nb_box--;
        }
        
        color((unsigned char)(255 * (1 - percentage)), (unsigned char)(255 * percentage), (unsigned char)(90 * (1 - percentage)));
        printf("    %.2f%%", percentage * 100);
    }
    printf("\n");
    reset_color();
}

void print_plateau_tab(_plateau* plateau_tab, int taille, int max_happiness, int happy_bar_len) {
    color(190, 175, 30);
    printf("------------------------------------- informations des plateaux --------------------------------------------------------------------------------\n");
    reset_color();
    
    for (int i = 0; i < taille; i++) {
        print_plateau(plateau_tab[i], max_happiness, happy_bar_len);
    }
    printf("\n");
}

void make_tool_tab(int tab[], int a, int b, int c, int d, int e, int f, int g) {
    tab[0] = a; tab[1] = b; tab[2] = c; tab[3] = d;
    tab[4] = e; tab[5] = f; tab[6] = g;
}

_patient* cree_patient() {
    _patient* patient = malloc(sizeof(_patient));
    exit_if_null_pointer(patient);

    patient->hummeur = 100;
    patient->etat = satisfait;

    int desease_index = randint(1, NB_MALADIE);
    switch (desease_index) {
        case 1:
            patient->maladie.type = DESEASE_A;
            make_tool_tab(patient->maladie.tool_needed, 0, 1, 0, 0, 1, 0, 1);
            patient->maladie.profit = 3.20f;
            break;
        case 2:
            patient->maladie.type = DESEASE_B;
            make_tool_tab(patient->maladie.tool_needed, 1, 0, 0, 1, 1, 0, 0);
            patient->maladie.profit = 2.90f;
            break;
        case 3:
            patient->maladie.type = DESEASE_C;
            make_tool_tab(patient->maladie.tool_needed, 1, 0, 0, 1, 0, 1, 0);
            patient->maladie.profit = 4.14f;
            break;
        case 4:
            patient->maladie.type = DESEASE_D;
            make_tool_tab(patient->maladie.tool_needed, 0, 0, 1, 1, 0, 1, 0);
            patient->maladie.profit = 3.49f;
            break;
        case 5:
            patient->maladie.type = DESEASE_E;
            make_tool_tab(patient->maladie.tool_needed, 0, 1, 1, 0, 0, 0, 1);
            patient->maladie.profit = 5.65f;
            break;
        default:
            patient->maladie.type = TEST_DESEASE;
            make_tool_tab(patient->maladie.tool_needed, 0, 1, 0, 0, 1, 1, 0);
            patient->maladie.profit = 1.23f;
            break;
    }

    return patient;
}

int get_a_patient(_plateau* plateau_tab, int taille, int initial_hapiness) {
    if (plateau_tab == NULL || taille <= 0) return 0;
    
    int empty_found = 0;
    for (int i = 0; i < taille; i++) {
        if (plateau_tab[i].patient == NULL) {
            empty_found = 1;
            break;
        }
    }
    
    if (!empty_found) return 0;
    
    int attempts = 0;
    int ind;
    do {
        ind = randint(0, taille - 1);
        attempts++;
    } while (plateau_tab[ind].patient != NULL && attempts < taille * 2);
    
    if (plateau_tab[ind].patient != NULL) return 0;
    
    printf("Un patient s'est installé sur un plateau\n");
    plateau_tab[ind].patient = cree_patient();
    plateau_tab[ind].patient->hummeur = initial_hapiness;

    for (int j = 0; j < NB_TOOLS; j++) {
        if (plateau_tab[ind].used_tools[j] == 1) {
            printf("Le patient ne va pas payer car le plateau n'est pas totalement propre\n");
            plateau_tab[ind].patient->maladie.profit = 0;
            plateau_tab[ind].patient->etat = mecontant;
            break;
        }
    }
    return 1;
}

void update_patients_hapiness(_plateau* plateau_tab, int taille, int* all_happy, int* full, float* profit, int* hummeur_tab, int max_happiness) {
    int full_var = 1;
    
    for (int i = 0; i < taille; i++) {
        if (plateau_tab[i].patient != NULL) {
            plateau_tab[i].patient->hummeur--;

            if (plateau_tab[i].patient->hummeur <= (int)(0.45 * max_happiness)) {
                plateau_tab[i].patient->etat = mecontant;
            }

            if (plateau_tab[i].patient->hummeur <= 0) {
                printf("Un patient est parti furieux par manque de patience, il n'a rien donné\n");
                hummeur_tab[2]++;
                
                for (int ind = 0; ind < NB_TOOLS; ind++) {
                    plateau_tab[i].used_tools[ind] += plateau_tab[i].tools[ind];
                    plateau_tab[i].tools[ind] = 0;
                }

                free(plateau_tab[i].patient);
                plateau_tab[i].patient = NULL;
                *all_happy = 0;
            }
        } else {
            full_var = 0;
        }
    }
    *full = full_var;
}

void patients_spawning_regulation(_plateau* plateau_tab, int taille, int min_spawn_time, int spawn_time_range, int* current_patient_spawning_time, int initial_hapiness) {
    if (*current_patient_spawning_time <= 0) {
        if (get_a_patient(plateau_tab, taille, initial_hapiness)) {
            *current_patient_spawning_time = min_spawn_time + randint(0, spawn_time_range);
        }
    } else {
        (*current_patient_spawning_time)--;
    }
}

int update_patients(_plateau* plateau_tab, int taille, int min_spawn_time, int spawn_time_range, int* current_patient_spawning_time, int initial_hapiness, float* profit, int* hummeur_tab) {
    int full = 0;
    int all_happy = 1;

    patients_spawning_regulation(plateau_tab, taille, min_spawn_time, spawn_time_range, current_patient_spawning_time, initial_hapiness);
    update_patients_hapiness(plateau_tab, taille, &all_happy, &full, profit, hummeur_tab, min_spawn_time + spawn_time_range);
    
    printf("next patient in %d step(s)\n", *current_patient_spawning_time);
    
    return !(full && !all_happy);
}

void get_grid_size_from_string(char map_string[], int* size_x, int* size_y) {
    int temp_size_x = 0;
    int new_size_x = 0;
    int new_size_y = 0;
    int i = 0;
    
    while (map_string[i] != '@' && map_string[i] != '\0') {
        if (map_string[i] == '_') {
            if (temp_size_x > new_size_x && new_size_y == 0) {
                new_size_x = temp_size_x;
            } else if (temp_size_x != new_size_x && new_size_y != 0) {
                printf("Le string map a une forme irrégulière x:%d!=%d && y=%d\n", temp_size_x, new_size_x, new_size_y);
                exit(1);
            }
            new_size_y++;
            temp_size_x = 0;
        } else {
            temp_size_x++;
        }
        i++;
    }
    *size_x = new_size_x;
    *size_y = new_size_y;
}

char* get_random_map_string() {
    int choice = randint(0, 3);
    switch (choice) {
        case 1:
            return "11111111111331111111_10000000100000030001_10000011100111111101_100P00jJ100000u4U001_100000iI100000v4V001_11100111100000y4Y101_10000000000000w4W001_10abcde0fgh000x4X001_11ABCDE1FGH100z4Z001_11111111111111111111_@";
        case 2:
            return "111111I11111_101U4ui00a2A_103U4u0P0b2B_101U4uhhhc2C_1011JjhHhd2D_101T4thhhe2E_103T4t000f2F_101T4ti00g2G_131111I11111_@";
        case 3:
            return "0111AB1131CD111111EF11111333300_0100ab0000cd000000ef00000300300_0300000000000000000000000333300_0300001111110001111110000000100_01ii00tttuuu000vvvwww00ii100100_01Ii00tTtuUu0P0vVvwWw00iI100100_01ii00tttuuu000vvvwww00ii100100_1111100000000000000000000000100_1JJJ1000000hhhhh00000ggg0000100_1jjj0000001HHHHH10000gGg01001PP_11111111111111111111111111331PP_@";
        default:
            return "001111111000_001BCADE1000_001222221100_111bcade0111_1F2f0000t4T3_1G2g0P0000111_1H2h0000u4U3_111000000111_3W4wi0j0v4V3_1111I1J11111_000111110000_@";
    }
}

_jeu creer_jeu() {
    _jeu new_jeu;
    memset(&new_jeu, 0, sizeof(_jeu));
    
    new_jeu.play = 0;
    new_jeu.player.tool.type = 0;
    new_jeu.player.glove.type = 0;
    new_jeu.profit = 0.0f;

    char* map_string = get_random_map_string();
    get_grid_size_from_string(map_string, &new_jeu.grid_size_x, &new_jeu.grid_size_y);
    new_jeu.grid = make_grid_from_string(map_string, new_jeu.grid_size_x, new_jeu.grid_size_y);

    new_jeu.happy_bar_len = 32;
    new_jeu.plateau_tab = get_plateau_tab(new_jeu.grid, new_jeu.grid_size_x, new_jeu.grid_size_y, &new_jeu.nb_plateau);

    new_jeu.patient_minimum_spawn_intervalle = 4 * new_jeu.nb_plateau;
    new_jeu.patient_spawn_range = 10;
    new_jeu.patient_spawning_hapiness = 37 * new_jeu.nb_plateau;
    new_jeu.patient_hapiness_range = 7 * new_jeu.nb_plateau;
    new_jeu.next_patient_time = 13;
    new_jeu.nb_step = -1;

    new_jeu.player.pos = get_player_pos_from_grid(new_jeu.grid, new_jeu.grid_size_x, new_jeu.grid_size_y);

    return new_jeu;
}

int play_a_game(_jeu* game) {
    if (game == NULL) return 0;
    
    game->play = 1;
    int playing = 1;
    
    while (playing) {
        game->nb_step++;
        
        int initial_hapiness = game->patient_spawning_hapiness + randint(0, game->patient_hapiness_range);
        
        if (!update_patients(game->plateau_tab, game->nb_plateau, game->patient_minimum_spawn_intervalle, game->patient_spawn_range, &game->next_patient_time, initial_hapiness, &game->profit, game->hummeur_tab)) {
            color(250, 30, 30);
            printf("\n\n");
            printf("------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
            printf("        GAME OVER!! Le jeu a duré pour %d pas avec: un profit de %.2f$ ,%d patient(s) satisfait(s) ,%d patient(s) mécontent(s) ,%d patient(s) furieux\n\n",game->nb_step, game->profit, game->hummeur_tab[0], game->hummeur_tab[1], game->hummeur_tab[2]);
            printf("------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
            reset_color();
            sleep(2);

            FILE* score_file = fopen("scoreboard.txt", "a");
            if (score_file != NULL) {
                fprintf(score_file, "%s %d %.2f %d %d %d\n", game->username, game->nb_step, game->profit,
                        game->hummeur_tab[0], game->hummeur_tab[1], game->hummeur_tab[2]);
                fclose(score_file);
            }

            return 0;
        }
        
        print_grid(game->grid, game->grid_size_x, game->grid_size_y, game->plateau_tab, game->nb_plateau, game->player);
        print_plateau_tab(game->plateau_tab, game->nb_plateau, game->patient_spawning_hapiness + game->patient_hapiness_range, game->happy_bar_len);
        print_player_status(game->player, game->profit);
        print_total_patient(game->hummeur_tab);
        ask_to_do_player_action(game->grid, game->grid_size_x, game->grid_size_y, &game->player, game->plateau_tab, game->nb_plateau,&game->profit, game->hummeur_tab, &playing);
    }
    return 1;
}

void print_scoreboard() {
    FILE* score_file = fopen("scoreboard.txt", "r");
    if (score_file == NULL) {
        printf("Impossible d'ouvrir le fichier scoreboard.txt ou aucun score enregistré.\n");
        printf("\nAppuyez sur une touche puis Entrée pour revenir au menu principal\n");
        char c;
        scanf(" %c", &c);
        return;
    }

    char var_ligne[200];
    int nb_ligne = 0;
    
    while (fgets(var_ligne, sizeof(var_ligne), score_file) != NULL) {
        nb_ligne++;
    }
    rewind(score_file);

    if (nb_ligne == 0) {
        printf("Aucun score enregistré.\n");
        fclose(score_file);
        printf("\nAppuyez sur une touche puis Entrée pour revenir au menu principal\n");
        char c;
        scanf(" %c", &c);
        return;
    }

    _score* score_tab = malloc(nb_ligne * sizeof(_score));
    if (score_tab == NULL) {
        fclose(score_file);
        printf("Erreur d'allocation mémoire\n");
        return;
    }

    int actual_count = 0;
    for (int i = 0; i < nb_ligne; i++) {
        // Limite la lecture du username à 49 caractères
        if (fscanf(score_file, "%49s %d %f %d %d %d",score_tab[actual_count].username,&score_tab[actual_count].nb_step,&score_tab[actual_count].profit,&score_tab[actual_count].hummeur_tab[0],&score_tab[actual_count].hummeur_tab[1],&score_tab[actual_count].hummeur_tab[2]) == 6) {
            score_tab[actual_count].username[USERNAME_SIZE - 1] = '\0';
            actual_count++;
        }
    }
    fclose(score_file);

    // Tri à bulles
    for (int i = 0; i < actual_count - 1; i++) {
        for (int j = 0; j < actual_count - 1 - i; j++) {
            int swap = 0;
            if (score_tab[j].nb_step < score_tab[j + 1].nb_step) {
                swap = 1;
            } else if (score_tab[j].nb_step == score_tab[j + 1].nb_step) {
                if (score_tab[j].profit < score_tab[j + 1].profit) {
                    swap = 1;
                }
            }
            if (swap) {
                _score temp = score_tab[j];
                score_tab[j] = score_tab[j + 1];
                score_tab[j + 1] = temp;
            }
        }
    }

    printf("-----------------------------------------\n");
    printf("--------------scoreboard-----------------\n");
    
    for (int i = 0; i < actual_count; i++) {
        color(175, 120, 50);
        printf("    |  %02d  |", i + 1);
        printf("name:%s ", score_tab[i].username);
        reset_color();
        printf("time(in step):%d ", score_tab[i].nb_step);
        color(50, 145, 180);
        printf("profit:%.2f ", score_tab[i].profit);
        color(100, 200, 100);
        printf("satisfait:%d ", score_tab[i].hummeur_tab[0]);
        color(150, 150, 75);
        printf("mecontent:%d ", score_tab[i].hummeur_tab[1]);
        color(200, 75, 75);
        printf("furieux:%d ", score_tab[i].hummeur_tab[2]);
        printf("\n");
        reset_color();
    }

    free(score_tab);

    printf("\nAppuyez sur une touche puis Entrée pour revenir au menu principal\n");
    char c;
    scanf(" %c", &c);
}

_menu ask_menu(_jeu* current_game, _menu current_menu) {
    char menu;
    int valid = 0;
    
    printf("    Veuillez choisir l'une des options suivantes\n\n");
    
    if (current_menu == select_menu) {
        if (current_game->play == 1) {
            printf("         |  save current game (press s)\n");
            printf("         |  continue current game (press d)\n");
        }
        printf("         |  play a new game (press f)\n");
        printf("         |  scoreboard (press g)\n");
    } else {
        printf("         |  go to menu (press q)\n");
    }
    printf("         |  quit program (press h)\n");
    printf("\n\n\n\n");
    
    do {
        if (scanf(" %c", &menu) != 1) {
            printf("Erreur de saisie, veuillez recommencer\n");
            while (getchar() != '\n');
            continue;
        }
        
        if (current_menu == select_menu) {
            valid = ((menu == 's' || menu == 'd') && current_game->play == 1) ||menu == 'f' || menu == 'g' || menu == 'h';
        } else {
            valid = (menu == 'h' || menu == 'q');
        }
        
        if (!valid) {
            printf("Le menu choisi est incorrect, veuillez recommencer\n");
        }
    } while (!valid);
    
    printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    
    switch (menu) {
        case 'q': 
            return select_menu;
        case 's':
            return save;
        case 'd': 
            return continu;
        case 'f': 
            return new_jeu;
        case 'g': 
            return scoreboard;
        case 'h': 
            return quit;
        default: 
            return select_menu;
    }
}

void start() {
    _menu current_menu = select_menu;
    _jeu current_game;
    memset(&current_game, 0, sizeof(_jeu));
    
    char* username = malloc(USERNAME_SIZE * sizeof(char));
    if (username == NULL) {
        printf("Erreur allocation mémoire pour username\n");
        return;
    }

    printf("Veuillez saisir le nom de votre joueur: ");
    if (scanf("%49s", username) != 1) {
        printf("Erreur lecture username\n");
        free(username);
        return;
    }
    username[USERNAME_SIZE - 1] = '\0';

    int running = 1;
    while (running) {
        printf("_________________________________________________________________________________________________________________________________________\n");
        printf("_________________________________________________________________________________________________________________________________________\n");
        printf("         ####                                  #####                                                                                     \n");
        printf("         #    ####  #   # ###  ### #   #         #   ####  #### # ## #### #### #### ### ####            made by:-Assia                   \n");
        printf("         #    #  #  #   #  #    #   # #          #   #  #  ##   ##   #    #  # #  # #   ####                    -Catherine               \n");
        printf("         #    ####   # #   #    #    #           #   ####    ## # #  ###  #  # ###  #   #                       -Théo R.                 \n");
        printf("         #### #  #    #   ###   #    #           #   #  #  #### #  # #    #### #  # ### ####                                             \n");
        printf("_________________________________________________________________________________________________________________________________________\n");
        printf("__________________________________________________menu principal_________________________________________________________________________\n");

        current_menu = ask_menu(&current_game, current_menu);
        
        switch (current_menu) {
            case new_jeu:
                free_game(&current_game);
                current_game = creer_jeu();
                strncpy(current_game.username, username, USERNAME_SIZE - 1);
                current_game.username[USERNAME_SIZE - 1] = '\0';
                
                if (!play_a_game(&current_game)) {
                    free_game(&current_game);
                    memset(&current_game, 0, sizeof(_jeu));
                }
                break;

            case continu:
                if (!load_game(&current_game, "save.dat")) {
                    printf("Echec du chargement, retour au menu\n");
                    memset(&current_game, 0, sizeof(_jeu));
                } else {
                    if (!play_a_game(&current_game)) {
                        free_game(&current_game);
                        memset(&current_game, 0, sizeof(_jeu));
                    }
                }
                break;

            case scoreboard:
                print_scoreboard();
                break;

            case save:
                if (current_game.play == 1) {
                    save_game(&current_game, "save.dat");
                } else {
                    printf("Aucune partie en cours à sauvegarder\n");
                }
                break;

            case quit:
                running = 0;
                break;

            default:
                break;
        }
        
        current_menu = select_menu;
    }

    free_game(&current_game);
    free(username);
}

int main() {
    srand(time(NULL));
    printf("Running program\n\n\n\n");

    start();

    printf("program ended\n");
    return 0;
}
