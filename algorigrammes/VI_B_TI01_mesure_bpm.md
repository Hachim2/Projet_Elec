# VI.B — TI-01 : mesure de la fréquence cardiaque

**Emplacement dans le rapport :** Partie VI, test concerné, « 1. Protocole expérimental ».

**Ce que représente l’algorigramme :** Protocole expérimental prévu pour vérifier les exigences ET1.1 à ET1.5 sur le système complet.

```mermaid
flowchart TD
    DEBUT(["Début du test TI-01"])
    V1{"Capteur WPSE340 et traitement réalisé par le code du groupe ?"}
    E1["Consigner l'écart"]
    DOIGT["Poser le doigt de manière stable sur le capteur"]
    ATT{"Les 10 intervalles sont disponibles ?"}
    PAT["Attendre le battement suivant"]
    REL[/"Relever le signal PPG, les battements détectés et le BPM"/]
    COH{"BPM cohérent avec les intervalles détectés ?"}
    E2["Consigner l'écart"]
    LUM["Régler l'éclairage : faible, puis ambiant, puis fort"]
    OBS[/"Relever les détections manquées ou en trop et les pertes de mesure"/]
    ROB{"Mesure maintenue, sans série de faux battements ?"}
    E3["Consigner l'écart pour cet éclairage"]
    TROIS{"Les trois éclairages sont testés ?"}
    LONG["Laisser fonctionner environ 15 minutes, doigt immobile"]
    CMP[/"Comparer le comportement au début et à la fin"/]
    DER{"Dérive progressive ou interruption anormale ?"}
    E4["Consigner l'écart"]
    BILAN[/"Noter le verdict de ET1.1 à ET1.5"/]
    FIN(["Fin du test"])
    DEBUT --> V1
    V1 -->|"Oui"| DOIGT
    V1 -->|"Non"| E1 --> DOIGT
    DOIGT --> ATT
    ATT -->|"Non"| PAT --> ATT
    ATT -->|"Oui"| REL --> COH
    COH -->|"Oui"| LUM
    COH -->|"Non"| E2 --> LUM
    LUM --> OBS --> ROB
    ROB -->|"Oui"| TROIS
    ROB -->|"Non"| E3 --> TROIS
    TROIS -->|"Non"| LUM
    TROIS -->|"Oui"| LONG --> CMP --> DER
    DER -->|"Non"| BILAN
    DER -->|"Oui"| E4 --> BILAN
    BILAN --> FIN
```

**Légende proposée :** Algorigramme du protocole TI-01 : cohérence du BPM, robustesse à la luminosité et fonctionnement prolongé.

**Note :** Il s’agit du protocole prévu. Dans le rapport, les relevés sous trois éclairages et sur 15 minutes ne sont pas encore documentés.
