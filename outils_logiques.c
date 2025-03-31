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
			|| !strcmp(multicar, "ï") || !strcmp(multicar, "ô") || !strcmp(multicar, "ö") || !strcmp(multicar, "ù") || !strcmp(multicar, "û") || !strcmp(multicar, "ç"))
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
	
	ln->multiligne = longueur_str(ln->txt) / (COLS - 5);
	if (ln->multiligne * (COLS - 5) < longueur_str(ln->txt) || !longueur_str(ln->txt))
	{ln->multiligne++;}
	
	return ln->multiligne;
}


ligne* init_ligne(int num)
//Initialise la structure d'une ligne (en la lisant dans le fichier) lorsque le numéro est positif.
//Libère la structure d'une ligne si le numéro est négatif.
//Renvoie la dernière ligne initialisée si le numéro est 0.
//Sinon, renvoie le numéro de la ligne initialisée ou libérée.
{
	static int num_ligne_max = 0; //nbre de lignes lues du fichier jusqu'à date
	static fpos_t pos_max;
	static ligne* anc_ligne = NULL; //ancienne ligne (dernière crée)
	static bool txt_a_ignorer = 1; //indique si on se trouve entre (0) ou avant/après (1) le -DÉBUT- et la -FIN- dans le fichier
	ligne* nouv_ligne; //nouvelle ligne (présentement utilisé)
	char car_lu = EOF;
	int compteur = 0;
	
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
		for (compteur = 0; car_lu != '\n' && car_lu != EOF; compteur++)
		{
			nouv_ligne->txt[compteur] = car_lu;
			car_lu = fgetc(fichier);
		}
		nouv_ligne->txt[compteur] = '\000';
		//Trouve son nbre de lignes et sa position à l'écran (TODO)
		
		anc_ligne->suivant = nouv_ligne;
		nouv_ligne->precedent = anc_ligne;
		if (car_lu == EOF)
		{nouv_ligne->suivant = &FIN_FICHIER; FIN_FICHIER.precedent = nouv_ligne; FIN_FICHIER.num = nouv_ligne->num + 1;} //fin du fichier
		else
		{nouv_ligne->suivant = NULL;}
		
		fgetpos(fichier, &pos_max);
		num_ligne_max++;
		anc_ligne = nouv_ligne;
		
		if (!strcmp(nouv_ligne->txt, "-DÉBUT-") && txt_a_ignorer)
		{txt_a_ignorer = 0; nouv_ligne->tag = DEBUT;}
		else if (!strcmp(nouv_ligne->txt, "-FIN-") && !txt_a_ignorer)
		{txt_a_ignorer = 1; nouv_ligne->tag = FIN;}
		else if (txt_a_ignorer)
		{nouv_ligne->tag = IGNORE;}
		//À COMPLÉTER!
		else
		{nouv_ligne->tag = 0;} //ne devrait jamais arriver...
		
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
}