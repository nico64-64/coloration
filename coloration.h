#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <locale.h>
#include <time.h>


#ifndef _COLORATION_H

#define _COLORATION_H


//Version du programme:
#define VERSION "0.2.4"


//Valeurs Macros:
#define NBRE_CAR_MAX_PAR_LIGNE 499



//Structs et enums:

typedef enum _tag _tag;
typedef struct ligne ligne;

typedef enum _tag
//Étiquette identifiant le contenu d'une ligne du fichier ouvert.
{
	IGNORE = -1, //texte ignoré car placé avant le -DÉBUT- ou après la -FIN-
	DEBUT = -2, // -DÉBUT-
	FIN = -3, // -FIN-
	//aucun = 0 (ne devrait pas être assigné à aucune ligne, sauf si elles sont vides)
	_ignore = 1, // * texte *
	_commentaire = 2, // /* texte */
	_pointeur = 3, // PTR_OBJET
	_element = 4, // car: valeur
	_override = 5, // @PTR_OBJET ...
	_element_ovr = 6, // @car: valeur
	_modele = 7, // modèle:
} _tag;

typedef enum _type
//...
{
	_inconnu = -1, //élément de type inconnu (donc il y a une erreur soit dans la syntaxe de l'élément, soit dans le code de ce programme)
	_pas_un_element = 0, //cette ligne n'étant pas un élément, cette enum est inutile
	
	//Éléments nécessitant un POINTEUR:
	_endroit,
	_dest,
	_imp,
	_d_i,
	
	//Éléments nécessitant du texte seul:
	_rum,
	_img,
	_ouv,
	_fer,
	_deb,
	_bar,
	_int,
	
	//Éléments nécessitant du texte seul ou entre guillemets:
	_nom,
	_descr,
	_detail,
	
	//Éléments nécessitant du texte entre guillemets:
	_amb,
	_ptv,
	_a_d,
	_a_a,
	
	//Éléments nécessitant un nombre:
	_poids,
	_cap,
	_vie,
	_conf,
	_deg,
	_lum,
	_faim,
	_etat,
	_dist,
	
	//Syntaxe personalisée:
	_cond,
} _type;

struct ligne
//Structure d'une ligne du fichier ouvert.
{
	ligne* precedent; //ligne précédente
	int num; //numéro de la ligne dans le fichier
	fpos_t pos; //position de la ligne dans le fichier
	char txt[NBRE_CAR_MAX_PAR_LIGNE + 50]; //texte contenu par cette ligne
	_tag tag; //indique ce que contient cette ligne (voir l'enum correspondante)
	_type type; //indique le type d'élément contenu dans cette ligne (s'il y a lieu) (voir l'enum correspondante)
	int multiligne; //indique le nombre de lignes qu'occupe cette ligne dans l'écran
	ligne* suivant; //ligne suivante
};



//Variables Globales:

//Symboles internes:
void* ERREUR; //pointeur signifiant que la fonction a eu une erreur
ligne DEBUT_FICHIER; //pointeur représentant le début du fichier
ligne FIN_FICHIER; //pointeur représentant la fin du fichier

//Gestion des erreurs et débogage:
bool err_log = FALSE; //indique si les erreurs doivent être logguées dans un fichier
char nom_ferreur[] = "erreurs.txt"; //nom du fichier de log d'erreurs (si activé)
bool debogage = FALSE; //indique si le mode débogage est activé
char element_debogue = 'i'; //indique ce qui est débogué (lorsque le mode débogage est activé)
							//Valeurs possibles: i = input, p = position, n = input (raw)

//Gestion du fichier ouvert:
char nom_fichier[100]; //nom du fichier actuellement modifié (buffer très long pour (entre autres) accomodé tout le chemin d'accès au fichier, si nécessaire...)
FILE* fichier = NULL; //pointeur vers le fichier actuellement modifié
ligne* lignes; //liste des lignes contenus dans le fichier

