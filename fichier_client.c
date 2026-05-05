#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

void gestion_fichier_patient(Patient p)
{
  FILE fichier = NULL;

  fichier = fopen("pathologie_client.txt", "w");

  if (fichier == NULL){
     printf("Ouverture du fichier impossible\n");
     printf("code d'erreur = %d \n", errno );
     printf("Message d'erreur = %s \n", strerror(errno) );
     exit(1);
 }

fprintf(fichier, "%d", &p.pathologie);
fprintf(fichier, "%d", &p.ustensiles);
fprintf(fichier, "%f", &p.montant_a_payer);

fclose(fichier);

}
