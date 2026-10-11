# V.G — Gestion de l’horloge RTC

**Emplacement dans le rapport :** Partie V, au début de la partie concernée, avant l’explication du fonctionnement.

**Ce que représente l’algorigramme :** Module `donnees` du programme final : initialisation du RTC au démarrage, réglage par le port série, lecture périodique avec correction de la dérive.

```mermaid
flowchart TD
    subgraph DEMARRAGE["Au démarrage"]
        direction TB
        SDEB(["Initialisation des données"])
        SRTC["Démarrer la liaison avec le RTC et lever la protection en écriture"]
        SVAL{"Horloge arrêtée ou date invalide ?"}
        SREG["Régler à l'heure de compilation + 28 s, démarrer l'horloge, désactiver la recharge de la pile"]
        SREF["Mémoriser la référence de correction dans la RAM du RTC"]
        SFIN(["Fin de l'initialisation"])
        SDEB --> SRTC --> SVAL
        SVAL -->|"Oui"| SREG --> SREF --> SFIN
        SVAL -->|"Non"| SFIN
    end
    subgraph SERIE["À chaque tour : réglage par le port série"]
        direction TB
        RDEB(["Tour de la boucle principale"])
        RLIGNE{"Ligne complète reçue ?"}
        RFORM{"14 chiffres et valeurs plausibles ?"}
        RREG["Régler l'horloge et réinitialiser la référence de correction"]
        RERR[/"Afficher le format attendu : AAAA-MM-JJ HH:MM:SS"/]
        RFIN(["Suite de la boucle"])
        RDEB --> RLIGNE
        RLIGNE -->|"Non"| RFIN
        RLIGNE -->|"Oui"| RFORM
        RFORM -->|"Oui"| RREG --> RFIN
        RFORM -->|"Non"| RERR --> RFIN
    end
    subgraph LECTURE["Toutes les 200 ms : lecture de l'heure"]
        direction TB
        LDEB(["Demande de l'heure"])
        LVAL{"Horloge en marche et date valide ?"}
        LTIRET[/"Fournir un texte de tirets"/]
        LLIRE[/"Lire la date et l'heure du RTC"/]
        LREF{"Référence présente et antérieure à l'heure lue ?"}
        LNEW["Écrire l'heure lue comme nouvelle référence"]
        LCALC["Retard = temps écoulé depuis la référence, divisé par 4623 s (division entière)"]
        LRET{"Retard supérieur à 0 ?"}
        LCORR["Retirer le retard en secondes, réécrire l'heure dans le RTC et avancer la référence"]
        LSORT[/"Transmettre l'heure et la date à l'affichage et à l'horodatage"/]
        LDEB --> LVAL
        LVAL -->|"Non"| LTIRET
        LVAL -->|"Oui"| LLIRE --> LREF
        LREF -->|"Non"| LNEW --> LSORT
        LREF -->|"Oui"| LCALC --> LRET
        LRET -->|"Oui"| LCORR --> LSORT
        LRET -->|"Non"| LSORT
    end
    SFIN -.-> RDEB
    RFIN -.-> LDEB
```

**Légende proposée :** Algorigramme de la gestion du RTC : initialisation, réglage par le port série et lecture avec correction de la dérive.

**Note :** La valeur de 4623 s vient de la dérive mesurée (+17 s en 21 h 50). La lecture corrigée sert aussi à horodater les enregistrements. Les flèches entre sous-graphes indiquent l’ordre général, pas un enchaînement à chaque appel.
