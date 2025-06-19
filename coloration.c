#include "outils_logiques.c"


void quitter(int code)
//Permet de fermer le programme en s'assurant de fermer le fichier modifié et en quittant ncurses.
//Cette fonction doit obligatoirement être appelée pour fermer le programme.
//Le paramètre "code" sert uniquement à ne pas fermer le programme après avoir tout fermer s'il vaut -1.
{
	fclose(fichier);
	endwin();
	while (erreur(0, "...") > 0) {} //affiche toutes les erreurs qui n'ont pas pu s'afficher encore
	if (code != -1)
	{exit(0);}
}


int erreur(int code, char message[])
//Gère les erreurs dans le programme.
//Log une erreur (et son message de moins de 400 caractères) qui pourra être consultée plus tard si le code est positif.
//Ces erreurs sont aussi loguées dans ferreur, si possible et que l'option (err_log) est activée.
//Affiche la dernière erreur en date si le code est 0, sauf si le message est "init", auquel cas une marque sera plutôt ajoutée à ferreur pour signifier le démarrage du programme.
//Renvoie toujours le nombre d'erreurs encore "actives" (incluant l'erreur consultée, s'il y a lieu).
//Ferme le programme après trop d'erreurs non-consultées.
{
	static char msgs[5][400]; //variable contenant le message d'erreur à afficher (s'il y a lieu)
	static int codes[5];
	static int nbre_erreurs = 0; //en fait, non...
	char buffer[450];
	FILE* ferr = NULL;
	
	if (!code && !strcmp(message, "init") && err_log)
	{
		if ((ferr = fopen(nom_ferreur, "a+")) != NULL)
		{
			fprintf(ferr, "\nOuverture et initialisation du programme.\n");
			fclose(ferr);
		}
		else
		{
			err_log = 0;
			erreur(5, "Impossible d'ouvrir ou créer le fichier de log d'erreurs.");
		}
	}
	
	if (!code)
	//Affichage de l'erreur:
	{
		if (!nbre_erreurs)
		{return 0;} //aucune erreur à afficher
		
		if (isendwin())
		{printf("Erreur %d: %s\n", codes[nbre_erreurs - 1], msgs[nbre_erreurs - 1]);}
		else
		{
			sprintf(buffer, "Erreur %d: %s", codes[nbre_erreurs - 1], msgs[nbre_erreurs - 1]);
			print_msg(buffer);
		}
		
		nbre_erreurs--;
		return (nbre_erreurs + 1);
	}
	
	else //nouvelle erreur:
	{
		//Log une erreur:
		if (err_log)
		{
			if ((ferr = fopen(nom_ferreur, "a+")) != NULL)
			{
				fprintf(ferr, "Erreur %d: %s\n", code, message);
				fclose(ferr);
			}
			else
			{
				err_log = 0;
				erreur(5, "Impossible d'ouvrir ou créer le fichier de log d'erreurs.");
			}
		}
		
		if (nbre_erreurs == 5)
		//Trop d'erreurs!
		{
			quitter(-1); //ferme tout et fait le ménage, mais sans vrm quitter
			while(erreur(0, "...") > 0) {}
			printf("\nOups!\nLe programme s'est interrompu après avoir reçu trop d'erreurs d'affilée.\n\n");
			exit(2);
		}
		else
		{
			codes[nbre_erreurs] = code;
			strcpy(msgs[nbre_erreurs], message);
		}
		
		nbre_erreurs++;
		return nbre_erreurs;
	}
}


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
	
	mvprintw(2, 4, "COLORATION %s\n\n    ---\n\n    ", VERSION);
	printw("Éditeur de texte inspiré de GNU nano.\n\n    Ce projet a pour but de faciliter l'édition des fichiers sources de scénarios (listes d'objets) du projet \"text-adventure game\".\n\n    ");
	printw("Programmé par Nicolas Audette.");
	
	while (strcmp(keyname(input), "^[") != 0 && strcmp(keyname(input), "^M") != 0)
	{input = getch();}
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
	int _x;
	
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
	mvprintw(8, 2, "Details:");
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
{
	char titre[100] = "Aide";
	char buffer[200] = "";
	int input = ERR;
	int selection = 1;
	int decalage = 0;
	int marge = 10;
	
	//Ajustement du titre et gestion de ce qui ne devrait jamais arriver:
	if (num_cmd > 0)
	{strcat(titre, ": "); strcat(titre, cmds[num_cmd].nom);}
	else if (num_cmd < 0)
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
	mvaddstrc(0, titre); //affiche le nom du fichier modifié
	
	//Vérification de la taille du terminal:
	if (COLS < 44)
	{
		rafraichir();
		curs_set(1);
		print_msg("Terminal trop petit pour afficher l'aide de l'éditeur.");
		return;
	}
	else if (COLS < 54)
	{marge = COLS - 44;}
		
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
		if (COLS >= 44)
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
		if (COLS >= 44)
		{mvprintw(LINES - 1, 48, "Lire l'article sélectionné");}
	}
	
	//Titres des colonnes et message de bienvenue:
	mvprintw(1, marge, "Nom:         | Touche: |        Description:");
	print_msg("Bienvenue dans le module d'aide de Coloration! Appuyer sur \"Enter\" pour en savoir plus.");
	
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
				buffer[COLS - 39] = '.';
				buffer[COLS - 38] = '.';
				buffer[COLS - 37] = '.';
				buffer[COLS - 36] = '\000';
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
			aide(0);
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
					aide(0);
					return;
				}
			}
			break;
		}
		print_msg(NULL);
	}
}


