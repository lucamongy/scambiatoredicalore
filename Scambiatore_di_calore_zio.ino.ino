#include <Wire.h>
#include <Time.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <DHT.h>
#include "DHT.h"
//definisco il pin del sensore umidità (esterna)
#define DHTPIN1 9
//definisco il tipo di sensore
#define DHTTYPE DHT11
//pin errore rtc
#define errore_rtc 12
//pin led relè
#define reled 11
//pin rele
#define rele 10
//creo l'oggetto DHT (esterna)
DHT dht1(DHTPIN1, DHTTYPE);
//creo l'oggetto TimeElements
TimeElements te;
const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

tmElements_t tm;

void setup() {
  bool parse=false;
  bool config=false;

  // get the date and time the compiler was run
  if (getDate(__DATE__) && getTime(__TIME__)) {
    parse = true;
    // and configure the RTC with this info
    if (RTC.write(tm)) {
      config = true;
    }
  }

  Serial.begin(9600);
  while (!Serial) ; // wait for Arduino Serial Monitor
  delay(200);
  if (parse && config) {
    Serial.print("DS1307 configured Time=");
    Serial.print(__TIME__);
    Serial.print(", Date=");
    Serial.println(__DATE__);
  } else if (parse) {
    Serial.println("DS1307 Communication Error :-{");
    Serial.println("Please check your circuitry");
  } else {
    Serial.print("Could not parse info from the compiler, Time=\"");
    Serial.print(__TIME__);
    Serial.print("\", Date=\"");
    Serial.print(__DATE__);
    Serial.println("\"");
  }
  delay(1000);
  //led errore rtc
  pinMode(errore_rtc, OUTPUT); 
  //led relè
  pinMode(reled, OUTPUT); 
  //pin relè
  pinMode(rele, OUTPUT); 
}
bool rtc_error=false;

void loop() {
  if (RTC.read(te))
  {
    int inizio=9;
    int fine=16;
    //salvo l'umidità esterna attuale in una variabile
    int umiditaest = dht1.readHumidity();
    //stampo su monitor seriale l'orario e l'umidità
    Serial.print("Tempo RTC = ");
    Serial.print(te.Hour);
    Serial.write(':');
    Serial.print(te.Minute);
    Serial.write(':');
    Serial.println();
    Serial.print("Umidita' esterna = ");
    Serial.print(umiditaest);
    //condizione orario
    if(te.Hour>=inizio & te.Hour<fine){
      //condizione umidità
      if(umiditaest<60){
        //accendo motorino
        digitalWrite(rele,HIGH);
        digitalWrite(reled,HIGH);
      }else{
      //spengo motorino se prima era acceso
      digitalWrite(rele,LOW); 
      digitalWrite(reled,LOW);
      }
    }else{
       //eseguo nuovamente lo spegnimento
      digitalWrite(rele,LOW);
      digitalWrite(reled,LOW);       
    }
  }else rtc_error=true;
  if(rtc_error) digitalWrite(errore_rtc,HIGH); //accendo il led errore dell'RTC
  else digitalWrite(errore_rtc,LOW);
  //leggi ogni secondo
  delay(1000);
}
// scrivo le funzioni per ricavare data e ora attuale
bool getTime(const char *str)
{
  int Hour, Min, Sec;

  if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
  tm.Hour = Hour;
  tm.Minute = Min;
  tm.Second = Sec;
  return true;
}

bool getDate(const char *str)
{
  char Month[12];
  int Day, Year;
  uint8_t monthIndex;

  if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3) return false;
  for (monthIndex = 0; monthIndex < 12; monthIndex++) {
    if (strcmp(Month, monthName[monthIndex]) == 0) break;
  }
  if (monthIndex >= 12) return false;
  tm.Day = Day;
  tm.Month = monthIndex + 1;
  tm.Year = CalendarYrToTm(Year);
  return true;
}
