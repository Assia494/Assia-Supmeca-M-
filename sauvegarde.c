#include <stdio.h>
#include <stdlib.h>

int sauvegarde_existe(){
}
    FILE fichier = NULL;
    
    fichier = fopen("sauvegarde.txt", "r"); // On essaie d’ouvrir le fichier en lecture car Le fichier doit exister au préalable
    
    if(fichier!= NULL){ // verifie si le fichier existe
        fclose(fichier); 
        printf("Partie chargée !\n");
        return 1; // ferme le fichier et retourne vrai
    }
    
    printf("Aucune sauvegarde trouvée.\n");
    return 0; // sinon il n'existe pas
}

void sauvegarderPartieTexte(Jeu *jsauv) {
   FILE fichier = NULL;
    
    fichier = fopen("sauvegarde.txt", "w");
    
    if (fichier == NULL){
       printf("Ouverture du fichier impossible\n");
       printf("code d'erreur = %d \n", errno );
       printf("Message d'erreur = %s \n", strerror(errno) );
       exit(1);
    }

    // Écriture ligne par ligne
    
    // affiche les dentistes
    fputs  ("Le dentiste\n", fichier                              );
    fprintf(fichier, "%d\n", &jsauv->dentiste->position.x         );
    fprintf(fichier, "%d\n", &jsauv->dentiste->position.y         );
    fprintf(fichier, "%d\n", &jsauv->dentiste.gants               );
    fprintf(fichier, "%d\n", &jsauv->patient.ustensile_en_main    );
    fprintf(fichier, "%d\n", &jsauv->patient.main                 );
    fprintf(fichier, "%d\n", &jsauv->patient.proprete_ustensile   );
    
    // affiche les patients déjà présent
    fputs  ("Le patient\n", fichier                               );
    fprintf(fichier, "%d\n", &jsauv->patient->plateau             );
    fprintf(fichier, "%c\n", &jsauv->patient.pathologie           );
    fprintf(fichier, "%f\n", &jsauv->patient.montant_a_payer      );
    fprintf(fichier, "%d\n", &jsauv->patient.humeur               );
    // affiche heure_arrive
    fputs  ("Heure arrivé du patient\n", fichier                  );
    fprintf(fichier, "%d\n", &jsauv->patient.heure_arrive.heure   );
    fprintf(fichier, "%d\n", &jsauv->patient.heure_arrive.minute  );
    fprintf(fichier, "%d\n", &jsauv->patient.heure_arrive.seconde );
    
    // affiche temps de le debut de la partie
    fputs  ("Temps de la partie\n", fichier                       );
    fprintf(fichier, "%d\n", &jsauv->temps_debut_partie.heure     );
    fprintf(fichier, "%d\n", &jsauv->temps_debut_partie.minute    );
    fprintf(fichier, "%d\n", &jsauv->temps_debut_partie.seconde   );
    
    // affiche argent_cabinet et nb de client partie et leur humeur
    fputs  ("Argent cabinet\n", fichier                           );
    fprintf(fichier, "%f\n", &jsauv->argent_cabinet               );
    fputs  ("Nombre de client partie par catégorie\n", fichier    );
    fprintf(fichier, "%d\n", &jsauv->humeur                       );

    fclose(fichier);

    printf("Partie sauvegardée avec succès !\n");
}

void recuperation_de_sauvegarde(Jeu *jsauv) //jsauv est la partie sauvegardé que l'on récupère
{
    FILE fichier = NULL;
    
    fichier = fopen("sauvegarde.txt", "r");
    
    if (fichier == NULL){
       printf("Ouverture du fichier impossible\n");
       printf("code d'erreur = %d \n", errno );
       printf("Message d'erreur = %s \n", strerror(errno) );
       exit(1);
    }
    // on veut récuper une partie sauvegardé dans le fichier
    
    // recupère les dentistes
    fscanf(fichier, "%d", &jsauv->dentiste->position.x         );
    fscanf(fichier, "%d", &jsauv->dentiste->position.y         );
    fscanf(fichier, "%d", &jsauv->dentiste.gants               );
    fscanf(fichier, "%d", &jsauv->patient.ustensile_en_main    );
    fscanf(fichier, "%d", &jsauv->patient.main                 );
    fscanf(fichier, "%d", &jsauv->patient.proprete_ustensile   );
    
    // recupère les patients déjà présent
    fscanf(fichier, "%d", &jsauv->patient->plateau             );
    fscanf(fichier, "%d", &jsauv->patient.pathologie           );
    fscanf(fichier, "%f", &jsauv->patient.montant_a_payer      );
    fscanf(fichier, "%d", &jsauv->patient.humeur               );
    // recupère heure_arrive
    fscanf(fichier, "%d", &jsauv->patient.heure_arrive.heure   );
    fscanf(fichier, "%d", &jsauv->patient.heure_arrive.minute  );
    fscanf(fichier, "%d", &jsauv->patient.heure_arrive.seconde );
    
    // recupère temps du debut de la partie
    fscanf(fichier, "%d", &jsauv->temps_debut_partie.heure     );
    fscanf(fichier, "%d", &jsauv->temps_debut_partie.minute    );
    fscanf(fichier, "%d", &jsauv->temps_debut_partie.seconde   );
    
    // recupère argent_cabinetnt et nb de client partie et leur humeur
    fscanf(fichier, "%f", &jsauv->argent_cabinet               );
    fscanf(fichier, "%d", &jsauv->humeur                       );
    
    fclose(fichier);
    return 0;
}