void enregistrer(char nom_sauvegarde[])
//Enregistre/Sauvegarde le buffer actuellement modifié.
//Le nom du fichier peut être reçu en paramètre ou pas.
//Si aucun nom de fichier n'est reçu en paramètre, un nom sera demandé à l'utilisateur.
{
	int input = EOF;
	char nom[75] = "";
	int pos = 0;
	ligne* ln = DEBUT_FICHIER.suivant;
	FILE* fsauv = NULL;
		
	//Prise du nom de fichier:
	if (nom_sauvegarde == NULL)
	{
		//Effaçage du bas de l'écran:
		print_msg(NULL);
		mvhline(LINES - 1, 0, ' ', COLS);
		
		//Liste des options:
		mvprintw(LINES - 1, 4, "Annuler");
		mvprintw(LINES - 1, 16, "Aide");
		mvprintw(LINES - 1, 25, "Visualiser");
		
		if (COLS - longueur_str("Enregistrement ") > 34)
		{mvprintw(LINES - 1, COLS - longueur_str("Enregistrement "), "Enregistrement ");}
		
		//Effaçage de la ligne de commande
		attrset(COLOR_PAIR(10));
		mvhline(LINES - 2, 0, ' ', COLS);
		
		//Écriture des raccourcis:
		mvprintw(LINES - 1, 1, "^C");
		mvprintw(LINES - 1, 13, "^A");
		mvprintw(LINES - 1, 22, "^V");
		
		//Mise en forme de la ligne de commande:
		mvprintw(LINES - 2, 1, "Nom du fichier: ");
		
		//Prise de la commande:
		while (strcmp(keyname(input), "^M") != 0) //Ctrl-M = Enter
		{
			do
			{input = getch();} while (input == -1);
			
			//Caractères imprimables (sauf les accents, mais bon...)
			if (input >= ' ' && input <= 126 && pos < 74)
			{nom[pos] = input; pos++; addch(input);}
			
			//KEY_BACKSPACE: Effacer
			else if (input == KEY_BACKSPACE && pos > 0)
			{pos--; mvaddch(LINES - 2, 17 + pos, ' '); move(LINES - 2, 17 + pos);}
			
			//Ctrl-C: Annuler
			else if (!strcmp(keyname(input), "^C"))
			{rafraichir(); return;}
			
			//Ctrl-A: Aide
			else if (!strcmp(keyname(input), "^A"))
			{aide(5); enregistrer(nom_sauvegarde); rafraichir(); return;}
		}
		nom[pos] = '\000';
	}
	else
	{strcpy(nom, nom_sauvegarde);}
	
	//Création du fichier de sauvegarde:
	fsauv = fopen(nom, "w");
	if (fsauv == NULL)
	{print_msg("Impossible d'enregistrer le fichier."); erreur(100, "Impossible de créer le fichier de sauvegarde..."); return;}
	
	//Écriture du texte:
	fputs(ln->txt, fsauv);
	ln = ln->suivant;
	while (ln != NULL & ln != &FIN_FICHIER)
	{
		fputc('\n', fsauv);
		fputs(ln->txt, fsauv);
		ln = ln->suivant;
	}
	
	//Fermeture du fichier de sauvegarde:
	fclose(fsauv);
	
	//Écriture d'un message de confirmation:
	rafraichir();
	print_msg("Fichier enregistré.");
}


