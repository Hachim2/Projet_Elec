# V.F — Commande du buzzer

**Emplacement dans le rapport :** Partie V, au début de la partie concernée, avant l’explication du fonctionnement.

**Ce que représente l’algorigramme :** Module `peripheriques` du programme final : bouton de son, choix de la tonalité selon le BPM et bip de 40 ms à chaque battement.

```mermaid
flowchart TD
    DEBUT(["Tour de la boucle principale"])
    BTN[/"Lire le bouton de son sur D13 (anti-rebond de 30 ms)"/]
    REL{"Bouton de son relâché ?"}
    INV["Inverser le réglage du son et l'enregistrer en EEPROM"]
    BAT{"Battement signalé par l'interruption ?"}
    SON{"Son activé ?"}
    ZONE{"BPM courant ?"}
    F500["Tonalité de 500 Hz"]
    F1000["Tonalité de 1000 Hz"]
    F2000["Tonalité de 2000 Hz"]
    BIP[/"Lancer le signal sur D5 pour 40 ms"/]
    ARRET["Arrêt automatique du bip après 40 ms (Timer2)"]
    SUITE(["Tour suivant"])
    DEBUT --> BTN --> REL
    REL -->|"Oui"| INV --> BAT
    REL -->|"Non"| BAT
    BAT -->|"Non"| SUITE
    BAT -->|"Oui"| SON
    SON -->|"Non"| SUITE
    SON -->|"Oui"| ZONE
    ZONE -->|"De 1 à 59"| F500 --> BIP
    ZONE -->|"De 60 à 100, ou 0"| F1000 --> BIP
    ZONE -->|"Plus de 100"| F2000 --> BIP
    BIP --> ARRET --> SUITE
    SUITE -->|"Boucle"| DEBUT
```

**Légende proposée :** Algorigramme de la commande du buzzer : un bip de 40 ms par battement, trois tonalités, coupure par bouton.

**Note :** Les trois tonalités et la coupure du son sont présentes dans le code final. La temporisation du bip n’est pas gérée par la boucle : la fonction tone() arrête le signal seule. Un BPM de 0 (moyenne pas encore disponible) donne la tonalité de 1000 Hz. Le test unitaire IV.F, plus ancien, commandait le buzzer en tout ou rien.
