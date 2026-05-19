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

//-----------------------------------------------------------
void color(unsigned char r, unsigned char g, unsigned char b){
    printf("\x1B[38;2;%d;%d;%dm", r, g, b);
}
//-----------------------------------------------------------
void reset_color(){
    printf("\x1B[0m");
}
//-----------------------------------------------------------
int randint(int a,int b) {
    return rand()%(b-a+1) +a;
}
//-----------------------------------------------------------
int inter_check(int nb,int a,int b) {
    return (a<=nb)&&(nb<=b);
}
//-----------------------------------------------------------
void exit_if_null_pointer(void* pointer) {
    if(pointer == NULL) {
        printf(">>Got a NULL pointer\n");
        exit(1);
    }
}
//-----------------------------------------------------------

void free_game(_jeu *jeu)
{
    if (jeu == NULL) {
        return;
    }

    // LibÃ©ration des patients
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
_tile cree_tile() {
    _tile new_tile;
    new_tile.value = 0;
    new_tile.player = 0;
    return new_tile;
}
//-----------------------------------------------------------
_tile** cree_grid(int size_x,int size_y) {
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
_coord get_player_pos_from_grid(_tile** grid,int size_x,int size_y) {
    _coord pos;
    pos.x = -1;
    pos.y = -1;
    exit_if_null_pointer(grid);
    for(int dy=0 ; dy<size_y ; dy++) {
        for(int dx=0 ; dx<size_x ; dx++) {
            if(grid[dy][dx].player == 1) {
                pos.x = dx;
                pos.y = dy;
                return pos;
            }
        }
    }
    printf("Player not found\n");
    return pos;
}
//-----------------------------------------------------------
_tile get_tile_from_pos(_tile** grid,int size_x,int size_y,int x,int y) {
    exit_if_null_pointer(grid);

    if(!inter_check(x,0,size_x-1) || !inter_check(y,0,size_y-1)) {
        printf("Got incoherent value  size=(x=%d/y=%d) ,position=(x=%d/y=%d)",size_x,size_y,x,y);
        exit(1);
    }
    return grid[y][x];
}
//-----------------------------------------------------------
 
   void save_game(_jeu *jeu, const char *filename)
{
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

    if (fwrite(&jeu->patient_hapiness_range, sizeof(int), 1,  f) != 1) {
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
    if (fwrite(jeu->username, sizeof(char), username_SIZE,f) != username_SIZE) {
        printf("Erreur sauvegarde username\n");
        fclose(f);
        return;
    }

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

        if (fread(jeu->plateau_tab[i].tools, sizeof(int),
                  NB_TOOLS, f) != NB_TOOLS)
        {
            printf("Erreur lecture tools\n");
            fclose(f);
            free_game(jeu);
            return;
        }

        if (fread(jeu->plateau_tab[i].used_tools, sizeof(int),
                  NB_TOOLS, f) != NB_TOOLS)
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
_jeu fonction_gestion_argent_cabinet(_jeu j)
{
    float argent_ustensile=0;

    FILE *fichier  = NULL;
    FILE *fichier2 = NULL;
    fichier2 = fopen ("pathologie_client.txt", "w" );
    fichier =fopen("ustensiles.txt","r");

    if (fichier == NULL){
        printf("Ouverture du fichier impossible\n");
        printf("code d'erreur = %d \n", errno );
        exit(1);
    }
    if (fichier2 == NULL){
        printf("Ouverture du fichier impossible\n");
        printf("code d'erreur = %d \n", errno );
        fclose(fichier);
        exit(1);
    }

    if (j.player.glove.type == 0){
        printf("Il faut mettre ses gants!");
    }
    else{
        int number = 0;
        for(int i=0;i<8;i++){
            fscanf(fichier, "%f %d" , &argent_ustensile ,&number);
            if(number == j.player.tool.type-'a'){
                j.profit -= argent_ustensile;
                fprintf(fichier2, "%f", j.profit);
                break;
            }
        }
    }

    fclose(fichier);
    fclose(fichier2);
    return j;
}

//-----------------------------------------------------------
_coord get_element_pos_from_grid(_tile** grid,int size_x,int size_y,int element) {
    _coord pos;
    pos.x = -1;
    pos.y = -1;
    exit_if_null_pointer(grid);
    for(int dy=0 ; dy<size_y ; dy++) {
        for(int dx=0 ; dx<size_x ; dx++) {
            if(grid[dy][dx].value == element) {
                pos.x = dx;
                pos.y = dy;
                return pos;
            }
        }
    }
    return pos;
}
//-----------------------------------------------------------
_plateau cree_plateau(int new_id){
    _plateau new_plateau;
    for(int i=0;i<NB_TOOLS;i++){
        new_plateau.tools[i] = 0;
        new_plateau.used_tools[i] = 0;
    }

    new_plateau.patient = NULL;
    new_plateau.id = new_id;
    return new_plateau;
}
//-----------------------------------------------------------
int can_move_at_pos(_tile** grid,int size_x,int size_y,int x,int y) {
    int tile_value = 0;
    _tile tile;

    tile = get_tile_from_pos(grid,size_x,size_y,x,y);
    tile_value = tile.value;
    if(inter_check(tile_value,'a','z')){
        return 1;
    }
    switch(tile_value) {
        default:
            return 0;
        case 0:
            return 1;
    }
}
//-----------------------------------------------------------
void tile_print(_tile tile ,_plateau* plateau_tab ,int taille ,_player player) {
	if(tile.player == 1) { // si le joueur est présent
	    
		if((player.glove.type =='h')){
		    printf("😷");   // si le joueur porte des gants
		}
		else{
		    printf("😐");   // si le joueur ne porte pas des gants
		}
	}
	else {
	    if( inter_check(tile.value,'T','Z') ){ // si la case représente "la case d'affichage d'un plateau"(la case où on voit si il y a un patient ou pas à ce plateau)
	        
	        char c;                                               
	        c = tile.value -'T' +'t';                               //    converti les valeurs de 'T' à 'Z' en minuscule ('t' à 'z') stocker dans la variable c pour 
	        for(int i=0;i<taille;i++){                              //    chercher le plateau correspondant dans le tableau plateau, plateau_tab ,(en fonction de son id qui est dans l'intervalle [t,z])
	            if(plateau_tab[i].id==c){
    	            if(plateau_tab[i].patient != NULL){//si il y a un patient present à ce plateau
    	                if(plateau_tab[i].patient->etat==satisfait){
    	                    printf("🤒");//si le patient est satisfait
    	                }
    	                else{
    	                    printf("😥");//si le patient est mécontant    
    	                }
    	                
    	                
    	            }
    	            else{
    	                printf("🪑");//si il n'y a pas de patient    
    	            }
    	            break;
	            }
	        }
	        
	    }        
	    else{
    		switch(tile.value) {
    		default:
    		    if( inter_check(tile.value,'t','z') ){//si la case est la case d'action des plateaux
    		        printf(" %c",tile.value);
    		    }
    		    else if( inter_check(tile.value,'a','h')||inter_check(tile.value,'i','j') ){//si c'est des cases d'actions autre que celles des plateux
    		        printf(" .",tile.value);
    		    }
    		    else{
    		        printf("  ");
    		    }
    			break;
    		case 0:
    			printf("  ");
    			break;
    		case 1:
    			printf("⬛" );
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

    		}
	    }
    /*
    -----------
    .player = 1 :😷
    -------
    0 :
    1 :⬛
    2 :🧊️
    3 :️🚪
    -------
    A :🪛  
    B :⚙️
    C :🔩
    D :🔬
    E :💉
    
    
    F :🩹
    G :💭
    H :🧤
    ️
    I :♻️
    J :☣️
	*/
	}
}
void print_grid(_tile** grid,int size_x,int size_y,_plateau* plateau_tab,int taille,_player player) {
    exit_if_null_pointer(grid);
    printf("\n\n");
    for(int dy=0 ; dy<size_y ; dy++) {
        printf("       ");
        for(int dx=0 ; dx<size_x ; dx++) {
            tile_print(grid[dy][dx] ,plateau_tab ,taille ,player);
        }
        printf("\n");
    }
    printf("\n");
}
//-----------------------------------------------------------
void move_player(_tile** grid,int size_x,int size_y,_movement movement) {
    _coord initial_pos;
    _coord final_pos;

    initial_pos = get_player_pos_from_grid(grid,size_x,size_y);
    if((initial_pos.x<0)||(initial_pos.y<0)){
        printf("player can't be found\n");
        exit(2);
    }

    if (inter_check(initial_pos.x,0,size_x-1)&&inter_check(initial_pos.y,0,size_y-1)) {
        final_pos.x = initial_pos.x +(movement==RIGHT) -(movement==LEFT) ;
        final_pos.y = initial_pos.y -(movement==UP) +(movement==DOWN) ;

        if (inter_check(final_pos.x,0,size_x-1)&&inter_check(final_pos.y,0,size_y-1)) {
            if (can_move_at_pos(grid,size_x,size_y,final_pos.x,final_pos.y)) {
                grid[initial_pos.y][initial_pos.x].player = 0;
                grid[final_pos.y][final_pos.x].player = 1;
            }
        }
    }
}
//-----------------------------------------------------------
int cure_if_got_tools(_plateau* plateau ,float* profit ,int* hummeur_tab){
    _plateau plate;
    plate = *plateau;
    int can_cure = 1;

    for(int i=0;i<NB_TOOLS;i++){
        if(plate.tools[i]<plate.patient->maladie.tool_needed[i]){
            can_cure = 0;
            break;
        }
    }
    if(can_cure){
        for(int i=0;i<NB_TOOLS;i++){
            (*plateau).tools[i] -= plate.patient->maladie.tool_needed[i];
            (*plateau).used_tools[i] += plate.patient->maladie.tool_needed[i];
        }

        if(plate.patient->etat == satisfait){
            printf("Un patient est parti satisfait car il a Ã©tÃ© soignÃ©, il a donnÃ© %.2f$\n",plateau->patient->maladie.profit);
            hummeur_tab[0]++;
            *profit += plateau->patient->maladie.profit;
        }
        else {
            if(plateau->patient->maladie.profit != 0){
                printf("Un patient est parti mÃ©content car il a Ã©tÃ© soignÃ© et il a attendue longtemps, il a donnÃ© %.2f$\n",0.5*(plateau->patient->maladie.profit));
                hummeur_tab[1]++;
                *profit += 0.5*(plateau->patient->maladie.profit);
            }
            else{
                printf("Un patient est parti mÃ©content car il a Ã©tÃ© soignÃ© et le plateau a Ã©tÃ© sale lors de son arrivÃ©, il a donnÃ© 0.0$\n");
                hummeur_tab[1]++;
            }
        }
        free((*plateau).patient);
        (*plateau).patient = NULL;

        return 1;
    }
    return 0;
}
//-----------------------------------------------------------
int try_cure_patient(_plateau* plateau ,float* profit ,int* hummeur_tab){
    if(plateau->patient!=NULL){
        return cure_if_got_tools(plateau,profit,hummeur_tab);
    }
    return 0;
}
//-----------------------------------------------------------
char try_do_action(_tile** grid,int size_x,int size_y,_player* player ,_plateau* plateau_tab ,int taille ,float* profit ,int* hummeur_tab){
    _tile current_tile;
    int tile_value;

    player->pos = get_player_pos_from_grid(grid ,size_x ,size_y);
    if((player->pos.x<0)||(player->pos.y<0)){
        printf("player can't be found\n");
        exit(2);
    }
    current_tile = get_tile_from_pos(grid ,size_x ,size_y ,player->pos.x ,player->pos.y);
    tile_value = current_tile.value;

    if((tile_value=='h')&&(player->glove.type==0)){
        player->glove.type = 'h';
        player->glove.clean = 0 ;
        player->glove.used = 0 ;
        *profit -= 1.2;
        printf("1.2$ du profit est dÃ©pensÃ© pour avoir des gants\n");
    }
    if(inter_check(tile_value,'a','g')&&(player->tool.type==0)){
        player->tool.type = tile_value;
        player->tool.clean = (player->glove.type=='h')&&(player->glove.used==0) ;
        player->tool.used = 0 ;
        *profit -= 1.2;
        printf("1.2$ du profit est dÃ©pensÃ© pour avoir un outil\n");
    }
    else if(tile_value=='i'){
        if((player->tool.type!=0)&&(player->tool.used!=1)){
            player->tool.type = 0;
        }
        else if((player->glove.type=='h')&&(player->glove.used!=1)){
            player->glove.type = 0;
        }
    }
    else if(tile_value=='j'){
        if((player->tool.type!=0)&&(player->tool.used==1)){
            player->tool.type = 0;
        }
        else if((player->glove.type=='h')&&(player->glove.used==1)){
            player->glove.type = 0;
        }
    }
    else if(inter_check(tile_value,'t','z')){
        for(int i=0;i<taille;i++){
            if(plateau_tab[i].id==tile_value){
                if((player->tool.type!=0)&&(player->tool.clean==1)&&((plateau_tab[i].tools[(player->tool.type)-'a'] + plateau_tab[i].used_tools[(player->tool.type)-'a']) == 0)){
                    plateau_tab[i].tools[(player->tool.type)-'a'] = 1;
                    player->tool.type = 0;
                    break;
                }
                else if((player->glove.used==0)&&(player->tool.type==0)&&(player->glove.type=='h')&&(plateau_tab[i].patient!=NULL)){
                    if(try_cure_patient(&(plateau_tab[i]) ,profit ,hummeur_tab)){
                        player->glove.used = 1;
                    }
                    break;
                }
                else if(((player->glove.used==1)||(player->glove.type!='h'))&&(plateau_tab[i].patient!=NULL)&&(player->tool.type==0)){
                    free(plateau_tab[i].patient);
                    plateau_tab[i].patient = NULL;
                    printf("Un patient est parti furieux par peur de contamination,\nil a contaminÃ© les outils prÃ©sents sur son plateau, il n'a rien donnÃ©\n");
                    hummeur_tab[2]++;
                    for(int ind=0;ind<NB_TOOLS;ind++){
                        if(plateau_tab[i].tools[ind]==1){
                            plateau_tab[i].tools[ind] = 0;
                            plateau_tab[i].used_tools[ind] = 1;
                        }
                    }
                }
                else{
                    for(int j=0;j<NB_TOOLS;j++){
                        if(plateau_tab[i].used_tools[j] == 1){
                            player->tool.type = 'a'+j;
                            player->tool.clean = 0;
                            player->tool.used = 1;
                            plateau_tab[i].used_tools[j] = 0;
                            break;
                        }
                    }
                }
            }
        }
    }
    return 0;
}
//-----------------------------------------------------------
void ask_to_do_player_action(_tile** grid,int size_x,int size_y,_player* player ,_plateau* plateau_tab,int taille ,float* profit ,int* hummeur_tab ,int* playing) {
    char move ;
    int error = 0;
    printf("Veuillez saisir votre action-----\n\n");
    printf("- type de dÃ©placement\n");
    printf(" |  z:vers le haut\n");
    printf(" |  d:vers la droite\n");
    printf(" |  s:vers le bas\n");
    printf(" |  q:vers la gauche\n");
    printf("- action\n");
    printf(" |  g:faire un action (si aucun action est possible rien se passe)\n");
    printf(" |  h:retour au menu principal (la partie n'est pas perdu)\n");
    do {
        error = scanf(" %c",&move);
        if(!error){
            printf("input went wrong,flushing scanf()\n");
            while(getchar() != '\n');
        }
        if((move!='z')&&(move!='d')&&(move!='s')&&(move!='q')&&(move!='g')&&(move!='h')) {
            printf("L'action saisi est incorrect,veuillez recommencez\n");
        }
    } while((move!='z')&&(move!='d')&&(move!='s')&&(move!='q')&&(move!='g')&&(move!='h'));
    printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    switch(move) {
    default:
        break;
    case 'z':
        move_player(grid,size_x,size_y,UP);
        break;
    case 'd':
        move_player(grid,size_x,size_y,RIGHT);
        break;
    case 's':
        move_player(grid,size_x,size_y,DOWN);
        break;
    case 'q':
        move_player(grid,size_x,size_y,LEFT);
        break;
    case 'g':
        try_do_action(grid,size_x,size_y,player,plateau_tab ,taille ,profit ,hummeur_tab);
        break;
    case 'h':
        *playing = 0;
        break;
    }
}
//-----------------------------------------------------------
void print_red_green(int a){//affiche vert pour 1 ou rouge pour 0 (affiche vrai ou faux avec couleur)
    if(a){
        printf(" 🟩 ");    
    }
    else{
        printf(" 🟥 ");    
    }
}
//-----------------------------------------------------------
void print_player_status(_player player ,float profit){//affiche les informations sur le joueur
    color(190,175,30);
    printf("\n----------------Le joueur-------------------\n");
    printf("    |💵 %.2f$",profit); //affichage de l'argent
    //si le joueur a des gants
    if(player.glove.type=='h'){ //affichage des gants si le joueur a des gant
        printf("    |🧤 ");
        if(player.glove.used==0){
            printf("🟩");//porpre
        }
        else{
            printf("🟫");//sale
        }
    }
     //si le joueur a un outil
    if(player.tool.type!=0){ //affichage des outils si le joueur a des outils
        printf("    |");
        switch(player.tool.type){
            default:
                printf("...");
                break;
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
        }
        if(player.tool.used==0){
            printf(" 🟩");//porpre
        }
        else{
            printf(" 🟫");//sale
        }
    }
    
    
    
    printf("\n");
    reset_color();
}
//-----------------------------------------------------------
void print_total_patient(int* hummeur_tab){//affiche la quantité de patient selon leur etat qui sont partis
    color(190,175,30);
    printf("    |patient score:  |%d 🟩    |%d 🟨    |%d 🟥  \n\n",hummeur_tab[0],hummeur_tab[1],hummeur_tab[2]);
    reset_color();
}


_tile** make_grid_from_string(char string[] ,int max_size_x ,int max_size_y){
    _tile** new_grid = NULL;
    new_grid = cree_grid(max_size_x ,max_size_y);
    int string_ind = 0;
    char c;
    char c2[2];
    c2[1] = '\0';
    for(int dy=0;dy<max_size_y;dy++){
        for(int dx=0 ;dx<max_size_x ;dx++){
            c = string[string_ind];
            if(c=='_'){
                string_ind++;
                c = string[string_ind];
            }
            if(c=='@'){
                return new_grid;
            }
            if(c!='P'){
                if(inter_check(c,'0','9')){
                    c2[0] = c;
                    new_grid[dy][dx].value = atoi(c2);
                }
                else{
                    new_grid[dy][dx].value = c;
                }
            }
            else{
                new_grid[dy][dx].value  = 0;
                new_grid[dy][dx].player = 1;
            }
            string_ind++;
        }
    }
    return new_grid;
}
//-----------------------------------------------------------
_plateau* get_plateau_tab(_tile** grid,int size_x,int size_y,int* taille){
    _coord plateau_pos;
    *taille = 0;
    for(int id='t';id<='z';id++){
        plateau_pos = get_element_pos_from_grid(grid ,size_x ,size_y ,id);
        if(inter_check(plateau_pos.x,0,size_x-1)&&inter_check(plateau_pos.y,0,size_y-1)){
            (*taille)++;
        }
    }
    _plateau* plateau_tab = NULL;
    plateau_tab = malloc((*taille)*sizeof(_plateau));
    exit_if_null_pointer(plateau_tab);
    int index = 0;
    for(int id='t';id<='z';id++){
        plateau_pos = get_element_pos_from_grid(grid ,size_x ,size_y ,id);
        if(inter_check(plateau_pos.x,0,size_x-1)&&inter_check(plateau_pos.y,0,size_y-1)){
            plateau_tab[index] = cree_plateau(id);
            index++;
        }
    }
    return plateau_tab;
}
//-----------------------------------------------------------
void print_plateau(_plateau plateau ,int max_happiness ,int happy_bar_len){ //affiche les information d'un plateau
    color(190,175,30);
    //printf("-------------------------------------plateau %c -------------------------------------------\n",plateau.id);
    printf("  <plateau %c > ",plateau.id);
    //affichage des outils present sur le plateau 
    //printf("    |tools :    ");
    for(int i=0;i<NB_TOOLS;i++){
        switch(i){
            default:
                printf("...");
                break;
            case 0:
                printf("🪛");
                break;
            case 1:
                printf("⚙️ ");
                break;
            case 2:
                printf("🔩");
                break;            
            case 3:
                printf("🔬");
                break;
            case 4:
                printf("💉");
                break;
            case 5:
                printf("🩹");
                break;
            case 6:
                printf("💭");
                break;
        }
        if(plateau.tools[i]){ //si outils present est propre
            if(plateau.patient != NULL){
                if (plateau.patient->maladie.tool_needed[i]){
                    printf(" ✅"); //si l'outils present est nécessaires 
                }
                else{
                    printf(" 🟩");
                }
            }
            else{
                printf(" 🟩"); 
            }
        }
        else if(plateau.used_tools[i]){ //si outils present est utilisée(sale) 
            if(plateau.patient != NULL){
                if (plateau.patient->maladie.tool_needed[i]){
                    printf(" 🚫"); //si l'outils present est nécessaires mais il est sale
                }
                else{
                    printf(" 🟫"); 
                }
            }
            else{
                printf(" 🟫"); 
            }
        }
        else if(plateau.patient != NULL){
            if (plateau.patient->maladie.tool_needed[i]){
                printf(" ⬜");  //si l'outils present est nécessaires et doit être ramené au plateau
            }
            else{
                printf(" 🟥");
            }
        }
        else{
            printf(" 🟥"); //l'outils n'est pas present sur le plateau  
        }
        printf("    ");
    }    

    
    if(plateau.patient!=NULL){//si il y a un patient à ce plateau ,affiche sa patience avec une jauge et en purcentage
        float percentage = 1.0*(plateau.patient->hummeur)/max_happiness;
        int nb_box = percentage*happy_bar_len;
        printf("|hummeur  ");
        for(int i=0;i<happy_bar_len;i++){
            if(nb_box<1){
                printf("⬛");
            }
            else if(i<=happy_bar_len*0.075){
                printf("🟥");
                //color(220,50,30);
            }
            else if(i<=happy_bar_len*0.22){
                printf("🟧");
                //color(190,90,30);
            }
            else if(i<=happy_bar_len*0.45){
                printf("🟨");
                //color(145,120,30);
            }
            else{
                printf("🟩");
                //color(50,150,30);
            }
            //printf(" ");
            nb_box--;
        }
        color(255*(1-percentage),255*percentage,90*(1-percentage));
        printf("    %.2f%",percentage*100);
        ///🟥🟧🟧🟨🟨🟨🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩⬛⬛
    }
    printf("\n");
    reset_color();
    
}
//-----------------------------------------------------------
void print_plateau_tab(_plateau* plateau_tab ,int taille ,int max_happiness ,int happy_bar_len){
    color(190,175,30);
    printf("------------------------------------- informations des plateaux --------------------------------------------------------------------------------\n");
    reset_color();
    for(int i=0;i<taille;i++){
        print_plateau(plateau_tab[i] ,max_happiness ,happy_bar_len);
    }
    printf("\n");
}
//-----------------------------------------------------------
void make_tool_tab(int tab[] ,int a,int b,int c,int d,int e,int f,int g){
    tab[0] = a;
    tab[1] = b;
    tab[2] = c;
    tab[3] = d;
    tab[4] = e;
    tab[5] = f;
    tab[6] = g;
}
//-----------------------------------------------------------
_patient* cree_patient(){
    _patient* patient = NULL;
    patient = malloc(sizeof(_patient));
    exit_if_null_pointer(patient);

    patient->hummeur = 100;
    patient->etat = satisfait;

    int desease_index = randint(1,NB_MALADIE);
    switch(desease_index){
        default:
            patient->maladie.type = TEST_DESEASE;
            make_tool_tab(patient->maladie.tool_needed,0,1,0,0,1,1,0);
            patient->maladie.profit = 1.23f;
            break;
        case 1:
            patient->maladie.type = DESEASE_A;
            make_tool_tab(patient->maladie.tool_needed,0,1,0,0,1,0,1);
            patient->maladie.profit = 3.20f;
            break;
        case 2:
            patient->maladie.type = DESEASE_B;
            make_tool_tab(patient->maladie.tool_needed,1,0,0,1,1,0,0);
            patient->maladie.profit = 2.90f;
            break;
        case 3:
            patient->maladie.type = DESEASE_C;
            make_tool_tab(patient->maladie.tool_needed,1,0,0,1,0,1,0);
            patient->maladie.profit = 4.14f;
            break;
        case 4:
            patient->maladie.type = DESEASE_D;
            make_tool_tab(patient->maladie.tool_needed,0,0,1,1,0,1,0);
            patient->maladie.profit = 3.49f;
            break;
        case 5:
            patient->maladie.type = DESEASE_E;
            make_tool_tab(patient->maladie.tool_needed,0,1,1,0,0,0,1);
            patient->maladie.profit = 5.65f;
            break;
    }

    return patient;
}
//-----------------------------------------------------------
int get_a_patient(_plateau* plateau_tab ,int taille ,int initial_hapiness){
    exit_if_null_pointer(plateau_tab);
    int more_patient = 0;
    for(int i=0;i<taille;i++){
        if(plateau_tab[randint(0,taille-1)].patient == NULL){
            more_patient = 1;
            break;
        }
    }
    if(more_patient){
        int ind = 0;
        do{
            ind = randint(0,taille-1);
        }while(plateau_tab[ind].patient != NULL);
        printf("Un patient s'est installÃ© sur un plateau\n");
        plateau_tab[ind].patient = cree_patient();
        plateau_tab[ind].patient->hummeur = initial_hapiness;

        for(int j=0;j<NB_TOOLS;j++){
            if(plateau_tab[ind].used_tools[j] == 1){
                printf("le patient ne va pas payer car le plateau n'est pas totalement propre\n");
                plateau_tab[ind].patient->maladie.profit = 0;
                plateau_tab[ind].patient->etat = mecontant;
                break;
            }
        }
        return 1;
    }
    return 0;
}
//-----------------------------------------------------------
void update_patients_hapiness(_plateau* plateau_tab ,int taille ,int* all_happy ,int* full ,float* profit ,int* hummeur_tab ,int max_happiness){
    int full_var = 1;
    for(int i=0;i<taille;i++){
        if(plateau_tab[i].patient != NULL){
            plateau_tab[i].patient->hummeur--;

            if (plateau_tab[i].patient->hummeur <= 0.45*max_happiness){
                plateau_tab[i].patient->etat = mecontant;
            }

            if( (plateau_tab[i].patient->hummeur)<=0  ){
                printf("Un patient est parti furieux par manque de patience ,il a rien donnÃ©\n");
                hummeur_tab[2]++;
                for(int ind=0;ind<NB_TOOLS;ind++){
                    plateau_tab[i].used_tools[ind] += plateau_tab[i].tools[ind];
                    plateau_tab[i].tools[ind] = 0;
                }

                free(plateau_tab[i].patient);
                plateau_tab[i].patient = NULL;
                *all_happy = 0;
            }
        }
        else{
            full_var = 0;
        }
    }
    *full = full_var;
}
//-----------------------------------------------------------
void patients_spawning_regulation(_plateau* plateau_tab ,int taille ,int min_spawn_time ,int spawn_time_range ,int* current_patient_spawning_time ,int initial_hapiness){
    if((*current_patient_spawning_time) <= 0){
        if(get_a_patient(plateau_tab ,taille ,initial_hapiness)){
            (*current_patient_spawning_time) = min_spawn_time + randint(0 ,spawn_time_range);
        }
    }
    else{
        (*current_patient_spawning_time)--;
    }
}
//-----------------------------------------------------------
int update_patients(_plateau* plateau_tab ,int taille ,int min_spawn_time ,int spawn_time_range ,int* current_patient_spawning_time ,int initial_hapiness ,float* profit ,int* hummeur_tab){
    int full = 0;
    int all_happy = 1;

    patients_spawning_regulation(plateau_tab ,taille ,min_spawn_time ,spawn_time_range ,current_patient_spawning_time ,initial_hapiness);
    update_patients_hapiness(plateau_tab ,taille ,&all_happy ,&full ,profit ,hummeur_tab ,min_spawn_time+spawn_time_range);
    printf("next patient in %d step(s)\n",*current_patient_spawning_time);
    if(full&&(!all_happy)){
        return 0;
    }
    return 1;
}
//-----------------------------------------------------------
void decrease_if_to_much(int* var ,int min){
    if(*var > min){
        (*var)--;
    }
}
//-----------------------------------------------------------
void get_grid_size_from_string(char map_string[] ,int* size_x ,int* size_y){
    int temp_size_x = 0;
    int new_size_x = 0;
    int new_size_y = 0;
    int i = 0;
    while(map_string[i] != '@'){
        if(map_string[i] == '_'){
            if((temp_size_x > new_size_x)&&(new_size_y==0)){
                new_size_x = temp_size_x;
            }
            else if((temp_size_x != new_size_x)&&(new_size_y!=0)){
                printf("le string map a une forme irregulier x:%d!=%d && y=%d\n",temp_size_x,new_size_x,new_size_y);
                exit(0);
            }
            new_size_y++;
            temp_size_x=0;
        }
        else{
            temp_size_x++;
        }
        i++;
    }
    *size_x = new_size_x;
    *size_y = new_size_y;
}
//-----------------------------------------------------------
char* get_random_map_string(){
    int choice = randint(0,3);
    switch(choice){
        default:
            return "001111111000_001BCADE1000_001222221100_111bcade0111_1F2f0000t4T3_1G2g0P0000111_1H2h0000u4U3_111000000111_3W4wi0j0v4V3_1111I1J11111_000111110000_@";
        case 1:
            return "11111111111331111111_10000000100000030001_10000011100111111101_100P00jJ100000u4U001_100000iI100000v4V001_11100111100000y4Y101_10000000000000w4W001_10abcde0fgh000x4X001_11ABCDE1FGH100z4Z001_11111111111111111111_@";
        case 2:
            return "111111I11111_101U4ui00a2A_103U4u0P0b2B_101U4uhhhc2C_1011JjhHhd2D_101T4thhhe2E_103T4t000f2F_101T4ti00g2G_131111I11111_@";
        case 3:
            return "0111AB1131CD111111EF11111333300_0100ab0000cd000000ef00000300300_0300000000000000000000000333300_0300001111110001111110000000100_01ii00tttuuu000vvvwww00ii100100_01Ii00tTtuUu0P0vVvwWw00iI100100_01ii00tttuuu000vvvwww00ii100100_1111100000000000000000000000100_1JJJ1000000hhhhh00000ggg0000100_1jjj0000001HHHHH10000gGg01001PP_11111111111111111111111111331PP_@";
    }
}
//-----------------------------------------------------------
_jeu creer_jeu(){
    _jeu new_game;
    new_game.play = 0;
    new_game.player.tool.type = 0;
    new_game.player.glove.type = 0;
    new_game.profit = 0.00f;

    new_game.grid_size_x = 0;
    new_game.grid_size_y = 0;
    new_game.grid = NULL;
    char* map_string = get_random_map_string();
    get_grid_size_from_string(map_string ,&(new_game.grid_size_x) ,&(new_game.grid_size_y));
    new_game.grid = make_grid_from_string(map_string ,new_game.grid_size_x ,new_game.grid_size_y);

    new_game.happy_bar_len = 32;
    new_game.nb_plateau = 0;
    new_game.plateau_tab = NULL;
    new_game.plateau_tab = get_plateau_tab(new_game.grid ,new_game.grid_size_x ,new_game.grid_size_y ,&(new_game.nb_plateau));

    new_game.hummeur_tab[0] = 0;
    new_game.hummeur_tab[1] = 0;
    new_game.hummeur_tab[2] = 0;
    new_game.patient_minimum_spawn_intervalle = 4*(new_game.nb_plateau);
    new_game.patient_spawn_range = 10;
    new_game.patient_spawning_hapiness = 37*(new_game.nb_plateau);
    new_game.patient_hapiness_range = 7*(new_game.nb_plateau);
    new_game.next_patient_time = 13 + 0*(new_game.patient_minimum_spawn_intervalle + randint(0 ,new_game.patient_spawn_range));
    new_game.nb_step = -1;

    return new_game;
}
//-----------------------------------------------------------
int play_a_game(_jeu* game, char *username){

    game->play = 1;
    int playing = 1;
    while(playing){
        game->nb_step++;
        if(!update_patients(game->plateau_tab ,game->nb_plateau ,game->patient_minimum_spawn_intervalle ,game->patient_spawn_range ,&(game->next_patient_time) ,game->patient_spawning_hapiness+randint(0,game->patient_hapiness_range) ,&(game->profit) ,game->hummeur_tab)){
            color(250,30,30);
            printf("\n\n");
            printf("------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
            printf("        GAME OVER!! Le jeu a durÃ© pour %d pas avec: un profit de %.2f$ ,%d patient(s) satisfait(s) ,%d patient(s) mÃ©content(s) ,%d patient(s) furieux\n\n ",game->nb_step ,game->profit ,game->hummeur_tab[0] ,game->hummeur_tab[1] ,game->hummeur_tab[2]);
            printf("------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
            reset_color();
            sleep(2);
            
            FILE* score_file=fopen("scoreboard.txt", "a+"); 
            exit_if_null_pointer(score_file); 
            fprintf(score_file,"%s %d %.2f %d %d %d\n",username, game->nb_step,game->profit,game->hummeur_tab[0], game->hummeur_tab[1],game->hummeur_tab[2]);
            fclose(score_file); 
                   
            return 0;
        }
        print_grid(game->grid,game->grid_size_x,game->grid_size_y,game->plateau_tab,game->nb_plateau,game->player);
        print_plateau_tab(game->plateau_tab ,game->nb_plateau ,game->patient_spawning_hapiness+(game->patient_hapiness_range) ,game->happy_bar_len);
        print_player_status(game->player ,game->profit);
        print_total_patient(game->hummeur_tab);
        ask_to_do_player_action(game->grid,game->grid_size_x,game->grid_size_y,&(game->player),game->plateau_tab,game->nb_plateau,&(game->profit) ,game->hummeur_tab ,&playing);
    }
    return 1;
}
//-----------------------------------------------------------
void score_swap(_score* a,_score* b){
    _score temp = *a;
    *a = *b;
    *b = temp;
}
//-----------------------------------------------------------
void _score_bubble_sort(_score* tab ,int taille){
    for(int i=0;i<taille-1;i++){
        for(int j=0;j<taille-1-i;j++){
            if(tab[j].nb_step < tab[j+1].nb_step){
                score_swap(tab+j,tab+j+1);
            }
            else if(tab[j].nb_step == tab[j+1].nb_step){
                if(tab[j].profit < tab[j+1].profit){
                    score_swap(tab+j,tab+j+1);
                }
            }
        }
    }
}
//-----------------------------------------------------------
int* cree_tab(int size){
    int* tab = NULL;
    tab = malloc(size*sizeof(int));
    exit_if_null_pointer(tab);
    for(int i=0;i<size;i++){
        tab[i] = randint(100,10000);
    }

    return tab;
}
//-----------------------------------------------------------
void print_scoreboard(){
    FILE* score_file = NULL;
    score_file = fopen("scoreboard.txt","r");
    if(score_file == NULL){
        printf("Impossible d'ouvrir le fichier scoreboard.txt\n");
        return;
    }

    char var_ligne[200] = "";
    int nb_ligne = 0;
    char* p_error = NULL;
    do{
        p_error = fgets(var_ligne ,200 ,score_file);
        if(p_error != NULL){
            nb_ligne++;
        }
    }while(p_error != NULL);
    printf("ligne : %d \n",nb_ligne);
    rewind(score_file);

    if(nb_ligne == 0){
        printf("Aucun score enregistrÃ©.\n");
        fclose(score_file);
        return;
    }

    _score* score_tab = NULL;
    score_tab = malloc(nb_ligne*sizeof(_score));
    exit_if_null_pointer(score_tab);

    for(int i=0;i<nb_ligne;i++){
        fscanf(score_file ,"%49s %d %f %d %d %d\n",score_tab[i].username,&(score_tab[i].nb_step) ,&(score_tab[i].profit) ,&(score_tab[i].hummeur_tab[0]) ,&(score_tab[i].hummeur_tab[1]) ,&(score_tab[i].hummeur_tab[2]));
    }
    fclose(score_file);

    _score_bubble_sort(score_tab ,nb_ligne);

    printf("-----------------------------------------\n");
    printf("--------------scoreboard-----------------\n");
    for(int i=0;i<nb_ligne;i++){
        color(175,120,50);
        if(i+1<10){
            printf("    |  0%d  |",i+1);
        }
        else{
            printf("    |  %d  |",i+1);
        }
        color(175,120,50);
        printf("name:%s ",score_tab[i].username);
        reset_color();
        printf("time(in step):%d ",score_tab[i].nb_step);
        color(50,145,180);
        printf("profit:%.2f ",score_tab[i].profit);
        color(100,200,100);
        printf("satisfait:%d ",score_tab[i].hummeur_tab[0]);
        color(150,150,75);
        printf("mecontent:%d ",score_tab[i].hummeur_tab[1]);
        color(200,75,75);
        printf("furieux:%d ",score_tab[i].hummeur_tab[2]);
        printf("\n");
        reset_color();
    }

    free(score_tab);

    printf("\n\nPress and enter anything to return to the main menu\n");
    char c;
    int error = 0;
    error = scanf(" %c",&c);
    if(!error){
        while(getchar() != '\n');
    }
}
//-----------------------------------------------------------
_menu ask_menu(_jeu* current_game ,_menu current_menu) {
    char menu;
    int menu_choice_correct = 0;
    int error = 0;
    printf("    Veuillez choisir l'une des options suivantes\n\n");
    if(current_menu == select_menu){
        if(current_game->play==1){
            printf("         |  save current game (press s)(not ready)  \n");
            printf("         |  continue  current game(press d)  \n");
        }
        printf("         |  play a new game  (press f)  \n");
        printf("         |  scoreboard   (press g)(not complete)  \n");
    }
    else{
        printf("         |  go to menu   (press q)  \n");
    }
    printf("         |  quit program (press h)  \n");
    printf("\n\n\n\n");
    do {
        error = scanf(" %c",&menu);
        menu_choice_correct = ((current_menu == select_menu)&&((((menu=='s')||(menu=='d'))&&(current_game->play==1))||(menu=='f')||(menu=='g')||(menu=='h'))) || ((current_menu != select_menu)&&((menu=='h')||(menu=='q')));
        if(!error){
            printf("input went wrong,flushing scanf()\n");
            while(getchar() != '\n');
        }
        if(!menu_choice_correct) {
            printf("Le menu choisi est incorrect,veuillez recommencez\n");
        }
    } while(!menu_choice_correct);
    printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    switch(menu) {
        default:
            break;
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
    }
    return select_menu;
}
//-----------------------------------------------------------
void start(){

    _menu current_menu = select_menu;
    _jeu current_game;
    current_game.play = 0;
    current_game.grid = NULL;
    current_game.plateau_tab = NULL;
    char* username = NULL; 
    while(current_menu == select_menu){
        printf("_________________________________________________________________________________________________________________________________________\n");
        printf("_________________________________________________________________________________________________________________________________________\n");
        printf("         ####                                  #####                                                                                     \n");
        printf("         #    ####  #   # ###  ### #   #         #   ####  #### # ## #### #### #### ### ####            made by:-Assia                   \n");
        printf("         #    #  #  #   #  #    #   # #          #   #  #  ##   ##   #    #  # #  # #   ####                    -Catherine               \n");
        printf("         #    ####   # #   #    #    #           #   ####    ## # #  ###  #  # ###  #   #                       -ThÃ©o R.                 \n");
        printf("         #### #  #    #   ###   #    #           #   #  #  #### #  # #    #### #  # ### ####                                             \n");
        printf("_________________________________________________________________________________________________________________________________________\n");
        printf("__________________________________________________menu principal_________________________________________________________________________\n");

    	if (username == NULL) {
           username = malloc(username_SIZE*sizeof(char));
           exit_if_null_pointer(username);

           printf("Veuillez saisir le nom de votre joueur\n");
           if(scanf("%49s", username) != 1){
               printf("Erreur de lecture de username\n");
	       free(username);
               exit(1);
           }
           username[username_SIZE - 1] = '\0';
     	}

        current_menu = ask_menu(&current_game ,current_menu);
        switch(current_menu){
        default:
            break;

        case new_jeu:

            free_game(&current_game);

            current_game = creer_jeu();

            if(!play_a_game(&current_game, username)){
                free_game(&current_game);
                current_game = creer_jeu();
            }

      	break;

        case continu:

            free_game(&current_game);

            load_game(&current_game, "save.dat");

             if(!play_a_game(&current_game, username))
             {
                   free_game(&current_game);
                   current_game = creer_jeu();
             }

        break;

        case scoreboard:
            print_scoreboard();
            break;

        case save:
            save_game(&current_game, "save.dat");
            break;

        case quit:
            free_game(&current_game);
            free(username);
            break;
        }
	if(current_menu != quit){
            current_menu = select_menu;
	}
    }
}
//-----------------------------------------------------------
int main() {
    srand(time(NULL));
    printf("Running program\n\n\n\n");

    start();

    printf("program ended\n");
    return 0;
}
