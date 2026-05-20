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



//-----------------------------------------------------------
typedef enum {UP=0,RIGHT=1,DOWN=2,LEFT=3} _movement;
//-----------------
typedef enum {TEST_DESEASE=0,DESEASE_A=1,DESEASE_B=2,DESEASE_C=3,DESEASE_D=4,DESEASE_E=5} _maladie_type;
//-----------------
typedef enum {satisfait=0,mecontant=1,furieux=2} _etat;
//-----------------
typedef enum {new_jeu=0,select_menu=1,scoreboard=2,save=3,continu=4,quit=5,load=6} _menu;
//-----------------
typedef struct {
	int value;  //type de case  
	int player; //presence de joueur
} _tile;     //case 
//-----------------
typedef struct {
	int x;
	int y;
} _coord;
//-----------------
typedef struct {
    int clean;  //sale (quand l'outil est pris et le joueur n'a pas de gant)
    int used;   //sale (quand l'outil est utilisé)
    int type;   //type d'outil  a: b: c: d: e: f: g: h:
} _tool;
//-----------------
typedef struct {
	_coord pos;
	_tool tool;   //si outil alors player.glove.type = (int)    sinon player.tool.type = 0
	_tool glove;  //si gant alors player.glove.type = 'h'       sinon player.glove.type = 0
} _player;
//-----------------
typedef struct {
    _maladie_type type;
    int tool_needed[NB_TOOLS] ;
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
    int tools[NB_TOOLS] ;     //{a,b,c,d,e,f,g}
    int used_tools[NB_TOOLS] ;
    _patient* patient;
    int id;   //noms possible des plateaux (t u v w x y z)
} _plateau;
//-----------------
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
    int patient_minimum_spawn_intervalle ; //le temps minimum à attendre avant un patient apparait  
    int patient_spawn_range ;              //le temps additionelle maximum à attendre pour l'apparition d'un patient
    int patient_spawning_hapiness ;        //la patience minimum d'un patient qui vient d'apparaitre
    int patient_hapiness_range;            //la patience additionelle maximum pour un patient qui vient d'apparaitre
    int next_patient_time ;                //le temps restant avant le prochain patient aparait (si il n'y a plus de place pour un nouveau patient le temps restera à 0)
} _jeu ;
//-----------------
typedef struct {
	int nb_step;
    int hummeur_tab[NB_hummeur];
    float profit;
    char username[username_SIZE];
} _score;



_coord get_player_pos_from_grid(_tile** grid,int size_x,int size_y);
_tile** make_grid_from_string(char string[] ,int max_size_x ,int max_size_y);
_patient* cree_patient();
_coord get_player_pos_from_grid(_tile** grid,int size_x,int size_y);

//-----------------------------------------------------------
void color(unsigned char r, unsigned char g, unsigned char b){    //couleur affichage           r ,g ,b apartient à l'intervalle [0,255]
	printf("\x1B[38;2;%d;%d;%dm", r, g, b);                       // red, green ,blue
}
//-----------------------------------------------------------
void reset_color(){
	printf("\x1B[0m");                                            // revenir a la couleur par defaut
}
//-----------------------------------------------------------     //pour avoir un nombre alératoire dans l'intervalle [a,b]  
int randint(int a,int b) {                                        // nombre entre intervalle pour pathologie et ustensiles...
	return rand()%(b-a+1) +a;
}
//-----------------------------------------------------------     //vérifier si a<=nb<=b est vrai
int inter_check(int nb,int a,int b) {                             //verifier si c dans une bon intervalle (si le deplacement est dans a taille de tableau
	return (a<=nb)&&(nb<=b);
}
//-----------------------------------------------------------
void exit_if_null_pointer(void* pointer) {                       //utile pour apres un malloc 
	if(pointer == NULL) {
		printf(">>Got a NULL pointer\n");
		exit(1);
	}
}
//-----------------------------------------------------------
_tile cree_tile() {                                              //creer une case de la grille avec des données par défaut                                    
	_tile new_tile;  
	new_tile.value = 0;
	new_tile.player = 0;
	return new_tile;
}

