# IV.C — Test unitaire des deux écrans OLED

**Emplacement dans le rapport :** Partie IV, module concerné, « 2. Protocole de test unitaire et critères de réussite ».

**Ce que représente l’algorigramme :** Programme de test `Oled_test.cpp` : initialisation des deux écrans sur le même bus I²C et affichage du même texte.

```mermaid
flowchart TD
    DEBUT(["Début"])
    BUS["Démarrer le bus I2C (SDA sur A4, SCL sur A5)"]
    ADR1["Écran 1 : fixer l'adresse 0x3D"]
    INI1["Initialiser l'écran 1"]
    ADR2["Écran 2 : fixer l'adresse 0x3C"]
    INI2["Initialiser l'écran 2"]
    AFF1[/"Afficher « BONJOUR » centré sur l'écran 1"/]
    AFF2[/"Afficher « BONJOUR » centré sur l'écran 2"/]
    REPOS["Boucle principale vide : l'image reste affichée"]
    DEBUT --> BUS --> ADR1 --> INI1 --> ADR2 --> INI2 --> AFF1 --> AFF2 --> REPOS
    REPOS -->|"Tant que la carte est alimentée"| REPOS
```

**Légende proposée :** Algorigramme du test unitaire des deux écrans OLED : deux adresses sur un bus I²C commun.

**Note :** La bibliothèque U8g2 reçoit l’adresse décalée d’un bit vers la gauche ; ce détail n’est pas représenté.
