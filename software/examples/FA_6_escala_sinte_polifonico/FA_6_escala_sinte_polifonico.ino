/*
    The Fenderino Arducaster Series

    Capitulo 6: Escala Sintetizador Polifonico

    Toca una escala usando los botones de la guitarra, el potenciometro
    usa la libreria Synth creada por DZL para Illutron en el 2014 que permite
    tocar polifonias y cinco tipos diferentes de onda (seno, cuadrada,
    sierra, rampa y ruido). Busca mas informacion de la libreria en:
    https://github.com/dzlonline/the_synth

    Este codigo esta licenciado bajo GPLv3

    (c) 2017 D. Cuartielles para Abierto.cc
    (c) 2014 DZL at dzlsevilgeniuslair.blogspot.dk

*/

// incluye la libreria del sintetizador
#include <synth.h>

// incluye los nombres de las notas
#include "notes.h"

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

// variables para definir cuando se mandan datos por el puerto serie
long timerSerie = 0;
long timerSerieValor = 500;

// variables para contorlar los avisos visuales
boolean aviso = true;
int avisoCount = 0;
long timerAvisoValor = 200;

// define las frecuencias de las notas para cada boton
int frecuencia[] = {
  NOTE_C1, NOTE_D1, NOTE_E1, NOTE_F1, NOTE_G1, NOTE_A1, NOTE_B1,
  NOTE_C2, NOTE_D2, NOTE_E2, NOTE_F2, NOTE_G2, NOTE_A2, NOTE_B2,
  NOTE_C3, NOTE_D3, NOTE_E3, NOTE_F3, NOTE_G3, NOTE_A3, NOTE_B3,
  NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_B4,
  NOTE_C5, NOTE_D5, NOTE_E5, NOTE_F5, NOTE_G5, NOTE_A5, NOTE_B5,
  NOTE_C6, NOTE_D6, NOTE_E6, NOTE_F6, NOTE_G6, NOTE_A6, NOTE_B6,
  NOTE_C7, NOTE_D7, NOTE_E7, NOTE_F7, NOTE_G7, NOTE_A7, NOTE_B7,
  NOTE_C8, NOTE_D8,
};

// define donde se empiezan a contar los tonos del array de frecuencias
int indice = 0;
int indiceOld = 1; // ese lo usaremos para saber cuando cambia

// declara la variable del sintetizador
synth sinte;

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
  //pinMode(speaker, OUTPUT);

  // inicializa el sintetizador para que use el pin 3
  sinte.begin(CHB);     //CHB

  // configura los osciladores que componen el timbre de una nota
  //setupVoice( voice[0-3] , waveform[SINE,TRIANGLE,SQUARE,SAW,RAMP,NOISE] , pitch[0-127], envelope[ENVELOPE0-ENVELOPE3], length[0-127], mod[0-127, 64=no mod])
  sinte.setupVoice(0, RAMP, 50, ENVELOPE2, 40, 20);
  sinte.setupVoice(1, TRIANGLE, 66, ENVELOPE1, 70, 20);
  sinte.setupVoice(2, NOISE, 30, ENVELOPE0, 100, 64);

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

  // desactiva el sonido si el pulsador esta en el centro
  if (valInterruptor[0] == 1 && valInterruptor[1] == 1) {
    noTone(3);
  } else {
    sinte.begin(CHB);
  }

  // cambia el tono en la salida segun los botones si el interruptor esta hacia arriba
  if (valInterruptor[0] == 0) {
    indice = round( map(valPot, 0, 1023, 0, (sizeof(frecuencia) / sizeof(int)) - 7) );

    // si el indice es multiplo de 8, que es el principio de escala, haz un aviso visual
    if (indice % 8 == 0 && indiceOld != 0)
      aviso = true;

    // asigna la nota dando preferencia a la mas alta si se aprietan dos teclas
    int nota = -1;
    for (int i = 0; i < 8; i++) {
      if (valBotones[i] == 0)
        nota = i;
    }
    if (nota > -1) {
      sinte.mTrigger(0, frecuencia[nota + indice] + 32);
      sinte.mTrigger(1, frecuencia[nota + indice] + 32);
      sinte.mTrigger(2, frecuencia[nota + indice] + 32);
    } 
  } 

  // cambia el tono en la salida segun los botones si el interruptor esta hacia abajo
  if (valInterruptor[1] == 0) {
    indice = round( map(valPot, 0, 1023, 0, (sizeof(frecuencia) / sizeof(int)) - 7) );

    // si el indice es multiplo de 8, que es el principio de escala, haz un aviso visual
    if (indice % 8 == 0 && indiceOld != 0)
      aviso = true;

    // asigna la nota dando preferencia a la mas alta si se aprietan dos teclas
    int nota = -1;
    for (int i = 0; i < 8; i++) {
      if (valBotones[i] == 0)
        nota = i;
    }
    if (nota > -1) {
      sinte.mTrigger(0, frecuencia[nota + indice] + 32);
      sinte.mTrigger(1, frecuencia[nota + indice] + 32);
      // Sin Distorsion!! sinte.mTrigger(2, frecuencia[nota + indice] + 32);
    } 
  } 

  // mapea el paso del tiempo en los leds, controlalo con el potenciometro
  // pero solo cuando no estes ejecutando una secuencia de aviso
  if (millis() - timer > valPot && !aviso) {
    valCount++;
    valCount %= 4;

    // enciende el led
    for (int i = 0; i < 4; i++) {
      if (i == valCount) digitalWrite(leds[i], HIGH);
      else digitalWrite(leds[i], LOW);
    }

    timer = millis();
  }

  // ejecuta una secuencia de aviso en los leds
  if (millis() - timer > timerAvisoValor && aviso) {
    avisoCount++;
    if (avisoCount % 2 == 0) {
      // enciende los leds
      for (int i = 0; i < 4; i++)
        digitalWrite(leds[i], HIGH);
    } else {
      // apaga los leds
      for (int i = 0; i < 4; i++)
        digitalWrite(leds[i], LOW);
    }

    // decide no volver a entrar is se llego al maximo
    if (avisoCount > 6) {
      avisoCount = 0;
      aviso = false;
    }

    timer = millis();
  }

  // envia los valores por el puerto serie cada timerSerieValor
  if (millis() - timerSerie > timerSerieValor) {
    for (int i = 0; i < 8; i++) {
      Serial.print(valBotones[i]); Serial.print(" | ");
    }
    for (int i = 0; i < 2; i++) {
      Serial.print(valInterruptor[i]); Serial.print(" | ");
    }
    Serial.print(indice); Serial.print(" | ");
    Serial.print((sizeof(frecuencia) / sizeof(int))); Serial.print(" | ");
    Serial.println(valPot);
    timerSerie = millis();
  }

  // guarda informacion de los indices
  indiceOld = indice;

}