void term()
//Donne accès au terminal via une ligne de commande.
//Il est préférable de passer par cmd("terminal") (ou cmd()) pour appeler cette fonction.
{
	int input = EOF;
	char commande[151] = "";
	int pos = 0;
	
	
	//Effaçage du bas de l'écran:
	print_msg(NULL);
	mvhline(LINES - 1, 0, ' ', COLS);
	
	//Liste des options:
	mvprintw(LINES - 1, 4, "Annuler");
	mvprintw(LINES - 1, 16, "Aide");
	if (COLS >= 53)
	{mvprintw(LINES - 1, 25, "Accéder au terminal complet");}
	else if (COLS >= 42)
	{mvprintw(LINES - 1, 25, "Terminal complet");}
	
	if (COLS - longueur_str("Accès au terminal ") > 54)
	{mvprintw(LINES - 1, COLS - longueur_str("Accès au terminal "), "Accès au terminal ");}
	
	//Effaçage de la ligne de commande
	attrset(COLOR_PAIR(10));
	mvhline(LINES - 2, 0, ' ', COLS);
	
	//Écriture des raccourcis:
	mvprintw(LINES - 1, 1, "^C");
	mvprintw(LINES - 1, 13, "^A");
	if (COLS >= 42)
	{mvprintw(LINES - 1, 22, "^T");}
	
	//Mise en forme de la ligne de commande:
	mvprintw(LINES - 2, 1, "Commande: ");
	
	//Prise du numéro de ligne:
	while (strcmp(keyname(input), "^M") != 0) //Ctrl-M = Enter
	{
		do
		{input = getch();} while (input == -1);
		
		//Caractères imprimables (sauf les accents, mais bon...)
		if (input >= ' ' && input <= 126 && pos < 150)
		{commande[pos] = input; pos++; addch(input);}
		
		//KEY_BACKSPACE: Effacer
		else if (input == KEY_BACKSPACE && pos > 0)
		{pos--; mvaddch(LINES - 2, 11 + pos, ' '); move(LINES - 2, 11 + pos);}
		
		//Ctrl-C: Annuler
		else if (!strcmp(keyname(input), "^C"))
		{rafraichir(); return;}
		
		//Ctrl-A: Aide
		else if (!strcmp(keyname(input), "^A"))
		{aide(11); term(); rafraichir(); return;}
		
		//Ctrl-O: Options
		else if (!strcmp(keyname(input), "^T"))
		{strcpy(commande, "bash"); pos = 4; input = 13;} //13 = Enter
	}
	commande[pos] = '\000';
	
	endwin(); //ferme temporairement l'interface ncurses, permettant de bien voir le terminal
	if (!strcmp(commande, "bash"))
	{
		system("clear"); //efface le contenu du terminal...
		printf("Terminal Bash ouvert depuis l'éditeur COLORATION:\nPour revenir à l'éditeur, entrez la commande \"exit\".\n\n"); //affiche un message indiquant comment quitter/revenir
		system(commande); //envoie la commande pour que la shell l'exécute
		system("clear");
	}
	else
	{
		system(commande); //envoie la commande pour que la shell l'exécute
		printf("\nAppuyer sur \"Enter\" pour revenir à l'éditeur."); //petit rappel affiché dans la console, une fois la commande exécutée
		getchar(); //on attend un input de l'utilisateur pour lui laisser le temps de lire l'output de sa commande
				   //J'ai choisi d'utiliser getchar() parce que getch() ne fonctionne pas vraiment bien dans ce contexte, et puisqu'on ne prend qu'un seul caractère (même si on entre ensuite une nouvelle commande,
				   // les 2 getchar() seront entrecoupés par ncurses), les folies rageantes de getchar() ne devraient pas nous affecter...
	}
	rafraichir(); //rétablissement de l'interface ncurses et affichage du programme
}


