#include <Arduino.h>

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

void interruptToTime(int interrupts);

void setup()
{
  Serial.begin(115200);

  // impiego il timer hardware numero 0, con prescaler 80, conteggio in avanti
  timer0 = timerBegin(0, 80, true);

  // associazione della ISR al timer per gestire l'interrupt periodico
  timerAttachInterrupt(timer0, onTimer, true);

  // impostazione del valore di soglia del contatore per innescare l'interrupt periodico
  timerAlarmWrite(timer0, 100000, true);

  // abilitazione del timer
  timerAlarmEnable(timer0);
}

void loop()
{
  if (interruptCounter > 0)
  {
    numberOfInterrupts += interruptCounter;
    interruptCounter = 0;
    interruptToTime(numberOfInterrupts);

    Serial.println(String(Timer.hours) + ":" + String(Timer.minutes) + ":" + String(Timer.seconds) + "." + String(Timer.tenth));
  }
}

void interruptToTime(int interrupts)
{
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