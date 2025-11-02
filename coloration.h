#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <locale.h>
#include <time.h>


#ifndef _COLORATION_H
#define _COLORATION_H


//Version du programme:
#define VERSION "0.6.2"

//Valeurs Macros:
#define NBRE_CAR_MAX_PAR_LIGNE 499

//Gestion avancée/complète des accents:
#include "accents.h"


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
	_endroit = 1,
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
	char details[1800];
	int cmd_liees[3];
};

struct p_avance
//Structure d'un paramètre avancé:
{
	char nom[30]; //sans espace et uniquement utilisé dans le fichier de configuration ("-" indique que ce paramètre ne doit pas être sauvegardé)
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
	{1, "Généralités", "", "En apprendre plus sur cet éditeur.", "Coloration est un éditeur de texte TUI (Terminal User Interface) très semblable à Nano (GNU nano).\nLe but de cet éditeur est de faciliter la création et l'édition des fichiers de liste d'objets du projet \"text-adventure game\". Il s'agit d'un projet en cours, donc certaines fonctionnalités ne sont pas encore implantées.\nLa liste (incomplète) des commandes disponibles est toujours affichée dans le bas de l'écran. Vous pouvez aussi consulter ce manuel en appuyant sur Ctrl-A (^A). Vous y trouverez l'intégralité des commandes disponibles.\nCertaines commandes agissent de manière silencieuse, tandis que d'autres affichent un message dans la barre d'état (3e ligne depuis le bas de l'écran) ou encore demandent un input de votre part. Dans ce dernier cas, les 2 lignes du bas de l'écran seront utilisées pour cela.\n \nNote sur la notation des touches:\nLe symbole \"^\" est utilisé pour représenter la touche Ctrl (exemple: ^A = Ctrl-A), tandis que les touches Alt, Cmd ou Alt-Car sont représentées par \"M-\" (exemple: M-# = Alt-Car #). \"Sh\" est parfois utilisé pour abrévier \"Shift\".", {3, 26, 0}},
	{2, "Menu des options", "Esc", "Accéder au menu des options.", "Vous pouvez accéder à ce menu en appuyant sur \"Escape\" depuis l'interface principale de l'éditeur.\nCe menu fournis plusieurs raccourcis dont la plupart sont aussi accessibles autrement. Il s'agit toutefois de la seule manière d'accéder aux paramètres avancés et aux crédits.\nUtilisez les flèches pour vous déplacer dans le menu, puis appuyez sur \"Enter\" pour effectuer l'action sélectionnée. Appuyer sur \"Escape\" fermera le menu.", {26, 3, 0}},
	{3, "Aide", "^A", "Accéder à cette page.", "Coloration est équipé d'un manuel d'aide intégré, que vous consultez en ce moment.\nOn peut y accéder à presque tous les moments en appuyant sur Ctrl-A.", {1, 26, 2}},
	{4, "Quitter", "^Q", "Fermer le programme.", "Utilisez Ctrl-Q pour fermer Coloration sans enregistrer vos modifications au fichier ouvert.", {1, 0, 0}},
	{5, "Enregistrer", "^E", "Enregistrer le fichier.", "Utilisez cette commande pour enregistrer le fichier ouvert. Vous pourrez choisir le nom et l'emplacement du fichier.", {6, 0, 0}},
	{6, "Sauvegarder", "^S", "Sauvegarder rapidement le fichier.", \
		"Utilisez cette commande pour sauvegarder rapidement le fichier ouvert.\nVous ne pourrez pas modifier ni le nom, ni l'emplacement du fichier. Utilisez plutôt \"Enregistrer\" pour cela.", {5, 0, 0}},
	{7, "Ouvrir un fichier", "^N", "Ouvrir un autre fichier ou en créer un.", "Utilisez cette commande pour ouvrir un autre fichier avec Coloration.\nVous devrez entrer le chemin d'accès au fichier depuis l'endroit d'où vous avex démarré Coloration.\nNotez bien que tout changement non sauvegrdé au fichier en cours de modification sera perdu, puisque l'éditeur sera redémarré.", {1, 5, 6}},
	{8, "Aller à", "^L", "Envoyer le curseur à une certaine ligne.", "Utilisez cette commande pour emmener rapidement votre curseur au début d'une certaine ligne du fichier.", {21, 22, 9}},
	{9, "Trouver & remplacer", "^F", "Trouver (et remplacer) une expression dans le fichier.", "Fonctionnalité à venir.\nÀ Faire!", {21, 22, 8}},
	{10, "Ligne de commande", "^P / F4", "Envoyer une commande à l'éditeur", "Ouvre une ligne de commande Coloration en bas de l'écran.\nVoici la liste des commandes acceptées par cette ligne de commande ainsi qu'une brève description de leur effet ou une référence à l'article approprié de ce manuel:\n- aide -> voir \"Aide\"\n- rafraichir -> voir \"Rafraichir\"\n- ligne / aller / aller à (Argument facultatif: numéro de ligne / \"DEBUT\" / \"FIN\" / \"fin\") -> Emmène le curseur à la ligne fournie en argument ou effectue la commande \"Aller à\" si aucun argument n'est fourni. Effectue la commande \"Aller au -DÉBUT-\" ou \"Aller à la -FIN-\" si l'argument y correspond. Emmène le curseur à la dernière ligne du fichier si l'argument est \"fin\".\n- DEBUT -> voir \"Aller au -DÉBUT-\"\n- FIN -> voir \"Aller à la -FIN-\"\n- compiler -> voir \"Compiler la liste\"\n- terminal / term -> voir \"Accès au terminal\"\n- enregistrer / enregistrer comme / enregistrer sous / enr (Argument facultatif: nom du fichier) -> Effectue la commande \"Enregistrer\" si aucun argument n'est fourni ou enregistre le fichier ouvert sous le nom fourni (au même endroit).\n- sauvegarder / sauv -> Voir \"Sauvegarder\".\n- ouvrir / nouveau / ouv / nouv -> voir \"Ouvrir un fichier\". Vous pouvez aussi spécifier le nom du fichier en argument.\n- coloration / syntaxe / couleurs -> voir \"Coloration syntaxique\".\n- menu / options -> Voir \"Menu des options\".\n- parametres / param -> voir \"Paramètres avancés\".\n- accents -> voir \"Gestion des accents\".\n- credits -> Affiche la page des crédits de l'éditeur (aussi accessible par le menu des options).\n", {11, 2, 0}},
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
	{19, "Aller au début", "^Home", "Aller à la première ligne du fichier.", "Appuyez sur Ctrl-Home pour revenir à la prmière ligne du fichier.\nDans certains terminaux (exemple: console Linux (tty)), Ctrl-Home n'est pas détecté. On peut alors utiliser la fonction \"Aller à\" à la place.", {20, 21, 8}},
	{20, "Aller à la fin", "^End", "Aller à la dernière ligne du fichier.", "Appuyer sur Ctrl-End pour aller à la dernière ligne du fichier.\nDans certains terminaux (exemple: console Linux (tty)), Ctrl-End n'est pas détecté. On peut alors utiliser la commande \"aller fin\" (voir \"Ligne de commande\") à la place.", {19, 22, 10}},
	{21, "Aller au -DÉBUT-", "M-Home", "Envoyer le curseur au début de l'objet.", "Appuyez sur Alt-Home pour emmener votre curseur à l'endroit où est écrit \"-DÉBUT-\" dans le fichier ouvert. Il s'agit du début d'une liste d'objets du projet \"text-adventure game\" (voir \"Compiler la liste\"), si ce fichier en est une.\nNe fais rien s'il n'y a pas de \"-DÉBUT-\" dans ce fichier.\nDans certains terminaux (exemple: console Linux (tty)), Alt-Home n'est pas détecté. Il faut alors passer par la commande correspondante (\"DEBUT\") (voir \"Ligne de commande\").", {13, 10, 22}},
	{22, "Aller à la -FIN-", "M-End", "Envoyer le curseur à la fin de l'objet.", "Appuyez sur Alt-End pour emmener votre curseur à l'endroit où est écrit \"-FIN-\" dans le fichier ouvert. Il s'agit de la fin d'une liste d'objets du projet \"text-adventure game\" (voir \"Compiler la liste\"), si ce fichier en est une.\nNe fais rien s'il n'y a pas de \"-FIN-\" dans ce fichier.\nDans certains terminaux (exemple: console Linux (tty)), Alt-End n'est pas détecté. Il faut alors passer par la commande correspondante (\"FIN\") (voir \"Ligne de commande\").", {13, 10, 21}},
	{23, "Rafraichir", "^R", "Redessiner le terminal et afficher les messages d'erreur.", \
		"Redessine l'écran de Coloration sur le terminal, permettant parfois de régler quelques petits glitchs.\nAffiche aussi tout message d'erreur en attente dans la barre d'état.", {24, 0, 0}},
	{24, "Débogage", "^D", "Activer ou désactiver le mode débogage.", "Appuyer sur Ctrl-D pour activer/désactiver le mode débogage.\nVous pouvez aussi changer de mode de débogage en appuyant sur M-# (Alt-Car 3). Il y a 4 modes de débogage: \"input\" (affiche le nom de la touche appuyée), \"position\" (affiche la position du curseur dans le fichier), \"tag/type\" (affiche le tag et le type (s'il y a lieu) de la ligne où se trouve le curseur) et \"input (raw)\" (donne la valeur numérique de la touche appuyée).\nCette fonctionnalité est une des seules à ne pas avoir de commande correspondante. Ctrl-D est la seule manière de l'activer.", {23, 25, 26}},
	{25, "Col. syntaxique", "F1", "Activer ou désactiver la coloration syntaxique.", "Active/Désactive la coloration syntaxique.\nCette coloration suit la syntaxe d'un fichier de liste d'objets source du projet \"text-adventure game\" (voir \"Compiler la liste\").\nAssurez-vous d'appuyer sur Fn en même temps que F1 pour que cela fonctionne.\nLa coloration syntaxique suivant la syntaxe attendue, elle n'apparaitra pas si le fichier n'est pas syntaxiquement conforme (voir \"Aller au -DÉBUT-\" et \"Aller à la -FIN-\").", {13, 21, 22}},
	{26, "Paramètres avancés", "F2", "Modifier certains paramètres avancés.", "Également accessible par le menu des options, les paramètres avancés permettent de modifier le comportement de l'éditeur dans certaines situations.\nLa modification de ces paramètres n'est pas recommandée.", {2, 1, 27}},
	{27, "Gestion des accents", "F3", "Informations sur le support et la gestion des accents par Coloration.", "Coloration fournit, selon sa version, un support \"minimal\" ou \"complet\" des accents.\nSi seul le support minimal est activé, la plupart des accents déjà présents dans un fichier seront affichés correctement, mais vous ne pourrez pas en en entrer au clavier (tout accent reçu du clavier sera ignoré ou transformé en \"?\").\nVous pouvez vérifier si le support complet est activé en ouvrant les paramètres avancés du programme. Si le support complet est activé, vous pouvez aussi y modifier la gestion des accents en entrant ^G.\n \nPar défaut, seuls les caractères suivants (en majuscule en en minuscule) sont supportés par le programme: àâä éèêë îï ôö ùû ç.\nToutefois, si le support complet des accents est activé, vous pouvez aussi définir manuellement jusqu'à 6 \"accents\" supplémentaires.\nDans ce cas, vous pouvez aussi changer les valeurs numériques associées aux différents caractères dans les paramètres avancés pour que l'éditeur s'adapte à votre clavier.\n \nExplication technique:\nLorsque vous appuyez sur une touche de votre clavier, ncurses envoie une certaine valeur numérique au programme. Cette valeur peut être très grande (comme pour les accents), ce qui nécessite plus qu'un octet. Dans ce cas-ci, le système de gestion des accents ignore le 1er octet, puisque celui-ci est normalement toujours 195 pour tous les caractères communs de la langue française. Cette manière de faire est toutefois très imparfaite, puisqu'elle prend aussi pour acquis que le 2e octet ne sera pas identique à aucune autre valeur numérique possiblement reçue du clavier. C'est normalement le cas, mais si la gestion des accents ne fonctionne pas sur votre machine, c'est certainement à cause de cela.", {26, 1, 0}}
};
const int nbre_cmds = 27;

