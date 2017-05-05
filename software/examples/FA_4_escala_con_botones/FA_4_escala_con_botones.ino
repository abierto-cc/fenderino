/*
 *  The Fenderino Arducaster Series
 *
 *  Capitulo 4: Escala Con Botones
 *
 * Toca una escala usando los botones de la guitarra
 *
 * Este codigo esta licenciado bajo GPLv3
 *
 * (c) 2017 D. Cuartielles para Abierto.cc
 *
 */

// incluye los nombres de las notas
#include "pitches.h"

// define los pines
int speaker = 3;
int botones[] = {9, 10, 15, 16, 17, 18, 19, 8};
int interruptor[] = {11, 12};
int leds[] = {4, 5, 7, 6};

// define arrays donde almacenar los valores
int valBotones[8] = {};
int valInterruptor[2] = {};
int valLeds[4] = {};
int valPot = 0;
int valCount = 0; // contador para almacenar que led se enciende

// variable para definir los tiempos de ciclo
long timer = 0;

// define las frecuencias de las notas para cada boton
int frecuencia[] = {NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5};

void setup() {
  // incializa el puerto serie
  Serial.begin(115200);

  // inicializa todos los botones como pines en pull-up
  for (int i = 0; i < 8; i++) pinMode(botones[i], INPUT_PULLUP);

  // inicializa el interruptor como pull-up
  for (int i = 0; i < 2; i++) pinMode(interruptor[i], INPUT_PULLUP);

  // inicializa el led como salida
  for (int i = 0; i < 4; i++) pinMode(leds[i], OUTPUT);

  // inicializa la salida de audio
  pinMode(speaker, OUTPUT);

  // inicializa el temporizador
  timer = millis();

}

void loop() {
  // lee toda la informacion y almacenala en sus arrays y variables correspondientes
  for (int i = 0; i < 8; i++) {
    valBotones[i] = digitalRead(botones[i]);
  }
  for (int i = 0; i < 2; i++) {
    valInterruptor[i] = digitalRead(interruptor[i]);
  }
  valPot = analogRead(A0);

  // cambia el tono en la salida segun los botones si el interruptor esta hacia arriba
  if (valInterruptor[0] == 0) {
    //int nota = round( map(valPot, 0, 1023, 0, 7) );

    // asigna la nota dando preferencia a la mas alta si se aprietan dos teclas
    int nota = -1;
    for (int i = 0; i < 8; i++) {
      if(valBotones[i] == 0)
        nota = i;
    }
    if (nota > -1)
      tone(3, frecuencia[nota]);
    else
      noTone(3);
  } else
    noTone(3);

  // mapea el paso del tiempo en los leds, controlalo con el potenciometro
  if (millis() - timer > valPot) {
    valCount++;
    valCount %= 4;
    for (int i = 0; i < 4; i++) {
      if (i == valCount) digitalWrite(leds[i], HIGH);
      else digitalWrite(leds[i], LOW);
    }
    timer = millis();
  }

  // envia los valores por el puerto serie
  for (int i = 0; i < 8; i++) {
    Serial.print(valBotones[i]); Serial.print(" | ");
  }
  for (int i = 0; i < 2; i++) {
    Serial.print(valInterruptor[i]); Serial.print(" | ");
  }
  Serial.println(valPot);

  delay(100);

}
