#include "structprojet.h"

void color(unsigned char r, unsigned char g, unsigned char b){            //couleur affichage           r ,g ,b apartient à l'intervalle [0,255]
	printf("\x1B[38;2;%d;%d;%dm", r, g, b);                                // red, green ,blue
}



void reset_color(){
	printf("\x1B[0m");                                                     // revenir a la couleur par defaut
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



_tile cree_tile() {                                                       // creer une case de la grille avec des données par défaut                                    
	_tile new_tile;  
	new_tile.value = 0;
	new_tile.player = 0;
	return new_tile;
}
