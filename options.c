#include "outils_logiques.c"


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
	MEVENT mev;
	bool touche_emulee = TRUE;
	
	curs_set(0); //rend le curseur invisible
	
	while (car != 27) //27 = Esc.
	{
		if (!touche_emulee)
		{car = getch();}
		else
		{touche_emulee = FALSE;}
		
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
		
		case KEY_MOUSE:
			getmouse(&mev);
			if (mev.bstate == 65536) //molette vers le haut
			{car = KEY_UP; touche_emulee = TRUE;}
			else if (mev.bstate == 2097152) //molette vers le bas
			{car = KEY_DOWN; touche_emulee = TRUE;}
			else if (mev.x >= xi && mev.x <= xf && mev.y >= yi && mev.y <= yf)
			{
				selection = mev.y - yi - 2;
				liste_options(selection);
				refresh();
				car = 13; //enter / ^M
				touche_emulee = TRUE;
			}
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
					curs_set(1);
					cmd("ouvrir");
					return;
				
				case 5: //Afficher les crédits
					credits();
					return;
				
				case 6: //Fermer le programme
					if (demande_quitter())
					{desinit(); quitter(0);}
					curs_set(1);
					return;
				}
			}
			else if (!strcmp(keyname(car), "^A"))
			{curs_set(1); aide(0); return;}
			else if (!strcmp(keyname(car), "^Q"))
			{desinit(); quitter(0);}
		}
	}
	
	rafraichir(); //reddessine la fenêtre sans le pop-up
	curs_set(1); //remet le curseur visible
}


void credits()
//Affiche les crédits du programme.
{
	int input = -1;
	#ifdef _ACCENTS_H
	bool ln_supp = FALSE;
	#else
	bool ln_supp = TRUE;
	#endif
	
	erase();
	attrset(COLOR_PAIR(10)); //noir sur blanc
	mvhline(0, 0, ' ', COLS); //dessine une ligne horizontale
	if (COLS / 2 > longueur_str("Crédits") / 2 + longueur_str("Coloration X.X.X ") + 2)
	{mvprintw(0, COLS - longueur_str("Coloration X.X.X "), "Coloration %s", VERSION);} //Nom de l'éditeur en haut à droite
	else if (COLS / 2 > longueur_str("Crédits") / 2 + longueur_str("Col ") + 2)
	{mvprintw(0, COLS - longueur_str("Col "), "Col ");} //Nom abrégé en haut à droite (manque de place)
	mvaddstrc(0, "Crédits"); //affiche le titre
	standend();
	
	if (LINES > 13 + ln_supp * 4) //version complète
	{
		mvprintw(2, 4, "COLORATION %s\n\n", VERSION);
		printw("    Éditeur de texte inspiré de GNU nano.\n\n\n");
		printw("    Ce projet a pour but de faciliter la visualisation et l'édition des\n    fichiers de scénario (listes d'objets) de mon projet de jeu d'aventure.\n\n");
		#ifndef _ACCENTS_H
		printw("    Cette version contient un support minimal des accents.\n    Incluez \"accents.h\" avant la compilation pour avoir un support complet.\n\n");
		#endif
		printw("\n    Programmé par Nicolas Audette.\n\n\n");
		printw("    Appuyez sur Escape ou Enter pour quitter.");
	}
	else //version abrégée
	{
		mvprintw(1, 1, "COLORATION %s\n", VERSION);
		printw(" Éditeur de texte inspiré de GNU nano et ayant\n pour but de faciliter l'édition des listes d'objets\n (scénarios) de mon projet de jeu d'aventure.\n");
		#ifndef _ACCENTS_H
		printw(" Cette version contient un support minimal seulement des accents.\n");
		#endif
		printw("\n Programmé par Nicolas Audette.\n\n");
		printw(" Appuyez sur Escape ou Enter pour quitter.");
	}
	
	while (strcmp(keyname(input), "^[") != 0 && strcmp(keyname(input), "^M") != 0 && strcmp(keyname(input), "^Q") != 0 && input != 'q' && input != KEY_RESIZE)
	{input = getch();}
	
	if (input == KEY_RESIZE && (LINES < 8 + ln_supp || COLS < 41))
	{rafraichir(); print_msg("Terminal trop petit pour afficher les crédits."); return;}	
	if (input == KEY_RESIZE)
	{credits();}
	
	rafraichir();
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
	MEVENT mev;
	
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
	mvprintw(LINES - 1, 1, "Enter");
	if (COLS >= 50)
	{
		mvaddch(LINES - 2, 35, ACS_UARROW);
		mvaddch(LINES - 1, 35, ACS_DARROW);
	}
	
	standend();
	mvhline(LINES - 3, 0, ACS_HLINE, COLS);
	
	mvprintw(LINES - 2, 7, "Revenir à l'éditeur");
	mvprintw(LINES - 1, 7, "Lire l'article sélectionné");
	if (COLS >= 50)
	{
		mvprintw(LINES - 2, 37, "Naviguer dans");
		mvprintw(LINES - 1, 37, "les articles");
	}
	
	//Titres des colonnes et message de bienvenue:
	mvprintw(1, marge, "Nom:         | Touche: |        Description:");
	if (num_cmd >= 0)
	{print_msg("Bienvenue dans le module d'aide de Coloration! Appuyez sur \"Enter\" pour en savoir plus.");}
	
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
		case KEY_RESIZE:
			aide(-1);
			return;
		
		case KEY_MOUSE:
			getmouse(&mev);
			if (mev.bstate == 65536) //molette vers le haut
			{goto monter;} //juste pour le fun!
			if (mev.bstate == 2097152) //molette vers le bas
			{goto descendre;} //juste pour le fun!
			else if (mev.y == LINES - 2 && mev.x < 34) //^Q
			{curs_set(1); rafraichir(); return;}
			else if (mev.y == LINES - 1 && mev.x < 34) //Enter
			{aide(selection); aide(-1); return;}
			else if (mev.y >= 2 && mev.y < LINES - 3 && mev.y + decalage < nbre_cmds + 2) //Clic sur une entrée
			{aide(mev.y + decalage - 1); aide(-1); return;}
			break;
		
		case KEY_UP:
		monter: //:)
			if (selection > 1)
			{
				selection--;
				if (selection - decalage < 1)
				{decalage--;}
			}
			break;
		
		case KEY_DOWN:
		descendre: //:)
			if (selection < nbre_cmds)
			{
				selection++;
				if (selection - decalage >= LINES - 4)
				{decalage++;}
			}
			break;
		
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
	MEVENT mev;
	
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
		printw("\n  [...] Appuyez sur Enter pour afficher la suite.");
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
	mvprintw(LINES - 1, 0, "Appuyez sur Escape pour revenir au manuel.");
	refresh();
	
	//Gestion de l'input:
	do
	{
		input = getch();
		if (input == KEY_MOUSE)
		{getmouse(&mev);}
		
		if ((!strcmp(keyname(input), "^M") || (input == KEY_MOUSE && (mev.bstate == 4 || mev.bstate == 4096))) && buff2 != NULL)
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
	} while (strcmp(keyname(input), "^M") != 0 && strcmp(keyname(input), "^[") != 0 && strcmp(keyname(input), "^Q") != 0 && !(input == KEY_MOUSE && (mev.bstate == 4 || mev.bstate == 4096)));
}


