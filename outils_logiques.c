#include "outils_graphiques.c"


bool est_un_nbre(char str[], char type)
//Renvoie TRUE si str contient exclusivement un nombre (on peut ensuite utiliser sscanf pour l'extraire).
//Renvoie FALSE dans le cas contraire.
//Le paramètre type permet de préciser quel type de nombres sont acceptés:
//b = bool (0 ou 1), u (ou I) = unsigned (entier positif), i = int (entier), F = float positif (nombre décimal positif), f (ou autre) = float (nombre décimal)
{
	bool negatif = FALSE;
	bool decimal = FALSE;
	
	if (type == 'b')
	{
		if ((str[0] == '0' || str[0] == '1') && str[1] == '\000')
		{return TRUE;}
		else
		{return FALSE;}
	}
	
	for (int compteur = 0; str[compteur] != '\000'; compteur++)
	{
		if (!isdigit(str[compteur]))
		{
			if (str[compteur] == '-' && (type == 'u' || type == 'I' || type == 'F' || negatif))
			{return FALSE;}
			else
			{negatif = TRUE;}
			
			if ((str[compteur] == '.' || str[compteur] == ',') && (type == 'u' || type == 'I' || type == 'i' || decimal))
			{return FALSE;}
			else
			{decimal = TRUE;}
		}
	}
	
	return TRUE;
}


#ifdef _ACCENTS_H
bool est_un_accent(int car)
//Indique si le caractère reçu en paramètre est un accent (défini dans liste_accents).
//Cette fonction est seulement utile avec un input ncurses!
//Elle ne détectera pas les accents dans un string.
//Cette fonction est remplacée par un macro hardcodé si accents.h n'est pas inclus.
{
	for (int compteur = 0; compteur < 20; compteur++)
	{
		if (liste_accents[compteur].valeur_min == car || liste_accents[compteur].valeur_maj == car)
		{return TRUE;}
	}
	
	return FALSE;
}


bool str_est_un_accent(char multicar[])
//Indique si la string reçue en paramètre est un des caractères accentués de liste_accents.
//Cette fonction n'est d'aucune utilité avec un input ncurses.
//Elle ne doit servir qu'à identifié un accent (préalablement isolé dans une string de longueur 3) dans une string.
//Cette fonction est remplacée par un macro hardcodé si accents.h n'est pas inclus.
{
	for (int compteur = 0; compteur < 20; compteur++)
	{
		if (!strcmp(liste_accents[compteur].minuscule, multicar) || !strcmp(liste_accents[compteur].majuscule, multicar))
		{return TRUE;}
	}
	
	return FALSE;
}
#endif


bool match_accent(int car, char accent[])
//Transforme un input accentué (format int) en un accent normal pouvant être placé dans une string (2 char).
//Cet accent (ou la lettre non modifiée si elle n'en est pas un) est storé dans le paramètre accent (passé par référence).
//Le paramètre accent devrait toujours pointer vers une string d'au moins 3 caractères de long.
//Renvoie TRUE en cas de succès (ça ne veut pas dire que c'est un accent!) ou FALSE en cas d'erreur.
{
	if (accent == NULL)
	{return FALSE;}
	
	if (!est_un_accent(car))
	{
		accent[0] = car;
		accent[1] = '\000';
		return TRUE;
	}
	
	#ifdef _ACCENTS_H
	int compteur;
	for (compteur = 0; car != liste_accents[compteur].valeur_min && car != liste_accents[compteur].valeur_maj && compteur < 20; compteur++) {}
	
	if (car == liste_accents[compteur].valeur_min)
	{strcpy(accent, liste_accents[compteur].minuscule);}
	else if (car == liste_accents[compteur].valeur_maj)
	{strcpy(accent, liste_accents[compteur].majuscule);}
	else
	{return FALSE;}
	#else
	accent[0] = '?';
	accent[1] = '\000';
	#endif
	
	return TRUE;
}


int longueur_str(char str[])
//Renvoie la longueur (nombre de caractères) d'une string.
//Cette fonction agit comme strlen(str), sauf qu'elle tient compte des accents (multicharacter constants).
//Renvoie -1 en cas d'erreur.
{
	int nbre_car = 1; //nombre de caractères que contient la string
	char multicar[3] = "  "; //string utilisée pour trouver les accents (multicharacter constants)
	
	if (str == NULL)
	{return -1;}
	
	if (!strcmp(str, ""))
	{return 0;} //renvoie 0 si str est vide
	
	//Calcul de la longueur de la string:
	for (int compteur = 1; str[compteur] != '\000'; compteur++)
	{
		multicar[0] = str[compteur - 1];
		multicar[1] = str[compteur];
		if (str_est_un_accent(multicar))
		{nbre_car--;}
		nbre_car++;
	}
	
	return nbre_car;
}


