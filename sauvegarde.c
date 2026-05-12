#include <stdio.h>
#include <stdlib.h>

int sauvegarde_existe(){

    FILE* fichier = NULL;
    
    fichier = fopen("sauvegarde.dat", "rb"); // On essaie d’ouvrir le fichier en lecture car Le fichier doit exister au préalable en binaire + rapide
    
    if(fichier!= NULL){ // verifie si le fichier existe
        fclose(fichier); 
        printf("Partie chargée !\n");
        return 1; // ferme le fichier et retourne vrai
    }
    
    printf("Aucune sauvegarde trouvée.\n");
    return 0; // sinon il n'existe pas
}

void sauvegarderPartieTexte(Jeu jsauv) {
   FILE* fichier = NULL;
    
    fichier = fopen("sauvegarde.dat", "wb");
    
    if (fichier == NULL){
       printf("Ouverture du fichier impossible\n");
       printf("code d'erreur = %d \n", errno );
       printf("Message d'erreur = %s \n", strerror(errno) );
       exit(1);
    }

     fwrite(&jsauv, sizeof(Jeu), 1, fichier);
    fclose(fichier);

    printf("Partie sauvegardée avec succès !\n");
}

void recuperation_de_sauvegarde(Jeu jsauv) //jsauv est la partie sauvegardé que l'on récupère
{
    FILE* fichier = NULL;
    
    fichier = fopen("sauvegarde.dat", "rb");
    
    if (fichier == NULL){
       printf("Ouverture du fichier impossible\n");
       printf("code d'erreur = %d \n", errno );
       printf("Message d'erreur = %s \n", strerror(errno) );
       exit(1);
    }
    // on veut récuper une partie sauvegardé dans le fichier

    fread(&jsauv, sizeof(Jeu), 1, fichier);
    fclose(fichier);
 return 0;
}


int main(){

Jeu jsauv; 

	if(sauvegarde_existe==1){
                    	
                          recuperation_de_sauvegarde(jsauv); 
                    }
                    else{
                          sauvegarderPartieTexte(jsauv); 
                          }
                          
return 0; 
}
	
