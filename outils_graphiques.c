#include "coloration.h"


void print_msg(char msg[])
//Affiche un message dans la barre d'état.
//Efface tout message de la barre d'état si msg est NULL.
{
	char message[300];
	
	standend();
	if (msg == NULL)
	{mvhline(LINES - 3, 0, ACS_HLINE, COLS); barre_dispo = 1; move(y, x); return;}
	
	barre_dispo = 0;
	mvhline(LINES - 3, 0, ACS_HLINE, COLS);
	sprintf(message, " %s ", msg);
	if (longueur_str(message) > COLS - 2 && COLS - 2 < 400)
	{message[COLS - 5] = '\000'; strcat(message, "...");}
	attrset(COLOR_PAIR(10));
	mvaddstrc(LINES - 3, message);
	standend();
	move(y, x);
}


int rafraichir()
//Redessine l'écran au complet (avec les nouvelles lignes et tout).
//Renvoie le numéro de la dernière ligne affichée (aussi disponible comme variable globale) ou 0 en cas d'erreur.
{
	int num = 0; //numéro de la ligne affichée
	int xi = x; //position en x avant le rafraichissement de l'écran
	int yi = y; //position en y avant le rafraichissement de l'écran
	ligne* ln = trouve_ligne(premiere_ligne); //ligne à afficher
	
	bordures();
	mv(1, 4);
	do
	{
		derniere_ligne = num;
		num = afficher_ligne(ln);
		ln = ln->suivant;
		mv(y, 4);
	} while (num > 0);
	
	derniere_ligne -= num; //ajoute 1 au numéro de la dernière ligne si celle-ci ne s'est pas affichée au complet (sinon, on n'a pas le bon numéro...)
	mv(yi, xi); //revient à la position initiale
	return derniere_ligne;
}


void bordures()
//Dessine les bordures de la fenêtre principale ainsi que quelques informations.
//(nom du fichier, nom et version de l'éditeur, liste des commandes, etc.)
{
	erase(); //efface tout l'écran au complet (lag moins que clear)
	attrset(COLOR_PAIR(10)); //noir sur blanc
	mvhline(0, 0, ' ', COLS); //dessine une ligne horizontale
	if (COLS / 2 > longueur_str(nom_fichier) / 2 + longueur_str("Coloration X.X.X ") + 2)
	{mvprintw(0, COLS - longueur_str("Coloration X.X.X "), "Coloration %s", VERSION);} //Nom de l'éditeur en haut à gauche
	else if (COLS / 2 > longueur_str(nom_fichier) / 2 + longueur_str("Col ") + 2)
	{mvprintw(0, COLS - longueur_str("Col "), "Col ");} //Nom abrégé en haut à gauche (manque de place)
	mvaddstrc(0, nom_fichier); //affiche le nom du fichier modifié
	
	mvvline(1, 0, ' ', LINES - 3); //dessine une ligne verticale
	mvvline(1, 1, ' ', LINES - 3); //...
	mvvline(1, 2, ' ', LINES - 3);
	
	//Liste des commandes:
	mvprintw(LINES - 2, 1, "^Q");
	mvprintw(LINES - 1, 1, "^S");
	
	if (COLS >= 28)
	{
		mvprintw(LINES - 2, 17, "^A");
		mvprintw(LINES - 1, 17, "^P");
		
		if (COLS >= 44)
		{
			mvprintw(LINES - 2, 30, "^N");
			mvprintw(LINES - 1, 30, "^E");
		
			if (COLS >= 56)
			{
				mvprintw(LINES - 2, 46, "^Z");
				mvprintw(LINES - 1, 46, "^Y");
				
				if (COLS >= 67)
				{
					mvprintw(LINES - 2, 58, "^C");
					mvprintw(LINES - 1, 58, "^V");
					
					if (COLS >= 82)
					{
						mvprintw(LINES - 2, 69, "^X");
						mvprintw(LINES - 1, 69, "^R");
						
						if (COLS >= 106)
						{
							mvprintw(LINES - 2, 84, "^F");
							mvprintw(LINES - 1, 84, "^L");
							
							if (COLS >= 128)
							{
								mvaddch(LINES - 2, 108, '^'); addch(ACS_DARROW);
								mvaddch(LINES - 1, 108, '^'); addch(ACS_UARROW);
								
								if (COLS >= 157)
								{
									mvprintw(LINES - 2, 130, "Shift-Home");
									mvprintw(LINES - 1, 130, "Shift-End ");
									
									if (COLS >= 180)
									{
										mvprintw(LINES - 2, 159, "^W");
										mvprintw(LINES - 1, 159, "^T");
										
										if (COLS >= 202)
										{
											mvprintw(LINES - 2, 182, "Esc");
											mvprintw(LINES - 1, 182, "^G ");
											
											if (COLS >= 228)
											{
												mvprintw(LINES - 2, 204, "F1");
												mvprintw(LINES - 1, 204, "^D");
											}
	}}}}}}}}}} //Raccourcis inutilisables: ^M (=enter), ^I (=tab), M-... (tous les Alt- ne fonctionnent pas...)
	
	//Retour à un affichage blanc sur noir:
	standend();
	mvhline(LINES - 3, 0, ACS_HLINE, COLS);
	
	//Liste des commandes (suite):
	mvprintw(LINES - 2, 4, "Quitter");
	mvprintw(LINES - 1, 4, "Sauvegarder");
	
	if (COLS >= 28)
	{
		mvprintw(LINES - 2, 20, "Aide");
		mvprintw(LINES - 1, 20, "Commande");
		
		if (COLS >= 44)
		{
			mvprintw(LINES - 2, 33, "Ouvrir");
			mvprintw(LINES - 1, 33, "Enregistrer");
			
			if (COLS >= 56)
			{
				mvprintw(LINES - 2, 49, "Annuler");
				mvprintw(LINES - 1, 49, "Refaire");
				
				if (COLS >= 67)
				{
					mvprintw(LINES - 2, 61, "Copier");
					mvprintw(LINES - 1, 61, "Coller");
					
					if (COLS >= 82)
					{
						mvprintw(LINES - 2, 72, "Couper");
						mvprintw(LINES - 1, 72, "Rafraichir");
						
						if (COLS >= 106)
						{
							mvprintw(LINES - 2, 87, "Trouver & Remplacer");
							mvprintw(LINES - 1, 87, "Aller à une ligne");
							
							if (COLS >= 128)
							{
								mvprintw(LINES - 2, 111, "Prochain élément");
								mvprintw(LINES - 1, 111, "Élément précédent");
								
								if (COLS >= 157)
								{
									mvprintw(LINES - 2, 141, "Aller au -DÉBUT-");
									mvprintw(LINES - 1, 141, "Aller à la -FIN-");
									
									if (COLS >= 180)
									{
										mvprintw(LINES - 2, 162, "Compter Caractères");
										mvprintw(LINES - 1, 162, "Accès au Terminal");
										
										if (COLS >= 202)
										{
											mvprintw(LINES - 2, 186, "Menu des Options");
											mvprintw(LINES - 1, 186, "Suspendre");
											
											if (COLS >= 228)
											{
												mvprintw(LINES - 2, 207, "Coloration Syntaxique");
												mvprintw(LINES - 1, 207, "Mode Débogage");
											}
	}}}}}}}}}}
}


