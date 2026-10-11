# IV.E — Test du bouton poussoir avec les LED

**Emplacement dans le rapport :** Partie IV, module concerné, « 2. Protocole de test unitaire et critères de réussite ».

**Ce que représente l’algorigramme :** Programme de test `btn_leds_test.cpp` : chaque appui validé fait avancer la séquence rouge, verte, jaune, toutes éteintes.

```mermaid
flowchart TD
    DEBUT(["Début"])
    INIT["Initialiser : trois sorties LED, entrée bouton, liaison série"]
    ETAT0["État 0 : toutes les LED éteintes, compteur à 0"]
    PRET[/"Afficher « Test prêt » et « Compteur : 0 »"/]
    LIRE[/"Lire le bouton"/]
    CHANGE{"Lecture différente de la précédente ?"}
    CHRONO["Relancer le délai anti-rebond"]
    STABLE{"Stable depuis 30 ms et différent de l'état validé ?"}
    VALIDE["Valider le nouvel état du bouton"]
    RELACHE{"Bouton relâché ?"}
    AVANCE["Compteur + 1 et état suivant : 0 vers 1, 1 vers 2, 2 vers 3, 3 vers 0"]
    ETEINT["Éteindre les trois LED"]
    ALLUME["Allumer la LED de l'état : 1 rouge, 2 verte, 3 jaune, 0 aucune"]
    AFF[/"Afficher le compteur et la LED allumée"/]
    DEBUT --> INIT --> ETAT0 --> PRET --> LIRE --> CHANGE
    CHANGE -->|"Oui"| CHRONO --> STABLE
    CHANGE -->|"Non"| STABLE
    STABLE -->|"Non"| LIRE
    STABLE -->|"Oui"| VALIDE --> RELACHE
    RELACHE -->|"Non"| LIRE
    RELACHE -->|"Oui"| AVANCE --> ETEINT --> ALLUME --> AFF --> LIRE
```

**Légende proposée :** Algorigramme du test du bouton poussoir et des trois LED : un changement d’état par appui, pris en compte au relâchement.

**Note :** Le bouton du test est câblé vers +5 V avec une résistance de rappel à la masse : l’état « relâché » correspond au niveau bas. Écart avec le montage final : dans ce test les LED rouge, verte et jaune sont sur D6, D5 et D4 et le bouton sur D2 ; dans le programme final elles sont sur D8, D7 et D6, et le bouton de retour sur D9 avec rappel interne.
