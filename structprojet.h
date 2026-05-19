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
 #define username_SIZE 50

typedef enum  _movement;
typedef enum  _maladie_type;
typedef enum  _etat;
typedef enum  _menu;
typedef struct _tile;
typedef struct  _coord;
typedef struct  _tool;
typedef struct  _player;
typedef struct _maladie;
typedef struct  _patient;
typedef struct _plateau;
typedef struct  _jeu;
typedef struct  _score;