int afficher_ligne(ligne* ln)
//Affiche une ligne à l'écran (et son numéro de ligne à gauche).
//Cette ligne (reçue en paramètre) doit avoir été déjà initialisée.
//Renvoie le numéro de la ligne ou 0 si la ligne ne peut pas être affichée.
//Renvoie -1 si la ligne ne s'est pas affichée au complet.
{
	if (ln == NULL || ln == ERREUR || ln == &FIN_FICHIER || ln == &DEBUT_FICHIER || y >= LINES - 3)
	{return 0;}
	
	//Affichage du numéro de ligne:
	mv(y, 0);
	attrset(COLOR_PAIR(10));
	printw("%d", ln->num);
	standend();
	mv(y, 4);
	
	//Comptage du nombre de lignes (à l'écran) occupées par cette ligne:
	compter_lignes(ln);
	
	//Affichage du -DÉBUT- et de la -FIN- en gras:
	if (ln->tag == DEBUT || ln->tag == FIN)
	{attrset(A_BOLD);}
	
	//Affichage de la ligne elle-même:
	for (int compteur = 0; compteur < 400 && ln->txt[compteur] != '\000'; compteur++)
	{
		getyx(stdscr, y, x);
		
		//Plus d'espace sur cette ligne:
		if (x >= COLS - 1)
		{y++; mv(y, 4);}
		
		//Plus d'espace pour débuter cette ligne:
		if (y >= LINES - 3)
		{return -1;}
		
		//Début d'un commentaire:
		if (ln->tag >= 0 && ln->txt[compteur - 1] == '/' && ln->txt[compteur] == '*' && getattrs(stdscr) != COLOR_PAIR(2))
		{
			attron(COLOR_PAIR(1));
			getyx(stdscr, y, x);
			mv(y, x - 1);
			printw("%c%c", ln->txt[compteur - 1], ln->txt[compteur]);
		}
		
		//Fin d'un commentaire:
		else if (ln->tag >= 0 && ln->txt[compteur - 1] == '*' && ln->txt[compteur] == '/' && getattrs(stdscr) == COLOR_PAIR(1))
		{printw("%c", ln->txt[compteur]); standend();}
		
		//Début/Fin d'une remarque ignorée:
		else if (ln->tag >= 0 && ln->txt[compteur] == '"' && ln->txt[compteur - 1] != '\\' && getattrs(stdscr) != (COLOR_PAIR(1)))
		{
			if (getattrs(stdscr) != COLOR_PAIR(2))
			{attron(COLOR_PAIR(2)); printw("%c", ln->txt[compteur]);}
			else
			{printw("%c", ln->txt[compteur]); attroff(COLOR_PAIR(2));}
		}
		
		//Aucune manipulation spéciale nécessaire:
		else
		{printw("%c", ln->txt[compteur]);} //addch(ln->txt[compteur]) n'affiche pas les accents...
		
		refresh();
	}
	
	//Fin de la ligne (nouvelle ligne):
	standend();
	y++;
	mv(y, 4);
	return ln->num;
}