int relativise_pos(char str[], int pos)
//Trouve la véritable position du curseur dans une string en compensant pour les accents.
//Reçoit la string en question et la position supposée (position à l'écran) en paramètres.
//Renvoie pos + le nombre d'accents avant pos.
//Renvoie -1 si str est NULL, si pos < 0 ou si pos > longueur_str(str).
//Renvoie 0 si str est vide ou si pos == 0.
{
	int nbre_accents = 0; //nombre d'accents trouvés jusqu'à présent
	char multicar[3] = "  "; //string utilisée pour trouver les accents (multicharacter constants)
	
	if (str == NULL || pos < 0 || pos > longueur_str(str))
	{return -1;}
	
	if (!strcmp(str, "") || pos == 0)
	{return 0;}
	
	for (int compteur = 1; compteur <= pos + nbre_accents && str[compteur] != '\000'; compteur++)
	{
		multicar[0] = str[compteur - 1];
		multicar[1] = str[compteur];
		if (str_est_un_accent(multicar))
		{nbre_accents++;}
	}
	
	return pos + nbre_accents;
}


char* insere_car(char str[], int car, unsigned pos, unsigned taille_max)
//Insère le caractère car dans la string str (de taille maximale taille_max), à la position pos.
//Renvoie la nouvelle string en cas de succès (la string est aussi modifiée directement par adresse).
//Renvoie ERREUR (et ne fait rien) en cas d'erreur (surtout quand il n'y a pas de place dans str pour insérer car).
{
	char lettre_inseree[3];
	
	if (str == NULL || pos >= taille_max || strlen(str) + 1 >= taille_max || !match_accent(car, lettre_inseree))
	{return ERREUR;}
	
	char buffer_pos = str[pos]; //contient le prochain caractère à remettre dans la string
	char buffer_suivante = '\000'; //contient le dernier caractère présentement en file pour être remis dans la string (soit le 2e si ncar == 1 et le 3e si ncar == 2)
	char buffer_decalage = '\000'; //contient le 2e caractère présentement en file pour être remis dans la string si ncar == 2 (n'est pas utilisé si ncar == 1)
	int ncar = 1; //nbre de caractères de décalage (1 pour un caractère normal, 2 pour un accent)
	unsigned compteur;
	
	str[pos] = lettre_inseree[0];
	#ifdef _ACCENTS_H
	if (est_un_accent(car))
	{
		buffer_decalage = str[pos + 1];
		str[pos + 1] = lettre_inseree[1];
		ncar = 2;
	}
	#endif
	for (compteur = pos + ncar; compteur < strlen(str) + ncar; compteur++)
	{
		buffer_suivante = str[compteur];
		str[compteur] = buffer_pos;
		if (ncar == 1)	
		{buffer_pos = buffer_suivante;}
		else
		{buffer_pos = buffer_decalage; buffer_decalage = buffer_suivante;}
	}
	str[compteur + 1] = buffer_pos;
	str[compteur + 2] = '\000';
	
	return str;
}


char* efface_car(char str[], unsigned pos)
//Efface le caractère situé à la position pos dans la string str.
//Fonctionne aussi avec les accents.
//Renvoie la string mise à jour (mais ce n'est pas nécessaire de l'utiliser, parce qu'elle s'est aussi mise à jour automatiquement, puisque passée par référence).
//Renvoie ERREUR en cas d'erreur.
{
	if (str == NULL || pos > strlen(str))
	{erreur(51, "Vous avez essayé d'effacer un caractère non-existant..."); return ERREUR;}
	
	char multicar[3] = {str[pos], str[pos + 1], '\000'};
	
	if (str_est_un_accent(multicar))
	{
		for (int compteur = pos; str[compteur + 1] != '\000'; compteur++)
		{str[compteur] = str[compteur + 2];}
	}
	else
	{
		for (int compteur = pos; str[compteur] != '\000'; compteur++)
		{str[compteur] = str[compteur + 1];}
	}
	
	return str;
}