void param_avances()
//Permet de modifier les paramètres avancés du programme.
{
	int input = 0;
	int selection = 1;
	bool redemarrage_nec = FALSE;
	int nbre_accents = 0;
	char buffer[250];
	MEVENT mev;
	bool touche_simulee = FALSE;
	
	if (LINES < 6 + nbre_p_avances || COLS < 25)
	{
		rafraichir();
		print_msg("Terminal trop petit pour afficher les paramètres avancés.");
		return;
	}
	
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
	if (COLS > 32)
	{mvprintw(LINES - 1, 25, "^A");}
	if (COLS > 70)
	{mvprintw(LINES - 1, 34, "Enter");}
	if (COLS > 107)
	{
		mvaddch(LINES - 1, 73, ACS_UARROW);
		printw(" ET ");
		addch(ACS_DARROW);
	}	
	standend();
	mvhline(LINES - 3, 0, ACS_HLINE, COLS);
	mvprintw(LINES - 1, 4, "Revenir à l'éditeur");
	if (COLS > 32)
	{mvprintw(LINES - 1, 28, "Aide");}
	if (COLS > 70)
	{mvprintw(LINES - 1, 40, "Modifier la valeur du paramètre");}
	if (COLS > 107)
	{mvprintw(LINES - 1, 80, "Naviguer dans les paramètres");}
	
	//Affichage du niveau de support des accents:
	#ifdef _ACCENTS_H
	if (COLS >= 68)
	{
		mvprintw(LINES - 4, 4, "Prise en charge complète des accents. Entrez ^G pour les gérer.");
		attrset(COLOR_PAIR(10));
		mvprintw(LINES - 4, 49, "^G");
		standend();
	}
	else
	{
		attrset(COLOR_PAIR(10));
		mvprintw(LINES - 4, 4, "^G");
		standend();
		printw(" pour gérer les accents.");
	}
	#else
	mvprintw(LINES - 4, 4, "Prise en charge PARTIELLE des accents.");
	#endif
	
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
				erreur(90, "Corruption de la liste des paramètres avancés!");
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
		if (!touche_simulee)
		{
			do
			{input = getch();} while (input == -1);
		}
		else
		{touche_simulee = FALSE;}
		
		switch (input)
		{
		case KEY_RESIZE:
			if (LINES < 6 + nbre_p_avances || COLS < 25)
			{
				rafraichir();
				print_msg("Terminal trop petit pour afficher les paramètres avancés.");
				return;
			}
			else
			{param_avances(); return;}
			break;
		
		case KEY_MOUSE:
			getmouse(&mev);
			if (mev.bstate == 65536) //molette vers le haut
			{input = KEY_UP; touche_simulee = TRUE;}
			else if (mev.bstate == 2097152) //molette vers le bas
			{input = KEY_DOWN; touche_simulee = TRUE;}
			else if (mev.y == LINES - 1)
			{
				if (mev.x < 24) //^Q
				{input = 17; touche_simulee = TRUE;}
				else if (mev.x < 33) //^A
				{input = 1; touche_simulee = TRUE;}
			}
			else if (mev.y >= 2 && mev.y < nbre_p_avances + 2) //sélection d'un paramètre (/enter?)
			{selection = mev.y - 1; input = 13; touche_simulee = TRUE;}
			else if (mev.y == LINES - 4) //^G
			{input = 7; touche_simulee = TRUE;}
			break;
		
		case KEY_UP:
			if (selection > 1)
			{selection--;}
			break;
		
		case KEY_DOWN:
			if (selection < nbre_p_avances)
			{selection++;}
			break;
		
		default:
			#ifdef _ACCENTS_H
			if (!strcmp(keyname(input), "^G"))
			{gestion_accents(); curs_set(0); param_avances(); return;}
			#endif
			
			if (!strcmp(keyname(input), "^A"))
			{aide(26); curs_set(0); param_avances(); return;}
			
			if (!strcmp(keyname(input), "^M") || input == ' ')
			{
				switch (p_avances[selection].type)
				{
				case 'b': //bool
					if (*(bool*) p_avances[selection].ptr_int == TRUE)
					{*(bool*) p_avances[selection].ptr_int = FALSE;}
					else
					{*(bool*) p_avances[selection].ptr_int = TRUE;}
					break;
				
				case 'c': //char
					print_msg("Entrer le caractère voulu maintenant.");
					*p_avances[selection].ptr_int = getch();
					break;
				
				case 'i': //int
					print_msg("Appuyez sur + pour augmenter ou - pour diminuer la valeur, Enter pour terminer.");
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
				if (p_avances[selection].redemarrage_nec)
				{redemarrage_nec = TRUE;}
				
				if (redemarrage_nec)
				{print_msg("Vous devrez redémarrer l'application pour appliquer ces changements");}
				else
				{print_msg(NULL);}
			}
			break;
		}
	}
	
	rafraichir();
	if (!enregistrer_parametres())
	{erreur(0, "...");}
}


