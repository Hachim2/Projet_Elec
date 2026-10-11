# IV.D — Test unitaire de l’encodeur et de son bouton

**Emplacement dans le rapport :** Partie IV, module concerné, « 2. Protocole de test unitaire et critères de réussite ».

**Ce que représente l’algorigramme :** Programme de test `encodeur_test.cpp` : détection du sens de rotation et de l’appui, avec affichage sur le moniteur série.

```mermaid
flowchart TD
    DEBUT(["Début"])
    INIT["Initialiser : liaison série, entrées A, B et bouton avec rappel interne, états de départ mémorisés"]
    LIREA[/"Lire la sortie A"/]
    FRONT{"A vient de passer à l'état haut ?"}
    SENS{"B différent de A ?"}
    DROITE[/"Afficher « Droite »"/]
    GAUCHE[/"Afficher « Gauche »"/]
    MEMA["Mémoriser l'état de A"]
    LIREB[/"Lire le bouton"/]
    CHANGE{"Lecture différente de la précédente ?"}
    CHRONO["Relancer le délai anti-rebond"]
    STABLE{"Stable depuis 30 ms et différent de l'état validé ?"}
    VALIDE["Valider le nouvel état du bouton"]
    APPUI{"État validé = bouton enfoncé ?"}
    AFFAPPUI[/"Afficher « Appui »"/]
    DEBUT --> INIT --> LIREA --> FRONT
    FRONT -->|"Oui"| SENS
    FRONT -->|"Non"| MEMA
    SENS -->|"Oui"| DROITE --> MEMA
    SENS -->|"Non"| GAUCHE --> MEMA
    MEMA --> LIREB --> CHANGE
    CHANGE -->|"Oui"| CHRONO --> STABLE
    CHANGE -->|"Non"| STABLE
    STABLE -->|"Oui"| VALIDE --> APPUI
    STABLE -->|"Non"| LIREA
    APPUI -->|"Oui"| AFFAPPUI --> LIREA
    APPUI -->|"Non"| LIREA
```

**Légende proposée :** Algorigramme du test unitaire de l’encodeur rotatif : sens de rotation et appui avec anti-rebond.

**Note :** Écart rapport/code : dans ce programme de test, A est sur D2, B sur D3 et le bouton sur D4. Dans le programme final, le bouton est sur D2, A sur D3 et B sur D4. De plus, ce test signale l’appui à l’enfoncement, alors que le programme final agit au relâchement.
