#include "display.h"

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



void print_red_green(int a){//affiche vert pour 1 ou rouge pour 0 (affiche vrai ou faux avec couleur)
    if(a){
        printf(" 🟩 ");    
    }
    else{
        printf(" 🟥 ");    
    }
}



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




void print_total_patient(int* hummeur_tab){//affiche la quantité de patient selon leur etat qui sont partis
    color(190,175,30);
    printf("    |patient score:  |%d 🟩    |%d 🟨    |%d 🟥  \n\n",hummeur_tab[0],hummeur_tab[1],hummeur_tab[2]);
    reset_color();
}



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