ligne* trouve_ligne(int num)
//Trouve une ligne à partir de son numéro (et renvoie un pointeur vers elle).
//Renvoie ERREUR en cas d'erreur.
{
	ligne* ln = &DEBUT_FICHIER;
	
	for (int compteur = 0; compteur < num && ln->suivant != NULL; compteur++)
	{ln = ln->suivant;}
	
	if (ln->num != num)
	{err_printf(101, "Un numéro de ligne trop grand a été demandé ou la liste de lignes s'est corrompue! Demandé=%d Trouvé=%d", num, ln->num); return ERREUR;}
	
	return ln;
}


ligne* trouve_tag(_tag tag, ligne* depart)
//Trouve et renvoie la première ligne ayant l'étiquette (tag) d'assigneé.
//Si depart est NULL, la recherche débutera au début du fichier. Sinon, elle débutera à la ligne donnée (inclusivement).
//Renvoie NULL s'il n'y a pas de ligne ayant cette étiquette d'assignée ou ERREUR en cas d'erreur.
{
	ligne* ln;
	
	if (depart == NULL)
	{ln = &DEBUT_FICHIER;}
	else if (depart == ERREUR || depart == &FIN_FICHIER)
	{return ERREUR;}
	else
	{ln = depart;}
	
	while (ln->tag != tag && ln->suivant != NULL)
	{ln = ln->suivant;}
	
	if (ln->tag != tag)
	{return NULL;}
	
	return ln;
}


int compter_lignes(ligne* ln)
//Compte (et renvoie) le nombre de lignes (à l'écran) occupées par la ligne reçue en paramètre.
//Ce nombre de ligne est aussi mis à jour dans la struct de la ligne (ln->multiligne).
//Renvoie 0 en cas d'erreur
{
	if (ln == NULL || ln == ERREUR || ln == &DEBUT_FICHIER || ln == &FIN_FICHIER)
	{return 0;}
	
	int longueur = longueur_str(ln->txt);
	if (longueur > NBRE_CAR_MAX_PAR_LIGNE)
	{longueur = NBRE_CAR_MAX_PAR_LIGNE;}
	
	ln->multiligne = longueur / (COLS - 5);
	if (ln->multiligne * (COLS - 5) < longueur || !longueur)
	{ln->multiligne++;}
	
	return ln->multiligne;
}


ligne* insere_ligne(ligne* ln_i, unsigned pos)
//Insère une nouvelle ligne juste avant le caractère occupant la position pos dans la ligne ln_i (tous 2 reçus en paramètres).
//Renvoie un ptr vers la nouvelle ligne ou ERREUR en cas d'erreur.
//Il sera nécessaire de rafraichir l'écran si on veut voir la modification.
{
	unsigned compteur;
	unsigned pos_locale = pos;
	
	//Vérification:
	if (ln_i == NULL || ln_i->suivant == NULL || ln_i->precedent == NULL || pos > strlen(ln_i->txt))
	{return ERREUR;}
	
	//Création de la nouvelle ligne:
	ligne* nouv_ln = (ligne*) malloc(sizeof(ligne));
	ligne* buffln = NULL;
	
	//Insère cette nouvelle ligne entre ln_i et la ligne suivante:
	nouv_ln->suivant = ln_i->suivant;
	nouv_ln->precedent = ln_i;
	nouv_ln->suivant->precedent = nouv_ln;
	ln_i->suivant = nouv_ln;
	
	//Décale les numéros de ligne:
	buffln = nouv_ln;
	for (int compteur = ln_i->num + 1; buffln->suivant != NULL; compteur++)
	{buffln->num = compteur; buffln = buffln->suivant;}
	buffln->num = buffln->precedent->num + 1;
	
	//Complétion de la ligne:
	strcpy(nouv_ln->txt, "");
	nouv_ln->tag = 0;
	nouv_ln->type = _pas_un_element;
	//nouv_ln->pos n'est pas setté (je mettrais quoi?)
	
	//Transfert du texte dans la nouvelle ligne (si nécessaire):
	for (compteur = 0; ln_i->txt[pos_locale] != '\000'; compteur++)
	{nouv_ln->txt[compteur] = ln_i->txt[pos_locale]; ln_i->txt[pos_locale] = '\000'; pos_locale++;}
	nouv_ln->txt[compteur] = '\000';
	
	//Recalcul des multilignes:
	if (compter_lignes(ln_i) == 0 || compter_lignes(nouv_ln) == 0)
	{return ERREUR;}
	
	//Revérification du tag/type (coloration syntaxique):
	verifie_syntaxe();
	
	return nouv_ln;
}


