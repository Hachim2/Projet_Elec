# V.B — Acquisition et traitement du signal PPG

**Emplacement dans le rapport :** Partie V, au début de la partie concernée, avant l’explication du fonctionnement.

**Ce que représente l’algorigramme :** Module `bpm` du programme final : détection des battements dans l’interruption du Timer1, puis calcul du BPM dans la boucle principale.

```mermaid
flowchart TD
    subgraph ISR["Interruption Timer1 : exécutée toutes les 5 ms"]
        direction TB
        IDEB(["Déclenchement du Timer1"])
        ILIRE[/"Lire la sortie du capteur sur A0"/]
        IMOY["Avancer l'horloge du capteur de 5 ms et mettre à jour la moyenne glissante de 100 échantillons"]
        ICENTRE["Signal recentré = mesure moins moyenne, puis dépôt dans la file du tracé"]
        ISTAB{"Moins de 1 s depuis le démarrage ?"}
        IBAS{"Signal inférieur ou égal au seuil bas de 50 ?"}
        IARME["Armer la détection"]
        IHAUT{"Détection armée et signal au moins égal au seuil haut de 200 ?"}
        IDESARME["Désarmer la détection et calculer l'écart depuis le dernier battement"]
        IPREM{"Un battement précédent existe ?"}
        IMIN{"Écart inférieur à 330 ms ?"}
        IPUB["Publier l'intervalle pour la boucle principale"]
        IMEM["Mémoriser l'instant du battement et signaler le battement pour le bip"]
        IFIN(["Retour d'interruption"])
        IDEB --> ILIRE --> IMOY --> ICENTRE --> ISTAB
        ISTAB -->|"Oui"| IFIN
        ISTAB -->|"Non"| IBAS
        IBAS -->|"Oui"| IARME --> IFIN
        IBAS -->|"Non"| IHAUT
        IHAUT -->|"Non"| IFIN
        IHAUT -->|"Oui"| IDESARME --> IPREM
        IPREM -->|"Non"| IMEM
        IPREM -->|"Oui"| IMIN
        IMIN -->|"Oui : battement ignoré"| IFIN
        IMIN -->|"Non"| IPUB --> IMEM
        IMEM --> IFIN
    end
    subgraph BOUCLE["Boucle principale : mise à jour du BPM à chaque tour"]
        direction TB
        BDEB(["Appel de la mise à jour du BPM"])
        BCOPIE["Copier, interruptions masquées : nouvel intervalle éventuel et durée depuis le dernier battement"]
        BSIL{"Moyenne non vide et aucun battement depuis plus de 2,5 s ?"}
        BRAZ1["Vider la moyenne : BPM = 0"]
        BNOUV{"Nouvel intervalle disponible ?"}
        BMAX{"Intervalle supérieur à 2000 ms ?"}
        BRAZ2["Vider la moyenne : BPM = 0"]
        BTROIS{"Au moins 3 intervalles en mémoire ?"}
        BECART{"Écart à la moyenne supérieur à 30 % ?"}
        BREJ{"Troisième rejet consécutif ?"}
        BRAZ3["Vider la moyenne : le rythme a changé"]
        BAJOUT["Remettre le compteur de rejets à 0 et ajouter l'intervalle à la moyenne glissante sur 10"]
        BCALC["BPM = 60 000 divisé par l'intervalle moyen en ms"]
        BSORT[/"Renvoyer le BPM courant"/]
        BDEB --> BCOPIE --> BSIL
        BSIL -->|"Oui"| BRAZ1 --> BNOUV
        BSIL -->|"Non"| BNOUV
        BNOUV -->|"Non"| BSORT
        BNOUV -->|"Oui"| BMAX
        BMAX -->|"Oui"| BRAZ2 --> BSORT
        BMAX -->|"Non"| BTROIS
        BTROIS -->|"Non"| BAJOUT
        BTROIS -->|"Oui"| BECART
        BECART -->|"Non"| BAJOUT
        BECART -->|"Oui"| BREJ
        BREJ -->|"Non : intervalle rejeté"| BSORT
        BREJ -->|"Oui"| BRAZ3 --> BAJOUT
        BAJOUT --> BCALC --> BSORT
    end
    IPUB -.->|"Intervalle partagé"| BCOPIE
```

**Légende proposée :** Algorigramme de l’acquisition et du traitement du signal PPG : détection dans l’interruption, calcul du BPM dans la boucle principale.

**Note :** Les deux traitements ne s’enchaînent pas : l’interruption s’exécute toutes les 5 ms, la mise à jour du BPM à chaque tour de boucle ; la flèche en pointillés indique seulement la donnée partagée. Le BPM est obtenu à partir de la moyenne des intervalles, convertie en BPM, et non d’une moyenne de valeurs de BPM. Un BPM égal à 0 signifie « pas de mesure ».
