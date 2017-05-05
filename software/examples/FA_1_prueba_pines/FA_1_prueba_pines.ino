/*
 * The Fenderino Arducaster Series
 * 
 * Capitulo 1: Manejo de los pines de entrada salida
 * 
 * El shield Fenderino Arduicaster hace uso de practicamente todos pines
 * de entrada / salida de tu Arduino Uno. La razon es que tenemos:
 * 
 * - 8 pulsadores para tocar notas
 * - un potenciometro controlable por software
 * - una salida de audio
 * - un cubo de 4 leds para efectos visuales
 * - un interruptor de 3 posiciones
 * - ademas contamos con un potenciometro para controlar el volumen de salida
 * 
 * Este sketch se encarga de mapear todos los valores de los botones, interruptor, potenciometro
 * hacia los cuatro leds de salida y el puerto serie, de modo que puedas aprender algo mas
 * sobre como funciona el Shield y la placa Arduino
 * 
 * Este codigo esta licenciado bajo GPLv3
 * 
 * (c) 2017 D. Cuartielles para Abierto.cc
 * 
 */

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

  // mapea el potenciometro en los leds
  int numLed = int(map(valPot, 0, 1023, 0, 4));
  for (int i = 0; i < 4; i++) {
    if (i == numLed) digitalWrite(leds[i], HIGH);
    else digitalWrite(leds[i], LOW);
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