bool enregistrer_parametres()
//Enregistre les paramètres avancés actuels dans le fichier de configuration fconfig.
//Renvoie TRUE en cas de succès et FALSE en cas d'erreur.
{
	FILE* fconfig = fopen(nom_fconfig, "w+");
	
	if (fconfig == NULL)
	{erreur(20, "Impossible de créer un fichier de configuration avec ce nom"); return FALSE;}
	
	fprintf(fconfig, "Fichier de configuration de l'éditeur de texte Coloration.\n");
	fprintf(fconfig, "Ce fichier a été généré automatiquement et sera écrasé à chaque modification des paramètres avancés de l'éditeur.\nModifiez-le à vos propres risques.\n\n");
	fprintf(fconfig, "Chaque paramètre doit être inscrit ainsi: \"[ PARAM:valeur ]\" (sans les guillemets).\nIl est important de respecter l'espacement avec les parenthèses carrées.\n");
	fprintf(fconfig, "Tout texte écrit en dehors de ces parentèses carrées sera ignoré par le programme.\n\n\n");
	
	for (int compteur = 1; compteur <= nbre_p_avances; compteur++)
	{
		if (strcmp(p_avances[compteur].nom, "-") != 0) //Les paramètres nommés "-" ne doivent pas être enregistrés.
		{
			switch (p_avances[compteur].type)
			{
			case 'b': //bool
				fprintf(fconfig, "[ %s:%d ]\n", p_avances[compteur].nom, *(bool*) p_avances[compteur].ptr_int);
				break;
			
			case 'i': //int
				fprintf(fconfig, "[ %s:%d ]\n", p_avances[compteur].nom, *p_avances[compteur].ptr_int);
				break;
			
			case 'c': //char
				fprintf(fconfig, "[ %s:%c ]\n", p_avances[compteur].nom, *p_avances[compteur].ptr_int);
				break;
			
			case 's': //string
				fprintf(fconfig, "[ %s:%s ]\n", p_avances[compteur].nom, p_avances[compteur].ptr_str);
				break;
			}
		}
	}
	
	fclose(fconfig);
	return TRUE;
}


