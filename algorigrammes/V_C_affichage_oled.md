# V.C — Affichage du BPM et du graphique

**Emplacement dans le rapport :** Partie V, au début de la partie concernée, avant l’explication du fonctionnement.

**Ce que représente l’algorigramme :** Partie de la boucle principale et du module `gui` qui met à jour le second écran, alimente le tracé en permanence et dessine le graphique lorsque sa page est ouverte.

```mermaid
flowchart TD
    DEBUT(["Tour de la boucle principale"])
    T200{"200 ms écoulées depuis la dernière lecture de l'heure ?"}
    HEURE["Lire l'heure corrigée et la mettre en forme"]
    HCHG{"Texte de l'heure différent de celui affiché ?"}
    MARQ1["Mémoriser l'heure et marquer l'écran 2 à redessiner"]
    BCHG{"BPM entier différent de celui affiché ?"}
    MARQ2["Mettre à jour les LED, mémoriser le BPM et marquer l'écran 2 à redessiner"]
    AREDESS{"Écran 2 à redessiner ?"}
    ECRAN2[/"Écran 2 : afficher l'heure, la date et le BPM, ou des tirets si le BPM vaut 0"/]
    FILE{"Un échantillon du signal attend dans la file ?"}
    SOMME["Ajouter l'échantillon à la somme de la colonne en cours"]
    PLEIN{"Nombre d'échantillons par colonne atteint (2, 3, 4, 6 ou 8 selon le zoom) ?"}
    COLONNE["Écrire la moyenne dans la colonne, avancer le curseur, revenir à gauche en fin d'écran"]
    PAGE{"Page graphique ouverte ?"}
    PAS[/"Lire les pas de l'encodeur"/]
    QUIT{"Clic de l'encodeur ou bouton retour ?"}
    MENU["Revenir au menu"]
    ZOOM{"Pas non nul et niveau de zoom modifiable ?"}
    CHZOOM["Changer le niveau de zoom et effacer le tracé"]
    T50{"50 ms écoulées depuis le dernier dessin ?"}
    DESSIN[/"Écran 1 : dessiner les axes, les graduations, la courbe, le BPM et la largeur de la fenêtre"/]
    SUITE(["Suite de la boucle principale"])
    DEBUT --> T200
    T200 -->|"Oui"| HEURE --> HCHG
    T200 -->|"Non"| BCHG
    HCHG -->|"Oui"| MARQ1 --> BCHG
    HCHG -->|"Non"| BCHG
    BCHG -->|"Oui"| MARQ2 --> AREDESS
    BCHG -->|"Non"| AREDESS
    AREDESS -->|"Oui"| ECRAN2 --> FILE
    AREDESS -->|"Non"| FILE
    FILE -->|"Oui"| SOMME --> PLEIN
    PLEIN -->|"Oui"| COLONNE --> FILE
    PLEIN -->|"Non"| FILE
    FILE -->|"Non : file vide"| PAGE
    PAGE -->|"Non"| SUITE
    PAGE -->|"Oui"| PAS --> QUIT
    QUIT -->|"Oui"| MENU --> SUITE
    QUIT -->|"Non"| ZOOM
    ZOOM -->|"Oui"| CHZOOM --> DESSIN
    ZOOM -->|"Non"| T50
    T50 -->|"Oui"| DESSIN
    T50 -->|"Non"| SUITE
    DESSIN --> SUITE
```

**Légende proposée :** Algorigramme de l’affichage : mise à jour du second écran, alimentation continue du tracé et dessin du graphique.

**Note :** Le tracé est alimenté à chaque tour, que la page graphique soit ouverte ou non ; il n’est dessiné que si elle est ouverte. Les cinq niveaux de zoom correspondent à des fenêtres d’environ 1,1 s à 4,4 s.
