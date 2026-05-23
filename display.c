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
