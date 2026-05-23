#include "plateau.h"

_plateau cree_plateau(int new_id){ //crée un variable plateau
    _plateau new_plateau;
    for(int i=0;i<NB_TOOLS;i++){
        new_plateau.tools[i] = 0;       //pas d'outils propre
        new_plateau.used_tools[i] = 0;  //pas d'outils usée
    }
    
    new_plateau.patient = NULL;     //pas de patient <=> plateau.patient == NULL
    new_plateau.id = new_id;        // le nom du plateau qui est dans l'intervalle [t,z]
    return new_plateau;
}

int cure_if_got_tools(_plateau* plateau ,float* profit ,int* hummeur_tab){//soigner si les outils propre necessaire sont tous present sur le plateau
    _plateau plate;
    plate = *plateau;
    int can_cure = 1;
    
    for(int i=0;i<NB_TOOLS;i++){ //vérifie si les outils propre nécessaire sont présent 
        if(plate.tools[i]<plate.patient->maladie.tool_needed[i]){
            can_cure = 0;
            break;
        }
    }
    if(can_cure){//si oigné le patient est possible

        for(int i=0;i<NB_TOOLS;i++){
            //les outils nécessaire devient usée
            (*plateau).tools[i] -= plate.patient->maladie.tool_needed[i];
            (*plateau).used_tools[i] += plate.patient->maladie.tool_needed[i];
        } 
        
        //si un patient satisfaisant est soigné
        if(plate.patient->etat == satisfait){
            printf("Un patient est parti satisfait car il a été soigné, il a donné %.2f$\n",plateau->patient->maladie.profit);
            hummeur_tab[0]++;
            *profit += plateau->patient->maladie.profit;
        }
        //si un patient mécontant est soigné
        else {
            if(plateau->patient->maladie.profit != 0){//si il doit payé plus que 0$
                printf("Un patient est parti mécontent car il a été soigné et il a attendue longtemps, il a donné %.2f$\n",0.5*(plateau->patient->maladie.profit));
                hummeur_tab[1]++;
                *profit += 0.5*(plateau->patient->maladie.profit);
            }
            else{//il n'a pas besoin de payé <=> le plateau est sale lors de son arrivé
                printf("Un patient est parti mécontent car il a été soigné et le plateau a été sale lors de son arrivé, il a donné 0.0$\n");
                hummeur_tab[1]++;
            }
        }
        // le patient part
        free((*plateau).patient);
        (*plateau).patient = NULL;
        
        return 1;
    }
    return 0;
}




int try_cure_patient(_plateau* plateau ,float* profit ,int* hummeur_tab){    //(pour soigner) verifie si il y a un patient ,si oui on essaye de le soigner
    if(plateau->patient!=NULL){

        return cure_if_got_tools(plateau,profit,hummeur_tab);
    }
}



_plateau* get_plateau_tab(_tile** grid,int size_x,int size_y,int* taille){
    //compter le nombre de plateau present dans la grille
    _coord plateau_pos;
    *taille = 0;
    for(int id='t';id<='z';id++){
        plateau_pos = get_element_pos_from_grid(grid ,size_x ,size_y ,id);
        if(inter_check(plateau_pos.x,0,size_x)&&inter_check(plateau_pos.y,0,size_y)){
            (*taille)++;    
        }
    }
    //allocation de la liste des plateau
    _plateau* plateau_tab = NULL;
    plateau_tab = malloc((*taille)*sizeof(_plateau));
    exit_if_null_pointer(plateau_tab);
    //creation des plateaux du tableau de plateau
    int index = 0;
    for(int id='t';id<='z';id++){
        plateau_pos = get_element_pos_from_grid(grid ,size_x ,size_y ,id);
        if(inter_check(plateau_pos.x,0,size_x)&&inter_check(plateau_pos.y,0,size_y)){
            plateau_tab[index] = cree_plateau(id);
            index++;
        }
    }
    return plateau_tab;
    
}
