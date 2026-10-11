# V.D — Menu et navigation par encodeur

**Emplacement dans le rapport :** Partie V, au début de la partie concernée, avant l’explication du fonctionnement.

**Ce que représente l’algorigramme :** Machine à états des pages dans `main.cpp` : effet d’une rotation, d’un clic de l’encodeur et du bouton retour selon la page active.

```mermaid
flowchart TD
    DEBUT(["Tour de la boucle principale"])
    ENTREES[/"Récupérer les pas comptés par l'interruption de l'encodeur, le clic (au relâchement) et le bouton retour"/]
    PAGE{"Page active ?"}
    M1["Menu : déplacer la sélection parmi 4 entrées et redessiner si elle change"]
    M2{"Clic sur une entrée ?"}
    M3{"Entrée validée ?"}
    OGRAPH["Ouvrir la page graphique"]
    ODON["Ouvrir la liste « Mes données »"]
    OENR["Tenter l'enregistrement puis ouvrir la page message"]
    OREG["Ouvrir la page réglages"]
    G1{"Graphique : clic ou retour ?"}
    G2["Appliquer le zoom et redessiner la courbe"]
    D1{"Données : retour ?"}
    D2{"Clic sur la dernière ligne « Effacer tout » ?"}
    D3["Ouvrir la confirmation d'effacement"]
    D4["Déplacer la sélection dans la liste"]
    C1{"Confirmation : clic ?"}
    C2["Effacer toutes les mesures"]
    C3{"Retour ?"}
    S1{"Message : clic, retour ou 2,5 s écoulées ?"}
    R1{"Réglages : retour ?"}
    R2{"Clic ?"}
    R3["Passer à la valeur suivante (LED, contraste ou langue) et l'enregistrer en EEPROM"]
    R4["Déplacer la sélection parmi 3 réglages"]
    OMENU["Ouvrir le menu"]
    SUITE(["Tour suivant"])
    DEBUT --> ENTREES --> PAGE
    PAGE -->|"Menu"| M1 --> M2
    M2 -->|"Non"| SUITE
    M2 -->|"Oui"| M3
    M3 -->|"Graphique"| OGRAPH --> SUITE
    M3 -->|"Mes données"| ODON --> SUITE
    M3 -->|"Enregistrer"| OENR --> SUITE
    M3 -->|"Réglages"| OREG --> SUITE
    PAGE -->|"Graphique"| G1
    G1 -->|"Oui"| OMENU
    G1 -->|"Non"| G2 --> SUITE
    PAGE -->|"Données"| D1
    D1 -->|"Oui"| OMENU
    D1 -->|"Non"| D2
    D2 -->|"Oui"| D3 --> SUITE
    D2 -->|"Non"| D4 --> SUITE
    PAGE -->|"Confirmation"| C1
    C1 -->|"Oui"| C2 --> ODON
    C1 -->|"Non"| C3
    C3 -->|"Oui"| ODON
    C3 -->|"Non"| SUITE
    PAGE -->|"Message"| S1
    S1 -->|"Oui"| OMENU
    S1 -->|"Non"| SUITE
    PAGE -->|"Réglages"| R1
    R1 -->|"Oui"| OMENU
    R1 -->|"Non"| R2
    R2 -->|"Oui"| R3 --> SUITE
    R2 -->|"Non"| R4 --> SUITE
    OMENU --> SUITE
    SUITE -->|"Boucle"| DEBUT
```

**Légende proposée :** Algorigramme de la navigation : actions de l’encodeur et du bouton retour selon la page affichée.

**Note :** Le programme final comporte six pages : menu, graphique, données, confirmation d’effacement, message et réglages. La page réglages et le choix de la langue existent dans le code. Sur la page menu, le bouton retour n’a aucun effet.
