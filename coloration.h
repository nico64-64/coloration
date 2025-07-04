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
#define VERSION "0.3.0"


//Valeurs Macros:
#define NBRE_CAR_MAX_PAR_LIGNE 499


//Structs et enums:

typedef enum _tag _tag;
typedef enum _type _type;
typedef struct commande commande;
typedef struct p_avance p_avance;
typedef struct ligne ligne;

enum _tag
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
};

enum _type
//Étiquette plus précise identifiant le type d'élément (si cette ligne en est un, au sens de sa valeur dans _tag) contenu sur une ligne du fichier ouvert.
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
	_cond
};

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

struct commande
//Structure d'une commande acceptée par le programme:
{
	int num;
	char nom[30];
	char touche[10];
	char descr[200];
	char details[1200];
	int cmd_liees[3];
};

struct p_avance
//Structure d'un paramètre avancé:
{
	char descr[200];
	char type; //0 = aucun, i = int, b = bool, c = char, s = string
	int* ptr_int;
	char* ptr_str;
};


//Variables Globales:

//Liste des commandes acceptées par le programme:
commande cmds[] =
{
	{0, "", "", "", "", {0, 0, 0}}, //Cette fausse commande doit toujours rester vide! Elle ne sert qu'à décaler les autres commandes et offrir la possibilité d'utiliser le 0 comme valeur spéciale.
	{1, "Généralités", "", "En apprendre plus sur cet éditeur.", "Coloration est un éditeur de texte TUI (Terminal User Interface) très semblable à Nano (GNU nano).\nLe but de cet éditeur est de faciliter la création et l'édition des fichiers de liste d'objets du projet \"text-adventure game\". Il s'agit d'un projet en cours, donc certaines fonctionnalités ne sont pas encore implantées.\nLa liste (incomplète) des commandes disponibles est toujours affichée dans le bas de l'écran. Voux pouvez aussi consulter ce manuel en appuyant sur Ctrl-A (^A). Vous y trouverez l'intégralité des commandes disponibles.\nCertaines commandes agissent de manière silencieuse, tandis que d'autres affichent un message dans la barre d'état (3e ligne depuis le bas de l'écran) ou encore demande un input de votre part. Dans ce dernier cas, les 2 lignes du bas de l'écran seront utilisées pour cela.\n \nNote sur la notation des touches:\nLe symbole \"^\" est utilisé pour représenter la touche Ctrl (exemple: ^A = Ctrl-A), tandis que les touches Alt, Cmd ou Alt-Car sont représentées par \"M-\" (exemple: M-# = Alt-Car #). \"Sh\" est parfois utilisé pour abrévier \"Shift\".", {3, 26, 0}},
	{2, "Menu des options", "Esc", "Accéder au menu des options.", "Vous pouvez accéder à ce menu en appuyant sur \"Escape\" depuis l'interface principale de l'éditeur.\nCe menu fournis plusieurs raccourcis dont la plupart sont aussi accessibles autrement. Il s'agit toutefois de la seule manière d'accéder aux paramètres avancés et aux crédits.\nUtilisez les flèches pour vous déplacer dans le menu, puis appuyez sur \"Enter\" pour effectuer l'action sélectionnée. Appuyer sur \"Escape\" fermera le menu.", {26, 3, 0}},
	{3, "Aide", "^A", "Accéder à cette page.", "Coloration est équipé d'un manuel d'aide intégré, que vous consultez en ce moment.\nOn peut y accéder à presque tous les moments en appuyant sur Ctrl-A.", {1, 26, 2}},
	{4, "Quitter", "^Q", "Fermer le programme.", "Utilisez Ctrl-Q pour fermer Coloration sans enregistrer vos modifications au fichier ouvert.", {1, 0, 0}},
	{5, "Enregistrer", "^E", "Enregistrer le fichier.", "Utilisez cette commande pour enregistrer le fichier ouvert. Vous pourrez choisir le nom et l'emplacement du fichier.", {6, 0, 0}},
	{6, "Sauvegarder", "^S", "Sauvegarder rapidement le fichier.", \
		"Utilisez cette commande pour sauvegarder rapidement le fichier ouvert.\nVous ne pourrez pas modifier ni le nom, ni l'emplacement du fichier. Utilisez plutôt \"Enregistrer\" pour cela.", {5, 0, 0}},
	{7, "Ouvrir un fichier", "^N", "Ouvrir un autre fichier ou en créer un.", "Fonctionnalité à venir.\nÀ Faire!", {1, 5, 6}},
	{8, "Aller à", "^L", "Envoyer le curseur à une certaine ligne.", "Utilisez cette commande pour emmener rapidement votre curseur au début d'une certaine ligne du fichier.", {21, 22, 9}},
	{9, "Trouver & remplacer", "^F", "Trouver (et remplacer) une expression dans le fichier.", "Fonctionnalité à venir.\nÀ Faire!", {21, 22, 8}},
	{10, "Ligne de commande", "^P", "Envoyer une commande à l'éditeur", "Ouvre une ligne de commande Coloration en bas de l'écran.\nVoici la liste des commandes acceptées par cette ligne de commande ainsi qu'une brève description de leur effet ou une référence à l'article approprié de ce manuel:\n- aide -> voir \"Aide\"\n- rafraichir -> voir \"Rafraichir\"\n- ligne / aller / aller à (Argument facultatif: numéro de ligne / \"DEBUT\" / \"FIN\") -> Emmène le curseur à la ligne fournie en argument ou effectue la commande \"Aller à\" si aucun argument n'est fourni. Effectue la commande \"Aller au -DÉBUT-\" ou \"Aller à la -FIN-\" si l'argument y correspond.\n- DEBUT -> voir \"Aller au -DÉBUT-\"\n- FIN -> voir \"Aller à la -FIN-\"\n- terminal / term -> voir \"Accès au terminal\"\n- enregistrer / enregistrer comme / enregistrer sous / enr (Argument facultatif: nom du fichier) -> Effectue la commande \"Enregistrer\" si aucun argument n'est fourni ou enregistre le fichier ouvert sous le nom fourni (au même endroit).\n- sauvegarder / sauv -> Voir \"Sauvegarder\".\n- menu / options -> Voir \"Menu des options\".\n", {11, 2, 0}},
	{11, "Accès au terminal", "^T", "Entrer une commande système.", "Permet d'entrer une commande bash depuis Coloration.\nEntrez Ctrl-T une deuxième fois pour que l'éditeur s'efface, laissant place à une toute nouvelle console bash prenant le contrôle de l'entièreté du terminal. Dans ce cas, n'oubliez pas d'entrer la commande bash \"exit\" pour revenir à l'éditeur.", {10, 0, 0}},
	{12, "Compter caractères", "^W", "Compter les caractères sélectionnés ou du fichier complet.", \
		"Indique combien de caractères on été sélectionnés. Si aucun caractère n'est sélectionné, le nombre de caractère total du fichier ouvert s'affichera.", {0, 0, 0}},
	{13, "Compiler la liste", "^G", "Transformer ce fichier en liste d'objets .c.", \
		"Utilisez cette commande comme raccourci pour transformer un fichier (qui doit avoir la bonne syntaxe (voir \"Coloration syntaxique\")) en fichiers de code C pouvant être utilisés pour compiler un scénario pour le \"text adventure game\".", {25, 26, 0}},
	{14, "Annuler", "^Z", "Annuler la dernière action.", "Fonctionnalité à venir.\nÀ Faire!", {15, 0, 0}},
	{15, "Refaire", "^Y", "Refaire l'action précédement annulée.", "Fonctionnalité à venir.\nÀ Faire!", {14, 0, 0}},
	{16, "Copier", "^C", "Copier le texte sélectionné.", \
		"Copie la sélection dans un presse-papier interne, afin de pouvoir être collée plus tard (dans le même document).\nNe fais rien si aucun caractère n'est sélectionné.", {17, 18, 0}},
	{17, "Coller", "^V", "Coller le texte copié.", "Colle le contenu du presse-papier interne à l'endroit où se trouve le curseur.\nSi du texte était sélectionné, celui-ci sera écrasé. Si le presse-papier interne est vide, rien ne se passera et un message s'affichera.\nLe presse-papier étant interne, le texte copié doit l'avoir été dans le même fichier et dans la même instance de Coloration.", {16, 18, 0}},
	{18, "Couper", "^X", "Couper le texte sélectionné.", \
		"Copie la sélection dans un presse-papier interne, afin de pouvoir être collée plus tard (dans le même document), puis supprimme le texte copié du fichier.\nNe fais rien si aucun caractère n'est sélectionné.", {16, 17, 0}},
	{19, "Début de ligne", "Home", "Envoyer le curseur au début de la ligne.", "Emmène le curseur au début de la ligne où il se trouve (après le dernier \"Enter\").", {20, 21, 0}},
	{20, "Fin de ligne", "End", "Envoyer le curseur à la fin de la ligne.", "Emmène le curseur à la fin de la ligne où il se trouve (avant le prochain \"Enter\").", {19, 22, 0}},
	{21, "Aller au -DÉBUT-", "Sh-Home", "Envoyer le curseur au début de l'objet.", "Appuyez sur Shift-Home pour emmener votre curseur à l'endroit où est écrit \"-DÉBUT-\" dans le fichier ouvert. Il s'agit du début d'une liste d'objets du projet \"text-adventure game\" (voir \"Compiler la liste\"), si ce fichier en est une.\nNe fais rien s'il n'y a pas de \"-DÉBUT-\" dans ce fichier.\nDans certains terminaux (exemple: console Linux (tty)), Shift-Home n'est pas détecté. Il faut alors passer par la commande correspondante (\"DEBUT\") (voir \"Ligne de commande\").", {13, 10, 22}},
	{22, "Aller à la -FIN-", "Sh-End", "Envoyer le curseur à la fin de l'objet.", "Appuyez sur Shift-End pour emmener votre curseur à l'endroit où est écrit \"-FIN-\" dans le fichier ouvert. Il s'agit de la fin d'une liste d'objets du projet \"text-adventure game\" (voir \"Compiler la liste\"), si ce fichier en est une.\nNe fais rien s'il n'y a pas de \"-FIN-\" dans ce fichier.\nDans certains terminaux (exemple: console Linux (tty)), Shift-End n'est pas détecté. Il faut alors passer par la commande correspondante (\"FIN\") (voir \"Ligne de commande\").", {13, 10, 21}},
	{23, "Rafraichir", "^R", "Redessiner le terminal et afficher les messages d'erreur.", \
		"Redessine l'écran de Coloration sur le terminal, permettant parfois de régler quelques petits glitchs.\nAffiche aussi tout message d'erreur en attente dans la barre d'état.", {24, 0, 0}},
	{24, "Débogage", "^D", "Activer ou désactiver le mode débogage.", "Appuyer sur Ctrl-D pour activer/désactiver le mode débogage.\nVous pouvez aussi changer de mode de débogage en appuyant sur M-# (Alt-Car 3). Il y a 3 modes de débogage: \"input\" (affiche le nom de la touche appuyée), \"position\" (affiche la position du curseur dans le fichier) et \"input (raw)\" (donne la valeur numérique de la touche appuyée).\nCette fonctionnalité est une des seules à ne pas avoir de commande correspondante. Ctrl-D est la seule manière de l'activer.", {23, 0, 0}},
	{25, "Col. syntaxique", "F1", "Activer ou désactiver la coloration syntaxique.", "Active/Désactive la coloration syntaxique.\nCette coloration suit la syntaxe d'un fichier de liste d'objets source du projet \"text-adventure game\" (voir \"Compiler la liste\").\nAssurez-vous d'appuyer sur Fn en même temps que F1 pour que cela fonctionne.\nLa coloration syntaxique suivant la syntaxe attendue, elle n'apparaitra pas si le fichier n'est pas syntaxiquement conforme (voir \"Aller au -DÉBUT-\" et \"Aller à la -FIN-\").", {13, 21, 22}},
	{26, "Paramètres avancés", "", "Modifier certains paramètres avancés.", "Accessible par le menu des options uniquement, les paramètres avancés comprennent quelques options de débogage ainsi que quelque variables internes modifiables par l'utilisateur.\nLa modification de ces paramètres n'est pas recommandée.", {2, 1, 0}},
};
const int nbre_cmds = 26;

