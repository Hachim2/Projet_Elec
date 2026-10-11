# Écarts entre le rapport, le code et le schéma

Relevés pendant la création des 19 algorigrammes (code du dépôt au commit 776a56d, 8 octobre 2026).

1. **Broches de l’encodeur.** Test unitaire `encodeur_test.cpp` : A sur D2, B sur D3, bouton sur D4. Programme final : bouton sur D2, A sur D3, B sur D4. Le rapport donne le brochage final.
2. **Bouton de l’encodeur.** Le test unitaire affiche « Appui » à l’enfoncement ; le programme final agit au relâchement.
3. **Buzzer.** Test unitaire : tout ou rien sur D8, 1 s de son puis 2 s de silence (un message de commit mentionne 2 s / 2 s). Programme final : D5, fonction tone(), trois tonalités de 500, 1000 et 2000 Hz, bip de 40 ms.
4. **Coupure du son.** Présente dans le code final (bouton sur D13, réglage conservé en EEPROM).
5. **Seuil haut des LED.** 100 BPM dans le code final et dans les pages Notion ; une version antérieure utilisait 90 BPM.
6. **Test du RTC.** Le rapport indique un réglage à l’heure de compilation ; le programme de test ne règle l’horloge que sur réception de la commande r.
7. **Test bouton et LED.** Broches du test (LED sur D6, D5, D4, bouton sur D2 avec rappel externe à la masse) différentes du montage final (LED sur D8, D7, D6, bouton retour sur D9 avec rappel interne).
8. **Enregistrement.** Refusé tant que la moyenne ne porte pas sur dix intervalles (présent dans le code). Déclenché depuis le menu par le bouton de l’encodeur, et non par un bouton poussoir dédié comme le demande ET6.2.
9. **Calcul du BPM.** Moyenne glissante des dix derniers intervalles, convertie en BPM (60 000 / intervalle moyen). Ce n’est pas une moyenne de dix valeurs de BPM.
10. **Langue.** Réglage français / anglais présent dans le code final (page « Réglages »).
11. **Schéma électrique.** Les deux boutons y sont dessinés en série avec une résistance de 10 kΩ vers la masse. Le code attend le bouton retour directement entre D9 et la masse, et le bouton de son entre +5 V et D13 avec 10 kΩ entre D13 et la masse.
12. **Tests d’intégration.** Les protocoles TI-01 à TI-06 sont représentés tels que prévus. Plusieurs étapes ne sont pas encore documentées par des résultats (éclairages et 15 minutes pour TI-01, relevé à 24 h pour TI-02, tonalités de 500 et 2000 Hz et bouton de son pour TI-04, coupure et effacement pour TI-06).

Vérification : les 19 blocs Mermaid ont été analysés sans erreur par l’analyseur de Mermaid 10.9.1. Le rendu graphique n’a pas été testé.
