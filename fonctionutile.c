#include "fonctionutile.h"

void color(unsigned char r, unsigned char g, unsigned char b){    //couleur affichage           r ,g ,b apartient à l'intervalle [0,255]
	printf("\x1B[38;2;%d;%d;%dm", r, g, b);                       // red, green ,blue
}

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


void reset_color(){
	printf("\x1B[0m");                                            // revenir a la couleur par defaut
}



                                                                           // pour avoir un nombre alératoire dans l'intervalle [a,b]  
int randint(int a,int b) {                                                // nombre entre intervalle pour pathologie et ustensiles...
	return rand()%(b-a+1) +a;
}



                                                                        // vérifier si a<=nb<=b est vrai
int inter_check(int nb,int a,int b) {                                   // verifier si c dans une bon intervalle (si le deplacement est dans a taille de tableau
	return (a<=nb)&&(nb<=b);
}



void exit_if_null_pointer(void* pointer) {                              // utile pour apres un malloc 
	if(pointer == NULL) {
		printf(">>Got a NULL pointer\n");
		exit(1);
	}
}




void decrease_if_to_much(int* var ,int min){//(fonction pas utile)  diminuer si la valeur de la variable est superieur à un seuil
    if(*var > min){
        *var--;
    }    
}



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
