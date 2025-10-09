#include "outils_graphiques.c"


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
		if (!strcmp(multicar, "é") || !strcmp(multicar, "è") || !strcmp(multicar, "ê") || !strcmp(multicar, "ë") || !strcmp(multicar, "à") || !strcmp(multicar, "â") || !strcmp(multicar, "ä") || !strcmp(multicar, "î") \
			|| !strcmp(multicar, "ï") || !strcmp(multicar, "ô") || !strcmp(multicar, "ö") || !strcmp(multicar, "ù") || !strcmp(multicar, "û") || !strcmp(multicar, "ç") || !strcmp(multicar, "É") || !strcmp(multicar, "È") \
			|| !strcmp(multicar, "Ê") || !strcmp(multicar, "Ë") || !strcmp(multicar, "À") || !strcmp(multicar, "Â") || !strcmp(multicar, "Ä") || !strcmp(multicar, "Î") || !strcmp(multicar, "Ï") || !strcmp(multicar, "Ô") \
			|| !strcmp(multicar, "Ö") || !strcmp(multicar, "Ù") || !strcmp(multicar, "Û") || !strcmp(multicar, "Ç"))
		{nbre_car--;}
		nbre_car++;
	}
	
	return nbre_car;
}


ligne* trouve_ligne(int num)
//Trouve une ligne à partir de son numéro (et renvoie un pointeur vers elle).
//Renvoie ERREUR en cas d'erreur.
{
	ligne* ln = &DEBUT_FICHIER;
	
	for (int compteur = 0; compteur < num && ln->suivant != NULL; compteur++)
	{ln = ln->suivant;}
	
	if (ln->num != num)
	{err_printf(101, "Un numéro de ligne trop grand a été demandé ou la liste de ligne s'est corrompue! Demandé=%d Trouvé=%d", num, ln->num); return ERREUR;}
	
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


ligne* init_ligne(int num)
//Initialise la structure d'une ligne (en la lisant dans le fichier) lorsque le numéro est positif.
//Libère la structure d'une ligne si le numéro est négatif.
//Renvoie la dernière ligne initialisée si le numéro est 0.
//Renvoie la nouvelle ligne initialisée si le numéro est positif.
//Sinon (numéro négatif ou erreur), renvoie NULL.
{
	static int num_ligne_max = 0; //nbre de lignes lues du fichier jusqu'à date
	static fpos_t pos_max;
	static ligne* anc_ligne = NULL; //ancienne ligne (dernière crée)
	static bool txt_a_ignorer = 1; //indique si on se trouve entre (0) ou avant/après (1) le -DÉBUT- et la -FIN- dans le fichier
	static int fin_commentaire = -1; //indique si un commentaire est terminé (et à quelle position dans le texte il se termine) (vaut 0 si le commentaire dure sur plusieurs lignes)
	static int fin_txt_ignore = -1; //même chose, mais pour le texte ignoré (entre *)
	ligne* nouv_ligne; //nouvelle ligne (présentement utilisé)
	char car_lu = EOF;
	unsigned compteur = 0;
	bool override = 0;
	
	if (num == 0)
	{return anc_ligne;}
	
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
		{											//Note: La différence entre strlen() et longueur_str() correspond au nombre de caractères accentués dans la string (vu qu'ils prennent 2 "caractères" de "large" par vrai caractère...)
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
		
		//Mise en place des étiquettes:
		nouv_ligne->tag = 0; //ne devrait jamais être le cas, sauf pour les lignes vides
		if (!strcmp(nouv_ligne->txt, "-DÉBUT-") && txt_a_ignorer)
		{txt_a_ignorer = 0; nouv_ligne->tag = DEBUT;}
		else if (txt_a_ignorer)
		{nouv_ligne->tag = IGNORE;}
		else
		{
			if (!strcmp(nouv_ligne->txt, "-FIN-"))
			{txt_a_ignorer = 1; nouv_ligne->tag = FIN;}
			else if (!strcmp(nouv_ligne->txt, "Modèle:") || !strcmp(nouv_ligne->txt, "Modele:") || !strcmp(nouv_ligne->txt, "modèle:") || !strcmp(nouv_ligne->txt, "modele:"))
			{nouv_ligne->tag = _modele;}
			else if ((nouv_ligne->txt[0] == '*' && nouv_ligne->txt[1] == ' ') || !fin_txt_ignore)
			{
				nouv_ligne->tag = _ignore;
				for (fin_txt_ignore = 0; nouv_ligne->txt[fin_txt_ignore] != '\000' && !(nouv_ligne->txt[fin_txt_ignore] == '*' && nouv_ligne->txt[fin_txt_ignore - 1] == ' ' \
					&& (nouv_ligne->txt[fin_txt_ignore - 1] == ' ' || nouv_ligne->txt[fin_txt_ignore - 1] == '\000')); fin_txt_ignore++) {}
				if (nouv_ligne->txt[fin_txt_ignore] == '\000')
				{fin_txt_ignore = 0;}
			}
			else if ((nouv_ligne->txt[0] == '/' && nouv_ligne->txt[1] == '*' && nouv_ligne->txt[2] == ' ') || !fin_commentaire)
			{
				nouv_ligne->tag = _commentaire;
				for (fin_commentaire = 0; nouv_ligne->txt[fin_commentaire] != '\000' && !(nouv_ligne->txt[fin_commentaire] == '*' && nouv_ligne->txt[fin_commentaire + 1] == '/'); fin_commentaire++) {}
				if (nouv_ligne->txt[fin_commentaire] == '\000')
				{fin_commentaire = 0;}
			}
			else
			{
				if (nouv_ligne->txt[0] == '@')
				{override = 1;}
				
				for (compteur = 3; nouv_ligne->txt[compteur] != ':' && nouv_ligne->txt[compteur] != '\000' && nouv_ligne->txt[compteur] != ' '; compteur++) {}
				
				if (nouv_ligne->txt[compteur] == ':')
				{
					if (override)
					{nouv_ligne->tag = _element_ovr;}
					else
					{nouv_ligne->tag = _element;}
				}
				else if (nouv_ligne->txt[compteur] == ' ' && nouv_ligne->txt[compteur + 1] == '-' && nouv_ligne->txt[compteur + 2] == '>' && nouv_ligne->txt[compteur + 3] == ' ')
				{nouv_ligne->tag = _override;}
				else if (nouv_ligne->txt[compteur] != ' ')
				{
					if (override)
					{nouv_ligne->tag = _override;}
					else
					{nouv_ligne->tag = _pointeur;}
				}
			}
		}
		
		//Identification du type d'élément:
		if (nouv_ligne->tag == _element || nouv_ligne->tag == _element_ovr)
		{
			if (tolower(nouv_ligne->txt[0 + override]) == 'n' && nouv_ligne->txt[1 + override] == 'o' && nouv_ligne->txt[2 + override] == 'm')
			{nouv_ligne->type = _nom;} //nom
			else if (tolower(nouv_ligne->txt[0 + override]) == 'd' && nouv_ligne->txt[1 + override] == 'e' && nouv_ligne->txt[2 + override] == 's' && nouv_ligne->txt[3 + override] == 'c' && nouv_ligne->txt[4 + override] == 'r')
			{nouv_ligne->type = _descr;} //descr
			else if (tolower(nouv_ligne->txt[0 + override]) == 'd' && (nouv_ligne->txt[1 + override] == 'e' || nouv_ligne->txt[1 + override] == -61) && (nouv_ligne->txt[2 + override] == 't' || nouv_ligne->txt[2 + override] == -87) \
				&& (nouv_ligne->txt[3 + override] == 'a' || (nouv_ligne->txt[3 + override] == 't' && nouv_ligne->txt[4 + override] == 'a')))
			{nouv_ligne->type = _detail;} //détail / detail
			else if (tolower(nouv_ligne->txt[0 + override]) == 'r' && nouv_ligne->txt[1 + override] == 'u' && nouv_ligne->txt[2 + override] == 'm')
			{nouv_ligne->type = _rum;} //rum
			else if (tolower(nouv_ligne->txt[0 + override]) == 'i' && nouv_ligne->txt[1 + override] == 'm' && nouv_ligne->txt[2 + override] == 'g')
			{nouv_ligne->type = _img;} //img
			else if (tolower(nouv_ligne->txt[0 + override]) == 'e' && nouv_ligne->txt[1 + override] == 'n' && nouv_ligne->txt[2 + override] == 'd')
			{nouv_ligne->type = _endroit;} //end
			else if (tolower(nouv_ligne->txt[0 + override]) == 'p' && nouv_ligne->txt[1 + override] == 'o' && nouv_ligne->txt[2 + override] == 'i' && nouv_ligne->txt[3 + override] == 'd' && nouv_ligne->txt[4 + override] == 's')
			{nouv_ligne->type = _poids;} //poids
			else if (tolower(nouv_ligne->txt[0 + override]) == 'c' && nouv_ligne->txt[1 + override] == 'a' && nouv_ligne->txt[2 + override] == 'p')
			{nouv_ligne->type = _cap;} //cap
			else if (tolower(nouv_ligne->txt[0 + override]) == 'v' && nouv_ligne->txt[1 + override] == 'i' && nouv_ligne->txt[2 + override] == 'e')
			{nouv_ligne->type = _vie;} //vie
			else if (tolower(nouv_ligne->txt[0 + override]) == 'c' && nouv_ligne->txt[1 + override] == 'o' && nouv_ligne->txt[2 + override] == 'n' && nouv_ligne->txt[3 + override] == 'f')
			{nouv_ligne->type = _conf;} //conf
			else if (tolower(nouv_ligne->txt[0 + override]) == 'd' && ((nouv_ligne->txt[1 + override] == 'e' && nouv_ligne->txt[2 + override] == 'g') || (nouv_ligne->txt[1 + override] == -61 && nouv_ligne->txt[2 + override] == -87 \
				&& nouv_ligne->txt[3 + override] == 'g')))
			{nouv_ligne->type = _deg;} //dég / deg
			else if (tolower(nouv_ligne->txt[0 + override]) == 'l' && nouv_ligne->txt[1 + override] == 'u' && nouv_ligne->txt[2 + override] == 'm')
			{nouv_ligne->type = _lum;} //lum
			else if (tolower(nouv_ligne->txt[0 + override]) == 'f' && nouv_ligne->txt[1 + override] == 'a' && nouv_ligne->txt[2 + override] == 'i' && nouv_ligne->txt[3 + override] == 'm')
			{nouv_ligne->type = _faim;} //faim
			else if ((tolower(nouv_ligne->txt[0 + override]) == 'e' && nouv_ligne->txt[1 + override] == 't' && nouv_ligne->txt[2 + override] == 'a' && nouv_ligne->txt[3 + override] == 't') \
				|| (nouv_ligne->txt[0 + override] == -61 && (nouv_ligne->txt[1 + override] == -87 || nouv_ligne->txt[1 + override] == -119) && nouv_ligne->txt[2 + override] == 't' && nouv_ligne->txt[3 + override] == 'a' \
				&& nouv_ligne->txt[4 + override] == 't'))
			{nouv_ligne->type = _etat;} //état / etat
			else if (tolower(nouv_ligne->txt[0 + override]) == 'c' && nouv_ligne->txt[1 + override] == 'o' && nouv_ligne->txt[2 + override] == 'n' && nouv_ligne->txt[3 + override] == 'd')
			{nouv_ligne->type = _cond;} //cond
			else if (tolower(nouv_ligne->txt[0 + override]) == 'd' && nouv_ligne->txt[1 + override] == 'e' && nouv_ligne->txt[2 + override] == 's' && nouv_ligne->txt[3 + override] == 't')
			{nouv_ligne->type = _dest;} //dest
			else if (tolower(nouv_ligne->txt[0 + override]) == 'i' && nouv_ligne->txt[1 + override] == 'm' && nouv_ligne->txt[2 + override] == 'p')
			{nouv_ligne->type = _imp;} //imp
			else if (tolower(nouv_ligne->txt[0 + override]) == 'd' && nouv_ligne->txt[1 + override] == 'i' && nouv_ligne->txt[2 + override] == 's' && nouv_ligne->txt[3 + override] == 't')
			{nouv_ligne->type = _dist;} //dist
			else if (tolower(nouv_ligne->txt[0 + override]) == 'a' && nouv_ligne->txt[1 + override] == 'm' && nouv_ligne->txt[2 + override] == 'b')
			{nouv_ligne->type = _amb;} //amb
			else if (tolower(nouv_ligne->txt[0 + override]) == 'p' && nouv_ligne->txt[1 + override] == 't' && nouv_ligne->txt[2 + override] == 'v')
			{nouv_ligne->type = _ptv;} //ptv
			else if (tolower(nouv_ligne->txt[0 + override]) == 'o' && nouv_ligne->txt[1 + override] == 'u' && nouv_ligne->txt[2 + override] == 'v')
			{nouv_ligne->type = _ouv;} //ouv
			else if (tolower(nouv_ligne->txt[0 + override]) == 'f' && nouv_ligne->txt[1 + override] == 'e' && nouv_ligne->txt[2 + override] == 'r')
			{nouv_ligne->type = _fer;} //fer
			else if (tolower(nouv_ligne->txt[0 + override]) == 'd' && ((nouv_ligne->txt[1 + override] == 'e' && nouv_ligne->txt[2 + override] == 'b') || (nouv_ligne->txt[1 + override] == -61 && nouv_ligne->txt[2 + override] == -87 \
				&& nouv_ligne->txt[3 + override] == 'b')))
			{nouv_ligne->type = _deb;} //déb / deb
			else if (tolower(nouv_ligne->txt[0 + override]) == 'b' && nouv_ligne->txt[1 + override] == 'a' && nouv_ligne->txt[2 + override] == 'r')
			{nouv_ligne->type = _bar;} //bar
			else if (tolower(nouv_ligne->txt[0 + override]) == 'i' && nouv_ligne->txt[1 + override] == 'n' && nouv_ligne->txt[2 + override] == 't')
			{nouv_ligne->type = _int;} //int
			else if (tolower(nouv_ligne->txt[0 + override]) == 'a' && nouv_ligne->txt[1 + override] == '-' && tolower(nouv_ligne->txt[2 + override]) == 'd')
			{nouv_ligne->type = _a_d;} //a-d
			else if (tolower(nouv_ligne->txt[0 + override]) == 'd' && nouv_ligne->txt[1 + override] == '-' && tolower(nouv_ligne->txt[2 + override]) == 'i')
			{nouv_ligne->type = _d_i;} //d-i
			else if (tolower(nouv_ligne->txt[0 + override]) == 'a' && nouv_ligne->txt[1 + override] == '-' && tolower(nouv_ligne->txt[2 + override]) == 'a')
			{nouv_ligne->type = _a_a;} //a-a
			else
			{nouv_ligne->type = _inconnu;}
		}
		else
		{nouv_ligne->type = _pas_un_element;}
		
		return nouv_ligne;
	}
	
	if (num < 0)
	{
		num = - num; //remet le numéro de ligne positif
		nouv_ligne = trouve_ligne(num); //trouve la ligne en question
		
		if (nouv_ligne == ERREUR || nouv_ligne == NULL)
		{return ERREUR;}
		
		if ((nouv_ligne->precedent == NULL || nouv_ligne->precedent == &DEBUT_FICHIER) && nouv_ligne->suivant != NULL)
		{nouv_ligne->suivant->precedent = NULL;}
		else if ((nouv_ligne->suivant == NULL || nouv_ligne->suivant == &FIN_FICHIER) && nouv_ligne->precedent != NULL)
		{nouv_ligne->precedent->suivant = NULL;}
		else
		{err_printf(12, "Impossible de déinitialiser cette ligne (%d): Elle n'est ni au début ni à la fin de la liste.", num); return ERREUR;}
		
		free(nouv_ligne);
		return NULL;
	}
	
	//Ce code ne sera jamais exécuté, mais gcc me met un warning, donc...
	return NULL;
}