//Paramètres avancés:
bool err_log = FALSE; //indique si les erreurs doivent être logguées dans un fichier (désactivé par défaut parce qu'à part remplir le fichier de lignes vides / "Initialisation...", ça ne fait rien d'utile)
char nom_ferreur[50] = "erreurs.txt"; //nom du fichier de log d'erreurs (si activé) (au nombre d'erreurs que j'ai définies, ça ne sert pas vraiment à rien...)
char cmd_compiler_liste[100] = "./createur_liste"; //commande permettant de compiler la liste qu'est le document en cours de modification (excluant le nom du fichier, qui est ajouté après cette string dans compilateur_liste)
char nom_fconfig[50] = "config.txt"; //nom du fichier de configuration de l'éditeur (soit le fichier où les paramètres avancés sont sauvegardés)
//Liste des paramètres avancés:
p_avance p_avances[] =
{
	{"-", "", '0', NULL, NULL}, //Ce faux paramètre avancé doit toujours rester vide. Il sert uniquement à décaler la liste pour qu'elle commence à 1.
	{"CMD_COMPILER", "Commande envoyée pour \"compiler la liste\" (voir manuel d'aide).", 's', NULL, &cmd_compiler_liste[0]},
	#ifdef _ACCENTS_H
	{"FACCENTS", "Nom et emplacement du fichier de sauvegarde des accents.", 's', NULL, &nom_faccents[0]},
	#endif
	{"ERRLOG", "Logguer les erreurs dans un fichier txt.", 'b', (int*) &err_log, NULL},
	{"FERREUR", "Nom et emplacement du fichier de log des erreurs (si activé).", 's', NULL, &nom_ferreur[0]},
	{"-", "Nom et emplacement du fichier de configuration de l'éditeur.", 's', NULL, &nom_fconfig[0]},
};
#ifdef _ACCENTS_H
const int nbre_p_avances = 5;
#else
const int nbre_p_avances = 4;
#endif

