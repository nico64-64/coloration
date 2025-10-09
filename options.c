#include "outils_logiques.c"


void credits()
//Affiche les crédits du programme.
{
	int input = -1;
	
	erase();
	attrset(COLOR_PAIR(10)); //noir sur blanc
	mvhline(0, 0, ' ', COLS); //dessine une ligne horizontale
	if (COLS / 2 > longueur_str("Crédits") / 2 + longueur_str("Coloration X.X.X ") + 2)
	{mvprintw(0, COLS - longueur_str("Coloration X.X.X "), "Coloration %s", VERSION);} //Nom de l'éditeur en haut à droite
	else if (COLS / 2 > longueur_str("Crédits") / 2 + longueur_str("Col ") + 2)
	{mvprintw(0, COLS - longueur_str("Col "), "Col ");} //Nom abrégé en haut à droite (manque de place)
	mvaddstrc(0, "Crédits"); //affiche le titre
	standend();
	
	mvprintw(2, 4, "COLORATION %s\n\n    ", VERSION);
	printw("Éditeur de texte inspiré de GNU nano.\n\n    Ce projet a pour but de faciliter la visualisation et l'édition des\n    fichiers de scénario (listes d'objets) de mon projet de jeu d'aventure.\n\n    ");
	printw("Programmé par Nicolas Audette.\n\n    Appuyez sur Escape ou Enter pour quitter.");
	
	while (strcmp(keyname(input), "^[") != 0 && strcmp(keyname(input), "^M") != 0 && strcmp(keyname(input), "^Q") != 0)
	{input = getch();}
	rafraichir();
}