void lire_parametres()
//Lit les paramètres avancés sauvegardés dans le fichier de configuration de l'éditeur (fconfig) et les applique.
//Ne fait rien s'il n'y a pas de fichier de configuration (utilise donc les réglages par défaut).
//Cette fonction doit être appelé immédiatement après gestion_arguments, avant l'initialisation du programme et de ncurses.
{
	char ligne[300];
	char* mot = "";
	int compteur = 0;
	int buffint = -1;
	FILE* fconfig = fopen(nom_fconfig, "r");
	
	if (fconfig == NULL) //s'il n'y a pas de fconfig, on va utiliser les réglages par défaut (sauf si l'utilisateur a précisé quel fichier utiliser via les options d'invocation)
	{
		if (fconfig_particulier)
		{printf("Erreur: Impossible d'ouvrir ce fichier de configuration (%s).\nAvez-vous fait une faute de frappe?\n\n", nom_fconfig); exit(0);}
		return;
	}
	
	while (fgets(ligne, sizeof(ligne), fconfig) != NULL)
	{
		mot = strtok(ligne, ":;, \t\n");
		while (mot != NULL && strcmp(mot, "[") != 0)
		{mot = strtok(NULL, ":;, \t\n");}
		if (mot != NULL)
		{
			mot = strtok(NULL, ":;, \t\n");
			for (compteur = nbre_p_avances; compteur > 0 && strcmp(mot, p_avances[compteur].nom); compteur--) {}
			mot = strtok(NULL, ":;, \t\n");
			if (!compteur || (p_avances[compteur].type != 's' && strcmp(strtok(NULL, ":;, \t\n"), "]") != 0))
			{erreur(22, "Ligne invalide dans le fichier de configuration");}
			else
			{
				switch (p_avances[compteur].type)
				{
				case 'b': //bool
					sscanf(mot, "%d", &buffint);
					if (buffint == 1 || buffint == 0)
					{*(bool*) p_avances[compteur].ptr_int = (bool) buffint;}
					else
					{erreur(23, "Ligne invalide dans le fichier de configuration (faux paramètre booléen)");}
					break;
				
				case 'i': //int
					sscanf(mot, "%d", &buffint);
					if (buffint == -1)
					{erreur(24, "Ligne invalide dans le fichier de configuration (faux paramètre numérique)");}
					else
					{*p_avances[compteur].ptr_int = buffint;}
					break;
				
				case 'c': //char
					*p_avances[compteur].ptr_int = mot[0];
					break;
				
				case 's': //string
					strcpy(p_avances[compteur].ptr_str, mot);
					mot = strtok(NULL, ":;, \t\n");
					while (mot != NULL && strcmp(mot, "]") != 0)
					{
						strcat(p_avances[compteur].ptr_str, " ");
						strcat(p_avances[compteur].ptr_str, mot);
						mot = strtok(NULL, ":;, \t\n");
					}
					break;
				}
			}
		}
	}
	
	fclose(fconfig);
	return;
}


#ifdef _ACCENTS_H