//Symboles internes:
void* ERREUR; //pointeur signifiant que la fonction le retournant a eu une erreur
ligne DEBUT_FICHIER; //variable dont l'adresse sert à représenter le début du fichier
ligne FIN_FICHIER; //variable dont l'adresse sert à représenter la fin du fichier

//Débogage:
bool debogage = FALSE; //indique si le mode débogage est activé
char element_debogue = 'i'; //indique ce qui est débogué (lorsque le mode débogage est activé)
							//Valeurs possibles: i = input, p = position, t = tag/type, n = input (raw)
							
//Gestion du fichier ouvert:
char nom_fichier[100] = ""; //nom du fichier actuellement modifié (buffer assez grand pour (entre autres) accomoder tout le chemin d'accès au fichier, si nécessaire...)
FILE* fichier = NULL; //pointeur vers le fichier actuellement modifié
ligne* lignes; //liste des lignes contenus dans le fichier

//Position sur l'écran et dans le fichier:
int y = 1; //position en y du curseur sur l'écran
int x = 4; //position en x du curseur sur l'écran
int premiere_ligne = 1; //numéro de la ligne la plus haute à l'écran
int derniere_ligne = 10; //numéro de la ligne la plus basse à l'écran
int pos_y = 1; //position en y dans la "ligne" modifiée par le curseur (soit le "numéro de ligne" multiligne dans cette ligne, pour lorsqu'elle est trop longue)   /!\ Attention! Cette variable commence à 1 (et non 0)!
ligne* ln_mod; //ligne modifiée présentement (ligne où se trouve le curseur)

