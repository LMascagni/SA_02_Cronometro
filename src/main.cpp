#include <Arduino.h>

// Definizione dei pin di start e stop
#define startPin 23
#define stopPin 22

// Struttura per memorizzare le informazioni sul tempo
struct Timer
{
  int hours;
  int minutes;
  int seconds;
  int tenth;

  // Funzione per aggiornare i valori di tempo
  void update(int interrupts)
  {
    tenth = interrupts % 10;
    seconds = (interrupts % 600 - interrupts % 10) / 10;
    minutes = (interrupts % 36000 - interrupts % 600) / 600;
    hours = (interrupts % 864000 - interrupts % 36000) / 36000;
  }

  // Funzione per stampare il tempo sulla porta seriale
  void printTime()
  {
    Serial.printf("%02u:%02u:%02u.%u\n", hours, minutes, seconds, tenth);
  }
};

hw_timer_t *timer0 = NULL;         // Puntatore al timer hardware
volatile int interruptCounter = 0; // Contatore degli interrupt
int numberOfInterrupts = 0;        // Numero totale degli interrupt

Timer timer; // Istanzia un oggetto della struct Timer

// Interrupt routine per il timer
void IRAM_ATTR onTimer()
{
  interruptCounter++;
}

void setup()
{
  Serial.begin(115200);            // Inizializza la comunicazione seriale
  pinMode(startPin, INPUT_PULLUP); // Imposta il pin di start come input con pull-up
  pinMode(stopPin, INPUT_PULLUP);  // Imposta il pin di stop come input con pull-up

  timer0 = timerBegin(0, 80, true);            // Inizializza il timer hardware
  timerAttachInterrupt(timer0, onTimer, true); // Allega l'ISR al timer
  timerAlarmWrite(timer0, 100000, true);       // Imposta il valore di soglia del contatore per l'interrupt
}

void loop()
{
  if (!digitalRead(startPin))
  {                           // Se il pin di start è attivo
    timerAlarmEnable(timer0); // Abilita il timer
    numberOfInterrupts = 0;   // Resetta il conteggio degli interrupt
  }

  if (!digitalRead(stopPin))
  {                            // Se il pin di stop è attivo
    timerAlarmDisable(timer0); // Disabilita il timer
  }

  if (interruptCounter > 0)
  {                                         // Se ci sono interrupt avvenuti
    numberOfInterrupts += interruptCounter; // Aggiorna il numero totale di interrupt
    interruptCounter = 0;                   // Resetta il contatore degli interrupt

    timer.update(numberOfInterrupts); // Aggiorna la struct Timer con il nuovo tempo
    timer.printTime();                // Stampa il tempo sulla porta seriale
  }
}