bool gestion_accents()
//Interface de gestion des accents (genre de menu principal).
//Renvoie TRUE lorsque les changements ont bel et bien été enregistrés (sinon FALSE).
{
	int selection = 0;
	bool touche_simulee = FALSE;
	MEVENT mev;
	int decalage = 0;
	int input = ERR;
	int xi, xf, yi, yf;
	
	if (LINES < 6 || COLS < 33)
	{print_msg("Terminal trop petit pour gérer les accents."); return 1;} //Ne s'affichera jamais, mais bon...
	
	//Affichage des bordures:
	curs_set(0); //disparition du curseur
	erase(); //efface tout l'écran au complet (lag moins que clear)
	attrset(COLOR_PAIR(10)); //noir sur blanc
	mvhline(0, 0, ' ', COLS); //dessine une ligne horizontale en haut de l'écran
	if (COLS / 2 > longueur_str("Gestion des accents") / 2 + longueur_str("Coloration X.X.X ") + 2)
	{mvprintw(0, COLS - longueur_str("Coloration X.X.X "), "Coloration %s", VERSION);} //Nom de l'éditeur en haut à droite
	else if (COLS / 2 > longueur_str("Gestion des accents") / 2 + longueur_str("Col ") + 2)
	{mvprintw(0, COLS - longueur_str("Col "), "Col ");} //Nom abrégé en haut à droite (manque de place)
	mvaddstrc(0, "Gestion des accents"); //affiche le titre de la fenêtre
	standend();
	mvprintw(1, 5, "Nom du car.");
	mvprintw(1, COLS - 13, "Maj.   Min.");
	
	//Affichage des raccourcis:
	attrset(COLOR_PAIR(10));
	mvprintw(LINES - 2, 1, "^Q");
	mvprintw(LINES - 1, 1, "Enter");
	if (COLS > 32)
	{mvprintw(LINES - 2, 25, "^A");}
	if (COLS > 67)
	{
		mvaddch(LINES - 1, 30, ACS_UARROW);
		printw(" ET ");
		addch(ACS_DARROW);
	}	
	standend();
	mvhline(LINES - 3, 0, ACS_HLINE, COLS);
	mvprintw(LINES - 2, 4, "Revenir à l'éditeur");
	mvprintw(LINES - 1, 7, "Modifier ce caractère");
	if (COLS > 32)
	{mvprintw(LINES - 2, 28, "Aide");}
	if (COLS > 67)
	{mvprintw(LINES - 1, 37, "Choisir un caractère à modifier");}
	
	if (msg_en_attente[0] != '\000')
	{print_msg(msg_en_attente); msg_en_attente[0] = '\000';}
	
	//Gestion de l'input:
	while (strcmp(keyname(input), "^[") != 0 && strcmp(keyname(input), "^Q") != 0)
	{
		//Affichage de la liste des paramètres avancés:
		for (int compteur = 0; compteur < 20 && compteur + 2 < LINES - 3; compteur++)
		{
			//Sélection:
			if (selection - decalage == compteur)
			{
				attrset(COLOR_PAIR(10));
				mvhline(compteur + 2, 1, ' ', COLS - 3);
				mvaddch(compteur + 2, 2, '>');
			}
			else
			{
				standend();
				mvhline(compteur + 2, 1, ' ', COLS - 3);
			}
			
			//Nom:
			mvaddstr(compteur + 2, 4, liste_accents[compteur + decalage].nom);
			
			//Valeurs:
			mvprintw(compteur + 2, COLS - 14, "%s:%d", liste_accents[compteur + decalage].majuscule, liste_accents[compteur + decalage].valeur_maj);
			mvprintw(compteur + 2, COLS - 7, "%s:%d", liste_accents[compteur + decalage].minuscule, liste_accents[compteur + decalage].valeur_min);
		}
		standend();
		refresh();
		
		//Gestion de l'input:
		if (!touche_simulee)
		{
			do
			{input = getch();} while (input == -1);
		}
		else
		{touche_simulee = FALSE;}
		
		switch (input)
		{
		case KEY_RESIZE:
			if (LINES < 6 || COLS < 33)
			{
				print_msg("Terminal trop petit pour gérer les accents"); //Ne sera jamais affiché, mais bon...
				input = 27; //Esc
				break;
			}
			else
			{return gestion_accents();}
			break;
		
		case KEY_MOUSE:
			getmouse(&mev);
			if (mev.bstate == 65536) //molette vers le haut
			{input = KEY_UP; touche_simulee = TRUE;}
			else if (mev.bstate == 2097152) //molette vers le bas
			{input = KEY_DOWN; touche_simulee = TRUE;}
			else if (mev.y == LINES - 2 && mev.x < 24) //^Q
			{input = 17; touche_simulee = TRUE;}
			else if (mev.y == LINES - 2 && mev.x < 33) //^A
			{input = 1; touche_simulee = TRUE;}
			else if (mev.y == LINES - 1 && mev.x < 29) //Enter
			{input = 13; touche_simulee = TRUE;}
			else if (mev.y > 2 && mev.y < LINES - 3 && mev.y + decalage < 22) //Clic sur une entrée
			{selection = mev.y + decalage - 2; input = 13; touche_simulee = TRUE;}
			break;
		
		case KEY_UP:
			if (selection > 0)
			{
				if (selection - decalage < 1)
				{decalage--;}
				selection--;
			}
			break;
		
		case KEY_DOWN:
			if (selection < 19)
			{
				if (selection - decalage > LINES - 7)
				{decalage++;}
				selection++;
			}
			break;
		
		default:
			if (!strcmp(keyname(input), "^A")) //Ctrl-A = Aide
			{aide(27); return gestion_accents();} //Esc
			else if (!strcmp(keyname(input), "^M")) //Ctrl-M = Enter = Modifier le caractère
			{modifier_accent(selection); return gestion_accents();}
			break;
		}
		
		print_msg(NULL);
	}
	
	if (!modification_faccents)
	{
		if (COLS > 40 && LINES > 10)
		{
			//Trouve les coordonnées du pop-up:
			xi = (COLS - 40) / 2;
			xf = xi + 40;
			yi = (LINES - 2 - 10) / 2;
			yf = yi + 10;
			
			//Redessine les bordures du pop-up:
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
			
			//Écriture du texte:
			mvaddstrc(yi, " Modification non enregistrées ");
			mvaddstrc(yi + 2, "Le fichier de sauvegarde des accents");
			mvaddstrc(yi + 3, "a été modifié et barré manuellement.");
			mvaddstrc(yi + 4, "Ouvrez le fichier et attribuez la");
			mvaddstrc(yi + 5, "valeur 1 à \"modification-permise\" si");
			mvaddstrc(yi + 6, "vous voulez utiliser cette interface.");
			mvaddstrc(yf - 3, "Appuyez sur \"Enter\" pour continuer.");
			standend();
			mvaddstrc(yf - 1, "< OK >");
			refresh(); //affichage du tout
		}
		else
		{
			attrset(COLOR_PAIR(10));
			for (int compteur = 0; compteur < LINES; compteur++)
			{mvhline(compteur, 0, ' ', COLS);}
			mvaddstr(0, 0, "Modification non enregistrées.");
			mvaddstr(2, 0, "Agrandissez votre terminal pour en savoir plus.");
			mvaddstr(LINES - 3, 0, "Appuyez sur \"Enter\" pour continuer.");
			standend();
			mvaddstrc(LINES - 1, "< OK >");
			refresh(); //affichage du tout
		}
		
		do
		{input = getch();} while (strcmp(keyname(input), "^M") != 0 && strcmp(keyname(input), "^[") != 0);
		return TRUE;
	}
	
	curs_set(1); //réapparition du curseur
	if (!cree_faccents())
	{return FALSE;}
	else
	{return TRUE;}
}