bool supprime_ligne(ligne* ln)
//Supprime la ligne reçue en paramètre.
//Si celle-ci n'est pas vide, son contenu sera appendé à la ligne précédente.
//Renvoie TRUE en cas de succès et FALSE en cas d'erreur.
{
	//Erreur:
	if (ln == NULL || ln == ERREUR || ln == &DEBUT_FICHIER || ln == &FIN_FICHIER)
	{erreur(52, "Vous avez essayé de supprimer une ligne qui n'existe pas!"); return FALSE;}
	
	//Modification de la liste des lignes:
	ligne* buffln = ln->suivant;
	ln->precedent->suivant = buffln;
	buffln->precedent = ln->precedent;
	
	//Décalage des numéros de ligne:
	for (int compteur = ln->num; buffln->suivant != NULL; compteur++)
	{buffln->num = compteur; buffln = buffln->suivant;}
	buffln->num = buffln->precedent->num + 1;
	
	//Transcription du texte de la ligne supprimée:
	if (strlen(ln->precedent->txt) + strlen(ln->txt) > NBRE_CAR_MAX_PAR_LIGNE) //lignes trop longues
	{
		erreur(53, "Ces lignes sont trop longues pour être appendées ensemble sans pertes! Ne sauvegardez pas ce document!");
		ln->txt[NBRE_CAR_MAX_PAR_LIGNE - strlen(ln->precedent->txt)] = '\000'; //Je vais couper ça là, et tant pis pour le reste... (Des lignes aussi longues ne devrait quand même pas être si fréquentes que ça, non?)
	}
	strcat(ln->precedent->txt, ln->txt);
	
	//Vérification de la coloration syntaxique (tag/type):
	verifie_syntaxe();
	
	//Libération de la mémoire:
	free(ln);
	return TRUE;
}


