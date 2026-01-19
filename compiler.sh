#! /bin/bash

#Support des accents:
if [ $1 == --sans-accents ]
then
	sed -i 's|#include "accents.h"|//#include "accents.h"|g' coloration.h
elif [ $1 == --avec-accents ]
then
	sed -i 's|//#include "accents.h"|#include "accents.h"|g' coloration.h
else
	echo -e "Cette option n'est pas supportée.\n"
	echo "Les options supportées sont:"
	echo -e "--sans-accents\tCompile le programme sans le support avancé des accents"
	echo -e "--avec-accents\tCompile le programme en rétablissant ce support\n"
	echo -e "Le support avancé des accents est activé par défaut."
	echo -e "Si vous utilisez une des 2 options ci-dessus, elle reste en vigueur"
	echo -e "  jusqu'à ce qu'elle soit annulée par l'option contraire.\n"
	exit
fi

#Compilation:
gcc coloration.c -o coloration -Wall -Wextra -lncursesw -ggdb -fsanitize=address
#-Wall et -Wextra = activent des warnings supplémentaires de gcc
#-lncursesw = link ncurses wide_characters (ncurses avec accents)
#-ggdb = dgb debugging symbols
#-fsanitize=address = débogueur de memory leaks et de Segmentation Faults

#Exécuter comme suit:
#./coloration fichier.txt