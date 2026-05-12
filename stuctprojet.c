#include <stdio.h>
#include <stdbool.h>

typedef struct {
	int value;  //type de case
	int player; //presence de joueur
} _tile;

typedef struct{
    int heure                    ;
    int minute                   ;
    int seconde                  ;
} Temps; 

typedef struct{    
    Dentiste  dentiste           ; // on peut augmenter au bout d'un montant
    Temps     temps_debut_partie ;   
    Patient   patient[7]         ; 
    int       nb_client          ; // depuis le debut de la parti
    float     argent_cabinet     ; // ensemble argent gagne
    int       humeur[3]          ; //nb patient mecontent, content et furieux
} Jeu;

typedef enum{ //choix dif etats 
    VERT, 
    ORANGE, 
    ROUGE, 
}Humeur; 

typedef struct {
	int x;
	int y;
} _coord;

typedef enum{
    Satisfait;
    Mecontent;
    Furieux;
} EtatPatient;


typedef struct{
    _coord position               ; // du dentiste sur plateau
    int    gants                  ; // mis = 1 pas mis = 0
    int    ustensile_en_main      ; // num ustensile
    int    main                   ; // pleine = 1 pas pleine = 0   
    bool   proprete_ustensile     ; // true=propre false=sale 
    bool   proprete_gants         ;        //
} Dentiste;

typedef struct{
    int    plateau[7]              ; // tableau avec num ustensiles
    int    ustensile            ; // 1 ustensile = 1 nombre
    int    pathologie           ; // 1 pathologie = 1 nombre
    float  montant_a_payer      ; // diminue si qualité mauvaise
    Humeur    humeur           ; // jauge
    int    fauteuil             ; //vide ou non
    Temps  heure_arrive         ; // a partir du moment où il est dans le  fauteuil
    EtatPatient etat            ; //mecontent, furieux, heureux
} Patient;
