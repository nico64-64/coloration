# Coloration

Un éditeur de texte TUI inspiré de nano.

## Installation

Utilisez le script compiler.sh pour compiler le programme avec les bons arguments. Vous pouvez aussi vous en servir pour activer/désactiver facilement le support avancé des accents.

### Note importante

*Puisque coloration utilise ncurses pour s'afficher dans le terminal, ce programme ne fonctionne pas sur Windows.*

coloration est testé sur Linux x86_64, mais devrait aussi fonctionner sur n'importe quel système basé sur Unix (Linux, BSD, macOS, etc.), en autant que ce système fournissent un librairie ncurses wide-characters utilisable.

## Usage

Pour ouvrir un fichier avec coloration, tapez simplement `./coloration FICHIER` ou `./coloration /CHEMIN/VERS/FICHIER` dans votre terminal.

Pour en apprendre plus sur le fonctionnement de coloration, utilisez le programme pour ouvrir un fichier, puis entrez Ctrl-A pour afficher le manuel du programme. Chaque "commande" de l'éditeur y est expliquée en détails.

### Limitations

Coloration devrait fonctionner sur à peu près n'importe quel terminal (tout ceux supportés par ncurses), mais certaines fonctionnalités pourraient moins bien fonctionner, notamment les suivantes:

- Le support avancé des accents ne fonctionnera pas si votre librairie ncurses ne supporte pas les wide-characters (rare)
- Le support avancé des accents pourrait mal fonctionner si votre locale n'est pas UTF-8 ou si vous avez un setup linguistique bizarre
- La couleur vert pâle s'affiche en gris peu visible ou en blanc sur les terminaux qui ne supportent que 16 couleurs (consoles Linux)
- Avec certains terminaux, il faut appuyer 2 fois sur la touche Escape pour qu'elle soit détectée (un délai est aussi possible, bien qu'improbable)
- Les combinaisons Shift-Home, Ctrl-Home, Alt-Home et leurs équivalents avec End ne fonctionnent pas sur les consoles Linux (ils sont perçu comme étant Home ou End sans modificateur) *
- Sur certaines distributions Linux (notamment Arch Linux), les consoles Linux ne détectent pas certaines combinaisons de touches comme Ctrl-P et Ctrl-X *

\* Ces 2 derniers problèmes peuvent être contournés en utilisant la ligne de commande et en utilisant F4 au lieu de Ctrl-P.

### Note

Cet éditeur de texte a à la base été créé dans le but d'éditer des fichiers que j'ai appelés "liste d'objets". Ces fichiers sont des scénarios pour un jeu que j'avais commencé à développer lorsque j'ai commencé ce projet. Un exemple de liste d'objets est fourni avec ce programme (liste.txt).

La coloration syntaxique ne s'applique qu'à ce type de fichier.

## Progression

Coloration est encore loin d'être terminé! Voici (en date de la version 0.7.2) la liste de ce qui fonctionne et de ce qui est encore à venir:

#### Ce qui fonctionne:
- Ouverture, navigation et visualisation d'un fichier texte
- Modification du fichier (écriture et effaçage)
- Support complet des accents français (avec possibilité de "support" médiocre très portable)
- Enregistrement du fichier ouvert/modifié
- Ouverture d'un autre fichier depuis l'éditeur
- Coloration syntaxique dynamique (selon les modifications)
- Accès au terminal
- Ligne de commande interne
- Menu des options (accessible avec Escape)
- Manuel d'aide complet (et intégration aux différentes commandes)
- Paramètres avancés (plus les réglages du support avancé des accents)
- Mode débogage
- Support de la souris (dans le fichier et dans l'interface)

#### À venir:
- Sélection du texte
- Copier/Coller/Couper
- Recherche (et remplacement) d'une expression dans le fichier ouvert
- Compteur de caractères
- Création d'un fichier lorsque démarré sans argument (au lieu du message d'usage/erreur actuel)