//Position sur l'écran et dans le fichier:
int y = 1; //position en y du curseur dans l'écran
int x = 4; //position en x du curseur dans l'écran
int premiere_ligne = 1; //numéro de la ligne la plus haute à l'écran
int derniere_ligne = 10; //numéro de la ligne la plus basse à l'écran
int pos_y = 1; //position en y dans la "ligne" modifiée par le curseur (soit le numéro de ligne dans cette ligne, pour lorsqu'elle est trop longue)
ligne* ln_mod; //ligne modifiée présentement (ligne où se trouve le curseur)

//Autres:
bool barre_dispo = 1; //indique si un message est présentement affiché dans la barre d'état (0 = occupée, 1 = libre)



//Fonctions Macros:

//Affichage général:
#define mv(position_y, position_x);		move(position_y, position_x); y = position_y; x = position_x; //permet de déplacer le curseur tout en mettant les variables x et y à jour (remplacement de "move")
#define mvaddstrc(position_y, texte);	mvaddstr(position_y, (COLS - strlen(texte)) / 2, texte); //affiche une string centrée (sur une ligne y)

//Affichage personnalisé:
#define init();		for (int _COMPTEUR = 1; init_ligne(_COMPTEUR) != NULL; _COMPTEUR++) {} //initialise toutes les lignes du fichier
#define desinit();	for (int _COMPTEUR = init_ligne(0)->num; _COMPTEUR >= 1; _COMPTEUR--) {init_ligne(-_COMPTEUR);} //libère toute les lignes du fichier

//Gestion des erreurs et débogage:
#define msg_printf(txt, ...);		char _INTERNAL_BUFFER_[400]; sprintf(_INTERNAL_BUFFER_, txt, __VA_ARGS__); print_msg(_INTERNAL_BUFFER_); //Affiche un message en style printf (à éviter...)
#define err_printf(code, txt, ...);	char _INTERNAL_BUFFER[400]; sprintf(_INTERNAL_BUFFER, txt, __VA_ARGS__); erreur(code, _INTERNAL_BUFFER); //Log une erreur en style printf
#define DEBUG(txt, ...);			err_printf(1, txt, __VA_ARGS__); erreur(0, "..."); //macro de débogage (utiliser en style printf)
#define DEBUG_RADICAL(txt, ...);	err_printf(1, txt, __VA_ARGS__); quitter(-1); erreur(0, "..."); exit(-100); //comme le macro de débogage standard, mais quitte à la fin pour être sûr que le message se rend et que ça ne plante pas



//Fonctions:
//Se référer au fichier .c où est implémentée la fonction pour plus de détails.
//coloration.c:
ligne* aller_a (); //envoie l'utilisateur à une certaine ligne du fichier (demandée par la fonction)
void cmd (char commande[]); //exécute une commande (en demande une si commande est NULL)
void enregistrer (char nom_sauvegarde[]); //enregistre le buffer modifié dans un fichier dont le nom est reçu en paramètre (si NULL, un nom sera demandé)
int erreur (int code, char message[]); //log une erreur ou en affiche une (s'il y en a une) si code = 0
int main (int argc, char* argv[]); //contient l'ouverture et l'initialisation du programme ainsi que la main loop
void quitter (int code); //ferme proprement le programme
void term (); //permet à l'utilisateur d'envoyer une commande au terminal (et de consulter son output)
//outils_logiques.c:
int compter_lignes(ligne* ln); //compte le nombre de lignes (à l'écran) qu'occupe une ligne du fichier
ligne* init_ligne (int num); //initialise la structure d'une ligne en la lisant dans le fichier ouvert
int longueur_str (char str[]); //trouve le nombre de caractères d'une string en tenant compte des accents
ligne* trouve_ligne (int num); //renvoie un pointeur vers la ligne correspondant au numéro de ligne reçu
ligne* trouve_tag (_tag tag, ligne* depart); //renvoie la première ligne ayant un certain tag d'assigné depuis la ligne de départ (ou le début du fichier si depart est NULL)
//outils_graphiques.c:
int afficher_ligne(ligne* ln); //affiche une ligne du fichier à l'écran
void bordures (); //redessine les bordures (et quelques autres choses) de l'écran
void menu_options(); //affiche le menu des options (et gère son utilisation)
void print_msg (char message[]); //affiche un message dans la barre d'état (ou l'efface si message est NULL)
int rafraichir (); //redessine l'écran au complet


#endif //!_COLORATION_H