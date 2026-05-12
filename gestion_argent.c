#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

Jeu fonction_gestion_argent_cabinet(Jeu j, Dentiste d) // j variable de type Jeu, d dentiste
{
    FILE* fichier  = NULL ;
    FILE* fichier2 = NULL ;
    
    fichier  = fopen ("ustensiles.txt", "r");
    fichier2 = fopen ("pathologie_client.txt", "w" );
    
    if (fichier == NULL){
        printf("Ouverture du fichier impossible\n");
        printf("code d'erreur = %d \n", errno );
        printf("Message d'erreur = %s \n", strerror(errno) );
        exit(1);
    }
    if (fichier2 == NULL){
        printf("Ouverture du fichier impossible\n");
        printf("code d'erreur = %d \n", errno );
        printf("Message d'erreur = %s \n", strerror(errno) );
        exit(1);
    }
    
    if (d.main == 0){
        printf("Il faut mettre ses gants!");
    }
    
    else{
        
        switch(d.ustensiles){
        
        case 1:
            fscanf(fichier, "%f" , &argent_ustensile);
            j.argent_cabinet = j.argent_cabinet - argent_ustensile;
            fprintf(fichier2, "%f", j.argent_cabinet);
        break;
        
        case 2:
            fscanf(fichier, "%f" , &argent_ustensile);
            j.argent_cabinet = j.argent_cabinet - argent_ustensile;
            fprintf(fichier2, "%f", j.argent_cabinet);
        break;
        
        case 3:
            fscanf(fichier, "%f" , &argent_ustensile);
            j.argent_cabinet = j.argent_cabinet - argent_ustensile;
            fprintf(fichier2, "%f", j.argent_cabinet);
        break;
        
        case 4:
            fscanf(fichier, "%f" , &argent_ustensile);
            j.argent_cabinet = j.argent_cabinet - argent_ustensile;
            fprintf(fichier2, "%f", j.argent_cabinet);
        break;
        
        case 5:
            fscanf(fichier, "%f" , &argent_ustensile);
            j.argent_cabinet = j.argent_cabinet - argent_ustensile;
            fprintf(fichier2, "%f", j.argent_cabinet);
        break;
        
        case 6:
            fscanf(fichier, "%f" , &argent_ustensile);
            j.argent_cabinet = j.argent_cabinet - argent_ustensile;
            fprintf(fichier2, "%f", j.argent_cabinet);
        break;
        
        case 7:
            fscanf(fichier, "%f" , &argent_ustensile);
            j.argent_cabinet = j.argent_cabinet - argent_ustensile;
            fprintf(fichier2, "%f", j.argent_cabinet);
        break;
        
        case 8:
            fscanf(fichier, "%f" , &argent_ustensile);
            j.argent_cabinet = j.argent_cabinet - argent_ustensile;
            fprintf(fichier2, "%f", j.argent_cabinet);
        break;
        
        default:
        printf("Problème rencontrez !");
        break;
    }
    
    fclose(fichier) ;
    fclose(fichier2) ; 
    return j;
}