void modifier_accent(int num)
//Permet de changer les valeurs numériques d'un accent via une interface ncurses.
//Doit recevoir le numéro de l'accent (dans liste_accents) en paramètre.
{
	int input = ERR;
	char buffer[100] = "Nom: ";
	int selection = 1; //indique ce qui est présentement sélectionné (valeurs possibles: 0 = valeur_maj, 1 = majuscule (caractère), 2 = valeur_min, 3 = minuscule (caractère))
	char msg[200] = "";
	int xi, yi, xf, yf;
	
	if (COLS < 44 || LINES < 15)
	{strcpy(msg_en_attente, "Terminal trop petit pour modifier un caractère"); return;}
	
	//Initial:
	bkgd(COLOR_PAIR(10)); //fond blanc!
	erase();
	strcat(buffer, liste_accents[num].nom);
	attrset(COLOR_PAIR(11));
	mvhline(LINES - 2, 0, ACS_HLINE, COLS);
	standend();
	mvaddstrc(LINES - 2, " Utilisez les flèches pour choisir la valeur à modifier ");
	
	//Main loop:
	while (strcmp(keyname(input), "^[") != 0 && strcmp(keyname(input), "^Q") != 0)
	{		
		if (COLS < 44 || LINES < 15)
		{
			strcpy(msg_en_attente, "Terminal trop petit pour modifier un caractère");
			break;
		}
		
		//Méga boîte blanche:
		mvhline(1, 2, ACS_HLINE, COLS - 4);
		mvhline(LINES - 4, 2, ACS_HLINE, COLS - 4);
		mvvline(2, 1, ACS_VLINE, LINES - 4);
		mvvline(2, COLS - 2, ACS_VLINE, LINES - 4);
		mvaddch(1, 1, ACS_ULCORNER);
		mvaddch(1, COLS - 2, ACS_URCORNER);
		mvaddch(LINES - 4, 1, ACS_LLCORNER);
		mvaddch(LINES - 4, COLS - 2, ACS_LRCORNER);
		mvaddstrc(1, "Modifier un caractère accentué");
		mvvline(5, COLS / 2, ACS_VLINE, LINES - 10);
		
		//Contour de la boîte en noir:
		attrset(COLOR_PAIR(11)); //blanc sur noir
		mvvline(0, 0, ' ', LINES);
		mvvline(0, COLS - 1, ' ', LINES);
		mvhline(0, 0, ' ', COLS);
		mvhline(LINES - 3, 0, ' ', COLS);
		mvhline(LINES - 1, 0, ' ', COLS);
		attrset(COLOR_PAIR(10)); //noir sur blanc
		
		//Sous-titres:
		mvaddstrc(3, buffer);
		attron(A_BOLD | A_UNDERLINE);
		mvprintw(5, COLS / 4 - longueur_str("Majuscule:") / 2 + 1, "Majuscule:");
		mvprintw(5, COLS * 3 / 4 - longueur_str("Minuscule:") / 2, "Minuscule:");
		standend();
		
		//Majuscule:
		mvprintw(7, COLS / 4 - longueur_str("Caractère: XXX") / 2 + 1, "Caractère: ");
		attrset(COLOR_PAIR(11)); //blanc sur noir
		addch(' ');
		addstr(liste_accents[num].majuscule);
		addch(' ');
		standend();
		mvprintw(9, COLS / 4 - longueur_str("Valeur: XXXXX") / 2 + 1, "Valeur: ");
		attrset(COLOR_PAIR(11)); //blanc sur noir
		addch(' ');
		printw("%d", liste_accents[num].valeur_maj);
		addch(' ');
		standend();
		
		//Minuscule:
		mvprintw(7, COLS * 3 / 4 - longueur_str("Caractère: XXX") / 2, "Caractère: ");
		attrset(COLOR_PAIR(11)); //blanc sur noir
		addch(' ');
		addstr(liste_accents[num].minuscule);
		addch(' ');
		standend();
		mvprintw(9, COLS * 3 / 4 - longueur_str("Valeur: XXXXX") / 2, "Valeur: ");
		attrset(COLOR_PAIR(11)); //blanc sur noir
		addch(' ');
		printw("%d", liste_accents[num].valeur_min);
		addch(' ');
		
		//Instructions:
		mvprintw(LINES - 1, 4, "Enregistrer et Quitter");
		if (COLS > 51)
		{mvprintw(LINES - 1, 34, "Modifier la valeur");}
		if (COLS > 78)
		{mvprintw(LINES - 1, 57, "Supprimer cette entrée");}
		standend();
		mvprintw(LINES - 1, 1, "^Q");
		if (COLS > 51)
		{mvprintw(LINES  -1, 28, "Enter");}
		if (COLS > 78)
		{mvprintw(LINES  -1, 54, "^R");}
		
		//Affichage de la sélection
		for (int compteur = 0; compteur < 4; compteur++)
		{
			if (compteur < 2)
			{
				if (compteur == selection)
				{
					attrset(COLOR_PAIR(12) | A_BOLD);
					mvaddstr(7 + compteur * 2, 3, "->");
					standend();
				}
				else
				{mvaddstr(7 + compteur * 2, 3, "  ");}
			}
			else
			{
				if (compteur == selection)
				{
					attrset(COLOR_PAIR(12) | A_BOLD);
					mvaddstr(3 + compteur * 2, COLS / 2 + 2, "->");
					standend();
				}
				else
				{mvaddstr(3 + compteur * 2, COLS / 2 + 2, "  ");}
			}
		}
		
		//Redessinage de la barre d'état:
		if (msg[0] != '\000')
		{
			if (longueur_str(msg) > COLS - 2)
			{msg[COLS - 3] = '\000'; msg[COLS - 4] = '.'; msg[COLS - 5] = '.'; msg[COLS - 6] = '.';}
			mvaddstrc(LINES - 2, msg);
			msg[0] = '\000';
		}
		
		do
		{input = getch();} while (input == -1);
		
		//Input:
		switch (input)
		{
		case KEY_RESIZE:
			//Rien à faire ici...
			break;
		
		case KEY_MOUSE:
			strcpy(msg, " Souris non prise en charge: Utilisez les flèches pour vous déplacer ");
			break;
		
		case KEY_UP:
			if (selection == 1 || selection == 3)
			{selection--;}
			break;
		
		case KEY_DOWN:
			if (selection == 0 || selection == 2)
			{selection++;}
			break;
		
		case KEY_RIGHT:
			if (selection < 2)
			{selection += 2;}
			break;
		
		case KEY_LEFT:
			if (selection >= 2)
			{selection -= 2;}
			break;
		
		default:
			if (!strcmp(keyname(input), "^M")) //Enter
			{
				if (selection == 0 || selection == 2)
				{
					if (num < 14)
					{strcpy(msg, " Vous ne pouvez pas modifier ce caractère ");}
					else
					{
						//Trouve les coordonnées du pop-up:
						xi = (COLS - 40) / 2;
						xf = xi + 40;
						yi = (LINES - 2 - 10) / 2;
						yf = yi + 11;
						
						//Redessine les bordures du pop-up:
						attrset(COLOR_PAIR(11)); //blanc sur noir
						mvhline(yi, xi, ACS_HLINE, 40);
						mvhline(yf, xi, ACS_HLINE, 40);
						mvvline(yi, xi, ACS_VLINE, 11);
						mvvline(yi, xf, ACS_VLINE, 11);
						mvaddch(yi, xi, ACS_ULCORNER);
						mvaddch(yf, xi, ACS_LLCORNER);
						mvaddch(yi, xf, ACS_URCORNER);
						mvaddch(yf, xf, ACS_LRCORNER);
						
						//Remplit le pop-up:
						for (int compteur = yi + 1; compteur < yf; compteur++)
						{mvhline(compteur, xi + 1, ' ', 39);}
						
						//Écriture du texte:
						mvaddstrc(yi, " Modification manuelle seulement ");
						mvaddstrc(yi + 2, "Pour remplacer ce caractère par un");
						mvaddstrc(yi + 3, "autre, vous devez ouvrir le fichier de");
						mvaddstrc(yi + 4, "sauvegarde des accents et y modifier");
						mvaddstrc(yi + 5, "manuellement le caractère désiré.");
						mvaddstrc(yi + 6, "Vous trouverez le nom du fichier dans");
						mvaddstrc(yi + 7, "les paramètres avancés.");
						mvaddstrc(yf - 3, "Appuyez sur \"Enter\" pour continuer.");
						standend();
						mvaddstrc(yf - 1, "< OK >");
						refresh(); //affichage du tout
						
						do
						{input = getch();} while (strcmp(keyname(input), "^M") != 0 && strcmp(keyname(input), "^[") != 0);
					}
				}
				else
				{
					mvaddstrc(LINES - 2, " Appuyez sur la touche ou la combinaison de touches à mapper à ce caractère ");
					do
					{input = getch();} while (input == -1);
					if (input == 195)
					{input = getch();} //il faut mettre un autre getch, parce que sinon, on a seulement le 195 qui compose la 1ère moitié de chaque accent français...
					if (selection == 1)
					{liste_accents[num].valeur_maj = input;}
					else
					{liste_accents[num].valeur_min = input;}
				}
			}
			
			else if (!strcmp(keyname(input), "^R"))
			{
				liste_accents[num].valeur_maj = 0;
				liste_accents[num].valeur_min = 0;
			}
			break;
		}
		
		erase();
		attrset(COLOR_PAIR(11));
		mvhline(LINES - 2, 0, ACS_HLINE, COLS);
		standend();
	}
	
	bkgd(COLOR_PAIR(0)); //retour en blanc sur noir
}


