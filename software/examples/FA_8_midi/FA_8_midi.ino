/*
    The Fenderino Arducaster Series

    Capítulo 8: Escala Con Botones, Potenciómetro y salida MIDI polifónica

    Toca una escala usando los botones de la guitarra, el potenciómetro
    te permite subir y bajar la escala, cuando la escala se ajuste a
    la primera tecla (es decir, Do en la primera tecla amarilla) las luces
    parpadean todas juntas

    Con el interruptor hacia arriba la salida es MIDI, el comportamiento es
    exactamente el mismo que el descrito en el párrafo anterior pero además
    tienes sonido polifónico (puedes tocar más de una tecla a la vez).

    ARDUINO LEONARDO (fácil) ---------------------------------------------------

    Este sketch utiliza la libreria "MIDIUSB" de Gary Grewal & Arduino
    (disponible desde el gestor de librerías del IDE de Arduino) para gestionar
    las notas desde el Leonardo:

    ==> https://github.com/arduino-libraries/MIDIUSB

    ARDUINO UNO (un poco más difícil) ------------------------------------------

    Este sketch utiliza la libreria "MIDI Library" de FortySevenEffects
    (disponible desde el gestor de librerías del IDE de Arduino) para gestionar
    las notas desde el Uno:

    ==> https://github.com/FortySevenEffects/arduino_midi_library

    Para que la connexión MIDI funcione via USB el controlador serie de la
    placa (atmega8u2 o atmega16u2) debe flashearse con el firmware HIDUINO:

    ==> https://github.com/ddiakopoulos/hiduino

    Para modificar el firmware del atmega16u2 y después para programar el
    atmega328p necesitarás un programador AVR. Para subir el código al
    atmega328 sitúa el interruptor en el centro.

    ----------------------------------------------------------------------------

    Una vez subido el código conecta tu Arduino (Uno o Leonardo) por USB a un
    cliente MIDI y selecciona la entrada "arduino_midi:0" (Uno) o
    "Arduino Leonardo:0" (Leonardo) y a tocar!

    Este codigo esta licenciado bajo GPLv3

    (c) 2017 D. Cuartielles para Abierto.cc
    (c) 2017 Xose Pérez (MIDI y soporte para Leonado)

*/

// -----------------------------------------------------------------------------

#include "Arduino.h"

// incluye los nombres de las notas
#include "pitches.h"
#include "pitches_midi.h"

#ifdef ARDUINO_AVR_UNO
    #include <MIDI.h>
    MIDI_CREATE_DEFAULT_INSTANCE();
#endif
#ifdef ARDUINO_AVR_LEONARDO
    #include "MIDIUSB.h"
#endif

// -----------------------------------------------------------------------------

// configuración del MIDI (comentar primera línea para desactivar MIDI)
#define MIDI_CHANNEL    1
#define MIDI_VELOCITY   64

// define los pines
int speaker = 3;
int botones[] = {9, 10, A1, A2, A3, A4, A5, 8};
int interruptor[] = {11, 12};
int leds[] = {4, 5, 7, 6};

// define arrays donde almacenar los valores
int valBotones[8] = {};
int valNotas[8] = {0};
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

// números de las notas MIDI
int notes[] = {
    MIDI_NOTE_C1, MIDI_NOTE_D1, MIDI_NOTE_E1, MIDI_NOTE_F1, MIDI_NOTE_G1, MIDI_NOTE_A1, MIDI_NOTE_B1,
    MIDI_NOTE_C2, MIDI_NOTE_D2, MIDI_NOTE_E2, MIDI_NOTE_F2, MIDI_NOTE_G2, MIDI_NOTE_A2, MIDI_NOTE_B2,
    MIDI_NOTE_C3, MIDI_NOTE_D3, MIDI_NOTE_E3, MIDI_NOTE_F3, MIDI_NOTE_G3, MIDI_NOTE_A3, MIDI_NOTE_B3,
    MIDI_NOTE_C4, MIDI_NOTE_D4, MIDI_NOTE_E4, MIDI_NOTE_F4, MIDI_NOTE_G4, MIDI_NOTE_A4, MIDI_NOTE_B4,
    MIDI_NOTE_C5, MIDI_NOTE_D5, MIDI_NOTE_E5, MIDI_NOTE_F5, MIDI_NOTE_G5, MIDI_NOTE_A5, MIDI_NOTE_B5,
    MIDI_NOTE_C6, MIDI_NOTE_D6, MIDI_NOTE_E6, MIDI_NOTE_F6, MIDI_NOTE_G6, MIDI_NOTE_A6, MIDI_NOTE_B6,
    MIDI_NOTE_C7, MIDI_NOTE_D7, MIDI_NOTE_E7, MIDI_NOTE_F7, MIDI_NOTE_G7, MIDI_NOTE_A7, MIDI_NOTE_B7,
    MIDI_NOTE_C8, MIDI_NOTE_D8,
};

