#ifndef ENCODER_HPP
#define ENCODER_HPP

// Configure A, B et le bouton en INPUT_PULLUP et initialise leur etat.
void init_encodeur();

// Applique les mouvements au menu. Renvoie true une fois au relachement
// du bouton, apres un appui valide et le filtrage des rebonds.
bool lire_encodeur(int* selected, int menu_size);

// Accepte aussi plusieurs pas accumules pendant le dessin de l'ecran.
void apply_encoder_step(int delta, int* selected, int menu_size);

#endif