//Autres:
bool coloration_syntaxique = TRUE; //indique si la coloration syntaxique est activée
char compilateur_liste[205]; //commande réellement utilisée pour compiler la liste d'objets de ce fichier (= cmd_compiler_liste + ' ' + nom_fichier)
bool fconfig_particulier = FALSE; //indique si l'utilisateur a spécifié un fconfig en option d'invocation
bool barre_dispo = TRUE; //indique si un message est présentement affiché dans la barre d'état (0 = occupée, 1 = libre) (très peu utilisé...)
char msg_en_attente[300] = ""; //indique si un message devrait être réaffiché parce qu'il n'a pas fonctionné


//Fonctions Macros:

//Affichage général:
#define mv(position_y, position_x);		move(position_y, position_x); y = position_y; x = position_x; //permet de déplacer le curseur tout en mettant les variables x et y à jour (remplacement de "move")
#define mvaddstrc(position_y, texte);	mvaddstr(position_y, (COLS - longueur_str(texte)) / 2, texte); //affiche une string centrée (sur une ligne y)

//Initialisation et libération de la mémoire:
#define init();		for (int _COMPTEUR = 1; init_ligne(_COMPTEUR) != NULL; _COMPTEUR++) {}; verifie_syntaxe(); //initialise toutes les lignes du fichier
#define desinit();	for (int _COMPTEUR = init_ligne(0)->num; _COMPTEUR >= 1; _COMPTEUR--) {init_ligne(-_COMPTEUR);} //libère toute les lignes du fichier

