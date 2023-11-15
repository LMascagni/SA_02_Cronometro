#include <Arduino.h>

#define startPin 23
#define stopPin  22

// puntatore alla struttura timer da associare al timer hardware
hw_timer_t *timer0 = NULL;

// segnala che si è verificato un interrupt del timer
volatile int interruptCounter = 0;

// conteggio totale degli interrupt periodici
int numberOfInterrupts = 0;

struct timer
{
  int hours;
  int minutes;
  int seconds;
  int tenth;
};

timer Timer;

// dichiarazione della interrupt handling routine
void IRAM_ATTR onTimer();

//dichiarazione della funzione di conversione
void interruptToTime(int interrupts);

void setup()
{
  Serial.begin(115200);

  //definizione degli ingressi
  pinMode(startPin, INPUT_PULLUP);
  pinMode(stopPin, INPUT_PULLUP);

  // impiego il timer hardware numero 0, con prescaler 80, conteggio in avanti
  timer0 = timerBegin(0, 80, true);

  // associazione della ISR al timer per gestire l'interrupt periodico
  timerAttachInterrupt(timer0, onTimer, true);

  // impostazione del valore di soglia del contatore per innescare l'interrupt periodico
  timerAlarmWrite(timer0, 100000, true);

}

void loop()
{
  //start
  if(!digitalRead(startPin))
  {
    //abilitazione del timer e azzeramento di conteggio
    timerAlarmEnable(timer0);
    numberOfInterrupts = 0;
  }
  
  //stop
  if(!digitalRead(stopPin))
  {
    //disabilitazione del timer
    timerAlarmDisable(timer0);
  }

  if (interruptCounter > 0)
  {
    numberOfInterrupts += interruptCounter;
    interruptCounter = 0;

    //conversione dei dati
    interruptToTime(numberOfInterrupts);
    
    //stampa dei valori
    printf("%02u:%02u:%02u.%u\n",Timer.hours, Timer.minutes, Timer.seconds, Timer.tenth);
  }
}

void interruptToTime(int interrupts)
{
  //coversione della variabile di conteggio
  Timer.tenth   = interrupts % 10;
  Timer.seconds = (interrupts % 600 - interrupts % 10)/10;
  Timer.minutes = (interrupts % 36000 - interrupts % 600)/600;
  Timer.hours   = (interrupts % 864000 - interrupts % 36000)/36000;
}

// definition of the interrupt handling routine
void IRAM_ATTR onTimer()
{
  interruptCounter++;
}