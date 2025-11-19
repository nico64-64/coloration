//ACCENTS.H
//Ce fichier est designé pour être optionnel.
//Il peut être retiré du projet sans autre effet qu'une diminution drastique du support des accents.


#ifndef _ACCENTS_H
#define _ACCENTS_H

/*
            ( _)                ( _)
           / / \\              / /\_\_
          / /   \\            / / | \ \
         / /     \\          / /  |\ \ \
        /  /   ,  \ ,       / /   /|  \ \
       /  /    |\_ /|      / /   / \   \_\
      /  /  |\/ _ '_| \   / /   /   \    \\
     |  /   |/  0 \0\    / |    |    \    \\
     |    |\|      \_\_ /  /    |     \    \\
     |  | |/    \.\ o\o)  /      \     |    \\
     \    |     /\\`v-v  /        |    |     \\
      \ \/    /_| \\_|@@/         |    | \    \\
      | |    /__/_ `-`@@@  _____  |    |  \    \\
      \|    [__]  \_/ @@@@@______ \   |   \    ()
       /   [___]  (   @@@@@@      \  |\ |   |  (/
      |    [___]     @@@@@@@@      |\| \|   /  |/
     /|    [____\                  \  |/\ / / ||
    (  \   [_____\     ) _\      \  \    \| | ||
     \  \  [_____|    / /     __/    \   / / //
     |   \_[_____/   / /        \    |   \/ //
     |   /  '----|   /=\____   _/    |   / //
  __ /  /        |  /   ___/  _/\    \  | ||
 (/-(/-\)       /   \  (/\/\)/  |    /  | /
               (/\/\)           /   /   //
                      _________/   /    /
                     \____________/    (

         "Here be dragons..."
*/

//Structure d'une lettre accentuée:
typedef struct _accent
{
	char minuscule[3]; //exemple: "é"
	char majuscule[3]; //exemple: "É"
	int valeur_min; //exemple: 169 (Cette valeur est en fait le 2e byte lu par getch. Le 1er est toujours 195 dans les cas qui nous intéressent.)
	int valeur_maj; //exemple: 137 (Ici aussi...)
	const char nom[100]; //exemple: "E-aigu"
} _accent;


//Liste / Base de données des accents et de leurs valeurs numériques:
_accent liste_accents[20] =
{
	{"é", "É", 169, 137, "E-aigu"},
	{"è", "È", 168, 136, "E-grave"},
	{"ê", "Ê", 170, 138, "E-circonflexe"},
	{"ë", "Ë", 171, 139, "E-trema"},
	
	{"à", "À", 160, 128, "A-grave"},
	{"â", "Â", 162, 130, "A-circonflexe"},
	{"ä", "Ä", 164, 132, "A-trema"},
	
	{"î", "Î", 174, 142, "I-circonflexe"},
	{"ï", "Ï", 175, 143, "I-trema"},
	
	{"ô", "Ô", 180, 148, "O-circonflexe"},
	{"ö", "Ö", 182, 150, "O-trema"},
	
	{"ù", "Ù", 185, 153, "U-grave"},
	{"û", "Û", 187, 155, "U-circonflexe"},
	
	{"ç", "Ç", 167, 135, "C-cedille"},
	
	{"?", "?", 0, 0, "Perso-1"}, //#14
	{"?", "?", 0, 0, "Perso-2"}, //#15
	{"?", "?", 0, 0, "Perso-3"}, //#16
	{"?", "?", 0, 0, "Perso-4"}, //#17
	{"?", "?", 0, 0, "Perso-5"}, //#18
	{"?", "?", 0, 0, "Perso-6"}, //#19
};


//Autres variables:
char nom_faccents[50] = "accents.txt"; //nom du fichier où est enregistré liste_accents
bool modification_faccents = TRUE; //indique si on peut modifier faccents


//Fonctions:
//Se référer au fichier .c où est implémentée la fonction pour plus de détails.
//Seules les fonctions spécifique à la prise en charge complète des accents sont listées ici. Les autres sont listées dans coloration.h.

//options.c:
bool cree_faccents(); //crée le fichier de sauvegarde des accents
bool gestion_accents(); //gère l'interface de gestion des accents
bool lire_faccents(); //lit la "base de données" des accents si elle existe
void modifier_accent(int num); //gère l'interface de modification d'un accent

//outils_logiques.c:
bool est_un_accent(int car); //indique si le caractère reçu est un accent ou pas (implanté en macro hardcodé si !_ACCENTS_H)
bool str_est_un_accent(char multicar[]); //indique si la string reçue est un accent ou pas (aussi implanté en macro si !_ACCENTS_H)


#endif //!_ACCENTS_H