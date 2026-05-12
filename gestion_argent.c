#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

Jeu fonction_gestion_argent_cabinet(Jeu j) // j variable de type Jeu, d dentiste
{

float argent_ustensile=0; 

    FILE *fichier  = NULL ;
    FILE *fichier2 = NULL ;
        fichier2 = fopen ("pathologie_clien.txt", "w" );
        fichier =fopen("ustensiles.txt","r"); 
    
    if (fichier == NULL){
        printf("Ouverture du fichier impossible\n");
        printf("code d'erreur = %d \n", errno );
        //printf("Message d'erreur = %s \n", strerror(errno) );
        exit(1);
    }
    if (fichier2 == NULL){
        printf("Ouverture du fichier impossible\n");
        printf("code d'erreur = %d \n", errno );
        //printf("Message d'erreur = %s \n", strerror(errno) );
        exit(1);
    }
    
    if (j.dentiste.gants == 0){
        printf("Il faut mettre ses gants!");
    }
    
    else{
        
        
        int number = 0;
        for(int i=0;i<8;i++){
       		fscanf(fichier, "%f %d" , & argent_ustensile ,&number);
       		if(number == j.dentiste.ustensile_en_main){
       			j.argent_cabinet -= argent_ustensile;
       			fprintf(fichier2, "%f", j.argent_cabinet);
       			break;
       		}
        }   
       
    }
    
    fclose(fichier) ;
    fclose(fichier2) ; 
    return j;
}


int main(){
Jeu argent; 
Jeu j;

printf("Entrer valeurs\n"); 
scanf("%f", &j.argent_cabinet); 
printf("main:\n"); 
scanf("%d", &j.dentiste.gants); 

j.dentiste.ustensile_en_main =5; 

argent= fonction_gestion_argent_cabinet( j);
return 0; 
}

