# VI.D — TI-03 : signalisation lumineuse

**Emplacement dans le rapport :** Partie VI, test concerné, « 1. Protocole expérimental ».

**Ce que représente l’algorigramme :** Protocole expérimental prévu pour vérifier ET3.1 à ET3.3 avec des BPM imposés aux limites des zones, puis avec le capteur réel.

```mermaid
flowchart TD
    DEBUT(["Début du test TI-03"])
    REGL["Vérifier que les voyants sont activés dans les réglages"]
    IMP["Imposer dans le programme la valeur suivante : 59, 60, 100, 101, puis mesure invalide"]
    OBS[/"Relever l'état des trois voyants"/]
    EXC{"Un seul voyant allumé au maximum ?"}
    E1["Consigner l'écart sur ET3.1"]
    COUL{"État attendu : 59 jaune, 60 vert, 100 vert, 101 rouge, invalide aucun ?"}
    E2["Consigner l'écart sur ET3.2 ou ET3.3"]
    TOUT{"Toutes les valeurs sont testées ?"}
    REEL["Revenir au fonctionnement normal avec le capteur WPSE340"]
    V3{"La couleur correspond au BPM affiché ?"}
    E3["Consigner l'écart"]
    RETIRE["Retirer le doigt et attendre l'invalidation de la mesure"]
    V4{"Les trois voyants sont éteints ?"}
    E4["Consigner l'écart sur ET3.3"]
    BILAN[/"Noter le verdict de ET3.1, ET3.2 et ET3.3"/]
    FIN(["Fin du test"])
    DEBUT --> REGL --> IMP --> OBS --> EXC
    EXC -->|"Oui"| COUL
    EXC -->|"Non"| E1 --> COUL
    COUL -->|"Oui"| TOUT
    COUL -->|"Non"| E2 --> TOUT
    TOUT -->|"Non"| IMP
    TOUT -->|"Oui"| REEL --> V3
    V3 -->|"Oui"| RETIRE
    V3 -->|"Non"| E3 --> RETIRE
    RETIRE --> V4
    V4 -->|"Oui"| BILAN
    V4 -->|"Non"| E4 --> BILAN
    BILAN --> FIN
```

**Légende proposée :** Algorigramme du protocole TI-03 : essais aux limites des zones, exclusivité des voyants et extinction sans mesure.

**Note :** Seuils de la version testée : jaune sous 60 BPM, vert de 60 à 100 BPM, rouge au-dessus de 100 BPM. Ils correspondent au code final. Une version antérieure du code et du rapport plaçait le seuil haut à 90 BPM : vérifier que cette valeur n’apparaît plus ailleurs dans le rapport.
