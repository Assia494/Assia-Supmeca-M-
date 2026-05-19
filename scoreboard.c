#include <scoreboard.h>

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
        printf("profit:%d ",score_tab[i].hummeur_tab[0]);
        color(150,150,75);
        printf("profit:%d ",score_tab[i].hummeur_tab[1]);
        color(200,75,75);
        printf("profit:%d ",score_tab[i].hummeur_tab[2]);
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
        printf("profit:%d ",score_tab[i].hummeur_tab[0]);
        color(150,150,75);
        printf("profit:%d ",score_tab[i].hummeur_tab[1]);
        color(200,75,75);
        printf("profit:%d ",score_tab[i].hummeur_tab[2]);
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
