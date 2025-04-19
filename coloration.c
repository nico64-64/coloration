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
			{/* À venir... */}
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
	while (ln != NULL & ln != &FIN_FICHIER)
	{fputs(ln->txt, fsauv); fputc('\n', fsauv); ln = ln->suivant;}
	
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
	mvprintw(LINES - 1, 25, "Options");
	
	if (COLS - longueur_str("Accès au terminal") > 34)
	mvprintw(LINES - 1, COLS - longueur_str("Accès au terminal"), "Accès au terminal");
	
	//Effaçage de la ligne de commande
	attrset(COLOR_PAIR(10));
	mvhline(LINES - 2, 0, ' ', COLS);
	
	//Écriture des raccourcis:
	mvprintw(LINES - 1, 1, "^C");
	mvprintw(LINES - 1, 13, "^A");
	mvprintw(LINES - 1, 22, "^O");
	
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
		{/* À venir... */}
		
		//Ctrl-O: Options
		else if (!strcmp(keyname(input), "^O"))
		{/* À venir... */}
	}
	commande[pos] = '\000';
	
	endwin(); //ferme temporairement l'interface ncurses, permettant de bien voir le terminal
	system(commande); //envoie la commande pour que la shell l'exécute
	printf("\nAppuyer sur \"Enter\" pour revenir à l'éditeur."); //petit rappel affiché dans la console, une fois la commande exécutée
	getchar(); //on attend un input de l'utilisateur pour lui laisser le temps de lire l'output de sa commande
			   //J'ai choisi getchar() parce que getch() ne fonctionne pas vraiment bien dans ce contexte, et puisqu'on ne prend qu'un seul caractère (même si on entre ensuite une nouvelle commande,
			   // les 2 getchar() seront entrecoupés par ncurses), les folies rageantes de getchar() ne devraient pas nous affecter...
	rafraichir(); //rétablissement de l'interface ncurses et affichage du programme
}


ligne* aller_a()
//Demande (grâce à une ligne de commande) un numéro de ligne à l'utilisateur et l'y emmène.
//Renvoie un pointeur vers la ligne en question ou NULL si ce n'est pas un numéro de ligne valide.
//Cette fonction ne devrait pas être appelée par aucune autre que cmd() (mais on peut le faire indirectement via cmd("aller a")).
{
	char nl[8] = "";
	int pos = 0;
	int input = EOF;
	int num;
	
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
		{/* À venir... */}
	}
	nl[pos] = '\000';
	sscanf(nl, "%d", &num);
	
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


void cmd(char commande[])
//Exécute une commande reçue en paramètre.
//Si ce paramètre est NULL, une commande sera d'abord demandée à l'utilisateur.
//L'écran sera ensuite redessiné et un message sera peut-être affiché dans la barre d'état.
{
	int input = EOF; //input de l'utilisateur (utilisé si commande est NULL)
	int pos = 0; //position dans cmd (nbre de caractères lus) (utilisé si commande est NULL)
	char cmd[75] = ""; //commande à exécuter (soit "commande" ou la commande reçue de l'utilisateur si c'est NULL)
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
			{cmd[pos] = input; pos++; addch(input);}
			
			//KEY_BACKSPACE: Effacer
			else if (input == KEY_BACKSPACE && pos > 0)
			{pos--; mvaddch(LINES - 2, 11 + pos, ' '); move(LINES - 2, 11 + pos);}
			
			//Ctrl-C: Annuler
			else if (!strcmp(keyname(input), "^C"))
			{rafraichir(); return;}
			
			//Ctrl-A: Aide
			else if (!strcmp(keyname(input), "^A"))
			{/* À venir... */}
		}
		cmd[pos] = '\000';
	}
	
	else //commande != NULL
	{strcpy(cmd, commande);}
	
	//########################################################
	
	//Application de la commande:
	//Aide:
	if (!strcmp(cmd, "aide"))
	{/* À venir... */ rafraichir();}
	
	//Rafraichir:
	else if (!strcmp(cmd, "rafraichir"))
	{rafraichir(); erreur(0, "..."); return;}
	
	//Quitter:
	else if (!strcmp(cmd, "quitter"))
	{desinit(); quitter(0);}
	
	//Aller à une certaine ligne dans le fichier:
	else if (!strcmp(cmd, "aller a") || !strcmp(cmd, "aller à"))
	{
		buffln = aller_a();
		if (buffln != NULL)
		{ln_mod = buffln; pos_y = 1; mv(1, 4);}
	}
	
	//Aller au -DÉBUT-:
	else if (!strcmp(cmd, "DEBUT") || !strcmp(cmd, "DÉBUT"))
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
	else if (!strcmp(cmd, "FIN"))
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
	else if (!strcmp(cmd, "terminal") || !strcmp(cmd, "term"))
	{term();}
	
	//Enregistrer ou Sauvegarder:
	else if (!strcmp(cmd, "enregistrer") || !strcmp(cmd, "enr"))
	{enregistrer(NULL);}
	else if (!strcmp(cmd, "sauvegarder") || !strcmp(cmd, "sauv"))
	{enregistrer(nom_fichier);}
	
	//Aucune commande:
	else if (cmd[0] == '\000')
	{rafraichir();}
	
	//Commande non-reconnue:
	else
	{rafraichir(); print_msg("Ceci n'est pas une commande valide.");}
}


int main(int argc, char* argv[])
{
	int input = EOF; //caractère reçu en input (doit être déclaré int pour accepter les accents, Ctrl-car., etc.)
	int buffint = 0;
	
	
	//Erreurs d'invocation:
	if (argc < 2)
	{printf("Veuillez spécifier un fichier contenant une liste d'objets à afficher.\nExemple: \"%s liste_objets.txt\"\n\n", argv[0]); exit(0);}
	
	strcpy(nom_fichier, argv[1]);
	fichier = fopen(nom_fichier, "r");
	if (fichier == NULL)
	{printf("Impossible d'ouvrir le fichier \"%s\".\nExiste-t-il? Assurez-vous d'entrer le chemin complet ou le chemin relatif à ce dossier.\n\n", nom_fichier); exit(1);}
	
	
	//Initialisation:
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
			buffint = derniere_ligne;
			rafraichir();
			if (y >= LINES - 3)
			{
				for (int compteur = 0; compteur <= LINES - 3 - y; compteur++)
				{
					if (buffint == derniere_ligne) //le nombre de lignes n'a pas changé
					{pos_y -= 1;}
					else
					{ln_mod = ln_mod->precedent; pos_y = ln_mod->multiligne;}
				}
				mv(LINES - 4, x);
			}
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
				mv(y, 4);
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
				{premiere_ligne--; rafraichir(); y += ln_mod->multiligne - 1;} //ce calcul permettant de placer le curseur à la dernière ligne de la "ligne" DOIT être fait après rafraichir()!
				else
				{y--;}
				mv(y, 4);
			}
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
				}
			}
			else if (keyname(input)[0] == 'M' && keyname(input)[1] == '-')
			{
				switch (keyname(input)[2])
				{
				case '#': //Alt-# [en mode débogage] = change de mode de débogage
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
		}
	}
}