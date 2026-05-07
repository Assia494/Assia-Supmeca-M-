#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include<unistd.h>

//#define GRID_SIZE_X 20
//#define GRID_SIZE_Y 10

#define NB_TOOLS 7  //sans le gant
#define NB_MALADIE 5 //sans le test_desease
#define NB_hummeur 3

//-----------------------------------------------------------
typedef enum {UP=0,RIGHT=1,DOWN=2,LEFT=3} _movement;
//-----------------
typedef enum {TEST_DESEASE=0,DESEASE_A=1,DESEASE_B=2,DESEASE_C=3,DESEASE_D=4,DESEASE_E=5} _maladie_type;
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
    int clean;
    int used;
    int type;
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
    int va_payer;
} _patient;
//-----------------
typedef struct {
    int tools[NB_TOOLS] ;     //{a,b,c,d,e,f,g}
    int used_tools[NB_TOOLS] ;
    _patient* patient;
    int id;   //noms possible des plateaux (t u v w x y z)
} _plateau;

typedef struct {
    //tableau pour stocker le nombre de patient en fonction de leur hummeur quand il sont parti
    int hummeur_tab[NB_hummeur] ; // index 0:satisfait ,index 1:mécontent ,index 2:furieux
    
    //initialisation du joueur
	_player player ;
	float profit ;
	
	//initialisation du lieu de jeu
	int grid_size_x ;
    int grid_size_y ;
	_tile** grid ;
    //char map_string[5000] ;

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
    
    int nb_step ;
    int a;
} _jeu ;
//-----------------------------------------------------------
void color(unsigned char r, unsigned char g, unsigned char b){    //couleur affichage
	printf("\x1B[38;2;%d;%d;%dm", r, g, b);                       // red, green ,blue
}
//-----------------------------------------------------------
void reset_color(){
	printf("\x1B[0m");                                           // revenir a la couleur par defaut
}
//-----------------------------------------------------------
int randint(int a,int b) {                                        // nombre entre intervalle pour pathologie et ustensiles...
	return rand()%(b-a+1) +a;
}
//-----------------------------------------------------------
int inter_check(int nb,int a,int b) {                             //verifier si c dans une bon intervalle (si le deplacement est dans a taille de tableau
	return (a<=nb)&&(nb<=b);
}
//-----------------------------------------------------------
void exit_if_null_pointer(void* pointer) {                       //apres malloc 
	if(pointer == NULL) {
		printf(">>Got a NULL pointer\n");
		exit(1);
	}
}
//-----------------------------------------------------------
_tile cree_tile() {                                              //creer case dans la grille                                    
	_tile new_tile;  
	new_tile.value = 0;
	new_tile.player = 0;
	return new_tile;
}
//-----------------------------------------------------------
_tile** cree_grid(int size_x,int size_y) {                       //tab 2D

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
_coord get_player_pos_from_grid(_tile** grid,int size_x,int size_y) {        //deplacer dans chauque case de la grille si la case player est 1 : retrourner la position -> utile pr en cas de reinitialisation. 
	_coord pos;
	pos.x = -1;
	pos.y = -1;
	exit_if_null_pointer(grid);
	for(int dy=0 ; dy<size_y ; dy++) {                                     //parcours ligne
		for(int dx=0 ; dx<size_x ; dx++) {                              //parcours colonne
			if(grid[dy][dx].player == 1) {
				pos.x = dx;
				pos.y = dy;
				return pos;
			}
		}
	}
	return pos;              
}
//-----------------------------------------------------------
_coord get_element_pos_from_grid(_tile** grid,int size_x,int size_y,int element) {         //deplacer dans chauque case de la grille si la case value est 1 : retrourner la position -> utile pr en cas de reinitialisation. 
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
_tile get_tile_from_pos(_tile** grid,int size_x,int size_y,int x,int y) {      // a partir d une position on veut recup la case
	exit_if_null_pointer(grid);

	if(!inter_check(x,0,size_x) + !inter_check(x,0,size_x)) {                                  
		printf("Got incoherent value  size=(x=%d/y=%d) ,position=(x=%d/y=%d)",size_x,size_y,x,y);         // pr pas que la case choisie ne soit pas negative ou en dehors du tableau
		exit(1);
	}
	return grid[y][x];

}
//-----------------------------------------------------------
int can_move_at_pos(_tile** grid,int size_x,int size_y,int x,int y) {             //position d arriver x et y 
	int tile_value = 0;
	_tile tile;

	tile = get_tile_from_pos(grid,size_x,size_y,x,y) ;                         // a partir du type de case est ce que  le joueur peut sy rendre
	tile_value = tile.value;
	if(inter_check(tile_value,'a','z')){                                      // a z cases actiuons 
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
	if(tile.player == 1) {
	    
		if((player.glove.type =='h')){
		    printf("😷");
		}
		else{
		    printf("😐");    
		}
	}
	else {
	    if( inter_check(tile.value,'T','Z') ){
	        
	        char c;                                               //c->convertit en minuscules T et Z les values
	        c = tile.value -'T' +'t';
	        for(int i=0;i<taille;i++){
	            if(plateau_tab[i].id==c){
    	            if(plateau_tab[i].patient != NULL){
    	                if(plateau_tab[i].patient->va_payer==1){
    	                    printf("🤒");
    	                }
    	                else{
    	                    printf("😥");    
    	                }
    	                
    	                
    	            }
    	            else{
    	                printf("🪑");    
    	            }
    	            break;
	            }
	        }
	        
	    }        
	    else{
    		switch(tile.value) {
    		default:
    		    if( inter_check(tile.value,'t','z') ){
    		        printf(" %c",tile.value);
    		    }
    		    else if( inter_check(tile.value,'a','h')||inter_check(tile.value,'i','j') ){
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
void print_grid(_tile** grid,int size_x,int size_y,_plateau* plateau_tab,int taille,_player player) {         //afficher la grille 
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
    
	if (inter_check(initial_pos.x,0,size_x)&&inter_check(initial_pos.y,0,size_y)) {
		//printf("%d %d %d %d\n",movement==UP,movement==RIGHT,movement==DOWN,movement==LEFT);
		final_pos.x = initial_pos.x +(movement==RIGHT) -(movement==LEFT) ;
		final_pos.y = initial_pos.y -(movement==UP) +(movement==DOWN) ;
		//printf("(%d;%d) -> (%d;%d)\n",initial_pos.x,initial_pos.y,final_pos.x,final_pos.y);
		if (inter_check(final_pos.x,0,size_x)&&inter_check(final_pos.y,0,size_y)) {

			if (can_move_at_pos(grid,size_x,size_y,final_pos.x,final_pos.y)) {
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
int cure_if_got_tools(_plateau* plateau ,float* profit ,int* hummeur_tab){
    _plateau plate;
    plate = *plateau;
    int can_cure = 1;
    
    for(int i=0;i<NB_TOOLS;i++){
        if(plate.tools[i]<plate.patient->maladie.tool_needed[i]){
            can_cure = 0;    
        }
    }
    if(can_cure){
        
        for(int i=0;i<NB_TOOLS;i++){
            (*plateau).tools[i] -= plate.patient->maladie.tool_needed[i];
            (*plateau).used_tools[i] += plate.patient->maladie.tool_needed[i];
        }  
        
        printf("Un patient est parti satisfaisant car il est soigné ,il a donné %.2f$\n ",plateau->patient->maladie.profit);
        hummeur_tab[0] ++;
        *profit += plateau->patient->maladie.profit; 
        free((*plateau).patient);
        (*plateau).patient = NULL;
        
        return 1;
    }
    return 0;
}
//-----------------------------------------------------------
int try_cure_patient(_plateau* plateau ,float* profit ,int* hummeur_tab){//verifie si il y a les outils pour soigner
    if(plateau->patient!=NULL){

        return cure_if_got_tools(plateau,profit,hummeur_tab);
    }
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
    tile_value = current_tile.value;                                                            //case actuelle dentiste
    
    if((tile_value=='h')&&(player->glove.type==0)){
        //prendre des gants si le joueur n'a pas des gants
        player->glove.type = 'h';
        player->glove.clean = 0 ;
        player->glove.used = 0 ;    
    }
    if(inter_check(tile_value,'a','g')&&(player->tool.type==0)){
        //prendre un objet si le joueur n'a pas d'outil
        player->tool.type = tile_value;
        player->tool.clean = (player->glove.type=='h')&&(player->glove.used==0) ;
        player->tool.used = 0 ;
    }
    else if(tile_value=='i'){
        //mettre les outils dans la poubelle de recyclage
        if((player->tool.type!=0)&&(player->tool.used!=1)){
            player->tool.type = 0;    
        }
        //mettre les gants dans la poubelle de recyclage
        else if((player->glove.type=='h')&&(player->tool.used!=1)){
            player->glove.type = 0;     
        }
    }
    else if(tile_value=='j'){
        //mettre les outils dans la poubelle biologique
        if((player->tool.type!=0)&&(player->tool.used==1)){
            player->tool.type = 0;    
        }
        //mettre les gants dans la poubelle biologique
        else if((player->glove.type=='h')&&(player->glove.used==1)){
            player->glove.type = 0;     
        }
    }
    else if(inter_check(tile_value,'t','z')){
        //pour les plateaux
        //on cherche le plateau
        for(int i=0;i<taille;i++){
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
                else if(    ((player->glove.used==1)||(player->glove.type!='h'))&&(plateau_tab[i].patient!=NULL)&&(player->tool.type==0) ){
                    //le patient part
                    free(plateau_tab[i].patient);
                    plateau_tab[i].patient = NULL;
                    printf("Un patient est parti furieux par peur de contamination,\nil a contaminé les outils présents sur son plateau, il n'a rien donné\n");
                    hummeur_tab[2] ++;
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
} 
//-----------------------------------------------------------
void ask_to_do_player_action(_tile** grid,int size_x,int size_y,_player* player ,_plateau* plateau_tab,int taille ,float* profit ,int* hummeur_tab ) {
	char move ;
	int error = 0;
	printf("Veuillez saisir votre action-----\n\n");
	printf("- type de déplacement\n");
	printf(" |  z:vers le haut\n");
	printf(" |  q:vers la droite\n");
	printf(" |  s:vers le bas\n");
	printf(" |  d:vers la gauche\n");
	printf("- action\n");
	printf(" |  g:faire un action\n");
	do {
		error = scanf(" %c",&move);
		if(!error){
		    printf("input went wrong,flushing scanf()\n");
		    do{
		    }while(!scanf("%c"),&move);
		}
		if((move!='z')&&(move!='d')&&(move!='s')&&(move!='q')&&(move!='g')) {
			printf("L'action saisi est incorrect,veuillez recommencez\n");
		}
	} while((move!='z')&&(move!='d')&&(move!='s')&&(move!='q')&&(move!='g'));
	switch(move) {
	default:
		break;
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
	case 'g':
		try_do_action(grid,size_x,size_y,player,plateau_tab ,taille ,profit ,hummeur_tab);
		break;
	}
}
//-----------------------------------------------------------
void print_red_green(int a){
    if(a){
        printf(" 🟩 ");    
    }
    else{
        printf(" 🟥 ");    
    }
}
//-----------------------------------------------------------
void print_player_status(_player player ,float profit){
    color(190,175,30);
    printf("\n----------------Le joueur-------------------\n");
    
    printf("    |current profit : %.2f$ \n",profit);
    
    printf("    |le joueur possède des gants?   :");
    print_red_green(player.glove.type=='h');
    printf("-----\n");
    
    if(player.glove.type=='h'){
        printf("        |des gants usées?       :");
        print_red_green(player.glove.used==1);
        printf("-----\n");
    }
    
    printf("    |le joueur possède des outils?  :");
    print_red_green(player.tool.type != 0);
    printf("-----\n");
    
    if(player.tool.type!= 0 ){
        printf("        |type d'outils?         : ");
        switch(player.tool.type){
            default:
                printf("...");
                break;
            case 'a':
                printf("🪛");
                break;
            case 'b':
                printf("⚙️");
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
        printf("\n");
        printf("        |des outils propres?    :");
        print_red_green(player.tool.clean);
        printf("-----\n");
    
        printf("        |des outils usées?      :");
        print_red_green(player.tool.used);
        printf("-----\n");
    }
    printf("\n");
    reset_color();
}
//-----------------------------------------------------------
//pour www.onlinegdb.com en raison de fscanf
_tile** make_grid_from_string(char string[] ,int max_size_x ,int max_size_y){
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
    //creation des tableaux
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
void print_plateau(_plateau plateau ,int max_happiness ,int happy_bar_len){
    color(190,175,30);
    printf("-------------------------------------plateau %c -------------------------------------------\n",plateau.id);
    //affichage des outils present sur le plateau
    printf("    |tools :                    ");
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
        if(plateau.tools[i]){
            printf(" 🟩");   //si outils present est propre 
        }
        else if(plateau.used_tools[i]){
            printf(" 🟫");  //si outils present est utilisée 
        }
        else{
            printf(" 🟥");  //pas d'outils present est utilisée    
        }
        printf("    ");
    }    
    printf("\n");
    
    if(plateau.patient!=NULL){
        printf("    |tools needed for curring :    ");
        //affiche les outils nécessaires
        for(int i=0;i<NB_TOOLS;i++){
            printf("");
            if(plateau.patient->maladie.tool_needed[i]){
                printf("🔼   ");   //si outils est nécessaire 
            }
            else{
                printf("-    ");   //si outils est non nécessaire
            }
            printf("    ");
        }    
        printf("\n");
    
        float percentage = 1.0*(plateau.patient->hummeur)/max_happiness;
        int nb_box = percentage*happy_bar_len;
        printf("    |hummeur  ");
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
            printf("");
            nb_box--;
        }
        color(255*(1-percentage),255*percentage,90*(1-percentage));
        printf("    %.2f%\n",percentage*100);
        ///🟥🟧🟧🟨🟨🟨🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩⬛⬛
        
        
    }
    reset_color();
    
}    
//-----------------------------------------------------------
void print_plateau_tab(_plateau* plateau_tab ,int taille ,int max_happiness ,int happy_bar_len){
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
    patient->va_payer = 1;
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
}  //les profits ,outils necéssaire pour soigner sont paramétrés ici
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
        plateau_tab[ind].patient = cree_patient();
        plateau_tab[ind].patient->hummeur = initial_hapiness; 
        //verifie si le plateau est propre ou vide  /aucun chose sale
        for(int j=0;j<NB_TOOLS;j++){
            if(plateau_tab[ind].used_tools[j] == 1){
                plateau_tab[ind].patient->va_payer = 0;//le patient ne va pas payer en raison que le plateau n'est pas totalement propre ,de plus il est mécontent
            }
        }
            
            
            
            
        return 1;
    }
    return 0;
}
//-----------------------------------------------------------
void update_patients_hapiness(_plateau* plateau_tab ,int taille ,int* all_happy ,int* full ,float* profit ,int* hummeur_tab){
    int full_var = 1;
    for(int i=0;i<taille;i++){
        if(plateau_tab[i].patient != NULL){
            plateau_tab[i].patient->hummeur--;
            if( (plateau_tab[i].patient->hummeur)<=0  ){
                
                if(plateau_tab[i].patient->va_payer==1){
                    *profit += (plateau_tab[i].patient->maladie.profit)*0.5;
                    printf("Un patient est parti mécontent ,par manque de patience ,il a donné %.2f$\n",(plateau_tab[i].patient->maladie.profit)*0.5); 
                    hummeur_tab[1]++;
                    free(plateau_tab[i].patient);
                    plateau_tab[i].patient = NULL;
                    *all_happy = 0;
                }
                else{
                    printf("Un patient est parti mécontent ,par manque de patience ,il a rien donné car il est déja mécontent",(plateau_tab[i].patient->maladie.profit)*0.5); 
                    hummeur_tab[1]++;
                    free(plateau_tab[i].patient);
                    plateau_tab[i].patient = NULL;
                    *all_happy = 0;    
                }

            }
        }
        else{
            full_var = 0;
        }
    }
    *full = full_var;
}
//-----------------------------------------------------------
//pour l'arrivée des patients
//si un patient n'a pas de place il attendera avant d'entrer dans la salle(1 patient en attentes au maximum)
void patients_spawning_regulation(_plateau* plateau_tab ,int taille ,int min_spawn_time ,int spawn_time_range ,int* current_patient_spawning_time ,int initial_hapiness){
    if(     (*current_patient_spawning_time) <= 0   ){
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
    update_patients_hapiness(plateau_tab ,taille ,&all_happy ,&full ,profit ,hummeur_tab);
    printf("next patient in %d step(s)\n",*current_patient_spawning_time);
    if(full&(!all_happy)){
        //game Over
        return 0;
    }
    return 1;
    
}
//-----------------------------------------------------------
void decrease_if_to_much(int* var ,int min){
    if(*var > min){
        *var--;
    }    
}
//-----------------------------------------------------------
void get_grid_size_from_string(char map_string[] ,int* size_x ,int* size_y){
    int temp_size_x = 0;
    int new_size_x = 0;
    
    int new_size_y = 0;
    //for(int i=0;i<taille;i++){
    int i = 0;
    while(map_string[i] != '@'){
        if(map_string[i] == '_'){
            if((temp_size_x > new_size_x)&&(new_size_y==0)){
                new_size_x = temp_size_x;
            }
            else if((temp_size_x != new_size_x)&&(new_size_y!=0)){
                printf("le string map a une form irregulier x:%d!=%d && y=%d\n",temp_size_x,new_size_x,new_size_y);
                exit(0);
            }
            new_size_y ++;
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
_jeu creer_jeu(){
    _jeu new_jeu;
    new_jeu.a = 9;
     //initialisation du joueur
	new_jeu.player.tool.type = 0;
	new_jeu.player.glove.type = 0;
    new_jeu.profit = 0.00f;
    
    //initialisation de la grille de jeu
	new_jeu.grid_size_x = 0;
    new_jeu.grid_size_y = 0;
	new_jeu.grid = NULL;
    char map_string[5000] = "11111111111331111111_10000000100000030001_10000011100111111101_100P00jJ100000u4U001_100000iI100000v4V001_11100111100000y4Y101_10000000000000w4W001_10abcde0fgh000x4X001_11ABCDE1FGH100z4Z001_11111111111111111111_@";
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
    new_jeu.patient_minimum_spawn_intervalle = 25;
    new_jeu.patient_spawn_range = 20 ;
    new_jeu.patient_spawning_hapiness = 100;
    new_jeu.patient_hapiness_range = 25;
    new_jeu.next_patient_time = new_jeu.patient_minimum_spawn_intervalle + randint(0 ,new_jeu.patient_spawn_range);
    //----------
    new_jeu.nb_step = -1;
    //----------
    
    return new_jeu;
}
//-----------------------------------------------------------
void main() {
    srand(time(NULL));
    printf("Running program\n");
    _jeu game;
    game = creer_jeu();
    
    if(1){
        while(1){
            printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
            game.nb_step++;
            
            if(!update_patients(game.plateau_tab ,game.nb_plateau ,game.patient_minimum_spawn_intervalle ,game.patient_spawn_range ,&(game.next_patient_time) ,game.patient_spawning_hapiness+randint(0,game.patient_hapiness_range) ,&(game.profit) ,game.hummeur_tab)){
                break; //game over
            }
    		print_grid(game.grid,game.grid_size_x,game.grid_size_y,game.plateau_tab,game.nb_plateau,game.player);
    		print_plateau_tab(game.plateau_tab ,game.nb_plateau ,game.patient_spawning_hapiness+game.patient_hapiness_range ,game.happy_bar_len);
    		print_player_status(game.player ,game.profit);
    		printf("gant %d \n",game.player.glove.type);
    		ask_to_do_player_action(game.grid,game.grid_size_x,game.grid_size_y,&(game.player),game.plateau_tab,game.nb_plateau,&(game.profit) ,game.hummeur_tab);
    		//sleep(1);
    	    
        }
        color(250,30,30);
        printf("\n\n        GAME OVER!! Le jeu a duré pour %d pas avec: un profit de %.2f$ ,%d patient(s) satisfait(s) ,%d patient(s) mécontent(s) ,%d patient(s) furieux ",game.nb_step ,game.profit ,game.hummeur_tab[0] ,game.hummeur_tab[1] ,game.hummeur_tab[2]);
        reset_color();
    }
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


