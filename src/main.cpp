/*
  Nome del Programma: Cronometro ESP32
  Autore: Mascagni Lorenzo
  Scopo del Programma:
  Questo programma implementa un cronometro utilizzando ESP32.
  Il cronometro inizia quando il pin di startPin viene attivato sul fronte di salita,
  si ferma quando il pin di stopPin viene attivato e mostra il tempo trascorso sulla porta seriale.
*/

#include <Arduino.h>
#include <Bounce2.h>

// Definizione dei pin di start e stop
#define startPin 23
#define stopPin 22

// Struttura per memorizzare le informazioni sul tempo
struct Timer {
  int hours;
  int minutes;
  int seconds;
  int tenth;

  // Funzione per aggiornare i valori di tempo
  void update(int interrupts) {
    // Calcola ore, minuti, secondi e decimi in base al numero di interrupts
    tenth = interrupts % 10;
    seconds = (interrupts % 600 - interrupts % 10) / 10;
    minutes = (interrupts % 36000 - interrupts % 600) / 600;
    hours = (interrupts % 864000 - interrupts % 36000) / 36000;
  }

  // Funzione per stampare il tempo sulla porta seriale
  void printTime() {
    printf("%02u:%02u:%02u.%u\n", hours, minutes, seconds, tenth);
  }
};

// Variabili globali
hw_timer_t *timer0 = NULL;
volatile int interruptCounter = 0;
int numberOfInterrupts = 0;

Timer timer;

Bounce start = Bounce();
Bounce stop = Bounce();

bool timerActive = false; // Flag per indicare se il cronometro è attivo o no

// Interrupt routine per il timer
void IRAM_ATTR onTimer() {
  interruptCounter++;
}

void setup() {
  Serial.begin(115200); // Inizializza la comunicazione seriale

  start.attach(startPin, INPUT_PULLUP);
  stop.attach(stopPin, INPUT_PULLUP);

  start.interval(50);
  stop.interval(50);

  // Inizializza il timer hardware
  timer0 = timerBegin(0, 80, true);
  timerAttachInterrupt(timer0, onTimer, true); // Allega l'ISR al timer
  timerAlarmWrite(timer0, 100000, true); // Imposta il valore di soglia del contatore per l'interrupt
}

void loop() {
  start.update();
  stop.update();

  // Avvia il cronometro solo se il pin di start è attivo sul fronte di discesa
  if (start.fell() && !timerActive) {
    timerActive = true;
    timerAlarmEnable(timer0);
    numberOfInterrupts = 0; // Resetta il conteggio degli interrupt
  }

  // Disabilita il cronometro se il pin di stop è attivo
  if (stop.fell()) {
    timerAlarmDisable(timer0);
    timerActive = false;
  }

  // Se ci sono interrupt avvenuti e il cronometro è attivo, aggiorna e stampa il tempo
  if (interruptCounter > 0 && timerActive) {
    numberOfInterrupts += interruptCounter;
    interruptCounter = 0;

    timer.update(numberOfInterrupts);
    timer.printTime();
  }
}