bool cree_faccents()
//Crée un fichier contenant les accents et leurs valeurs numériques associées pour pouvoir les enregistrer.
//Sera appelé pour mettre à jour faccents s'il est modifiée via l'interface du programme.
//Devrait aussi être appelé dès l'initialisation si le fichier n'existe pas déjà.
//Renvoie TRUE en cas de succès et FALSE en cas d'erreur.
{
	FILE* faccents = NULL;
	
	//Création du fichier:
	faccents = fopen(nom_faccents, "w+");
	if (faccents == NULL)
	{
		erreur(40, "Impossible de créer le fichier de sauvegarde de la base de données des accents!");
		return 0;
	}
	
	//Entête:
	fprintf(faccents, "Fichier de sauvegarde de la base de données (liste) des accents de l'éditeur de texte Coloration.\n");
	fprintf(faccents, "Ce fichier a été généré automatiquement et sera écrasé à chaque modification de ces données via l'interface de l'éditeur.\nModifiez-le à vos propres risques.\n\n");
	fprintf(faccents, "Les 14 premières entrées ne peuvent pas être modifiées (sauf leurs valeurs numériques).\nIl ne peut pas y avoir plus que 20 entrées au total.\nTout caractère associé à une valeur de 0 sera ignoré.\n\n");
	fprintf(faccents, "Chaque caractère accentué doit être inscrit ainsi: \"[ NOM:N:n:C:c ]\" (sans les guillemets), où:\n");
	fprintf(faccents, "- N et n sont les valeurs numériques ncurses de la majuscule et de la minuscule du caractère.\n- C et c sont ces caractères eux-mêmes (majuscule et minuscule).\n\n");
	fprintf(faccents, "Il est important de respecter l'espacement avec les parenthèses carrées.\n");
	fprintf(faccents, "Tout texte écrit en dehors de ces parentèses carrées sera ignoré par le programme.\n\n\n");
	
	//Données:
	fprintf(faccents, "Ne modifiez pas ces entrées (sauf les valeurs numériques):\n");
	for (int compteur = 0; compteur < 20; compteur++)
	{
		fprintf(faccents, "[ %s:%s:%s:%d:%d ]\n", liste_accents[compteur].nom, liste_accents[compteur].majuscule, liste_accents[compteur].minuscule, liste_accents[compteur].valeur_maj, liste_accents[compteur].valeur_min);
		if (compteur == 13)
		{fprintf(faccents, "\nLes prochaines entrées sont pour vous (mais ne changez pas les noms):\n");}
	}
	
	fprintf(faccents, "\n\nRemplacez cette valeur par 0 pour que ce fichier ne soit pas écrasé (vous ne pourrez plus gérer vos accents via l'interface de Coloration):\n[ modification-permise:%d ]\n", modification_faccents);
	
	//Fermeture du fichier:
	fclose(faccents);
	return 1;
}


