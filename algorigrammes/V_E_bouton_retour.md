# V.E — Gestion du bouton retour

**Emplacement dans le rapport :** Partie V, au début de la partie concernée, avant l’explication du fonctionnement.

**Ce que représente l’algorigramme :** Lecture du bouton retour dans le module `peripheriques` (anti-rebond de 30 ms, événement au relâchement) et action selon la page active.

```mermaid
flowchart TD
    DEBUT(["Tour de la boucle principale"])
    LIRE[/"Lire le bouton retour sur D9"/]
    CHANGE{"Lecture différente de la précédente ?"}
    CHRONO["Mémoriser la lecture et relancer le délai anti-rebond"]
    STABLE{"État différent de l'état validé et stable depuis 30 ms ?"}
    VALIDE["Valider le nouvel état"]
    RELACHE{"Bouton relâché ?"}
    AUCUN["Aucun événement"]
    EVT["Événement « retour »"]
    PAGE{"Page active ?"}
    RIEN["Menu : aucun effet"]
    OMENU["Graphique, données, message ou réglages : ouvrir le menu"]
    OLISTE["Confirmation : revenir à la liste sans effacer"]
    SUITE(["Tour suivant"])
    DEBUT --> LIRE --> CHANGE
    CHANGE -->|"Oui"| CHRONO --> STABLE
    CHANGE -->|"Non"| STABLE
    STABLE -->|"Non"| AUCUN
    STABLE -->|"Oui"| VALIDE --> RELACHE
    RELACHE -->|"Non : bouton enfoncé"| AUCUN
    RELACHE -->|"Oui"| EVT --> PAGE
    PAGE -->|"Menu"| RIEN --> SUITE
    PAGE -->|"Autres pages"| OMENU --> SUITE
    PAGE -->|"Confirmation"| OLISTE --> SUITE
    AUCUN --> SUITE
    SUITE -->|"Boucle"| DEBUT
```

**Légende proposée :** Algorigramme de la gestion du bouton retour : anti-rebond, déclenchement au relâchement et action selon la page.

**Note :** Le bouton est lu à chaque tour de boucle pour que l’anti-rebond suive son état. L’événement est produit une seule fois, au relâchement. Le code attend un bouton entre D9 et la masse, avec la résistance de rappel interne.
