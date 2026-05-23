#include "base.h"

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
		    } while(!scanf("%hhd", &menu));
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
		    	    save_game(&current_game, filename);
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
