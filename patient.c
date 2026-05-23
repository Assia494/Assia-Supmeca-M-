#include "patient.h"

_patient* cree_patient(){ //cree un patient avec des paramètre par defaut
    _patient* patient = NULL;
    patient = malloc(sizeof(_patient));
    exit_if_null_pointer(patient);
    
    patient->hummeur = 100;
    patient->etat = satisfait;
    
    int desease_index = randint(1,NB_MALADIE);//choisir une maladie aleratoire
    switch(desease_index){//avoir une maladie (les outils necessaire ,type de maladie ,le profit pour soigner ce maladie)
        //parametre de chaque maladie
        default:
            patient->maladie.type = TEST_DESEASE;
            make_tool_tab(patient->maladie.tool_needed,0,1,0,0,1,1,0);
            patient->maladie.profit = 1.23f;
            break;
        case 1:
            patient->maladie.type = DESEASE_A;
            make_tool_tab(patient->maladie.tool_needed,0,1,0,0,1,0,1);
            patient->maladie.profit = 3.20f;
            break;
        case 2:
            patient->maladie.type = DESEASE_B;
            make_tool_tab(patient->maladie.tool_needed,1,0,0,1,1,0,0);
            patient->maladie.profit = 2.90f;
            break;
        case 3:
            patient->maladie.type = DESEASE_C;
            make_tool_tab(patient->maladie.tool_needed,1,0,0,1,0,1,0);
            patient->maladie.profit = 4.14f;
            break;
        case 4:
            patient->maladie.type = DESEASE_D;
            make_tool_tab(patient->maladie.tool_needed,0,0,1,1,0,1,0);
            patient->maladie.profit = 3.49f;
            break;
        case 5:
            patient->maladie.type = DESEASE_E;
            make_tool_tab(patient->maladie.tool_needed,0,1,1,0,0,0,1);
            patient->maladie.profit = 5.65f;
            break;
    }
    
    return patient;
}  //les profits ,outils necéssaire pour soigner sont paramétrés ici
//-----------------------------------------------------------
int get_a_patient(_plateau* plateau_tab ,int taille ,int initial_hapiness){//essaye d'avoir un patient si il y a un plateau libre
    exit_if_null_pointer(plateau_tab);
    int more_patient = 0;
    //cherche si un plateau est libre
    for(int i=0;i<taille;i++){
        if(plateau_tab[randint(0,taille-1)].patient == NULL){
            more_patient = 1;
            break;
        }    
    }
    if(more_patient){//si il y a un plateau libre
        int ind = 0; 
        do{
            ind = randint(0,taille-1);
        }while(plateau_tab[ind].patient != NULL);//continue si le plateau choisi est deja occupé par un autre patient
        printf("Un patient s'est installé sur un plateau\n");
        plateau_tab[ind].patient = cree_patient();
        plateau_tab[ind].patient->hummeur = initial_hapiness; 
        
        //vérifie si le plateau n'a pas d'outils sale sinon le patient ne va pas payer
        for(int j=0;j<NB_TOOLS;j++){
            if(plateau_tab[ind].used_tools[j] == 1){
                printf("le patient ne va pas payer car le plateau n'est pas totalement propre\n");
                plateau_tab[ind].patient->maladie.profit = 0;//le patient ne va pas payer en raison que le plateau n'est pas totalement propre ,de plus il est mécontent
                plateau_tab[ind].patient->etat = mecontant;
                break;
            }
        } 
        return 1;//si il y a un plteau libre
    }
    return 0;//si il y a aucun plateau libre
}
//-----------------------------------------------------------
void update_patients_hapiness(_plateau* plateau_tab ,int taille ,int* all_happy ,int* full ,float* profit ,int* hummeur_tab ,int max_happiness){//mettre à jour la patient des patients
    int full_var = 1;
    for(int i=0;i<taille;i++){
        if(plateau_tab[i].patient != NULL){
            plateau_tab[i].patient->hummeur--;//diminu la patience
            
            if (plateau_tab[i].patient->hummeur <= 0.45*max_happiness){//si la patience dépasse un seuil le patient devient mecontant
                plateau_tab[i].patient->etat = mecontant;    
            }
            
            
            if( (plateau_tab[i].patient->hummeur)<=0  ){ //si il ne plus de patience le patience part furieux
                printf("Un patient est parti furieux par manque de patience ,il a rien donné\n"); 
                hummeur_tab[2]++;
                //les outils present deviennent sales (usées)
                for(int ind=0;ind<NB_TOOLS;ind++){
                    plateau_tab[i].used_tools[ind] += plateau_tab[i].tools[ind];
                    plateau_tab[i].tools[ind] = 0;
                }
                
                 free(plateau_tab[i].patient);
                plateau_tab[i].patient = NULL;
                *all_happy = 0; //il y a au moins un patient furieux

            }
        }
        else{
            full_var = 0;
        }
    }
    *full = full_var; //remplie? (si il ne reste plus de plateau libre)
}

//-----------------------------------------------------------
//pour l'arrivée des patients
//si un patient n'a pas de place il attendera avant d'entrer dans la salle(1 patient en attentes au maximum)
void patients_spawning_regulation(_plateau* plateau_tab ,int taille ,int min_spawn_time ,int spawn_time_range ,int* current_patient_spawning_time ,int initial_hapiness){
    if(     (*current_patient_spawning_time) <= 0   ){ //si le temps restant avant le prochain patient est nulle -> essaye d'avoir un nouveau patient 
		if(get_a_patient(plateau_tab ,taille ,initial_hapiness)){
		    (*current_patient_spawning_time) = min_spawn_time + randint(0 ,spawn_time_range); //mettre à jour le temps avant le prochain patient restant
        }
	}
	else{
	    (*current_patient_spawning_time)--; //diminuer le temps avant le prochain patient restant
	}
}
//-----------------------------------------------------------
int update_patients(_plateau* plateau_tab ,int taille ,int min_spawn_time ,int spawn_time_range ,int* current_patient_spawning_time ,int initial_hapiness ,float* profit ,int* hummeur_tab){//mettre à jour les patients
    int full = 0;
    int all_happy = 1;
    
    patients_spawning_regulation(plateau_tab ,taille ,min_spawn_time ,spawn_time_range ,current_patient_spawning_time ,initial_hapiness);
    update_patients_hapiness(plateau_tab ,taille ,&all_happy ,&full ,profit ,hummeur_tab ,min_spawn_time+spawn_time_range);
    printf("next patient in %d step(s)\n",*current_patient_spawning_time);
    if(full&(!all_happy)){ //si pas de plateau libre et il y a au moins un patient furieux => parti de jeu terminé
        //game Over
        return 0;
    }
    return 1;
}
//si un patient n'a pas de place il attendera avant d'entrer dans la salle(1 patient en attentes au maximum)






