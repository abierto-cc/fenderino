/*
 *   The Fenderino Arducaster Series
 *
 *   Capitulo 7: Escala Con Voz
 *
 * Usamos la libreia Talkie para la generacion de voz 
 *
 * Este codigo esta licenciado bajo GPLv3
 *
 * (c) 2017 D. Cuartielles para Abierto.cc
 * (c) 2011 P. Knight para la libreria Talkie
 */

// incluye los nombres de las notas
#include "voz.h"

// incluye la libreria ROM to Speech
#include "talkie.h"

// declara el objeto que lleva la voz
Talkie voz;

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

// define las palabras posibles para cada boton en una tabla de voces
// los posibles sonidos vienen de voz.h
const uint8_t* const vozTabla[] PROGMEM = {
  spONE, spTWO, spTHREE, spFOUR, spFIVE, spSIX, spSEVEN, spEIGHT, spNINE, spTEN,
  spELEVEN, spTWELVE, spTHIRTEEN, spFOURTEEN, spFIFTEEN, spSIXTEEN, spSEVENTEEN,
  spEIGHTEEN, spNINETEEN, spTWENTY,
};

// define donde se empiezan a contar los sonidos del array de voces
int indice = 0;
int indiceOld = 1; // ese lo usaremos para saber cuando cambia

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
    indice = round( map(valPot, 0, 1023, 0, sizeof(vozTabla)/sizeof(uint8_t*) - 7) );

    // si el indice es multiplo de 8, que es el principio de escala, haz un aviso visual
    if (indice % 8 == 0 && indiceOld != 0)
      aviso = true;

    // asigna la nota dando preferencia a la mas alta si se aprietan dos teclas
    int nota = -1;
    for (int i = 0; i < 8; i++) {
      if (valBotones[i] == 0)
        nota = i;
    }

    // esta es la parte mas punky del programa, consiste en referenciar lo que se dice 
    // de modo que podamos subir la escala y asi cambiar el numero usando el potenciometro
    // para eso hemos hecho una tabla de arrays, esto solo se puede hacer con un puntero
    // por otra parte hay algo de ruido de alta frecuencia que igual habria que limpiar con
    // un condensador. Tambien seria bueno ver si se puede hackear la libreria Talkie para
    // que deje de tocar al presionar otra tecla
    if (nota > -1)
      voz.say((uint8_t*)pgm_read_word(&(vozTabla[nota + indice])));
    //else
      //noTone(3);
  } //else
    //noTone(3);

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
    Serial.print(sizeof(vozTabla)/sizeof(uint8_t*)); Serial.print(" | ");
    Serial.println(valPot);
    timerSerie = millis();
  }

  // guarda informacion de los indices
  indiceOld = indice;

}