void param_avances()
//Permet de modifier les paramètres avancés du programme.
{
	int input = 0;
	int selection = 1;
	int nbre_accents = 0;
	char buffer[250];
	
	//Affichage des bordures:
	erase(); //efface tout l'écran au complet (lag moins que clear)
	attrset(COLOR_PAIR(10)); //noir sur blanc
	mvhline(0, 0, ' ', COLS); //dessine une ligne horizontale en haut de l'écran
	if (COLS / 2 > longueur_str("Paramètres avancés") / 2 + longueur_str("Coloration X.X.X ") + 2)
	{mvprintw(0, COLS - longueur_str("Coloration X.X.X "), "Coloration %s", VERSION);} //Nom de l'éditeur en haut à droite
	else if (COLS / 2 > longueur_str("Paramètres avancés") / 2 + longueur_str("Col ") + 2)
	{mvprintw(0, COLS - longueur_str("Col "), "Col ");} //Nom abrégé en haut à droite (manque de place)
	mvaddstrc(0, "Paramètres avancés"); //affiche le titre de la fenêtre
	
	//Affichage des raccourcis:
	mvprintw(LINES - 1, 1, "^Q");
	if (COLS > 61)
	{mvprintw(LINES - 1, 25, "Enter");}
	if (COLS > 98)
	{
		mvaddch(LINES - 1, 64, ACS_UARROW);
		printw(" ET ");
		addch(ACS_DARROW);
	}	
	standend();
	mvhline(LINES - 3, 0, ACS_HLINE, COLS);
	mvprintw(LINES - 1, 4, "Revenir à l'éditeur");
	if (COLS > 61)
	{mvprintw(LINES - 1, 31, "Modifier la valeur du paramètre");}
	if (COLS > 98)
	{mvprintw(LINES - 1, 71, "Naviguer dans les paramètres");}
	
	while (strcmp(keyname(input), "^[") != 0 && strcmp(keyname(input), "^Q") != 0)
	{
		//Affichage de la liste des paramètres avancés:
		for (int compteur = 1; compteur <= nbre_p_avances && compteur + 1 < LINES - 3; compteur++)
		{
			//Sélection:
			if (selection == compteur)
			{
				attrset(COLOR_PAIR(10));
				mvhline(LINES - 2, 0, ' ', COLS);
				switch (p_avances[compteur].type)
				{
				case 's':
					mvprintw(LINES - 2, 1, "[string] Valeur du paramètre: \"%s\"", p_avances[compteur].ptr_str);
					break;
				
				case 'c':
					mvprintw(LINES - 2, 1, "[caractère]");
					break;
				
				case 'i':
					mvprintw(LINES - 2, 1, "[nombre]");
					break;
				
				case 'b':
					mvprintw(LINES - 2, 1, "[boolean]");
					break;
				}
				mvhline(compteur + 1, 1, ' ', COLS - 3);
				mvaddch(compteur + 1, 2, '>');
			}
			else
			{
				standend();
				mvhline(compteur + 1, 1, ' ', COLS - 3);
			}
			
			//Descripton:
			if (longueur_str(p_avances[compteur].descr) > COLS - 15)
			{
				strcpy(buffer, p_avances[compteur].descr);
				buffer[COLS - 17] = '\000';
				nbre_accents = strlen(buffer) - longueur_str(buffer);
				strcpy(buffer, p_avances[compteur].descr);
				buffer[COLS - 17 + nbre_accents] = '.';
				buffer[COLS - 16 + nbre_accents] = '.';
				buffer[COLS - 15 + nbre_accents] = '.';
				buffer[COLS - 14 + nbre_accents] = '\000';
				mvaddstr(compteur + 1, 4, buffer);
			}
			else
			{mvaddstr(compteur + 1, 4, p_avances[compteur].descr);}
			
			//Valeur:
			switch (p_avances[compteur].type)
			{
			case '0': //ne devrait jamais arriver!
				erreur(90, "Corruption de la liste des paramètres avancés.");
				rafraichir();
				return;
			
			case 'b': //bool
				if (*(bool*) p_avances[compteur].ptr_int == TRUE)
				{mvprintw(compteur + 1, COLS - 9, "[ X ]");}
				else
				{mvprintw(compteur + 1, COLS - 9, "[   ]");}
				break;
			
			case 'i': //int
				mvprintw(compteur + 1, COLS - 9, "[ %d ]", *p_avances[compteur].ptr_int);
				break;
			
			case 'c': //char
				mvprintw(compteur + 1, COLS - 9, "[ %c ]", *p_avances[compteur].ptr_int);
				break;
			
			case 's': //string
				mvprintw(compteur + 1, COLS - 9, "[ STR ]");
				break;
			}
		}
		standend();
		refresh();
		
		//Gestion de l'input:
		do
		{input = getch();} while (input == -1);
		
		switch (input)
		{
		case KEY_UP:
			if (selection > 1)
			{selection--;}
			break;
		
		case KEY_DOWN:
			if (selection < nbre_p_avances)
			{selection++;}
			break;
		
		case KEY_RESIZE:
			if (LINES < 5 + nbre_p_avances || COLS < 25)
			{
				rafraichir();
				print_msg("Terminal trop petit pour afficher les paramètres avancés.");
				return;
			}
			else
			{
				param_avances();
				rafraichir();
				return;
			}
			break;
		
		default:
			if (!strcmp(keyname(input), "^M"))
			{
				switch (p_avances[selection].type)
				{
				case 'b': //bool
					if (*(bool*) p_avances[selection].ptr_int == TRUE)
					{*(bool*) p_avances[selection].ptr_int = FALSE;}
					else
					{*(bool*) p_avances[selection].ptr_int = TRUE;}
					
					//Spécial:
					if ((bool*) p_avances[selection].ptr_int == &err_log)
					{erreur(0, "init");}
					break;
				
				case 'c': //char
					print_msg("Entrer le caractère voulu maintenant.");
					*p_avances[selection].ptr_int = getch();
					break;
				
				case 'i': //int
					print_msg("Appuyer sur + pour augmenter ou - pour diminuer la valeur, Enter pour terminer.");
					#pragma GCC diagnostic ignored "-Wunused-value" //nécessaire pour ne pas avoir de warning sur le ptr_int++ et ptr_int--
					do
					{
						input = getch();
						if (input == '+' && *p_avances[selection].ptr_int < 100)
						{*p_avances[selection].ptr_int++;}
						else if (input == '-' && *p_avances[selection].ptr_int > 0)
						{*p_avances[selection].ptr_int--;}
					} while (strcmp(keyname(input), "^M") != 0 && strcmp(keyname(input), "^[") != 0);
					#pragma GCC diagnostic warning "-Wunused-value"
					break;
				
				case 's': //str
					print_msg("Entrer la string voulue puis Enter.");
					attrset(COLOR_PAIR(10));
					do
					{
						input = getch();
						if (input == KEY_BACKSPACE && strlen(p_avances[selection].ptr_str) > 0)
						{
							p_avances[selection].ptr_str[strlen(p_avances[selection].ptr_str) - 1] = '\000';
							mvhline(LINES - 2, 0, ' ', COLS);
							mvprintw(LINES - 2, 1, "[string] Valeur du paramètre: \"%s\"", p_avances[selection].ptr_str);
						}
						else if (input >= ' ' && input <= 126)
						{
							p_avances[selection].ptr_str[strlen(p_avances[selection].ptr_str)] = input;
							p_avances[selection].ptr_str[strlen(p_avances[selection].ptr_str) + 1] = '\000';
							mvprintw(LINES - 2, 1, "[string] Valeur du paramètre: \"%s\"", p_avances[selection].ptr_str);
						}
					} while (strcmp(keyname(input), "^M") != 0 && strcmp(keyname(input), "^[") != 0);
					standend();
					break;
				}
				print_msg(NULL);
			}
			break;
		}
	}
	
	rafraichir();
}


