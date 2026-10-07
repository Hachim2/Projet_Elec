#include <Arduino.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>

const byte PIN_CLK = 10;
const byte PIN_DAT = 11;
const byte PIN_CE  = 12;

// Ordre : DAT, CLK, CE
ThreeWire liaison(PIN_DAT, PIN_CLK, PIN_CE);
RtcDS1302<ThreeWire> rtc(liaison);

void afficherDeuxChiffres(uint8_t valeur) {
  if (valeur < 10) {
    Serial.print('0');
  }
  Serial.print(valeur);
}

void setup() {
  Serial.begin(115200);
  rtc.Begin();

  rtc.SetIsWriteProtected(false);

  // Desactive la recharge de la pile.
  rtc.SetTrickleChargeSettings(DS1302Tcr_Disabled);

  Serial.println("=== Test RTC DS1302 ===");
  Serial.println("Envoyer r pour regler et demarrer l'horloge.");
  Serial.println("Ne pas envoyer r apres la coupure !");
}

void loop() {
  if (Serial.available() > 0) {
    char commande = Serial.read();

    if (commande == 'r' || commande == 'R') {
      // Date et heure de compilation du programme.
      rtc.SetDateTime(RtcDateTime(__DATE__, __TIME__));
      rtc.SetIsRunning(true);

      Serial.println("Horloge reglee et demarree.");
    }
  }

  if (!rtc.GetIsRunning()) {
    Serial.println("Horloge arretee : envoyer r pour l'initialiser.");
  }
  else if (!rtc.IsDateTimeValid()) {
    Serial.println("Date invalide : verifier le montage et la pile.");
  }
  else {
    RtcDateTime maintenant = rtc.GetDateTime();

    afficherDeuxChiffres(maintenant.Day());
    Serial.print('/');
    afficherDeuxChiffres(maintenant.Month());
    Serial.print('/');
    Serial.print(maintenant.Year());
    Serial.print("  ");

    afficherDeuxChiffres(maintenant.Hour());
    Serial.print(':');
    afficherDeuxChiffres(maintenant.Minute());
    Serial.print(':');
    afficherDeuxChiffres(maintenant.Second());
    Serial.println();
  }

  delay(1000);
}