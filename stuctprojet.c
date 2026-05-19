#include "structprojet.h"

#define NB_TOOLS 7
#define NB_MALADIE 5
#define NB_hummeur 3
#define MAX_map_string 5000
 #define username_SIZE 50
//-----------------------------------------------------------
typedef enum {UP=0,RIGHT=1,DOWN=2,LEFT=3} _movement;
//-----------------
typedef enum {TEST_DESEASE=0,DESEASE_A=1,DESEASE_B=2,DESEASE_C=3,DESEASE_D=4,DESEASE_E=5} _maladie_type;
//-----------------
typedef enum {satisfait=0,mecontant=1,furieux=2} _etat;
//-----------------
typedef enum {new_jeu=0,select_menu=1,scoreboard=2,save=3,continu=4,quit=5} _menu;
//-----------------
typedef struct {
    int value;
    int player;
} _tile;
//-----------------
typedef struct {
    int x;
    int y;
} _coord;
//-----------------
typedef struct {
    int clean;
    int used;
    int type;
} _tool;
//-----------------
typedef struct {
    _coord pos;
    _tool tool;
    _tool glove;
} _player;
//-----------------
typedef struct {
    _maladie_type type;
    int tool_needed[NB_TOOLS];
    float profit;
} _maladie;
//-----------------
typedef struct {
    int hummeur;
    _maladie maladie;
    _etat etat;
} _patient;
//-----------------
typedef struct {
    int tools[NB_TOOLS];
    int used_tools[NB_TOOLS];
    _patient* patient;
    int id;
} _plateau;
//-----------------
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
    char username[username_SIZE];
} _jeu;
//-----------------
typedef struct {
    char username[50];
    int nb_step;
    float profit;
    int hummeur_tab[NB_hummeur];
} _score;