//Paramètres avancés:
bool err_log = FALSE; //indique si les erreurs doivent être logguées dans un fichier
char nom_ferreur[50] = "erreurs.txt"; //nom du fichier de log d'erreurs (si activé)
char cmd_compiler_liste[100] = "./createur_liste "; //commande permettant de compiler la liste qu'est le document en cours de modification (le nom du fichier sera appendé à son ouverture)
char nom_fconfig[50] = "config.txt"; //nom du fichier de configuration de l'éditeur
//Liste des paramètres avancés:
p_avance p_avances[] =
{
	{"", '0', NULL, NULL}, //Ce faux paramètre avancé doit toujours rester vide. Il sert uniquement à décaler la liste pour qu'elle commence à 1.
	{"Logguer les erreurs dans un fichier txt.", 'b', (int*) &err_log, NULL},
	{"Nom et emplacement du fichier de log des erreurs (si activé).", 's', NULL, &nom_ferreur[0]},
	{"Commande envoyée pour \"compiler la liste\" (voir manuel d'aide).", 's', NULL, &cmd_compiler_liste[0]},
	{"Nom et emplacement du fichier de configuration de l'éditeur. (À FAIRE)", 's', NULL, &nom_fconfig[0]},
};
const int nbre_p_avances = 4;

//Symboles internes:
void* ERREUR; //pointeur signifiant que la fonction a eu une erreur
ligne DEBUT_FICHIER; //pointeur représentant le début du fichier
ligne FIN_FICHIER; //pointeur représentant la fin du fichier

//Débogage:
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
bool coloration_syntaxique = 1; //indique si la coloration syntaxique est activée
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
ligne* aller_a (int num); //envoie l'utilisateur à une certaine ligne du fichier (reçue en paramètre ou demandée par la fonction si num = 0)
void cmd (char commande[]); //exécute une commande (en demande une si commande est NULL)
void enregistrer (char nom_sauvegarde[]); //enregistre le buffer modifié dans un fichier dont le nom est reçu en paramètre (si NULL, un nom sera demandé)
int erreur (int code, char message[]); //log une erreur ou en affiche une (s'il y en a une) si code = 0
int main (int argc, char* argv[]); //contient l'ouverture et l'initialisation du programme ainsi que la main loop
void menu_options (); //affiche le menu des options (et gère son utilisation)
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
void liste_options (int selection); //affiche la liste des options du menu des options et surligne l'option reçue en paramètre
void print_msg (char message[]); //affiche un message dans la barre d'état (ou l'efface si message est NULL)
int rafraichir (); //redessine l'écran au complet


#endif //!_COLORATION_H