ligne* aller_a(int num)
//Emmène l'utilisateur à une certaine ligne (reçue en paramètre) du fichier.
//Si num est 0, demande (grâce à une ligne de commande) un numéro de ligne à l'utilisateur et l'y emmène.
//Renvoie un pointeur vers la ligne en question ou NULL si ce n'est pas un numéro de ligne valide.
//Cette fonction ne devrait pas être appelée par aucune autre que cmd() (mais on peut le faire indirectement via cmd("aller a") ou cmd("ligne #ligne")).
{
	char nl[8] = "";
	int pos = 0;
	int input = EOF;
	ligne* buff = NULL;
	
	if (!num)
	{
		//Effaçage du bas de l'écran:
		print_msg(NULL);
		mvhline(LINES - 1, 0, ' ', COLS);
		
		//Liste des options:
		mvprintw(LINES - 1, 4, "Annuler");
		mvprintw(LINES - 1, 16, "Aide");
		//mvprintw(LINES - 1, 25, "Options");
		
		//Effaçage de la ligne de commande
		attrset(COLOR_PAIR(10));
		mvhline(LINES - 2, 0, ' ', COLS);
		
		//Écriture des raccourcis:
		mvprintw(LINES - 1, 1, "^C");
		mvprintw(LINES - 1, 13, "^A");
		//mvprintw(LINES - 1, 22, "^O");
		
		//Mise en forme de la ligne de commande:
		mvprintw(LINES - 2, 1, "Numéro de ligne: ");
		
		//Prise du numéro de ligne:
		while (strcmp(keyname(input), "^M") != 0) //Ctrl-M = Enter
		{
			do
			{input = getch();} while (input == -1);
			
			//Caractères imprimables (sauf les accents, mais bon...)
			if (input >= '0' && input <= '9' && pos < 7)
			{nl[pos] = input; pos++; addch(input);}
			
			//KEY_BACKSPACE: Effacer
			else if (input == KEY_BACKSPACE && pos > 0)
			{pos--; mvaddch(LINES - 2, 18 + pos, ' '); move(LINES - 2, 18 + pos);}
			
			//Ctrl-C: Annuler
			else if (!strcmp(keyname(input), "^C"))
			{rafraichir(); return NULL;}
			
			//Ctrl-A: Aide
			else if (!strcmp(keyname(input), "^A"))
			{aide(8); buff = aller_a(num); rafraichir(); return buff;}
		}
		nl[pos] = '\000';
		sscanf(nl, "%d", &num);
	}
	
	if (num <= 0 || num > init_ligne(0)->num)
	{rafraichir(); print_msg("Ce n'est pas un numéro de ligne valide."); return NULL;}
	else
	{
		premiere_ligne = num;
		rafraichir();
		ln_mod = trouve_ligne(num);
		return ln_mod;
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
					//Paramètres avancés...
					break;
				
				case 3: //Sauvegarder
					cmd("sauvegarder");
					curs_set(1);
					return;
				
				case 4: //Ouvrir un autre fichier
					//Ouvrir...
					break;
				
				case 5: //Afficher les crédits
					credits();
					break;
				
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


void cmd(char commande[])
//Exécute une commande reçue en paramètre.
//Si ce paramètre est NULL, une commande sera d'abord demandée à l'utilisateur.
//L'écran sera ensuite redessiné et un message sera peut-être affiché dans la barre d'état.
{
	int input = EOF; //input de l'utilisateur (utilisé si commande est NULL)
	int pos = 0; //position dans _cmd (nbre de caractères lus) (utilisé si commande est NULL)
	int buffint = 0;
	char _cmd[75] = ""; //commande à exécuter (soit "commande" ou la commande reçue de l'utilisateur si c'est NULL)
	char mot[4][25]; //array qui contiendra les différents mots (jusqu'à 4) de la commande une fois parsée
	ligne* buffln = NULL;
	
	
	//Prise de la commande:
	if (commande == NULL)
	{
		//Effaçage du bas de l'écran:
		print_msg(NULL);
		mvhline(LINES - 1, 0, ' ', COLS);
		
		//Liste des options:
		mvprintw(LINES - 1, 4, "Annuler");
		mvprintw(LINES - 1, 16, "Aide");
		//mvprintw(LINES - 1, 25, "Options");
		
		//Effaçage de la ligne de commande
		attrset(COLOR_PAIR(10));
		mvhline(LINES - 2, 0, ' ', COLS);
		
		//Écriture des raccourcis:
		mvprintw(LINES - 1, 1, "^C");
		mvprintw(LINES - 1, 13, "^A");
		//mvprintw(LINES - 1, 22, "^O");
		
		//Mise en forme de la ligne de commande:
		mvprintw(LINES - 2, 1, "Commande: ");
		
		//Prise de la commande:
		while (strcmp(keyname(input), "^M") != 0) //Ctrl-M = Enter
		{
			do
			{input = getch();} while (input == -1);
			
			//Caractères imprimables (sauf les accents, mais bon...)
			if (input >= ' ' && input <= 126 && pos < 74)
			{_cmd[pos] = input; pos++; addch(input);}
			
			//KEY_BACKSPACE: Effacer
			else if (input == KEY_BACKSPACE && pos > 0)
			{pos--; mvaddch(LINES - 2, 11 + pos, ' '); move(LINES - 2, 11 + pos);}
			
			//Ctrl-C: Annuler
			else if (!strcmp(keyname(input), "^C"))
			{rafraichir(); return;}
			
			//Ctrl-A: Aide
			else if (!strcmp(keyname(input), "^A"))
			{aide(10); cmd(NULL); return;}
		}
		_cmd[pos] = '\000';
	}
	
	else //commande != NULL:
	{strcpy(_cmd, commande);}
	
	//Parsing de la commande:
	sprintf(mot[0], "%s", strtok(_cmd, " "));
	for (int compteur = 1; compteur < 4 && mot[compteur -1] != NULL; compteur++)
	{sprintf(mot[compteur], "%s", strtok(NULL, " "));}
	
	//########################################################
	
	//Application de la commande:
	//Aide:
	if (!strcmp(mot[0], "aide"))
	{aide(0); return;}
	
	//Rafraichir:
	else if (!strcmp(mot[0], "rafraichir"))
	{rafraichir(); erreur(0, "..."); return;}
	
	//Quitter:
	else if (!strcmp(mot[0], "quitter"))
	{desinit(); quitter(0);}
	
	//Aller à une certaine ligne dans le fichier:
	else if (!strcmp(mot[0], "aller") || !strcmp(mot[0], "ligne"))
	{
		if (!strcmp(mot[1], "a") || !strcmp(mot[1], "à") || !strcmp(mot[1], "au"))
		{
			strcpy(mot[1], mot[2]);
			if (!strcmp(mot[1], "la"))
			{strcpy(mot[1], mot[3]);}
		}
		
		if (!strcmp(mot[1], "(null)"))
		{buffln = aller_a(0);}
		else if (!strcmp(mot[1], "DEBUT") || !strcmp(mot[1], "DÉBUT"))
		{cmd("DÉBUT"); buffln = NULL;}
		else if (!strcmp(mot[1], "FIN"))
		{cmd("FIN"); buffln = NULL;}
		else
		{
			sscanf(mot[1], "%d", &buffint);
			if (buffint <= 0)
			{
				rafraichir();
				print_msg("Seul un numéro de ligne valide peut être accepté comme argument par cette commande.");
				buffln == NULL;
			}
			else
			{buffln = aller_a(buffint);}
		}
		
		if (buffln != NULL)
		{ln_mod = buffln; pos_y = 1; mv(1, 4);}
	}
	
	//Aller au -DÉBUT-:
	else if (!strcmp(mot[0], "DEBUT") || !strcmp(mot[0], "DÉBUT"))
	{
		buffln = trouve_tag(DEBUT, NULL);
		if (buffln == NULL)
		{print_msg("Ce fichier ne contient aucun -DÉBUT-.");}
		else
		{
			premiere_ligne = buffln->num;
			rafraichir();
			ln_mod = buffln;
			pos_y = 1;
			mv(1, 4);
		}
	}
	
	//Aller à la -FIN-:
	else if (!strcmp(mot[0], "FIN"))
	{
		buffln = trouve_tag(FIN, NULL);
		if (buffln == NULL)
		{print_msg("Ce fichier ne contient aucune -FIN-.");}
		else
		{
			premiere_ligne = buffln->num;
			rafraichir();
			ln_mod = buffln;
			pos_y = 1;
			mv(1, 4);
		}
	}
	
	//Accéder au terminal:
	else if (!strcmp(mot[0], "terminal") || !strcmp(mot[0], "term"))
	{term();}
	
	//Enregistrer ou Sauvegarder:
	else if (!strcmp(mot[0], "enregistrer") || !strcmp(mot[0], "enr"))
	{
		if (!strcmp(mot[1], "comme") || !strcmp(mot[1], "sous"))
		{strcpy(mot[1], mot[2]);}
		if (!strcmp(mot[1], "(null)"))
		{enregistrer(NULL);}
		else
		{enregistrer(mot[1]);}
	}
	else if (!strcmp(mot[0], "sauvegarder") || !strcmp(mot[0], "sauv"))
	{enregistrer(nom_fichier);}
	
	//Ouvir le menu des options:
	else if (!strcmp(mot[0], "menu") || !strcmp(mot[0], "options"))
	{menu_options();}
	
	//Aucune commande:
	else if (!strcmp(mot[0], "(null)"))
	{rafraichir();}
	
	//Commande non-reconnue:
	else
	{rafraichir(); print_msg("Ceci n'est pas une commande valide.");}
}


int main(int argc, char* argv[])
{
	int input = EOF; //caractère reçu en input (doit être déclaré int pour accepter les accents, Ctrl-car., etc.)
	int buffint = 0;
	char buffer[20] = "";
	
	
	//Erreurs d'invocation:
	if (argc < 2)
	{printf("Veuillez spécifier un fichier contenant une liste d'objets à afficher.\nExemple: \"%s liste_objets.txt\"\n\n", argv[0]); exit(0);}
	
	strcpy(nom_fichier, argv[1]);
	fichier = fopen(nom_fichier, "r");
	if (fichier == NULL)
	{printf("Impossible d'ouvrir le fichier \"%s\".\nExiste-t-il? Assurez-vous d'entrer le chemin complet ou le chemin relatif à ce dossier.\n\n", nom_fichier); exit(1);}
	
	
	//Initialisation:
	strcat(cmd_compiler_liste, nom_fichier); //ajout du nom du fichier à la commande permettant de le compiler comme liste d'objets
	setlocale(LC_ALL, "en_CA.UTF-8"); //permet l'affichage des accents!
	initscr(); //initialise ncurses et créé stdscr
	nodelay(stdscr, TRUE); //enlève le delai au rafraichissement de l'écran
	raw(); //prend son input direct du terminal, sans attendre de \n et sans jamais lever de signal
	nonl(); //Empêche ^J, ^M et Enter d'être la même foutue key en ne convertissant pas le enter en newline... (^M et Enter sont quand même pareils, par contre...)
	ESCDELAY = 0; //enlève le délai à la réception du escape
	keypad(stdscr, TRUE); //permet de prendre les flèches et autres touches du clavier comme input
	noecho(); //n'echo pas l'input automatiquement
	if (debogage)
	{erreur(0, "init");} //log un message signalant l'ouverture du programme (lorsque le débogage est activé)
	if (can_change_color) //le terminal supporte différentes couleurs et on peut modifier ces couleurs
	{
		start_color(); //initialise la gestion des couleurs de ncurses
		init_pair(1, COLOR_GREEN, COLOR_BLACK); //crée une paire (#1) vert sur noir
		init_pair(2, COLOR_YELLOW, COLOR_BLACK); //etc.
		init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
		init_pair(4, COLOR_CYAN, COLOR_BLACK);
		init_pair(5, COLOR_BLUE, COLOR_BLACK);
		init_pair(6, COLOR_RED, COLOR_BLACK);
		init_pair(7, 190, COLOR_BLACK);
		init_pair(8, COLOR_WHITE, COLOR_RED);
		init_pair(10, COLOR_BLACK, COLOR_WHITE);
	}
	else
	{printf("Ce terminal ne supporte pas l'affichage en couleur.\nVeuillez réessayer avec un autre terminal.\n\n"); quitter(1);}
	
	//Initialisation:
	init(); //des lignes du fichier
	rafraichir(); //de l'écran d'affichage
	ln_mod = trouve_ligne(premiere_ligne);
	mv(1, 4);
	
	
	//Main Loop:
	while (1)
	{
		//Débogage de la position:
		if (debogage && element_debogue == 'p' && barre_dispo)
		{msg_printf("Ligne #%d.%d", ln_mod->num, pos_y);}
		
		//Prise de l'input:
		do
		{input = getch();} while (input == -1);
		
		//Réaffichage de la barre si nécessaire:
		if (!barre_dispo)
		{print_msg(NULL);}
		
		//Débogage de l'input:
		if (debogage && element_debogue == 'i') //mode input
		{
			print_msg((char*) keyname(input)); //affichage du caractère reçu (son "nom"...)
			if (strcmp(keyname(input), "^D") != 0 && strcmp(keyname(input), "M-#") != 0 && strcmp(keyname(input), "^Q") != 0)
			{input = -1;} //dans ce mode de débogage, l'input ne doit pas être interprété, sauf ces 3 keys
		}
		else if (debogage && element_debogue == 'n') //mode input (raw)
		{
			msg_printf("input: %d", input); //affichage de la valeur numérique du caractère reçu
			if (strcmp(keyname(input), "^D") != 0 && strcmp(keyname(input), "M-#") != 0 && strcmp(keyname(input), "^Q") != 0)
			{input = -1;}  //dans ce mode de débogage, l'input ne doit pas être interprété, sauf ces 3 keys
		}
		
		switch (input)
		{
		case KEY_RESIZE: //on a resizé le terminal
			rafraichir();
			sprintf(buffer, "ligne %d", premiere_ligne); //Pour se sauver du trouble, on va juste envoyer le curseur au début de la première ligne...
			cmd(buffer);
			break;
		
		case KEY_DOWN: //flèche vers le bas
			if (ln_mod->suivant == &FIN_FICHIER && pos_y == ln_mod->multiligne)
			{print_msg("Vous avez atteint la fin du fichier.");}
			else
			{
				if (pos_y == ln_mod->multiligne) //on est à la ligne la plus basse de la "ligne"...
				{ln_mod = ln_mod->suivant; pos_y = 1;}
				else //on reste sur la même "ligne"
				{pos_y++;}
				
				if (y == LINES - 4) //on est sur la dernière ligne de l'écran
				{premiere_ligne++; rafraichir(); y += 1 - trouve_ligne(premiere_ligne - 1)->multiligne;} //ce calcul permettant de compenser le décalage causé par une ancienne 1ère ligne multiligne DOIT être fait après rafraichir()!
				else
				{y++;}
				
				if (x - 3 > longueur_str(ln_mod->txt) - (ln_mod->multiligne - 1) * (COLS - 5) && pos_y == ln_mod->multiligne)
				{mv(y, 4 + longueur_str(ln_mod->txt) - (ln_mod->multiligne - 1) * (COLS - 5));}
				else
				{mv(y, x);}
			}
			break;
		
		case KEY_UP: //flèche vers le haut
			if (ln_mod->num == 1 && pos_y == 1)
			{print_msg("Vous avez atteint le début du fichier.");}
			else
			{
				if (pos_y == 1) //on est à la ligne la plus haute de la "ligne"...
				{ln_mod = ln_mod->precedent; compter_lignes(ln_mod); pos_y = ln_mod->multiligne;}
				else //on monte d'une ligne dans la même "ligne"
				{pos_y--;}
				
				if (y == 1) //on est à la première ligne sur l'écran
				{premiere_ligne--; rafraichir(); y += ln_mod->multiligne - 1;} //Ce calcul permettant de placer le curseur à la dernière ligne de la "ligne" DOIT être fait après rafraichir()!
				else
				{y--;}
				
				if (x - 3 > longueur_str(ln_mod->txt) - (ln_mod->multiligne - 1) * (COLS - 5))
				{mv(y, 4 + longueur_str(ln_mod->txt) - (ln_mod->multiligne - 1) * (COLS - 5));}
				else
				{mv(y, x);}
			}
			break;
		
		case KEY_LEFT: //flèche vers la gauche
			if (x <= 4)
			{
				if (pos_y != 1)
				{y--; mv(y, COLS - 2); pos_y--;}
				else if (ln_mod->num == 1)
				{print_msg("Vous avez atteint le début du fichier.");}
				else if (y == 1)
				{
					ln_mod = ln_mod->precedent;
					premiere_ligne--;
					rafraichir();
					y += ln_mod->multiligne - 1; //Ce calcul permettant de placer le curseur à la dernière ligne de la "ligne" DOIT être fait après rafraichir()!
					mv(y, 4 + longueur_str(ln_mod->txt) - (ln_mod->multiligne - 1) * (COLS - 5));
					pos_y = ln_mod->multiligne;
				}
				else
				{
					ln_mod = ln_mod->precedent;
					y--;
					mv(y, 4 + longueur_str(ln_mod->txt) - (ln_mod->multiligne - 1) * (COLS - 5));
					pos_y = ln_mod->multiligne;
				}
			}
			else
			{mv(y, x - 1);}
			break;
		
		case KEY_RIGHT: //flèche vers la droite
			if (x == COLS - 2 && pos_y < ln_mod->multiligne) //multiligne
			{
				if (y == LINES - 4) //on est sur la dernière ligne de l'écran
				{
					premiere_ligne++;
					rafraichir();
					y += 1 - trouve_ligne(premiere_ligne - 1)->multiligne; //Ce calcul permettant de compenser le décalage causé par une ancienne 1ère ligne multiligne DOIT être fait après rafraichir()!
					mv(y, 4);
					pos_y++;
				}
				else
				{y++; mv(y, 4); pos_y++;}
			}
			else if (x - 3 > longueur_str(ln_mod->txt) - (ln_mod->multiligne - 1) * (COLS - 5) && pos_y == ln_mod->multiligne) //nouvelle ligne
			{
				if (ln_mod->suivant == &FIN_FICHIER && pos_y == ln_mod->multiligne)
				{print_msg("Vous avez atteint la fin du fichier.");}
				else if (y == LINES - 4) //on est sur la dernière ligne de l'écran
				{
					premiere_ligne++;
					rafraichir();
					y += 1 - trouve_ligne(premiere_ligne - 1)->multiligne; //Ce calcul permettant de compenser le décalage causé par une ancienne 1ère ligne multiligne DOIT être fait après rafraichir()!
					mv(y, 4);
					pos_y = 1;
					ln_mod = ln_mod->suivant;
				}
				else
				{y++; mv(y, 4); pos_y = 1; ln_mod = ln_mod->suivant;}
			}
			else
			{mv(y, x + 1);}
			break;
		
		case KEY_HOME: //Home = début de la ligne
			x = 4;
			y += 1 - pos_y;
			pos_y = 1;
			mv(y, x);
			break;
		
		case KEY_END: //End = fin de la ligne
			while (y + ln_mod->multiligne - pos_y > LINES - 4) //la fin de cette ligne est en dehors de l'écran
			{
				premiere_ligne++;
				pos_y++;
				y += 1 - trouve_ligne(premiere_ligne - 1)->multiligne; //compense l'éventuel décalage causé par une ancienne première ligne multiligne
				rafraichir();
			}
			x = longueur_str(ln_mod->txt) - (ln_mod->multiligne - 1) * (COLS - 5) + 4;
			y += ln_mod->multiligne - pos_y;
			pos_y = ln_mod->multiligne;
			mv(y, x);
			break;
		
		case KEY_SHOME: //Shift-Home = aller au -DÉBUT-
			cmd("DEBUT");
			break;
		
		case KEY_SEND: //Shift-End = aller à la -FIN-
			cmd("FIN");
			break;
		
		default:
			if (keyname(input)[0] == '^')
			//Ctrl-...
			{
				switch (keyname(input)[1])
				{
				case '[': //Esc (^[) = Menu des options
					menu_options();
					break;
				
				case 'Q': //Ctrl-Q = Quitter
					desinit();
					quitter(0);
					break;
				
				case 'R': //Ctrl-R = Rafraichir
					rafraichir();
					erreur(0, "..."); //Profitons-en pour afficher les erreurs qui traineraient dans le buffer...
					break;
				
				case 'A': //Ctrl-A = Aide
					cmd("aide");
					break;
				
				case 'D': //Ctrl-D = Débogage
					if (!debogage)
					{debogage = TRUE; print_msg("Mode débogage activé.");}
					else
					{debogage = FALSE; print_msg("Mode débogage désactivé.");}
					break;
				
				case 'P': //Ctrl-P = Commande
					cmd(NULL);
					break;
				
				case 'L': //Ctrl-L = Aller à une ligne
					cmd("aller a");
					break;
				
				case 'T': //Ctrl-T = Accès au terminal
					cmd("terminal");
					break;
				
				case 'E': //Ctrl-E = Enregistrer le fichier sous
					cmd("enregistrer");
					break;
				
				case 'S': //Ctrl-S = Sauvegarder le fichier
					cmd("sauvegarder");
					break;
				
				case 'G': //Ctrl-G = Compiler la liste
					endwin();
					printf("\n");
					system(cmd_compiler_liste);
					printf("Liste compilée.\nAppuyer sur \"Enter\" pour revenir à l'éditeur.\n");
					fgets(buffer, sizeof(buffer), stdin);
					rafraichir();
					break;
				
				case 'O': //Ctrl-O = Modifier les paramètres avancés (pas dans la barre de raccourcis)
					//Paramètres avancés... (aussi accessibles par le menu) (exemple: modifier la commande de compilation)
					break;
				}
			}
			else if (keyname(input)[0] == 'M' && keyname(input)[1] == '-')
			//Alt-...
			{
				switch (keyname(input)[2])
				{
				case '#': //Alt-# (en mode débogage) = changer de mode de débogage
					if (debogage)
					{
						if (element_debogue == 'i')
						{element_debogue = 'p'; print_msg("Mode de débogage: position");}
						else if (element_debogue == 'p')
						{element_debogue = 'n'; print_msg("Mode de débogage: input (raw)");}
						else
						{element_debogue = 'i'; print_msg("Mode de débogage: input");}
					}
					break;
				}
			}
			else if (keyname(input)[0] == 'K' && keyname(input)[1] == 'E' && keyname(input)[2] == 'Y' && keyname(input)[3] == '_' && keyname(input)[4] == 'F')
			//F1 à F12
			{
				switch (keyname(input)[6])
				{
				case '1': //F1 = Activation/Désactivation de la coloration syntaxique
					if (!coloration_syntaxique)
					{coloration_syntaxique = 1; rafraichir(); print_msg("Coloration syntaxique activée.");}
					else
					{coloration_syntaxique = 0; rafraichir(); print_msg("Coloration syntaxique désactivée.");}
					break;
				}
			}
		}
	}
}