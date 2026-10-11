# IV.G — Test unitaire de l’horloge RTC DS1302

**Emplacement dans le rapport :** Partie IV, module concerné, « 2. Protocole de test unitaire et critères de réussite ».

**Ce que représente l’algorigramme :** Programme de test `rtc_test.cpp` et manipulations de l’expérimentateur pour vérifier que l’heure continue d’avancer pendant une coupure, pile en place.

```mermaid
flowchart TD
    subgraph PROG["Programme de test"]
        direction TB
        PDEB(["Début"])
        PINIT["Initialiser : liaison série, RTC, protection en écriture levée, recharge de la pile désactivée"]
        PCMD{"Commande r reçue sur le port série ?"}
        PREGLE["Régler le RTC à l'heure de compilation et démarrer l'horloge"]
        PMARCHE{"Horloge en marche ?"}
        PARRET[/"Afficher « Horloge arrêtée »"/]
        PVALIDE{"Date et heure valides ?"}
        PINVAL[/"Afficher « Date invalide »"/]
        PLIRE[/"Lire la date et l'heure du RTC"/]
        PAFF[/"Afficher la date et l'heure sur le moniteur série"/]
        PATT["Attendre 1 s"]
        PDEB --> PINIT --> PCMD
        PCMD -->|"Oui"| PREGLE --> PMARCHE
        PCMD -->|"Non"| PMARCHE
        PMARCHE -->|"Non"| PARRET --> PATT
        PMARCHE -->|"Oui"| PVALIDE
        PVALIDE -->|"Non"| PINVAL --> PATT
        PVALIDE -->|"Oui"| PLIRE --> PAFF --> PATT
        PATT --> PCMD
    end
    subgraph MANIP["Manipulations de l'expérimentateur"]
        direction TB
        EDEB(["Début de l'essai"])
        E1["Envoyer r une seule fois pour régler l'horloge"]
        E2[/"Relever l'heure affichée"/]
        E3["Débrancher l'alimentation environ 30 s, pile en place"]
        E4["Rebrancher sans envoyer r"]
        E5[/"Relever de nouveau l'heure affichée"/]
        E6{"L'heure a continué d'avancer ?"}
        E7["Consigner : heure conservée"]
        E8["Consigner l'écart observé"]
        EFIN(["Fin de l'essai"])
        EDEB --> E1 --> E2 --> E3 --> E4 --> E5 --> E6
        E6 -->|"Oui"| E7 --> EFIN
        E6 -->|"Non"| E8 --> EFIN
    end
```

**Légende proposée :** Algorigramme du test unitaire du RTC DS1302 : programme de lecture et manipulations de coupure d’alimentation.

**Note :** Écart rapport/code : le rapport indique que l’horloge est réglée à l’heure de compilation. Dans le programme de test, ce réglage n’a lieu que sur réception de la commande r, ce qui évite de réécrire l’heure après la coupure.