void aide_specifique(int num_cmd)
//Affiche l'article d'aide associé au numéro (nombre entier supérieur à 0) reçu en paramètre.
//Doit obligatoirement être appelé par aide(num_cmd) ou aide().
{
	int input = ERR;
	char buffer[sizeof(cmds[0].details)];
	char* buff2 = NULL;
	int decalage = 0;
	int _y;
	#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
	int _x; //Cette variable contiendra le x de la position actuelle. Cette valeur n'est pas utilisée, mais je m'en fout! (d'où le #pragma)
	#pragma GCC diagnostic warning "-Wunused-but-set-variable"
	
	_y = getmaxy(stdscr);
	if (_y < 14)
	{
		print_msg("Ce terminal est trop petit pour afficher cet article.");
		mvprintw(LINES - 2, 0, "Appuyez sur n'importe quelle touche pour revenir en arrière.");
		do
		{input = getch();} while (input == -1);
		return;
	}
	
	attrset(A_BOLD);
	mvprintw(2, 2, "Nom:");
	mvprintw(2, 30, "Touche:");
	mvprintw(5, 2, "Description:");
	mvprintw(8, 2, "Détails:");
	if (_y >= 16)
	{mvprintw(LINES - 4, 2, "Commandes liées:");}
	standend();
	
	//Nom, touche et description:
	mvaddstr(3, 2, cmds[num_cmd].nom);
	mvaddstr(3, 30, cmds[num_cmd].touche);
	mvaddstr(6, 2, cmds[num_cmd].descr);
	
	//Détails:
	strcpy(buffer, cmds[num_cmd].details);
	for (int compteur = 0; buffer[compteur] != '\000'; compteur++)
	{
		if (buffer[compteur] == '\n')
		{decalage = compteur;}
		
		else if (compteur - decalage == COLS - 4)
		{
			for (; buffer[compteur] != ' ' && compteur > 0; compteur--) {}
			buffer[compteur] = '\n';
			decalage = compteur;
		}
	}
	mvaddstr(9, 2, strtok(buffer, "\n"));
	buff2 = strtok(NULL, "\n");
	if (_y < 16)
	{decalage = 3;} //decalage ne veut maintenant plus du tout dire la même chose qu'avant!
	else
	{decalage = 0;}
	for (int compteur = 0; buff2 != NULL && compteur + 9 < LINES - 7 + decalage; compteur++)
	{
		printw("\n  %s", buff2);
		buff2 = strtok(NULL, "\n");
	}
	if (buff2 != NULL)
	{
		attrset(COLOR_PAIR(6)); //texte en rouge
		printw("\n  [...] Appuyer sur Enter pour afficher la suite.");
		standend();
	}
	
	//Commandes liées:
	if (cmds[num_cmd].cmd_liees[0] != 0 && _y >= 16)
	{
		mvaddstr(LINES - 3, 2, cmds[cmds[num_cmd].cmd_liees[0]].nom);
		if (cmds[num_cmd].cmd_liees[1] != 0)
		{
			if (cmds[num_cmd].cmd_liees[2] == 0)
			{printw(" et %s", cmds[cmds[num_cmd].cmd_liees[1]].nom);}
			else
			{printw(", %s et %s", cmds[cmds[num_cmd].cmd_liees[1]].nom, cmds[cmds[num_cmd].cmd_liees[2]].nom);}
		}
	}
	
	//Bas de l'écran:
	mvprintw(LINES - 1, 0, "Appuyer sur Escape pour revenir au manuel.");
	refresh();
	
	//Gestion de l'input:
	do
	{
		input = getch();
		if (!strcmp(keyname(input), "^M") && buff2 != NULL)
		{
			move(9, 2);
			attrset(COLOR_PAIR(1)); //texte en vert
			printw("[...]");
			standend();
			for (int compteur = 0; buff2 != NULL && compteur + 9 < LINES - 7 + decalage; compteur++)
			{
				printw("\n  %s", buff2);
				buff2 = strtok(NULL, "\n");
			}
			if (buff2 != NULL)
			{
				attrset(COLOR_PAIR(6)); //texte en rouge
				printw("\n  [...] Appuyez sur Enter pour afficher la suite.");
				standend();
			}
			else
			{
				getyx(stdscr, _y, _x);
				do
				{printw("\n"); hline(' ', COLS - 1); _y++;} while (_y <= LINES - 7 + decalage);
			}
			refresh();
			input = 12;
		}
	} while (strcmp(keyname(input), "^M") != 0 && strcmp(keyname(input), "^[") != 0 && strcmp(keyname(input), "^Q") != 0);
}


