#ifndef STRUCTPROJET_H
 #define STRUCTPROJET_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include<unistd.h>

#include <string.h>
#include <errno.h> 

//#define GRID_SIZE_X 20
//#define GRID_SIZE_Y 10

#define NB_TOOLS 7  //sans le gant
#define NB_MALADIE 5 //sans le test_desease
#define NB_hummeur 3
#define MAX_map_string 5000
#define username_SIZE 50


typedef enum {UP=0,RIGHT=1,DOWN=2,LEFT=3} _movement;
typedef enum {TEST_DESEASE=0,DESEASE_A=1,DESEASE_B=2,DESEASE_C=3,DESEASE_D=4,DESEASE_E=5} _maladie_type;
typedef enum {satisfait=0,mecontant=1,furieux=2} _etat;
typedef enum {new_jeu=0,select_menu=1,scoreboard=2,save=3,continu=4,quit=5,load=6} _menu;

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
	_tool tool;   //si outil alors player.glove.type = (int)    sinon player.tool.type = 0
	_tool glove;  //si gant alors player.glove.type = 'h'       sinon player.glove.type = 0
} _player;

typedef struct {
    _maladie_type type;
    int tool_needed[NB_TOOLS] ;
    float profit;
} _maladie;

typedef struct {
    int hummeur;
    _maladie maladie;
    _etat etat;
} _patient;

typedef struct {
    int tools[NB_TOOLS] ;     //{a,b,c,d,e,f,g}
    int used_tools[NB_TOOLS] ;
    _patient* patient;
    int id;   //noms possible des plateaux (t u v w x y z)
} _plateau;

typedef struct { //structure qui stock les informations d'un partie de jeu
    
    int nb_step ;   //le temps écoulé depuis le début de la partie
    int play;       //est ce que la partie a commencé?
    
    //tableau pour stocker le nombre de patient en fonction de leur hummeur quand il sont parti
    int hummeur_tab[NB_hummeur] ; // index 0:satisfait ,index 1:mécontent ,index 2:furieux
    
    //initialisation du joueur
	_player player ;
	float profit ;
	
	//initialisation du lieu de jeu
	int grid_size_x ;
    int grid_size_y ;
	_tile** grid ;

    //initialisation des plateaux
    int happy_bar_len ;
    int nb_plateau ;
    _plateau* plateau_tab ;

    //initialisation des paramètre des patients
    int patient_minimum_spawn_intervalle ; 
    int patient_spawn_range ;             
    int patient_spawning_hapiness ;        
    int patient_hapiness_range;            
    int next_patient_time ;                
}
typedef struct {
	int nb_step;
    int hummeur_tab[NB_hummeur];
    float profit;
    char username[username_SIZE];
} _score;

#endif
