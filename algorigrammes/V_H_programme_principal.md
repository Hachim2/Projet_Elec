# V.H — Programme principal et gestion temporelle

**Emplacement dans le rapport :** Partie V, au début de la partie concernée, avant l’explication du fonctionnement.

**Ce que représente l’algorigramme :** Fonctions setup() et loop() de `main.cpp`, avec les appels aux modules, et rôle des deux interruptions.

```mermaid
flowchart TD
    subgraph PRINCIPAL["Programme principal"]
        direction TB
        DEBUT(["Mise sous tension"])
        S1["setup : liaison série, écrans et encodeur"]
        S2["Initialiser le buzzer, les LED et les boutons"]
        S3["Initialiser le RTC, l'EEPROM et les réglages, puis appliquer le contraste"]
        S4["Initialiser le calcul du BPM : démarrage du Timer1"]
        S5[/"Afficher le menu principal"/]
        L1["loop : mettre à jour le BPM (module bpm)"]
        L2["Traiter un réglage de l'heure reçu par le port série"]
        L3["Lire le bouton de son"]
        L4{"Battement signalé ?"}
        L5["Émettre un bip (module peripheriques)"]
        L6{"200 ms écoulées ?"}
        L7["Lire l'heure corrigée (module donnees)"]
        L8{"BPM ou heure modifié ?"}
        L9["Mettre à jour les LED et redessiner l'écran 2"]
        L10["Transférer les échantillons en attente vers le tracé"]
        L11["Lire le bouton retour"]
        L12["Traiter la page active : menu, graphique, données, confirmation, message ou réglages"]
        DEBUT --> S1 --> S2 --> S3 --> S4 --> S5 --> L1
        L1 --> L2 --> L3 --> L4
        L4 -->|"Oui"| L5 --> L6
        L4 -->|"Non"| L6
        L6 -->|"Oui"| L7 --> L8
        L6 -->|"Non"| L8
        L8 -->|"Oui"| L9 --> L10
        L8 -->|"Non"| L10
        L10 --> L11 --> L12
        L12 -->|"Tour suivant, sans temporisation bloquante"| L1
    end
    subgraph INTERRUPTIONS["Interruptions : exécutées en parallèle de la boucle"]
        direction TB
        I1(["Timer1 : toutes les 5 ms"])
        I2["Lire le capteur, détecter les battements et déposer un échantillon pour le tracé"]
        I3(["Changement d'état de l'encodeur"])
        I4["Décoder la rotation et compter les pas"]
        I1 --> I2
        I3 --> I4
    end
    I2 -.->|"Intervalles, battements, échantillons"| L1
    I4 -.->|"Pas en attente"| L12
```

**Légende proposée :** Algorigramme du programme principal : initialisation, boucle non bloquante et interruptions.

**Note :** La boucle n’utilise aucune temporisation bloquante : les périodes (200 ms pour l’heure, 50 ms pour le graphique) sont obtenues par comparaison avec millis(). Les flèches en pointillés représentent les données partagées avec les interruptions.