// nota midi que está sonando
unsigned int anterior = 0;

// define donde se empiezan a contar los tonos del array de frecuencias
int indice = 0;
int indiceOld = 1; // ese lo usaremos para saber cuando cambia

// -----------------------------------------------------------------------------

void midiControl(byte control, byte value, byte channel) {
    #ifdef ARDUINO_AVR_UNO
        MIDI.sendControlChange(control, value, channel);
    #endif
    #ifdef ARDUINO_AVR_LEONARDO
        midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
        MidiUSB.sendMIDI(event);
    #endif
}

void midiNoteOn(byte pitch, byte velocity, byte channel) {
    #ifdef ARDUINO_AVR_UNO
        MIDI.sendNoteOn(pitch, velocity, channel);
    #endif
    #ifdef ARDUINO_AVR_LEONARDO
        midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
        MidiUSB.sendMIDI(noteOn);
    #endif
}

void midiNoteOff(byte pitch, byte velocity, byte channel) {
    #ifdef ARDUINO_AVR_UNO
        MIDI.sendNoteOff(pitch, velocity, channel);
    #endif
    #ifdef ARDUINO_AVR_LEONARDO
        midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
        MidiUSB.sendMIDI(noteOff);
    #endif
}

// -----------------------------------------------------------------------------

void setup() {

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

    // inicializa el interfaz MIDI para el uno
    #ifdef ARDUINO_AVR_UNO
        MIDI.begin();
    #endif
    midiControl(0, 0, 1);

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

    // calculamos el desplazamiento de la nota en función del potenciómetro
    indice = round( map(valPot, 0, 1023, 0, (sizeof(frecuencia) / sizeof(int)) - 7) );

    // si el indice es multiplo de 8, que es el principio de escala, haz un aviso visual
    if (indice % 8 == 0 && indiceOld != 0) {
        aviso = true;
    }

    // con el interruptor hacia arriba hacemos sonar una nota por el speaker de la guitarra
    if (valInterruptor[0] == 0) {

        // asigna la nota dando preferencia a la mas alta si se aprietan dos teclas
        int nota = -1;
        for (int i = 0; i < 8; i++) {
            if (valBotones[i] == 0)
            nota = i;
        }

        if (nota > -1) {
            tone(3, frecuencia[nota + indice]);
        } else {
            noTone(3);
        }

    // con el interruptor hacia abajo hacemos sonar una nota por el interfaz MIDI
    } else if (valInterruptor[1] == 0) {

        for (int i = 0; i < 8; i++) {

            // calculamos el codigo de nota si el botón está pulsado o 0 si no
            int nota = (valBotones[i] == 0) ? notes[i+indice] : 0;

            // si la nota para esta posición ha cambiado deberemos cambiar la salida MIDI
            if (nota != valNotas[i]) {

                // si estaba sonando una nota la apagamos
                if (valNotas[i] > 0) {
                    midiNoteOff(valNotas[i], MIDI_VELOCITY, MIDI_CHANNEL);
                }

                // hacemos sonar la nota nueva (si se está pulsando el botón)
                if (nota > 0) {
                    midiNoteOn(nota, MIDI_VELOCITY, MIDI_CHANNEL);
                }

                // y guardamos registro de qué nota está sonando en esta posición
                valNotas[i] = nota;

            }

        }

    } else {

        noTone(3);

        // apagamos las notas que puedan estar sonando
        for (int i = 0; i < 8; i++) {
            if (valNotas[i] > 0) {
                midiNoteOff(valNotas[i], MIDI_VELOCITY, MIDI_CHANNEL);
                valNotas[i] = 0;
            }
        }

    }

    #ifdef ARDUINO_AVR_LEONARDO
        MidiUSB.flush();
    #endif

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
            for (int i = 0; i < 4; i++) {
                digitalWrite(leds[i], HIGH);
            }
        } else {
            // apaga los leds
            for (int i = 0; i < 4; i++) {
                digitalWrite(leds[i], LOW);
            }
        }

        // decide no volver a entrar is se llego al maximo
        if (avisoCount > 6) {
            avisoCount = 0;
            aviso = false;
        }

        timer = millis();

    }

    // guarda informacion de los indices
    indiceOld = indice;

}
