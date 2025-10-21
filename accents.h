//ACCENTS.H
//Ce fichier est designé pour être optionnel.
//Il peut être retiré du projet sans autre effet qu'une diminution drastique du support des accents.


#ifndef _ACCENTS_H
#define _ACCENTS_H


typedef struct _accent
{
	char minuscule[3]; //exemple: "é"
	char majuscule[3]; //exemple: "É"
	int valeur_min; //exemple: 169
	int valeur_maj; //exemple: 137
	const char nom[100]; //exemple: "E-aigu"
} _accent;


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
	
	{"?", "?", -100, -100, "Perso-1"}, //#14
	{"?", "?", -100, -100, "Perso-2"}, //#15
	{"?", "?", -100, -100, "Perso-3"}, //#16
	{"?", "?", -100, -100, "Perso-4"}, //#17
	{"?", "?", -100, -100, "Perso-5"}, //#18
	{"?", "?", -100, -100, "Perso-6"}, //#19
};


#endif //!_ACCENTS_H