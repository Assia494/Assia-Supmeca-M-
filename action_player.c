#include <action_player.h>

char try_do_action(_tile** grid,int size_x,int size_y,_player* player ,_plateau* plateau_tab ,int taille ,float* profit ,int* hummeur_tab){ //pour les actions autre que les déplacements
    _tile current_tile;
    int tile_value;
    
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
        *profit -= 1.2;  //utilisée le profit pour prendre l'outil
        printf("1.2$ du profit est dépensé pour avoir des gants\n");
    }
    if(inter_check(tile_value,'a','g')&&(player->tool.type==0)){  //si la case d'action est la case d'action pour prendre des outils ,de plus si le joueur n'a pas d'outil en main
        //prendre un objet si le joueur n'a pas d'outil
        player->tool.type = tile_value;
        player->tool.clean = (player->glove.type=='h')&&(player->glove.used==0) ;
        player->tool.used = 0 ;
        *profit -= 1.2;  //utilisée le profit pour prendre l'outil
        printf("1.2$ du profit est dépensé pour avoir un outil\n");
        
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
void ask_to_do_player_action(_tile** grid,int size_x,int size_y,_player* player ,_plateau* plateau_tab,int taille ,float* profit ,int* hummeur_tab ,int* playing) {//demande d'action de la part du joueur (déplacament ,action ,retourner vers le menu pricipal)
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
		try_do_action(grid,size_x,size_y,player,plateau_tab ,taille ,profit ,hummeur_tab);
		break;
	case 'h': //retourner au menu principal
		*playing = 0;
		break;
	}
}