//Gestion de base des accents si accents.h n'est pas inclus:
#ifndef _ACCENTS_H
//Indique si un caractère d'input ncurses (int) est un des 28 caractères accentués supportés en version minimaliste:
#define est_un_accent(car)		(car == 169 || car == 137 || car == 168 || car == 136 || car == 170 || car == 138 || car ==	171 || car == 139 || car ==	160 || car == 128 || car ==	162 || car == 130 || car ==	164 || car == 132 \
									|| car ==	174 || car == 142 || car ==	175 || car == 143 || car ==	180 || car == 148 || car ==	182 || car == 150 || car ==	185 || car == 153 || car ==	187 || car == 155 || car ==	167 || car == 135)
//Indique si une string est un de ces 28 caractères:
#define str_est_un_accent(car)	(!strcmp(car, "é") || !strcmp(car, "è") || !strcmp(car, "ê") || !strcmp(car, "ë") || !strcmp(car, "à") || !strcmp(car, "â") || !strcmp(car, "ä") || !strcmp(car, "î") || !strcmp(car, "ï") \
									|| !strcmp(car, "ô") || !strcmp(car, "ö") || !strcmp(car, "ù") || !strcmp(car, "û") || !strcmp(car, "ç") || !strcmp(car, "É") || !strcmp(car, "È") || !strcmp(car, "Ê") || !strcmp(car, "Ë") \
									|| !strcmp(car, "À") || !strcmp(car, "Â") || !strcmp(car, "Ä") || !strcmp(car, "Î") || !strcmp(car, "Ï") || !strcmp(car, "Ô") || !strcmp(car, "Ö") || !strcmp(car, "Ù") || !strcmp(car, "Û") \
									|| !strcmp(car, "Ç"))
//Ces 2 macros sont remplacés par des fonctions si accents.h est inclus.
#endif

//Gestion des erreurs et débogage:
#define msg_printf(txt, ...);		char _INTERNAL_BUFFER_[400]; sprintf(_INTERNAL_BUFFER_, txt, __VA_ARGS__); print_msg(_INTERNAL_BUFFER_); //Affiche un message en style printf (à éviter...)
#define err_printf(code, txt, ...);	char _INTERNAL_BUFFER[400]; sprintf(_INTERNAL_BUFFER, txt, __VA_ARGS__); erreur(code, _INTERNAL_BUFFER); //Log une erreur en style printf
#define DEBUG(txt, ...);			err_printf(1, txt, __VA_ARGS__); erreur(0, "..."); //macro de débogage (utiliser en style printf)
#define DEBUG_RADICAL(txt, ...);	err_printf(1, txt, __VA_ARGS__); quitter(-1); erreur(0, "..."); exit(-100); //comme le macro de débogage standard, mais quitte à la fin pour être sûr que le message se rend et que ça ne plante pas


