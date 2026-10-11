# VI.E — TI-04 : restitution sonore

**Emplacement dans le rapport :** Partie VI, test concerné, « 1. Protocole expérimental ».

**Ce que représente l’algorigramme :** Protocole expérimental prévu pour vérifier ET4.1 à ET4.3 : cadence des bips, tonalité selon la zone, coupure et reprise du son.

```mermaid
flowchart TD
    DEBUT(["Début du test TI-04"])
    DOIGT["Poser le doigt et attendre un BPM stable"]
    OSC[/"Observer à l'oscilloscope les battements détectés et la commande du buzzer"/]
    CAD{"Un bip par battement : la cadence des bips correspond au BPM ?"}
    E1["Consigner l'écart sur ET4.1"]
    IMP["Imposer un BPM dans la zone suivante : basse, normale, puis haute"]
    MES[/"Mesurer la fréquence de la tonalité"/]
    TON{"Tonalité attendue : 500 Hz, 1000 Hz ou 2000 Hz selon la zone ?"}
    E2["Consigner l'écart sur ET4.2"]
    ZONES{"Les trois zones sont mesurées ?"}
    B1["Appuyer sur le bouton de son"]
    COUPE{"Les bips cessent ?"}
    E3["Consigner l'écart sur ET4.3"]
    B2["Appuyer de nouveau sur le bouton de son"]
    REPR{"Les bips reprennent ?"}
    E4["Consigner l'écart sur ET4.3"]
    PAGES{"Coupure et reprise testées depuis plusieurs pages ?"}
    CHPAGE["Ouvrir une autre page de l'interface"]
    BILAN[/"Noter le verdict de ET4.1, ET4.2 et ET4.3"/]
    FIN(["Fin du test"])
    DEBUT --> DOIGT --> OSC --> CAD
    CAD -->|"Oui"| IMP
    CAD -->|"Non"| E1 --> IMP
    IMP --> MES --> TON
    TON -->|"Oui"| ZONES
    TON -->|"Non"| E2 --> ZONES
    ZONES -->|"Non"| IMP
    ZONES -->|"Oui"| B1 --> COUPE
    COUPE -->|"Oui"| B2
    COUPE -->|"Non"| E3 --> B2
    B2 --> REPR
    REPR -->|"Oui"| PAGES
    REPR -->|"Non"| E4 --> PAGES
    PAGES -->|"Non"| CHPAGE --> B1
    PAGES -->|"Oui"| BILAN --> FIN
```

**Légende proposée :** Algorigramme du protocole TI-04 : cadence des bips, fréquence des trois tonalités, coupure et reprise du son.

**Note :** La cadence des bips (un par battement, donc liée au BPM) est distincte de la fréquence de la tonalité (500, 1000 ou 2000 Hz). Dans le rapport, seules la cadence et la tonalité de la zone normale (998 Hz mesurés) sont documentées ; le reste du protocole est à réaliser.