void verifie_syntaxe()
//Vérifie si la syntaxe du fichier (au complet) correspond à celle d'une liste d'objets et assigne un tag et/ou un type à chaque ligne.
//Est appelé à la lecture du fichier (donc à l'initialisation du programme) et à la moindre modification de celui-ci.
{
	bool override = 0;
	unsigned compteur = 0;
	bool txt_a_ignorer = 1; //indique si on se trouve entre (0) ou avant/après (1) le -DÉBUT- et la -FIN- dans le fichier
	int fin_commentaire = -1; //indique si un commentaire est terminé (et à quelle position dans le texte il se termine) (vaut 0 si le commentaire dure sur plusieurs lignes)
	int fin_txt_ignore = -1; //même chose, mais pour le texte ignoré (entre *)
	ligne* ln = DEBUT_FICHIER.suivant;
	
	
	while (ln != &FIN_FICHIER)
	{
		override = 0;
		
		//Mise en place du tag:
		ln->tag = 0; //ne devrait jamais être le cas, sauf pour les lignes vides
		if (!strcmp(ln->txt, "-DÉBUT-") && txt_a_ignorer)
		{txt_a_ignorer = 0; ln->tag = DEBUT;}
		else if (txt_a_ignorer && strcmp(ln->txt, "") != 0)
		{ln->tag = IGNORE;}
		else
		{
			if (!strcmp(ln->txt, "-FIN-"))
			{txt_a_ignorer = 1; ln->tag = FIN;}
			else if (!strcmp(ln->txt, "Modèle:") || !strcmp(ln->txt, "Modele:") || !strcmp(ln->txt, "modèle:") || !strcmp(ln->txt, "modele:"))
			{ln->tag = _modele;}
			else if ((ln->txt[0] == '*' && ln->txt[1] == ' ') || !fin_txt_ignore)
			{
				ln->tag = _ignore;
				for (fin_txt_ignore = 0; ln->txt[fin_txt_ignore] != '\000' && !(ln->txt[fin_txt_ignore] == '*' && ln->txt[fin_txt_ignore - 1] == ' ' \
					&& (ln->txt[fin_txt_ignore - 1] == ' ' || ln->txt[fin_txt_ignore - 1] == '\000')); fin_txt_ignore++) {}
				if (ln->txt[fin_txt_ignore] == '\000')
				{fin_txt_ignore = 0;}
			}
			else if ((ln->txt[0] == '/' && ln->txt[1] == '*' && ln->txt[2] == ' ') || !fin_commentaire)
			{
				ln->tag = _commentaire;
				for (fin_commentaire = 0; ln->txt[fin_commentaire] != '\000' && !(ln->txt[fin_commentaire] == '*' && ln->txt[fin_commentaire + 1] == '/'); fin_commentaire++) {}
				if (ln->txt[fin_commentaire] == '\000')
				{fin_commentaire = 0;}
			}
			else
			{
				if (ln->txt[0] == '@')
				{override = 1;}
				
				for (compteur = 3; ln->txt[compteur] != ':' && ln->txt[compteur] != '\000' && ln->txt[compteur] != ' '; compteur++) {}
				
				if (ln->txt[compteur] == ':')
				{
					if (override)
					{ln->tag = _element_ovr;}
					else
					{ln->tag = _element;}
				}
				else if (ln->txt[compteur] == ' ' && ln->txt[compteur + 1] == '-' && ln->txt[compteur + 2] == '>' && ln->txt[compteur + 3] == ' ')
				{ln->tag = _override;}
				else if (ln->txt[compteur] != ' ')
				{
					if (override)
					{ln->tag = _override;}
					else if (strcmp(ln->txt, "") != 0)
					{ln->tag = _pointeur;}
				}
			}
		}
		
		//Identification du type d'élément:
		if (ln->tag == _element || ln->tag == _element_ovr)
		{
			if (tolower(ln->txt[0 + override]) == 'n' && ln->txt[1 + override] == 'o' && ln->txt[2 + override] == 'm')
			{ln->type = _nom;} //nom
			else if (tolower(ln->txt[0 + override]) == 'd' && ln->txt[1 + override] == 'e' && ln->txt[2 + override] == 's' && ln->txt[3 + override] == 'c' && ln->txt[4 + override] == 'r')
			{ln->type = _descr;} //descr
			else if (tolower(ln->txt[0 + override]) == 'd' && (ln->txt[1 + override] == 'e' || ln->txt[1 + override] == -61) && (ln->txt[2 + override] == 't' || ln->txt[2 + override] == -87) \
				&& (ln->txt[3 + override] == 'a' || (ln->txt[3 + override] == 't' && ln->txt[4 + override] == 'a')))
			{ln->type = _detail;} //détail / detail
			else if (tolower(ln->txt[0 + override]) == 'r' && ln->txt[1 + override] == 'u' && ln->txt[2 + override] == 'm')
			{ln->type = _rum;} //rum
			else if (tolower(ln->txt[0 + override]) == 'i' && ln->txt[1 + override] == 'm' && ln->txt[2 + override] == 'g')
			{ln->type = _img;} //img
			else if (tolower(ln->txt[0 + override]) == 'e' && ln->txt[1 + override] == 'n' && ln->txt[2 + override] == 'd')
			{ln->type = _endroit;} //end
			else if (tolower(ln->txt[0 + override]) == 'p' && ln->txt[1 + override] == 'o' && ln->txt[2 + override] == 'i' && ln->txt[3 + override] == 'd' && ln->txt[4 + override] == 's')
			{ln->type = _poids;} //poids
			else if (tolower(ln->txt[0 + override]) == 'c' && ln->txt[1 + override] == 'a' && ln->txt[2 + override] == 'p')
			{ln->type = _cap;} //cap
			else if (tolower(ln->txt[0 + override]) == 'v' && ln->txt[1 + override] == 'i' && ln->txt[2 + override] == 'e')
			{ln->type = _vie;} //vie
			else if (tolower(ln->txt[0 + override]) == 'c' && ln->txt[1 + override] == 'o' && ln->txt[2 + override] == 'n' && ln->txt[3 + override] == 'f')
			{ln->type = _conf;} //conf
			else if (tolower(ln->txt[0 + override]) == 'd' && ((ln->txt[1 + override] == 'e' && ln->txt[2 + override] == 'g') || (ln->txt[1 + override] == -61 && ln->txt[2 + override] == -87 \
				&& ln->txt[3 + override] == 'g')))
			{ln->type = _deg;} //dég / deg
			else if (tolower(ln->txt[0 + override]) == 'l' && ln->txt[1 + override] == 'u' && ln->txt[2 + override] == 'm')
			{ln->type = _lum;} //lum
			else if (tolower(ln->txt[0 + override]) == 'f' && ln->txt[1 + override] == 'a' && ln->txt[2 + override] == 'i' && ln->txt[3 + override] == 'm')
			{ln->type = _faim;} //faim
			else if ((tolower(ln->txt[0 + override]) == 'e' && ln->txt[1 + override] == 't' && ln->txt[2 + override] == 'a' && ln->txt[3 + override] == 't') \
				|| (ln->txt[0 + override] == -61 && (ln->txt[1 + override] == -87 || ln->txt[1 + override] == -119) && ln->txt[2 + override] == 't' && ln->txt[3 + override] == 'a' \
				&& ln->txt[4 + override] == 't'))
			{ln->type = _etat;} //état / etat
			else if (tolower(ln->txt[0 + override]) == 'c' && ln->txt[1 + override] == 'o' && ln->txt[2 + override] == 'n' && ln->txt[3 + override] == 'd')
			{ln->type = _cond;} //cond
			else if (tolower(ln->txt[0 + override]) == 'd' && ln->txt[1 + override] == 'e' && ln->txt[2 + override] == 's' && ln->txt[3 + override] == 't')
			{ln->type = _dest;} //dest
			else if (tolower(ln->txt[0 + override]) == 'i' && ln->txt[1 + override] == 'm' && ln->txt[2 + override] == 'p')
			{ln->type = _imp;} //imp
			else if (tolower(ln->txt[0 + override]) == 'd' && ln->txt[1 + override] == 'i' && ln->txt[2 + override] == 's' && ln->txt[3 + override] == 't')
			{ln->type = _dist;} //dist
			else if (tolower(ln->txt[0 + override]) == 'a' && ln->txt[1 + override] == 'm' && ln->txt[2 + override] == 'b')
			{ln->type = _amb;} //amb
			else if (tolower(ln->txt[0 + override]) == 'p' && ln->txt[1 + override] == 't' && ln->txt[2 + override] == 'v')
			{ln->type = _ptv;} //ptv
			else if (tolower(ln->txt[0 + override]) == 'o' && ln->txt[1 + override] == 'u' && ln->txt[2 + override] == 'v')
			{ln->type = _ouv;} //ouv
			else if (tolower(ln->txt[0 + override]) == 'f' && ln->txt[1 + override] == 'e' && ln->txt[2 + override] == 'r')
			{ln->type = _fer;} //fer
			else if (tolower(ln->txt[0 + override]) == 'd' && ((ln->txt[1 + override] == 'e' && ln->txt[2 + override] == 'b') || (ln->txt[1 + override] == -61 && ln->txt[2 + override] == -87 \
				&& ln->txt[3 + override] == 'b')))
			{ln->type = _deb;} //déb / deb
			else if (tolower(ln->txt[0 + override]) == 'b' && ln->txt[1 + override] == 'a' && ln->txt[2 + override] == 'r')
			{ln->type = _bar;} //bar
			else if (tolower(ln->txt[0 + override]) == 'i' && ln->txt[1 + override] == 'n' && ln->txt[2 + override] == 't')
			{ln->type = _int;} //int
			else if (tolower(ln->txt[0 + override]) == 'a' && ln->txt[1 + override] == '-' && tolower(ln->txt[2 + override]) == 'd')
			{ln->type = _a_d;} //a-d
			else if (tolower(ln->txt[0 + override]) == 'd' && ln->txt[1 + override] == '-' && tolower(ln->txt[2 + override]) == 'i')
			{ln->type = _d_i;} //d-i
			else if (tolower(ln->txt[0 + override]) == 'a' && ln->txt[1 + override] == '-' && tolower(ln->txt[2 + override]) == 'a')
			{ln->type = _a_a;} //a-a
			else
			{ln->type = _inconnu;}
		}
		else
		{ln->type = _pas_un_element;}
		
		ln = ln->suivant;
	}
}