//Fonctions:
//Se référer au fichier .c où est implémentée la fonction pour plus de détails.
//Les fonctions spécifiques au support complet des accents sont listées dans accents.h.

//coloration.c:
ligne* aller_a (int num); //envoie l'utilisateur à une certaine ligne du fichier (reçue en paramètre ou demandée par la fonction si num = 0)
void cmd (char commande[]); //exécute une commande (en demande une si commande est NULL)
void enregistrer (char nom_sauvegarde[]); //enregistre le buffer modifié dans un fichier dont le nom est reçu en paramètre (si NULL, un nom sera demandé)
int erreur (int code, char message[]); //log une erreur ou en affiche une (s'il y en a une) si code = 0
void gestion_arguments (char arg[]); //gère les options d'invocation du programme
int main (int argc, char* argv[]); //contient l'ouverture et l'initialisation du programme ainsi que la main loop
void ouvrir (char nom_nouveau_fichier[]); //ouvre un nouveau fichier (en redémarrant le programme)
void quitter (int code); //ferme proprement le programme
void term (); //permet à l'utilisateur d'envoyer une commande au terminal (et de consulter son output)

//options.c:
void aide (int num_cmd); //ouvre le module d'aide / manuel et gère (et affiche) l'aide interactive générale
void aide_specifique (int num_cmd); //affiche l'article du manuel à propos de la commande dont elle vient de recevoir le numéro
void credits (); //affiche et gère la fenêtre des crédits
bool enregistrer_parametres (); //enregistre les paramètres avancés dans fconfig
void lire_parametres(); //lit et applique les paramètres enregistrés dans fconfig
void menu_options (); //affiche le menu des options (et gère son utilisation)
void param_avances (); //affiche et gère la fenêtre des paramètres avancés

//outils_logiques.c:
int compter_lignes (ligne* ln); //compte le nombre de lignes (à l'écran) qu'occupe une ligne du fichier
char* efface_car (char str[], unsigned pos); //efface le caractère situé à la position pos dans la string str
bool est_un_nbre (char str[], char type); //indique si une string est un nombre (choisir le type de nombre via le paramètre correspondant)
ligne* init_ligne (int num); //initialise la structure d'une ligne en la lisant dans le fichier ouvert
ligne* insere_ligne (ligne* ln_i, unsigned pos); //insère une nouvelle ligne après la ligne reçue en paramètre, juste avant la position également reçue en paramètre
int longueur_str (char str[]); //trouve le nombre de caractères d'une string en tenant compte des accents
bool match_accent (int car, char accent[]); //convertit un accent reçu du clavier (int) en ce même caractère en format string
int relativise_pos (char str[], int pos); //trouve la position réelle du curseur dans une string contenant des accents
bool supprime_ligne (ligne* ln); //supprime une ligne et append son contenu à la ligne précédente.
ligne* trouve_ligne (int num); //renvoie un pointeur vers la ligne correspondant au numéro de ligne reçu
ligne* trouve_tag (_tag tag, ligne* depart); //renvoie la première ligne ayant un certain tag d'assigné depuis la ligne de départ (ou le début du fichier si depart est NULL)
void verifie_syntaxe (); //vérifie la syntaxe du document (pour la coloration syntaxique)

//outils_graphiques.c:
int afficher_ligne(ligne* ln); //affiche une ligne du fichier à l'écran
void bordures (); //redessine les bordures (et quelques autres choses) de l'écran
void liste_options (int selection); //affiche la liste des options du menu des options et surligne l'option reçue en paramètre
void print_msg (char message[]); //affiche un message dans la barre d'état (ou l'efface si message est NULL)
int rafraichir (); //redessine l'écran au complet


#endif //!_COLORATION_H