void aide(int num_cmd)
//Affiche le manuel de l'éditeur.
//Peut afficher l'article portant sur la commande dont le numéro est passé en paramètre ou l'aide interactive générale si cmd vaut 0.
//La fonction peut aussi s'appeler elle-même avec -1 en arguments pour afficher l'aide interactive générale sans le message de bienvenue.
{
	char titre[100] = "Aide";
	char buffer[200] = "";
	int input = ERR;
	int selection = 1;
	int decalage = 0;
	int marge = 10;
	int nbre_accents = 0;
	
	//Ajustement du titre et gestion de ce qui ne devrait jamais arriver:
	if (num_cmd > 0)
	{strcat(titre, ": "); strcat(titre, cmds[num_cmd].nom);}
	else if (num_cmd < -1)
	{erreur(30, "Le manuel de l'éditeur a tenté d'ouvrir une page négative. Quoi?!"); return;}
	
	//Empêche le curseur de clignoter à l'écran:
	curs_set(0);
	
	//Dessin des bordures:
	erase(); //efface tout l'écran au complet (lag moins que clear)
	attrset(COLOR_PAIR(10)); //noir sur blanc
	mvhline(0, 0, ' ', COLS); //dessine une ligne horizontale
	if (COLS / 2 > longueur_str(titre) / 2 + longueur_str("Coloration X.X.X ") + 2)
	{mvprintw(0, COLS - longueur_str("Coloration X.X.X "), "Coloration %s", VERSION);} //Nom de l'éditeur en haut à droite
	else if (COLS / 2 > longueur_str(titre) / 2 + longueur_str("Col ") + 2)
	{mvprintw(0, COLS - longueur_str("Col "), "Col ");} //Nom abrégé en haut à droite (manque de place)
	mvaddstrc(0, titre); //affiche le titre de la page ("aide")
	
	//Vérification de la taille du terminal:
	if (COLS < 44)
	{
		rafraichir();
		curs_set(1);
		print_msg("Terminal trop petit pour afficher l'aide de l'éditeur.");
		return;
	}
	else if (COLS < 56)
	{marge = COLS - 46;}
		
	//Accès à un article du manuel, si demandé:
	if (num_cmd > 0)
	{aide_specifique(num_cmd); curs_set(1); rafraichir(); return;}
	
	//Liste des commandes utilisables ici:
	mvprintw(LINES - 2, 1, "^Q");
	mvprintw(LINES - 1, 1, "^F");
	if (COLS >= 28)
	{
		mvaddch(LINES - 2, 25, ACS_UARROW);
		mvaddch(LINES - 1, 25, ACS_DARROW);
		if (COLS >= 74)
		{mvprintw(LINES - 1, 42, "Enter");}
	}
	
	standend();
	mvhline(LINES - 3, 0, ACS_HLINE, COLS);
	
	mvprintw(LINES - 2, 4, "Revenir à l'éditeur");
	mvprintw(LINES - 1, 4, "Chercher un article");
	if (COLS >= 28)
	{
		mvprintw(LINES - 2, 27, "Naviguer dans");
		mvprintw(LINES - 1, 27, "les articles");
		if (COLS >= 74)
		{mvprintw(LINES - 1, 48, "Lire l'article sélectionné");}
	}
	
	//Titres des colonnes et message de bienvenue:
	mvprintw(1, marge, "Nom:         | Touche: |        Description:");
	if (num_cmd >= 0)
	{print_msg("Bienvenue dans le module d'aide de Coloration! Appuyer sur \"Enter\" pour en savoir plus.");}
	
	//Gestion de l'input:
	while (1)
	{
		//Affichage de l'aide "interactive" générale:
		for (int compteur = 1; compteur < LINES - 4 && compteur <= nbre_cmds; compteur++)
		{
			if (compteur == selection - decalage)
			{attrset(COLOR_PAIR(10));}
			mvhline(compteur + 1, 2, ' ', COLS - 4);
			mvaddstr(compteur + 1, 4, cmds[decalage + compteur].nom); //nom
			mvaddstr(compteur + 1, 26, cmds[decalage + compteur].touche); //touche
			if (longueur_str(cmds[decalage + compteur].descr) > COLS - 36)
			{
				strcpy(buffer, cmds[decalage + compteur].descr);
				buffer[COLS - 39] = '\000';
				nbre_accents = strlen(buffer) - longueur_str(buffer);
				strcpy(buffer, cmds[decalage + compteur].descr);
				buffer[COLS - 39 + nbre_accents] = '.';
				buffer[COLS - 38 + nbre_accents] = '.';
				buffer[COLS - 37 + nbre_accents] = '.';
				buffer[COLS - 36 + nbre_accents] = '\000';
				mvaddstr(compteur + 1, 34, buffer);
			}
			else
			{mvaddstr(compteur + 1, 34, cmds[decalage + compteur].descr);} //description
			if (compteur == selection - decalage)
			{mvaddch(compteur + 1, 2, '>'); standend();}
		}
		
		do
		{input = getch();} while (input == -1);
		
		switch (input)
		{
		case KEY_UP:
			if (selection > 1)
			{
				selection--;
				if (selection - decalage < 1)
				{decalage--;}
			}
			break;
		
		case KEY_DOWN:
			if (selection < nbre_cmds)
			{
				selection++;
				if (selection - decalage >= LINES - 4)
				{decalage++;}
			}
			break;
		
		case KEY_RESIZE:
			aide(-1);
			return;
		
		default:
			if (keyname(input)[0] == '^')
			//Ctrl-...
			{
				switch (keyname(input)[1])
				{
				case '[': //Esc (^[) = retour à l'éditeur
				case 'Q': //Ctrl-Q (^Q) = retour à l'éditeur
					curs_set(1);
					rafraichir();
					return;
				
				case 'F': //Ctrl-F (^F) = chercher une commande
					//À venir...
					break;
				
				case 'M': //Ctrl-M (Enter) = lire l'article sélectionné
					aide(selection);
					aide(-1);
					return;
				}
			}
			break;
		}
		print_msg(NULL);
	}
}