ligne* init_ligne(int num)
//Initialise la structure d'une ligne (en la lisant dans le fichier) lorsque le numéro est positif (ne pas oublier d'ensuite vérifier la synatxe du fichier).
//Libère la structure d'une ligne si le numéro est négatif.
//Renvoie la dernière ligne du fichier si le numéro est 0.
//Renvoie la nouvelle ligne initialisée si le numéro est positif.
//Renvoie NULL si le numéro est négatif.
//Renvoie ERREUR en cas d'erreur.
{
	static int num_ligne_max = 0; //nbre de lignes lues du fichier jusqu'à date
	static fpos_t pos_max;
	static ligne* anc_ligne = NULL; //ancienne ligne (dernière crée)
	ligne* nouv_ligne; //nouvelle ligne (présentement utilisé)
	char car_lu = EOF;
	unsigned compteur = 0;
	
	if (num == 0)
	{return FIN_FICHIER.precedent;}
	
	if (num > 0)
	{
		if (num <= num_ligne_max)
		{erreur(11, "ligne déjà lue"); return ERREUR;}
		
		if (anc_ligne != NULL && anc_ligne->suivant == &FIN_FICHIER)
		{return NULL;} //fin du fichier
		
		nouv_ligne = (ligne*) malloc(sizeof(ligne));
		if (anc_ligne == NULL) //1ère fois, 1ère ligne
		{anc_ligne = &DEBUT_FICHIER; fgetpos(fichier, &pos_max); DEBUT_FICHIER.precedent = NULL; DEBUT_FICHIER.num = 0; FIN_FICHIER.num = 0; FIN_FICHIER.precedent = NULL; FIN_FICHIER.suivant = NULL;}
		fsetpos(fichier, &pos_max);
		nouv_ligne->pos = pos_max;
		nouv_ligne->precedent = &DEBUT_FICHIER;
		nouv_ligne->num = num;
		
		car_lu = fgetc(fichier);
		for (compteur = 0; car_lu != '\n' && car_lu != EOF && compteur < NBRE_CAR_MAX_PAR_LIGNE + 1; compteur++)
		{
			nouv_ligne->txt[compteur] = car_lu;
			car_lu = fgetc(fichier);
		}
		nouv_ligne->txt[compteur] = '\000';
		while (car_lu != '\n' && car_lu != EOF && compteur - (strlen(nouv_ligne->txt) - longueur_str(nouv_ligne->txt)) < NBRE_CAR_MAX_PAR_LIGNE + 1)
		{						//Note: La différence entre strlen() et longueur_str() correspond au nombre de caractères accentués dans la string (vu qu'ils prennent 2 "caractères" de "large" par vrai caractère...)
			nouv_ligne->txt[compteur] = car_lu;
			car_lu = fgetc(fichier);
			compteur++;
			nouv_ligne->txt[compteur] = '\000';
		}
		if (compteur >= NBRE_CAR_MAX_PAR_LIGNE + strlen(nouv_ligne->txt) - longueur_str(nouv_ligne->txt))
		{
			while (car_lu != '\n' && car_lu != EOF)
			{car_lu = fgetc(fichier);}
		}
		
		anc_ligne->suivant = nouv_ligne;
		nouv_ligne->precedent = anc_ligne;
		if (car_lu == EOF)
		{nouv_ligne->suivant = &FIN_FICHIER; FIN_FICHIER.precedent = nouv_ligne; FIN_FICHIER.num = nouv_ligne->num + 1;} //fin du fichier
		else
		{nouv_ligne->suivant = NULL;}
		
		fgetpos(fichier, &pos_max);
		num_ligne_max++;
		anc_ligne = nouv_ligne;
		
		return nouv_ligne;
	}
	
	if (num < 0)
	{
		num = -num; //remet le numéro de ligne positif
		nouv_ligne = trouve_ligne(num); //trouve la ligne en question
		
		if (nouv_ligne == ERREUR || nouv_ligne == NULL)
		{return ERREUR;}
		
		if ((nouv_ligne->precedent == NULL || nouv_ligne->precedent == &DEBUT_FICHIER) && nouv_ligne->suivant != NULL) //pas de ligne précédente et il y a une ligne suivante
		{nouv_ligne->suivant->precedent = NULL;}
		else if ((nouv_ligne->suivant == NULL || nouv_ligne->suivant == &FIN_FICHIER) && nouv_ligne->precedent != NULL) //pas de ligne suivante ni de ligne précédente
		{nouv_ligne->precedent->suivant = NULL;}
		else //erreur (il y a une ligne précédente?!)
		{err_printf(12, "Impossible de déinitialiser cette ligne (%d): Elle n'est ni au début ni à la fin de la liste.", num); return ERREUR;}
		
		free(nouv_ligne);
		return NULL;
	}
	
	//Ce code ne sera jamais exécuté, mais gcc me met un warning, donc...
	return ERREUR;
}