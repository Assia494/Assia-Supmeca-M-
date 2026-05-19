#include "economy.h"


_jeu fonction_gestion_argent_cabinet(_jeu j){
	float argent_ustensile=0;

	FILE *fichier = NULL;
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
		for(int i=0;i<NB_TOOLS;i++){
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