bool lire_faccents()
//Ouvre et lis le fichier contenant la "base de données" des accents (s'il existe).
//Appelé lors de l'initialisation seulement.
//Renvoie TRUE si le fichier existe et qu'il a pu être lu au complet avec succès et FALSE dans le cas contraire.
//Si cette fonction renvoie FALSE, cree_faccents devrait tout de suite être appelé.
{
	FILE* faccents = NULL;
	char ligne[250] = "";
	char* mot = "";
	int compteur = 0;
	
	//Vérification de la présence du fichier et ouverture de celui-ci:
	faccents = fopen(nom_faccents, "r");
	if (faccents == NULL)
	{return FALSE;}
	
	//Lecture des données:
	while (fgets(ligne, sizeof(ligne), faccents) != NULL)
	{
		mot = strtok(ligne, ":;, \t\n");
		while (mot != NULL && strcmp(mot, "[") != 0)
		{mot = strtok(NULL, ":;, \t\n");}
		if (mot != NULL)
		{
			//Nom:
			mot = strtok(NULL, ":;, \t\n");
			if (mot == NULL)
			{erreur(41, "Entrée invalide dans le fichier de sauvegarde des accents (terminaison inattendue)"); break;}
			for (compteur = 0; compteur < 19 && strcmp(mot, liste_accents[compteur].nom) != 0; compteur++) {}
			if (!strcmp(mot, "modification-permise") || !strcmp(mot, "modification_permise"))
			{
				mot = strtok(NULL, ":;, \t\n");
				if (mot == NULL || !est_un_nbre(mot, 'b'))
				{erreur(46, "Entrée invalide dans le fichier de sauvegarde des accents (modification-permise doit être 0 ou 1)"); break;}
				sscanf(mot, "%d", &compteur); //compteur sert ici de buffer...
				modification_faccents = (bool) compteur;
				
				mot = strtok(NULL, ":;, \t\n");
				if (mot == NULL || strcmp(mot, "]") != 0)
				{erreur(45, "Entrée invalide dans le fichier de sauvegarde des accents (aucun \"]\" pour compléter \"[\")");}
				break;
			}
			else if (strcmp(mot, liste_accents[compteur].nom) != 0)
			{erreur(42, "Entrée invalide dans le fichier de sauvegarde des accents (nom de caractère non reconnu)"); break;}
			
			//Majuscule:
			mot = strtok(NULL, ":;, \t\n");
			if (mot == NULL || !strcmp(mot, "]") || strlen(mot) > 2)
			{erreur(43, "Entrée invalide dans le fichier de sauvegarde des accents (majuscule de plus de 2 bytes ou terminaison inattendue)"); break;}
			strcpy(liste_accents[compteur].majuscule, mot);
			
			//Minuscule:
			mot = strtok(NULL, ":;, \t\n");
			if (mot == NULL || !strcmp(mot, "]") || strlen(mot) > 2)
			{erreur(43, "Entrée invalide dans le fichier de sauvegarde des accents (minuscule de plus de 2 bytes ou terminaison inattendue)"); break;}
			strcpy(liste_accents[compteur].minuscule, mot);
			
			//Valeur numérique de la majuscule:
			mot = strtok(NULL, ":;, \t\n");
			if (mot == NULL || !strcmp(mot, "]") || !est_un_nbre(mot, 'I'))
			{erreur(44, "Entrée invalide dans le fichier de sauvegarde des accents (fausse valeur numérique (majuscule) ou terminaison inattendue)"); break;}
			sscanf(mot, "%d", &liste_accents[compteur].valeur_maj);
			
			//Valeur numérique de la minuscule:
			mot = strtok(NULL, ":;, \t\n");
			if (mot == NULL || !strcmp(mot, "]") || !est_un_nbre(mot, 'I'))
			{erreur(44, "Entrée invalide dans le fichier de sauvegarde des accents (fausse valeur numérique (minuscule) ou terminaison inattendue)"); break;}
			sscanf(mot, "%d", &liste_accents[compteur].valeur_min);
			
			mot = strtok(NULL, ":;, \t\n");
			if (mot == NULL || strcmp(mot, "]") != 0)
			{erreur(45, "Entrée invalide dans le fichier de sauvegarde des accents (aucun \"]\" pour compléter \"[\")"); break;}
		}
	}
	
	//Fermeture du fichier:
	fclose(faccents);
	return TRUE;
}

#endif //_ACCENTS_H	