//-----------------------------------------------------------
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
	fprintf(file ,"profit %.2f\n",jeu.profit);
	
	//fprintf(file ,"\n\n");
	// les informations du joueur
	fprintf(file ,"tool_type %d\n",jeu.player.tool.type);
	fprintf(file ,"tool_clean %d\n",jeu.player.tool.clean);
	fprintf(file ,"tool_used %d\n",jeu.player.tool.used);
	fprintf(file ,"glove_type %d\n",jeu.player.glove.type);
	fprintf(file ,"glove_used %d\n",jeu.player.glove.used);
	jeu.player.pos = get_player_pos_from_grid(jeu.grid,jeu.grid_size_x,jeu.grid_size_y);
	fprintf(file ,"player_pos_x %d\n",jeu.player.pos.x);
	fprintf(file ,"player_pos_y %d\n",jeu.player.pos.y);
	
	//fprintf(file ,"\n\n");
	// les informations de la grille de jeu
	fprintf(file ,"grid_size_x %d\n",jeu.grid_size_x);
	fprintf(file ,"grid_size_y %d\n",jeu.grid_size_y);
	_tile tile;
	for(int dy=0;dy<jeu.grid_size_y;dy++){
		for(int dx=0;dx<jeu.grid_size_x;dx++){
			tile = jeu.grid[dy][dx];
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
	fprintf(file ,"nb_plateau %d\n",jeu.nb_plateau);
	
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
//-----------------------------------------------------------          //savoir la position du joueur dans la grille de jeu
_coord get_player_pos_from_grid(_tile** grid,int size_x,int size_y) {  //deplacer dans chaque case de la grille si la case player est 1 : retrourner la position -> utile pr en cas de reinitialisation. 
	_coord pos;
	pos.x = -1;
	pos.y = -1;
	exit_if_null_pointer(grid);
	for(int dy=0 ; dy<size_y ; dy++) {                                  //parcours ligne  de en haut vers le bas
		for(int dx=0 ; dx<size_x ; dx++) {                              //parcours colonne  de gauche vers la droite
			if(grid[dy][dx].player == 1) {
				pos.x = dx;
				pos.y = dy;
				return pos;                //retourner la position du joueur si trouvé
			}
		}
	}
	printf("Player not found\n");   
	return pos;                            //retourner la position (-1,-1) si le joueur n'est pas trouvé
}
//-----------------------------------------------------------
_tile get_tile_from_pos(_tile** grid,int size_x,int size_y,int x,int y) {      // a partir d une position on veut recup la case
	exit_if_null_pointer(grid);

	if(!inter_check(x,0,size_x) + !inter_check(x,0,size_x)) {                                             // si la case à la position demander depasse les limites de la grille (sort de la grille)
		printf("Got incoherent value  size=(x=%d/y=%d) ,position=(x=%d/y=%d)",size_x,size_y,x,y);         // pr pas que la case choisie ne soit pas negative ou en dehors du tableau
		exit(1);
	}
	return grid[y][x];                                                                                    //retourn la case trouver

}
//-----------------------------------------------------------
_coord get_element_pos_from_grid(_tile** grid,int size_x,int size_y,int element) { //avoir la position d'une case avec une valeur spécifique dans la grille de jeu(par exemple la position des cases d'actions)
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
_plateau cree_plateau(int new_id){ //crée un variable plateau
    _plateau new_plateau;
    for(int i=0;i<NB_TOOLS;i++){
        new_plateau.tools[i] = 0;       //pas d'outils propre
        new_plateau.used_tools[i] = 0;  //pas d'outils usée
    }
    
    new_plateau.patient = NULL;     //pas de patient <=> plateau.patient == NULL
    new_plateau.id = new_id;        // le nom du plateau qui est dans l'intervalle [t,z]
    return new_plateau;
}
//-----------------------------------------------------------
int can_move_at_pos(_tile** grid,int size_x,int size_y,int x,int y) {        //(x,y) est la position d'arriver du déplacement
	int tile_value = 0;
	_tile tile;

	tile = get_tile_from_pos(grid,size_x,size_y,x,y) ;                          // a partir du type de case est ce que  le joueur peut sy rendre
	tile_value = tile.value;
	if(inter_check(tile_value,'a','z')){                                        // les cases de valeurs 'a' à 'z' sont des cases d'actions  ,de plus ces case le joueur peut se déplacer sur cette case donc return 1
	    return 1;
	}
	switch(tile_value) {                                                   
    	default: //si la case n'est pas du vide
    		return 0; 
    	case 0: //si la case est du vide
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
//-----------------------------------------------------------
void print_grid(_tile** grid,int size_x,int size_y,_plateau* plateau_tab,int taille,_player player) {         //afficher la grille de gauche à doite ,de haut en bas;
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
void move_player(_tile** grid,int size_x,int size_y,_movement movement) {               //déplace le joueur dans la grille de jeu en fonction du mouvement demandé(stocké dans movement) 
	_coord initial_pos;
	_coord final_pos;

	initial_pos = get_player_pos_from_grid(grid,size_x,size_y);           //avoir la position du joueur
	if((initial_pos.x<0)||(initial_pos.y<0)){
        printf("player can't be found\n");
        exit(2);
    }
    
	if (inter_check(initial_pos.x,0,size_x)&&inter_check(initial_pos.y,0,size_y)) {
        //calcule la position final en fonction de la position du joueur et du mouvement demandé
		final_pos.x = initial_pos.x +(movement==RIGHT) -(movement==LEFT) ;              
		final_pos.y = initial_pos.y -(movement==UP) +(movement==DOWN) ;

		if (inter_check(final_pos.x,0,size_x)&&inter_check(final_pos.y,0,size_y)) { //vérifie si la position final est acceptable

			if (can_move_at_pos(grid,size_x,size_y,final_pos.x,final_pos.y)) {
			    //déplace le joueur
				grid[initial_pos.y][initial_pos.x].player = 0;
				grid[final_pos.y][final_pos.x].player = 1;

			}
			else{
			    //printf("Couldn't move\n");
			}


		}
	}

}
//-----------------------------------------------------------
int cure_if_got_tools(_plateau* plateau ,float* profit ,int* hummeur_tab){//soigner si les outils propre necessaire sont tous present sur le plateau
    _plateau plate;
    plate = *plateau;
    int can_cure = 1;
    
    for(int i=0;i<NB_TOOLS;i++){ //vérifie si les outils propre nécessaire sont présent 
        if(plate.tools[i]<plate.patient->maladie.tool_needed[i]){
            can_cure = 0;
            break;
        }
    }
    if(can_cure){//si oigné le patient est possible

        for(int i=0;i<NB_TOOLS;i++){
            //les outils nécessaire devient usée
            (*plateau).tools[i] -= plate.patient->maladie.tool_needed[i];
            (*plateau).used_tools[i] += plate.patient->maladie.tool_needed[i];
        } 
        
        //si un patient satisfaisant est soigné
        if(plate.patient->etat == satisfait){
            printf("Un patient est parti satisfait car il a été soigné, il a donné %.2f$\n",plateau->patient->maladie.profit);
            hummeur_tab[0]++;
            *profit += plateau->patient->maladie.profit;
        }
        //si un patient mécontant est soigné
        else {
            if(plateau->patient->maladie.profit != 0){//si il doit payé plus que 0$
                printf("Un patient est parti mécontent car il a été soigné et il a attendue longtemps, il a donné %.2f$\n",0.5*(plateau->patient->maladie.profit));
                hummeur_tab[1]++;
                *profit += 0.5*(plateau->patient->maladie.profit);
            }
            else{//il n'a pas besoin de payé <=> le plateau est sale lors de son arrivé
                printf("Un patient est parti mécontent car il a été soigné et le plateau a été sale lors de son arrivé, il a donné 0.0$\n");
                hummeur_tab[1]++;
            }
        }
        // le patient part
        free((*plateau).patient);
        (*plateau).patient = NULL;
        
        return 1;
    }
    return 0;
}
//-----------------------------------------------------------
int try_cure_patient(_plateau* plateau ,float* profit ,int* hummeur_tab){    //(pour soigner) verifie si il y a un patient ,si oui on essaye de le soigner
    if(plateau->patient!=NULL){
        return cure_if_got_tools(plateau,profit,hummeur_tab);
    }
}
//-----------------------------------------------------------
void fonction_gestion_argent_cabinet(float* profit ,int outils_id){
	FILE* file = NULL;
	file = fopen("ustensiles.txt","r");
	exit_if_null_pointer(file);
	int error = 0;
	int number = 0;
	float cost = 0;
	char temp[100];
	for(int i=0;i<NB_TOOLS+1;i++){
		fscanf(file ,"%d %f %s\n",&number ,&cost ,temp);
		if(error){
			*profit -= 1.2;
			printf("il y a au un erreur donc 1.2$ du profit est dépensé pour avoir un outil\n");
			break;
		}
		
		if(outils_id == number){ //convertir ['1','8'] en [1,8]
			*profit -= cost;
			printf("%.2f$ du profit est dépensé pour avoir un outil\n",cost);
			break;
		}
	}
	
	
	
	
}

//-----------------------------------------------------------
char try_do_action(_tile** grid,int size_x,int size_y,_player* player ,_plateau* plateau_tab ,int taille ,float* profit ,int* hummeur_tab ,int* game_over){ //pour les actions autre que les déplacements
    _tile current_tile;
    int tile_value;
    
    int full = 1;
    int angry_patient = 0;
    *game_over = 0;
    
    player->pos = get_player_pos_from_grid(grid ,size_x ,size_y);
    if((player->pos.x<0)||(player->pos.y<0)){   //si le joueur se situe à (-1,-1) ,qui n'est pas possible => exit()
        printf("player can't be found\n");
        exit(2);
    }
    current_tile = get_tile_from_pos(grid ,size_x ,size_y ,player->pos.x ,player->pos.y);       //la case où le dentiste est actuellement situé 
    tile_value = current_tile.value;                                                            //type de case où le dentiste est actuellement situé 
    
    if((tile_value=='h')&&(player->glove.type==0)){         //si la case d'action est la case d'action des gants et le joueur n'a pas des gants
        //prendre des gants si le joueur n'a pas des gants
        player->glove.type = 'h';
        player->glove.clean = 0 ;
        player->glove.used = 0 ; 
        fonction_gestion_argent_cabinet(profit ,'h'-'a'+1);
    }
    if(inter_check(tile_value,'a','g')&&(player->tool.type==0)){  //si la case d'action est la case d'action pour prendre des outils ,de plus si le joueur n'a pas d'outil en main
        //prendre un objet si le joueur n'a pas d'outil
        player->tool.type = tile_value;
        player->tool.clean = (player->glove.type=='h')&&(player->glove.used==0) ;
        player->tool.used = 0 ;
        fonction_gestion_argent_cabinet(profit ,tile_value-'a'+1);
    }
    else if(tile_value=='i'){  //si la case d'action est la case d'action de la poubelle pour les outils sales non usées
        //mettre les outils dans la poubelle de recyclage
        if((player->tool.type!=0)&&(player->tool.used!=1)){
            player->tool.type = 0;    
        }
        //mettre les gants dans la poubelle de recyclage
        else if((player->glove.type=='h')&&(player->tool.used!=1)){
            player->glove.type = 0;     
        }
    }
    else if(tile_value=='j'){ //si la case d'action est la case d'action de la poubelle pour les outils usées
        //mettre les outils dans la poubelle biologique
        if((player->tool.type!=0)&&(player->tool.used==1)){
            player->tool.type = 0;    
        }
        //mettre les gants dans la poubelle biologique
        else if((player->glove.type=='h')&&(player->glove.used==1)){
            player->glove.type = 0;   
        }
    }
    else if(inter_check(tile_value,'t','z')){ //si la case d'action est la case d'action des plateaux
        //pour les plateaux
        //on cherche le plateau
        for(int i=0;i<taille;i++){
            if(plateau_tab[i].patient==NULL){
            	full = 0;
            }
            if(plateau_tab[i].id==tile_value){
            	
            
                //mettre l'outil non sale au plateau si possible
                if(     (player->tool.type!=0)&&(player->tool.clean==1)&&(  (plateau_tab[i].tools[(player->tool.type)-'a'] + plateau_tab[i].used_tools[(player->tool.type)-'a']) == 0)    ){
                    plateau_tab[i].tools[(player->tool.type)-'a'] = 1;
                    player->tool.type = 0;
                    break;
                }
                //soigner le patient si possible
                else if(     (player->glove.used==0)&&(player->tool.type==0)&&(player->glove.type=='h')&&(plateau_tab[i].patient!=NULL)){
                    
                    if(try_cure_patient(&(plateau_tab[i]) ,profit ,hummeur_tab)){
                        player->glove.used = 1;    
                    }
                    break;
                }
                //si on tente de soigner un patient mais on n'a pas de gant ou des gants sales
                else if(    ((player->glove.type=='h')&&(player->glove.used==1))||((player->glove.type!='h')&&(plateau_tab[i].patient!=NULL)&&(player->tool.type==0)) ){
                    //le patient part
                    free(plateau_tab[i].patient);
                    plateau_tab[i].patient = NULL;
                    printf("Un patient est parti furieux par peur de contamination,\nil a contaminé les outils présents sur son plateau, il n'a rien donné\n");
                    hummeur_tab[2] ++;
                    angry_patient = 1;
                    //les outils deviennent sales ,par la panique du patient ,par peur de contamination
                    for(int ind=0;ind<NB_TOOLS;ind++){
                        if(plateau_tab[i].tools[ind]==1){
                            plateau_tab[i].tools[ind] = 0;
                            plateau_tab[i].used_tools[ind] = 1;    
                        }
                    }
                    
                    
                }
                else{
                    //prendre un outil sale du plateau si possible
                    //recherche d'un outil sale
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
    //verification si la partie est terminé
    if(angry_patient && full){
    	*game_over = 1;
    }
} 
//-----------------------------------------------------------
void ask_to_do_player_action(_tile** grid,int size_x,int size_y,_player* player ,_plateau* plateau_tab,int taille ,float* profit ,int* hummeur_tab ,int* playing ,int* game_over) {//demande d'action de la part du joueur (déplacament ,action ,retourner vers le menu pricipal)
	char move ;
	int error = 0;
	printf("Veuillez saisir votre action-----\n\n");
	printf("- type de déplacement\n");
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
		    do{
		    }while(!scanf("%c"),&move);
		}
		if((move!='z')&&(move!='d')&&(move!='s')&&(move!='q')&&(move!='g')&&(move!='h')) {
			printf("L'action saisi est incorrect,veuillez recommencez\n");
		}
	} while((move!='z')&&(move!='d')&&(move!='s')&&(move!='q')&&(move!='g')&&(move!='h'));
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	switch(move) {
	default:
		break;
	//deplacement
	case 'z':
		move_player(grid,size_x,size_y,UP);
		//printf("moved up\n");
		break;
	case 'd':
		move_player(grid,size_x,size_y,RIGHT);
		//printf("moved right\n");
		break;
	case 's':
		move_player(grid,size_x,size_y,DOWN);
		//printf("moved down\n");
		break;
	case 'q':
		move_player(grid,size_x,size_y,LEFT);
		//printf("moved left\n");
		break;
	
	case 'g': //faire un action
		try_do_action(grid,size_x,size_y,player,plateau_tab ,taille ,profit ,hummeur_tab ,game_over);
		break;
	case 'h': //retourner au menu principal
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
            printf("🟩");//propre
        }
        else{
            printf("🟫");//usée
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
        if(player.tool.used==0 && player.tool.clean==1){
            printf(" 🟩");//propre
        }
        else if(player.tool.used==0 && player.tool.clean==0){
            printf(" 🟥");//sale
        }
        else{
            printf(" 🟫");//usée
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
//-----------------------------------------------------------
_tile** make_grid_from_string(char string[] ,int max_size_x ,int max_size_y){ //à partir d'une d'une chaine de caractère ,on construit la grille de jeu
    _tile** new_grid = NULL;
    new_grid = cree_grid(max_size_x ,max_size_y);
    int string_ind = 0;
    char c ;
    char c2[2];
    c2[1] = '\0';
    for(int dy=0;dy<max_size_y;dy++){
        for(int dx=0 ;dx<1+max_size_x ;dx++){
            
            c = string[string_ind];
            if(c=='_'){
                dx = 0;
                dy ++;
                string_ind++;
            }
            //printf("c\n");
            c = string[string_ind];
            //printf("%c\n",c);
            if(string_ind < max_size_y*(1+max_size_x)){
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
                
                
                
            }
            string_ind++;
            
    }
    
    return new_grid;
}
}
//-----------------------------------------------------------
_plateau* get_plateau_tab(_tile** grid,int size_x,int size_y,int* taille){
    //compter le nombre de plateau present dans la grille
    _coord plateau_pos;
    *taille = 0;
    for(int id='t';id<='z';id++){
        plateau_pos = get_element_pos_from_grid(grid ,size_x ,size_y ,id);
        if(inter_check(plateau_pos.x,0,size_x)&&inter_check(plateau_pos.y,0,size_y)){
            (*taille)++;    
        }
    }
    //allocation de la liste des plateau
    _plateau* plateau_tab = NULL;
    plateau_tab = malloc((*taille)*sizeof(_plateau));
    exit_if_null_pointer(plateau_tab);
    //creation des plateaux du tableau de plateau
    int index = 0;
    for(int id='t';id<='z';id++){
        plateau_pos = get_element_pos_from_grid(grid ,size_x ,size_y ,id);
        if(inter_check(plateau_pos.x,0,size_x)&&inter_check(plateau_pos.y,0,size_y)){
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
        //printf("percentage = %f    ,plateau.patient->hummeur = %d       ,max_happiness = %d \n",percentage,plateau.patient->hummeur,max_happiness);                                                           
        int nb_box = percentage*happy_bar_len;
        //printf("nb_box = %d    ,percentage = %f       ,happy_bar_len = %d \n",nb_box,percentage,happy_bar_len);                                                         
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
        printf("    %.2f %% ",percentage*100);
        ///🟥🟧🟧🟨🟨🟨🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩⬛⬛
    }
    printf("\n");
    reset_color();
    
}    
//-----------------------------------------------------------
void print_plateau_tab(_plateau* plateau_tab ,int taille ,int max_happiness ,int happy_bar_len){//affiche l'information de tous les plateaux 
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
}  //remplir un tableau d'outils nécessaire à partir des paramètres  a b c d e f g
//-----------------------------------------------------------
_patient* cree_patient(){ //cree un patient avec des paramètre par defaut
    _patient* patient = NULL;
    patient = malloc(sizeof(_patient));
    exit_if_null_pointer(patient);
    
    patient->hummeur = 100;
    patient->etat = satisfait;
    
    int desease_index = randint(1,NB_MALADIE);//choisir une maladie aleratoire
    switch(desease_index){//avoir une maladie (les outils necessaire ,type de maladie ,le profit pour soigner ce maladie)
        //parametre de chaque maladie
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
}  //les profits ,outils necéssaire pour soigner sont paramétrés ici
//-----------------------------------------------------------
int get_a_patient(_plateau* plateau_tab ,int taille ,int initial_hapiness){//essaye d'avoir un patient si il y a un plateau libre
    exit_if_null_pointer(plateau_tab);
    int more_patient = 0;
    //cherche si un plateau est libre
    for(int i=0;i<taille;i++){
        if(plateau_tab[randint(0,taille-1)].patient == NULL){
            more_patient = 1;
            break;
        }    
    }
    if(more_patient){//si il y a un plateau libre
        int ind = 0; 
        do{
            ind = randint(0,taille-1);
        }while(plateau_tab[ind].patient != NULL);//continue si le plateau choisi est deja occupé par un autre patient
        printf("Un patient s'est installé sur un plateau\n");
        plateau_tab[ind].patient = cree_patient();
        plateau_tab[ind].patient->hummeur = initial_hapiness; 
        
        //vérifie si le plateau n'a pas d'outils sale sinon le patient ne va pas payer
        for(int j=0;j<NB_TOOLS;j++){
            if(plateau_tab[ind].used_tools[j] == 1){
                printf("le patient ne va pas payer car le plateau n'est pas totalement propre\n");
                plateau_tab[ind].patient->maladie.profit = 0;//le patient ne va pas payer en raison que le plateau n'est pas totalement propre ,de plus il est mécontent
                plateau_tab[ind].patient->etat = mecontant;
                break;
            }
        } 
        return 1;//si il y a un plteau libre
    }
    return 0;//si il y a aucun plateau libre
}
//-----------------------------------------------------------
void update_patients_hapiness(_plateau* plateau_tab ,int taille ,int* all_happy ,int* full ,float* profit ,int* hummeur_tab ,int max_happiness){//mettre à jour la patient des patients
    int full_var = 1;
    for(int i=0;i<taille;i++){
        if(plateau_tab[i].patient != NULL){
            plateau_tab[i].patient->hummeur--;//diminu la patience
            
            if (plateau_tab[i].patient->hummeur <= 0.45*max_happiness){//si la patience dépasse un seuil le patient devient mecontant
                plateau_tab[i].patient->etat = mecontant;    
            }
            
            
            if( (plateau_tab[i].patient->hummeur)<=0  ){ //si il ne plus de patience le patience part furieux
                printf("Un patient est parti furieux par manque de patience ,il a rien donné\n"); 
                hummeur_tab[2]++;
                //les outils present deviennent sales (usées)
                for(int ind=0;ind<NB_TOOLS;ind++){
                    plateau_tab[i].used_tools[ind] += plateau_tab[i].tools[ind];
                    plateau_tab[i].tools[ind] = 0;
                }
                
                 free(plateau_tab[i].patient);
                plateau_tab[i].patient = NULL;
                *all_happy = 0; //il y a au moins un patient furieux

            }
        }
        else{
            full_var = 0;
        }
    }
    *full = full_var; //remplie? (si il ne reste plus de plateau libre)
}

//-----------------------------------------------------------
//pour l'arrivée des patients
//si un patient n'a pas de place il attendera avant d'entrer dans la salle(1 patient en attentes au maximum)
void patients_spawning_regulation(_plateau* plateau_tab ,int taille ,int min_spawn_time ,int spawn_time_range ,int* current_patient_spawning_time ,int initial_hapiness){
    if(     (*current_patient_spawning_time) <= 0   ){ //si le temps restant avant le prochain patient est nulle -> essaye d'avoir un nouveau patient 
		if(get_a_patient(plateau_tab ,taille ,initial_hapiness)){
		    (*current_patient_spawning_time) = min_spawn_time + randint(0 ,spawn_time_range); //mettre à jour le temps avant le prochain patient restant
        }
	}
	else{
	    (*current_patient_spawning_time)--; //diminuer le temps avant le prochain patient restant
	}
}
//-----------------------------------------------------------
int update_patients(_plateau* plateau_tab ,int taille ,int min_spawn_time ,int spawn_time_range ,int* current_patient_spawning_time ,int initial_hapiness ,float* profit ,int* hummeur_tab){//mettre à jour les patients
    int full = 0;
    int all_happy = 1;
    
    patients_spawning_regulation(plateau_tab ,taille ,min_spawn_time ,spawn_time_range ,current_patient_spawning_time ,initial_hapiness);
    update_patients_hapiness(plateau_tab ,taille ,&all_happy ,&full ,profit ,hummeur_tab ,min_spawn_time+spawn_time_range);
    printf("next patient in %d step(s)\n",*current_patient_spawning_time);
    if(full&(!all_happy)){ //si pas de plateau libre et il y a au moins un patient furieux => parti de jeu terminé
        //game Over
        return 0;
    }
    return 1;
}
//-----------------------------------------------------------
void decrease_if_to_much(int* var ,int min){//(fonction pas utile)  diminuer si la valeur de la variable est superieur à un seuil
    if(*var > min){
        *var--;
    }    
}
//-----------------------------------------------------------
void get_grid_size_from_string(char map_string[] ,int* size_x ,int* size_y){    //avoir la taille de la grille à partir de sa chaine de caractere représentative
    int temp_size_x = 0;
    int new_size_x = 0;
    
    int new_size_y = 0;
    //for(int i=0;i<taille;i++){
    int i = 0;
    while(map_string[i] != '@'){ //tant que le caractere actuelle n'est pas le caractere de fin
        if(map_string[i] == '_'){//si le caractere est un caractere de changement de ligne (on passe à la prochaine ligne)
            if((temp_size_x > new_size_x)&&(new_size_y==0)){
                new_size_x = temp_size_x;
            }
            else if((temp_size_x != new_size_x)&&(new_size_y!=0)){//si le nombre de caractere entre chaque caractere de changement de ligne n'est pas tous la même que la premiere intervalle  (  10010ahbh_djzdj_..._@     1er intervalle = 9,2eme intervalle = 5 => 1er intervalle != 2eme intervalle => format de la chaine de caractère est incorrect               )
                printf("le string map a une forme irregulier x:%d!=%d && y=%d\n",temp_size_x,new_size_x,new_size_y);
                exit(0);
            }
            new_size_y ++;//compter les lignes
            temp_size_x=0;
        }
        else{
            temp_size_x++;//compter les colones
        }
        i++;
    }
    //noter la taille
    *size_x = new_size_x;
    *size_y = new_size_y;
}
//-----------------------------------------------------------
char* get_random_map_string(){// avoir la chaine de caractère de la map au choix de manière aleratoire
    //allocation de la chaine de caractère
    char* map_string = NULL;
    map_string = malloc(MAX_map_string*sizeof(char));
    exit_if_null_pointer(map_string);
    //choix aleratoire
    switch(randint(0,4)){
        //avoir la chaine de caractère
        default:
            map_string = "001111111000_001BCADE1000_001222221100_111bcade0111_1F2f0000t4T3_1G2g00000111_1H2h0P00u4U3_111000000111_3W4wi0j0v4V3_1111I1J11111_000111110000_@";
            return map_string;
        case 1:
            map_string = "11111111111331111111_10000000100000030001_10000011100111111101_100P00jJ100000u4U001_100000iI100000v4V001_11100111100000y4Y101_10000000000000w4W001_10abcde0fgh000x4X001_11ABCDE1FGH100z4Z001_11111111111111111111_@";
            return map_string;
        case 2:
            map_string = "010010010010010010010000000000000000000000_111111111111111111111111110000000111111111_3W4w000iii0jjj000001T1U1V10000000100100iI1_1110000iIi0jJj0000014141411111000100000iI1_3X4x000iii0jjj000000t0u0v00001000100111101_111000011111110000abcde0001001HHH100300001_3Y4y00000000000001ABCDE1001000hhh000100001_111001011111110000abcde0001001HHH100111111_11111100zzz000000000000000000111110010P001_00000100zZz0011111111111111111000100000001_000001111111110000000000000000000111111111_@";
            return map_string;
        case 3:
            map_string = "111111I11111_101U4ui00a2A_103U4u0P0b2B_101U4uhhhc2C_1011JjhHhd2D_101T4thhhe2E_103T4t000f2F_101T4ti00g2G_131111I11111_@";
            return map_string;
        case 4:
            map_string = "0111AB1131CD111111EF11111333300_0100ab0000cd000000ef00000300300_0300000000000000000000000333300_0300001111110001111110000000100_01ii00tttuuu000vvvwww00ii100100_01Ii00tTtuUu0P0vVvwWw00iI100100_01ii00tttuuu000vvvwww00ii100100_1111100000000000000000000000100_1JJJ1000000hhhhh00000ggg0000100_1jjj0000001HHHHH10000gGg01001PP_11111111111111111111111111331PP_@";
            return map_string;
        
    }
}
//-----------------------------------------------------------
_jeu creer_jeu(){  //creation de la variable jeu contenant les informations sur la partie de jeu
    _jeu new_jeu;
    new_jeu.play = 0;
     //initialisation du joueur
	new_jeu.player.tool.type = 0;
	new_jeu.player.tool.clean = 0;
	new_jeu.player.tool.used = 0;
	new_jeu.player.glove.type = 0;
	new_jeu.player.glove.clean = 0;
	new_jeu.player.glove.used = 0;
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
    //----------
    new_jeu.nb_step = -1;
    //----------
    
    return new_jeu;
}
// la page menu
//-----------------------------------------------------------
int play_a_game(_jeu* game ,char* username){
	if(game->play==1){
		game->next_patient_time++;
	}
	else{
    	game->play = 1;
    }
    
    int playing = 1;//est ce que le joueur veut continuer la partie 
    int game_over = 0;
    while(playing){
        game->nb_step++;
        //fin de jeu? + mettre à jour les patients
        if(game_over || !update_patients(game->plateau_tab ,game->nb_plateau ,game->patient_minimum_spawn_intervalle ,game->patient_spawn_range ,&(game->next_patient_time) ,game->patient_spawning_hapiness+randint(0,game->patient_hapiness_range) ,&(game->profit) ,game->hummeur_tab)){
            color(250,30,30);//game over
            printf("\n\n");
            printf("------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
            printf("        GAME OVER!! Le jeu a duré pour %d pas avec: un profit de %.2f$ ,%d patient(s) satisfait(s) ,%d patient(s) mécontent(s) ,%d patient(s) furieux\n\n ",game->nb_step ,game->profit ,game->hummeur_tab[0] ,game->hummeur_tab[1] ,game->hummeur_tab[2]);                                                                  
            printf("------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
            reset_color();
            sleep(2);
            /*
            //vider le scanf() 
            
            char c = 'a';
            int error = 0;
            do{
                error = scanf("%c",&c);
                printf("%d\n",error);
		    }while(!error && c != '\n');
		    */
		    //ajouter le score au ficher
		    FILE* score_file = NULL; 
		    score_file = fopen("scoreboard.txt","a+");
		    exit_if_null_pointer(score_file);
		    fprintf(score_file,"%s %d %.2f %d %d %d\n",username ,game->nb_step ,game->profit ,game->hummeur_tab[0] ,game->hummeur_tab[1] ,game->hummeur_tab[2]);
		    fclose(score_file);
		    
		    
		    
		    
            return 0;//la partie est terminer 
        }
        //affichage du jeu
    	print_grid(game->grid,game->grid_size_x,game->grid_size_y,game->plateau_tab,game->nb_plateau,game->player);
    	print_plateau_tab(game->plateau_tab ,game->nb_plateau ,game->patient_spawning_hapiness+(game->patient_hapiness_range) ,game->happy_bar_len);
    	print_player_status(game->player ,game->profit);
    	print_total_patient(game->hummeur_tab);
    	//demande d'action
    	ask_to_do_player_action(game->grid,game->grid_size_x,game->grid_size_y,&(game->player),game->plateau_tab,game->nb_plateau,&(game->profit) ,game->hummeur_tab ,&playing ,&game_over);
    }
    return 1;//la partie n'est pas terminer et peut etre continuer
} //return 0 <=> partie terminer    , return 1 <=> partie non terminer mais il est mis en pause 
//-----------------------------------------------------------
void score_swap(_score* a,_score* b){//echanger la valeur entre deux variable
    _score temp = *a;
    *a = *b;
    *b = temp;
}
//-----------------------------------------------------------
void _score_bubble_sort(_score* tab ,int taille){//ranger un tableau en ordre décroissant
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
//-----------------------------------------------------------(temporaire)
int* cree_tab(int size){//cree un tableau 
    //(temporaire)
    int* tab =NULL;
    tab = malloc(size*sizeof(int));
    exit_if_null_pointer(tab);
    for(int i=0;i<size;i++){
        tab[i] = randint(100,10000);  //initialisation des valeurs du tableau cree 
    }
    
    return tab;
}
//-----------------------------------------------------------
void print_scoreboard(){ //affichage des scores dont les meilleurs sont vers les 1er case du tableau (ordre decroissant)

	//avoir le ficher score
	FILE* score_file = NULL; 
	score_file = fopen("scoreboard.txt","r");
	exit_if_null_pointer(score_file);

	//compter le nombre de ligne
	char var_ligne[200] = "";
	int nb_ligne = 0;
	void* p_error = NULL;
	do{
		p_error = fgets(var_ligne ,200 ,score_file);
		if(p_error != NULL){
			nb_ligne++;
		}
	}while(p_error != NULL);
	printf("ligne : %d \n",nb_ligne);
	rewind(score_file);
	//allocation
	
	_score* score_tab = NULL;
	score_tab = malloc(nb_ligne*sizeof(_score));
	exit_if_null_pointer(score_tab);
	
	for(int i=0;i<nb_ligne;i++){
		fscanf(score_file ,"%s %d %f %d %d %d\n",score_tab[i].username,&(score_tab[i].nb_step) ,&(score_tab[i].profit) ,&(score_tab[i].hummeur_tab[0]) ,&(score_tab[i].hummeur_tab[1]) ,&(score_tab[i].hummeur_tab[2])	);
	}
	fclose(score_file);

    _score_bubble_sort(score_tab ,nb_ligne);
    
    //affichage des scores
    
    printf("-----------------------------------------\n");
    printf("--------------scoreboard-----------------\n");
    for(int i=0;i<nb_ligne;i++){
    	color(175,120,50);
    	if(i>=10){
    		reset_color();
    		break;
    	}
        if(i+1<10){
            printf("    |  0%d  |",i+1);// name:%s time(in step):%d score:%.2f satisfait:%d mecontant:%d furieux:%d		\n",i,score_tab[i].username ,score_tab[i].nb_step ,score_tab[i].profit ,score_tab[i].hummeur_tab[0] ,score_tab[i].hummeur_tab[1] ,score_tab[i].hummeur_tab[2]					);  
        }
        else{
            printf("    |  %d  |",i+1);// time(in step):%d score:%.2f satisfait:%d mecontant:%d furieux:%d		\n",i, ,score_tab[i].nb_step ,score_tab[i].profit ,score_tab[i].hummeur_tab[0] ,score_tab[i].hummeur_tab[1] ,score_tab[i].hummeur_tab[2]					);  
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
        printf("mécontent:%d ",score_tab[i].hummeur_tab[1]);
        color(200,75,75);
        printf("furieux:%d ",score_tab[i].hummeur_tab[2]);
        printf("\n");
        reset_color();
        
        
        
    }
    
    printf("\n\nPress and enter anything to return to the main menu\n"); //attendre le joueur pour retourner au menu principal
    char c;
    int error = 0;
    error = scanf(" %c",&c);
    if(!error){
        do{
        }while(!scanf("%c"),&c);
	}
	free(score_tab);
	
}
//-----------------------------------------------------------
_menu ask_menu(_jeu* current_game ,_menu current_menu ,const char* filename) {//selection de menu
	FILE* file = NULL;
	file = fopen(filename,"r");
	exit_if_null_pointer(file);
	int savefile = 0;
	fscanf(file ,"savefile %d\n",&savefile);
	fclose(file);
	
	char menu ;
	int menu_choice_correct = 0;
	int error = 0;
	printf("    Veuillez choisir l'une des options suivantes\n\n");
	if(current_menu == select_menu){
    	if(current_game->play==1){
    	    printf("         |  save current game (press s)\n");
    	    printf("         |  continue  current game(press d)\n");
    	}   
    	
    	if(savefile){
    		printf("         |  load a saved game(press j)\n");
    	}
    	 
    	printf("         |  play a new game  (press f)\n");
        printf("         |  scoreboard   (press g)\n");
	}
	else{
	    printf("         |  go to menu   (press q)\n");
	}
    printf("         |  quit program (press h)\n");
    printf("\n\n\n\n");
	do {
		error = scanf(" %c",&menu);
		// savoir si le choix de menu est correct en fonction du menu actuelle
		menu_choice_correct = ((current_menu == select_menu)&&((	(((menu=='s')||(menu=='d'))&&(current_game->play==1))||((menu=='j')&&(savefile))	)||(menu=='f')||(menu=='g')||(menu=='h'))) || ((current_menu != select_menu)&&((menu=='h')||(menu=='q')));  //savoir si le menu choisi est correct en fonction du menu actuel
		if(!error){
		    printf("input went wrong,flushing scanf()\n");
		    do{
		    }while(!scanf("%c"),&menu);
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
    	case 'j':
    		return load;
        case 'h':
    		return quit;
	}

}
//-----------------------------------------------------------
void start(){ //affichage du menu principal et gere quelle est le menu active
    _menu current_menu = select_menu;
    _jeu current_game;
    
    char* username = NULL;
    
    const char* filename = "save_dat.txt";
    
    
    while(1){
        //Projet Cavity taskforce
        printf("_________________________________________________________________________________________________________________________________________\n");
        printf("_________________________________________________________________________________________________________________________________________\n");
        printf("         ####                                  #####                                                                                     \n");
        printf("         #    ####  #   # ###  ### #   #         #   ####  #### # ## #### #### #### ### ####            made by:-Assia                   \n");
        printf("         #    #  #  #   #  #    #   # #          #   #  #  ##   ##   #    #  # #  # #   ####                    -Catherine               \n");
        printf("         #    ####   # #   #    #    #           #   ####    ## # #  ###  #  # ###  #   #                       -Théo R.                 \n");
        printf("         #### #  #    #   ###   #    #           #   #  #  #### #  # #    #### #  # ### ####                                             \n");
        printf("_________________________________________________________________________________________________________________________________________\n");
        printf("__________________________________________________menu principal_________________________________________________________________________\n");
        
        if(username == NULL){
        	username = malloc(username_SIZE*sizeof(char));
        	exit_if_null_pointer(username);
        	printf("Veuillez saisir le nom de votre joueur\n");
        	scanf(" %49s",username);
        	username[username_SIZE-1] = '\0'; 
        	printf("%s\n",username);
        
        
        }
        
        //free_game(_jeu *jeu)
        //save_game(_jeu *jeu, const char *filename)
        //load_game(_jeu *jeu, const char *filename)
        else{
		    current_menu = ask_menu(&current_game ,current_menu ,filename);
		    switch(current_menu){
		        default:
		    		break;
		        case new_jeu://jouer une parti
		        	current_game = creer_jeu(); //assuré que current_game est bien initialisé avant le free_game
		        	free_game(&current_game);
		            current_game = creer_jeu();
		       case continu:
		            if(!play_a_game(&current_game ,username)){
		                current_game = creer_jeu(); //si le jeu jouer est perdu => crée une nouvelle partie en attente d'être jouer    
		            };
		            break;
		        case load:
		        	current_game = creer_jeu(); //assuré que current_game est bien initialisé avant le free_game
		        	free_game(&current_game);
		        	load_game(&current_game ,filename);
		        	if(!play_a_game(&current_game ,username)){
		                current_game = creer_jeu(); //si le jeu jouer est perdu => crée une nouvelle partie en attente d'être jouer    
		            };
		        	break;
		        case scoreboard:
		            print_scoreboard(); //(pas complet)
		    	    break;
		    	case save://sauvegarder une partie
		    	    //  (rien pour le momment) 
		    	    save_game(current_game ,filename) ;
		    	    //free_game(&current_game) ;     
		    	    break;
		    	case quit://quitter le programme
		    		free_game(&current_game);
                    free(username);
		    	    break;
		    }
		    if(current_menu == quit){
		        break;
		    }
		    current_menu = select_menu;
		}
    }
    
}
//-----------------------------------------------------------
void main() {//fonction main
    srand(time(NULL));
    printf("Running program\n\n\n\n");
    
    start();
   
    printf("program ended\n");
}

/*
11111111111331111111_10000000100000030001_10000011100111111101_100P00jJ100000u4U001_100000iI100000v4V001_11100111100000y4Y101_10000000000000w4W001_10abcde0fgh000x4X001_11ABCDE1FGH100z4Z001_11111111111111111111_@   


11111111111331111111_
10000000100000030001_
10000011100111111101_
100P00jJ100000u4U001_
100000iI100000v4V001_
11100111100000y4Y101_
10000000000000w4W001_
10abcde0fgh000x4X001_
11ABCDE1FGH100z4Z001_
11111111111111111111_@  

001111111000_
001BCADE1000_
001222221100_
111bcade0111_
1F2f0000t4T3_
1G2g00000111_
1H2h0000u4U3_
111000000111_
3W4wi0j0v4V3_
1111I1J11111_
000111110000_@

001111111000_001BCADE1000_001222221100_111bcade0111_1F2f0000t4T3_1G2g00000111_1H2h0P00u4U3_111000000111_3W4wi0j0v4V3_1111I1J11111_000111110000_@

010010010010010010010000000000000000000000_
111111111111111111111111110000000111111111_
3W4w000iii0jjj000001T1U1V10000000100100iI1_
1110000iIi0jJj0000014141411111000100000iI1_
3X4x000iii0jjj000000t0u0v00001000100111101_
111000011111110000abcde0001001HHH100300001_
3Y4y00000000000001ABCDE1001000hhh000100001_
111001011111110000abcde0001001HHH100111111_
11111100zzz000000000000000000111110010P001_
00000100zZz0011111111111111111000100000001_
000001111111110000000000000000000111111111_@

010010010010010010010000000000000000000000_111111111111111111111111110000000111111111_3W4w000iii0jjj000001T1U1V10000000100100iI1_1110000iIi0jJj0000014141411111000100000iI1_3X4x000iii0jjj000000t0u0v00001000100111101_111000011111110000abcde0001001HHH100300001_3Y4y00000000000001ABCDE1001000hhh000100001_111001011111110000abcde0001001HHH100111111_11111100zzz000000000000000000111110010P001_00000100zZz0011111111111111111000100000001_000001111111110000000000000000000111111111_@

0111AB1131CD111111EF11111333300_
0100ab0000cd000000ef00000300300_
0300000000000000000000000333300_
0300001111110001111110000000100_
01ii00tttuuu000vvvwww00ii100100_
01Ii00tTtuUu0P0vVvwWw00iI100100_
01ii00tttuuu000vvvwww00ii100100_
1111100000000000000000000000100_
1JJJ1000000hhhhh00000ggg0000100_
1jjj0000001HHHHH10000gGg01001PP_
11111111111111111111111111331PP_@

0111AB1131CD111111EF11111333300_0100ab0000cd000000ef00000300300_0300000000000000000000000333300_0300001111110001111110000000100_01ii00tttuuu000vvvwww00ii100100_01Ii00tTtuUu0P0vVvwWw00iI100100_01ii00tttuuu000vvvwww00ii100100_1111100000000000000000000000100_1JJJ1000000hhhhh00000ggg0000100_1jjj0000001HHHHH10000gGg01001PP_11111111111111111111111111331PP_@

111111I11111_
101U4ui00a2A_
103U4u0P0b2B_
101U4uhhhc2C_
1011JjhHhd2D_
101T4thhhe2E_
103T4t000f2F_
101T4ti00g2G_
131111I11111_@

111111I11111_101U4ui00a2A_103U4u0P0b2B_101U4uhhhc2C_1011JjhHhd2D_101T4thhhe2E_103T4t000f2F_101T4ti00g2G_131111I11111_@

    -----------
    .player = 1 :😷
    -------
    0 :
    1 :⬛  mur
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
    
    
    
010010010010010010010000000000000000000000_111111111111111111111111110000000111111111_3W4w000iii0jjj000001T1U1V10000000100100iI1_1110000iIi0jJj0000014141411111000100000iI1_3X4x000iii0jjj000000t0u0v00001000100111101_111000011111110000abcde0001001HHH100300001_3Y4y00000000000001ABCDE1001000hhh000100001_111001011111110000abcde0001001HHH100111111_11111100zzz000000000000000000111110010P001_00000100zZz0011111111111111111000100000001_000001111111110000000000000000000111111111_@    
    
    
*/
