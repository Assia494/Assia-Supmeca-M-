#include "economy.h"
void fonction_gestion_argent_cabinet(float* profit ,int outils_id){
	FILE* file = NULL;
	file = fopen("ustensiles.txt","r");
	exit_if_null_pointer(file);
	int error = 0;
	int number = 0;
	float cost = 0;
	char temp[100];
	for(int i=0;i<NB_TOOLS+1;i++){
		fscanf(file ,"%d %f %s\n",&number ,&cost ,temp);
		if(error){
			*profit -= 1.2;
			printf("il y a au un erreur donc 1.2$ du profit est dépensé pour avoir un outil\n");
			break;
		}
		
		if(outils_id == number){ //convertir ['1','8'] en [1,8]
			*profit -= cost;
			printf("%.2f$ du profit est dépensé pour avoir un outil\n",cost);
			break;
		}
	}
	
	
	
	
}
