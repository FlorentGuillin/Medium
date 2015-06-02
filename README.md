Medium - file indexer

Langage: C++ / Qt

Base du projet: Un explorateur de fichiers (Indexation / Création de dossiers virtuels)

Installation: Compilation depuis QtCreator fonctionnelle, les librairies exiv2 et taglib ne devraient pas poser de problèmes. Podofo a été compilé via gcc en version 4.9.2

Fonctionnalités:

-Sélection d'un dossier comme répertoire de recherche (ou d'indexation)

-Le filtrage se fait à partir du dossier courant et de manière récursive: 
  - les fichiers textes (MimeType: text/*) sont analysés (contenu)
  - les images (MimeType: image/*) sont analysés (metadonnées - lib: Exiv2)
  - les fichiers audios (MimeType: audio/*) sont analysés (métadonnées - lib: taglib) 
  - les PDF (MimeType: application/pdf) sont analysés (métadonnées + contenu - lib: PoDoFo)

Il n'est pas garanti qu'une des librairies n'échoue pas à lire un fichier dans certains cas.
Les résultats sont affichés sous forme de marques-pages. On peut les supprimer, et on peut y ajouter des fichiers ou en supprimer (au niveau de la fenêtre de visualisation des fichiers, partie basse de l'interface).

Il est possible d'effectuer une recherche multicritère (case insensitive) à l'aide des opérateurs -AND- et -OR- (logique booléenne).
Une expression régulière est construite en fonction.
Exemples: 
- stras -OR- bourg => (stras|bourg)
- stras -AND- bourg => (?=.*stras)(?=.*bourg)
- bonjour -OR- (medium -AND- strasbourg) => (bonjour|(?=.*medium)(?=.*strasbourg))
- bonjour -OR- medium -AND- strasbourg => (?=.*(bonjour|medium))(?=.*strasbourg)

Il est possible de lancer une indexation textuelle (voir FileIndexer.cpp) depuis un répertoire courant (résultat visible dans medium.db (créé via SQLite3) cependant, la recherche n'a pas été relié à l'indexation (nécessite encore des améliorations). 

Gestion des tickets via Trello: https://trello.com/b/8Kfkl8Xw/medium-file-indexer (non maintenu)