void menu_options()
//Affiche le menu des options et gère son utilisation.
//Appelé lorsque l'utilisateur appuie sur Esc.
{
	int car = KEY_RESIZE; //input (doit être initialisé à KEY_RESIZE pour que le pop-up soit dessiné)
	int xi = (COLS - 40) / 2;
	int xf = xi + 40;
	int yi = (LINES - 2 - 10) / 2; //le -2 sert à centrer en y en excluant toutefois la barre de commandes d'en bas...
	int yf = yi + 10;
	int selection = 0; //indique quelle entrée est présentement sélectionnée
	
	curs_set(0); //rend le curseur invisible
	
	while (car != 27) //27 = Esc.
	{
		switch (car)
		{
		case KEY_RESIZE:
			//Trouve si la fenêtre est assez grande pour afficher le pop-up:
			if (COLS <= 40 || LINES <= 10)
			{
				rafraichir();
				print_msg("Ce terminal est trop petit pour afficher le menu des options.");
				curs_set(1);
				return;
			}
			
			//Trouve les nouvelles coordonnées du pop-up:
			xi = (COLS - 40) / 2;
			xf = xi + 40;
			yi = (LINES - 2 - 10) / 2;
			yf = yi + 10;
			
			//Redessine les bordures du pop-up:
			rafraichir(); //redessine la fenêtre en arrière du pop-up
			attrset(COLOR_PAIR(10)); //noir sur blanc
			mvhline(yi, xi, ACS_HLINE, 40);
			mvhline(yf, xi, ACS_HLINE, 40);
			mvvline(yi, xi, ACS_VLINE, 10);
			mvvline(yi, xf, ACS_VLINE, 10);
			mvaddch(yi, xi, ACS_ULCORNER);
			mvaddch(yf, xi, ACS_LLCORNER);
			mvaddch(yi, xf, ACS_URCORNER);
			mvaddch(yf, xf, ACS_LRCORNER);
			
			//Remplit le pop-up:
			for (int compteur = yi + 1; compteur < yf; compteur++)
			{mvhline(compteur, xi + 1, ' ', 39);}
			
			//Écrit le titre et liste les options:
			mvprintw(yi, xi + 16, " Options "); //titre
			liste_options(selection); //liste les options et "surligne" la bonne
			refresh(); //affichage du tout
			break;
		
		case KEY_DOWN:
			if (selection < 6)
			{selection++;}
			else
			{selection = 0;}
			liste_options(selection);
			break;
		
		case KEY_UP:
			if (!selection)
			{selection = 6;}
			else
			{selection--;}
			liste_options(selection);
			break;
		
		default:
			if (!strcmp(keyname(car), "^M")) //^M = Enter
			{
				switch (selection)
				{
				case 0: //retour à l'éditeur
					rafraichir();
					curs_set(1);
					return;
				
				case 1: //Accéder à l'aide
					aide(0);
					return;
				
				case 2: //Modifier les paramètres avancés
					param_avances();
					curs_set(1);
					return;
				
				case 3: //Sauvegarder
					cmd("sauvegarder");
					curs_set(1);
					return;
				
				case 4: //Ouvrir un autre fichier
					//Ouvrir...
					break;
				
				case 5: //Afficher les crédits
					credits();
					return;
				
				case 6: //Fermer le programme
					desinit();
					quitter(0);
					break;
				}
			}
			else if (!strcmp(keyname(car), "^A"))
			{curs_set(1); aide(0); return;}
			else if (!strcmp(keyname(car), "^Q"))
			{desinit(); quitter(0);}
		}
		
		car = getch();
	}
	
	rafraichir(); //reddessine la fenêtre sans le pop-up
	curs_set(1); //remet le curseur visible
}