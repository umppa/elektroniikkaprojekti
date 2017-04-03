#include <LiquidCrystal.h>                 //näytön kirjasto
#include <OneWire.h>                       
#include <DallasTemperature.h>             //18B20 kirjasto
#include <MsTimer2.h>                      //keskeytyspalvelimen kirjasto
#include <Wire.h>                          
#include "cactus_io_BME280_I2C.h"          //BME280 kirjasto

#define trigPin 13
#define echoPin 12
#define kirkkaus 10
#define ONE_WIRE_BUS 3

#define btnDOWN   0                       //tilakoneen tilat
#define btnUP     1
#define btnRIGHT  2
#define btnLEFT   3
#define btnSELECT 4

int lcd_key = 0;
int readkey;
int sekunnit = 0;
int minuutit = 21;
int tunnit = 9;

BME280_I2C bme(0x76);                     //BME280 sensorin i2c osoite

int napit()                               //nappia painettaessa palauttaa ohjelman tiettyyn tilaan.
{ 
  readkey = analogRead(0);
    if (readkey < 50)   return btnRIGHT;  //tuntien säätö
    if (readkey < 195)  return btnUP ;    //ulkolämpötila
    if (readkey < 380)  return btnDOWN;   //kellonaika
    if (readkey < 550)  return btnLEFT;   //sisälämpötila ja ilmankosteus
    if (readkey < 790)  return btnSELECT; //minuuttien säätö
}

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);      
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);      //alustaa lcd-näytön pinnit

void sekunti()                            //keskeytyspalvelimen funktio, joka lisää sekuntteja kelloon 
{ 
  sekunnit = sekunnit + 1;
  aika();
}

void aika() {                             //muuntaa sekunnit minuuteiksi ja tunneiksi
  if(sekunnit == 60) 
    {
      sekunnit = 0;
      minuutit = minuutit + 1;
    }
  if(minuutit == 60) 
    {
      minuutit = 0;
      tunnit = tunnit + 1;
    }
  if(tunnit == 24) 
    {
      tunnit = 0;
    } 
}

void setup() {
  Serial.begin (9600);
  sensors.begin();                        //käynnistää dallastemperature kirjaston

  bme.begin();                            //käynnistää BME280 kirjaston?
  
  MsTimer2::set(1000, sekunti);           //keskeytyspalvelin
  MsTimer2::start();
  
  lcd.begin(16, 2);                       //näytön alustus

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(kirkkaus, OUTPUT);
}
void loop() 
{ long aika, matka;
  
                                          //ultraäänisensorin laskutoimitukset jotta saadaan oikea matka
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  aika = pulseIn(echoPin, HIGH);
  matka = (aika/2) / 29.1;
  
  sensors.requestTemperatures();          //pyytää sensorilta(18B20) lämpötilan
  bme.readSensor();                       //lukee BME280 sensorin datan
  

  digitalWrite(kirkkaus, LOW);            //kun ultraäänisensorin matka ylittyy näytön valaistus sammuu  
  if(matka < 50) 
    {
      digitalWrite(kirkkaus, HIGH);   
    }
  lcd_key = napit();                      //nappien funktio
  
  switch(lcd_key)                         //tilakone
  {
    
  case btnLEFT:                           //sisälämpötila ja ilmankosteus (BME280)
    {
      lcd.clear();
      lcd.print(bme.getTemperature_C());  //hakee lämpötilan sensorilta celsiuksina
      lcd.print((char)223);
      lcd.print("C indoors");
      lcd.setCursor(0,1);
      lcd.print(bme.getHumidity());       //hakee ilmankosteuden sensorilta 
      lcd.print(" % humidity");
      delay (1000);
      break;
    }
    
  case btnUP:                             //ulkolämpötila (18B20)
    {
      lcd.clear();
      lcd.print(sensors.getTempCByIndex(0));  
      lcd.setCursor(5,0);
      lcd.print((char)223);
      lcd.print("C outdoors");
      break;  
    }
    
  case btnDOWN:                           //kellonaika
    {
      lcd.clear();
      if(tunnit < 10) 
        {
          lcd.print("0");
          lcd.print(tunnit);
        }
      else 
        {
          lcd.print(tunnit);
        }
      lcd.print(":");
      if(minuutit < 10) 
        {
          lcd.print("0");
          lcd.print(minuutit);
        }
      else 
        {
          lcd.print(minuutit);
        }
      break;
    }
    
  case btnSELECT:                         //minuuttien säätö kellossa
    {
      minuutit = minuutit + 1;
      lcd.clear();
      if(tunnit < 10) 
        {
          lcd.print("0");
          lcd.print(tunnit);
        }
      else 
        {
          lcd.print(tunnit);
        }
      lcd.print(":");
      if(minuutit < 10) 
        {
          lcd.print("0");
          lcd.print(minuutit);
        }
      else 
        {
          lcd.print(minuutit);
        }
      break;
    }
    
  case btnRIGHT:                            //tuntien säätö kellossa
    {
      tunnit = tunnit + 1;
      lcd.clear();
      if(tunnit < 10) 
        {
          lcd.print("0");
          lcd.print(tunnit);
        }
      else 
        {
          lcd.print(tunnit);
        }
      lcd.print(":");
      if(minuutit < 10) 
        {
          lcd.print("0");
          lcd.print(minuutit);
        }
      else 
        {
          lcd.print(minuutit);
        }
      break;
    }
  }
}
