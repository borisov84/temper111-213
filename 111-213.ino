//#include <OneWire.h>
#include <DallasTemperature.h>
#include <TM74HC595Display.h>
#include <TimerOne.h>

#define buzz 9
#define but 2
#define maxtemp 29
#define delta 2

OneWire oneWire(4); //зеленый на колодке 5 - 111 к
OneWire oneWire1(8); //синий, на колодке 2 - 213 к
DallasTemperature ds(&oneWire);
DallasTemperature ds1(&oneWire1);

DeviceAddress sensor1 = {0x28, 0xFF, 0x26, 0xD2, 0x94, 0x16, 0x05, 0xE5};//28 FF D9 98 A1 16 04 62 //111
DeviceAddress sensor2 = {0x28, 0xFF, 0x7B, 0xFB, 0xA1, 0x16, 0x03, 0x49};//{0x28, 0xFF, 0x0B, 0x40, 0xA0, 0x16, 0x03, 0x8A}; //213

int SCLK = 7;
int RCLK = 6;
int DIO = 5;
int c = 0;
int hot = 0;
int old_hot = 0;

volatile boolean gsilence = false; // молчат оба датчика
boolean silence = false;
boolean alm1 = false;
boolean alm2 = false;


TM74HC595Display disp(SCLK, RCLK, DIO);
unsigned char LED_0F[29];

void setup() {
  Serial.begin(9600);
  ds.begin();
  ds1.begin();

  Timer1.initialize(1500); // set a timer of length 1500
  Timer1.attachInterrupt(timerIsr); // attach the service routine here

  pinMode(buzz, OUTPUT); //пищалка
  pinMode(but, INPUT_PULLUP); //кнопка
  attachInterrupt(0, mute, FALLING); // прерывание на пине 2
}

void loop() {
  ds.requestTemperatures(); // считываем температуру с датчиков
  ds1.requestTemperatures(); // считываем температуру с датчиков

  /*Serial.print("Sensor 1: ");
  Serial.print(ds.getTempC(sensor1)); // отправляем температуру
  Serial.println("C");


  Serial.print("Sensor 2: ");
  Serial.print(ds1.getTempC(sensor2)); // отправляем температуру
  Serial.println("C");

  Serial.print("gsilence ");
  Serial.println(gsilence);
  Serial.print("Silence ");
  Serial.println(silence);*/

  disp.digit4(111, 0);
  // Если температура выше или равна максимальной
  if (ds.getTempC(sensor1) >= maxtemp)
  {
    if ((alm2 == true) && (gsilence == true) && (alm1 == false))
    {
      gsilence = false;
      alm1 = true;
      alm(true);
    }
    else
    {
      alm1 = true;
      alm(true);
    }
  } // если температура упала ниже максимальной температуры минус дельта
  else if (ds.getTempC(sensor1) <= maxtemp - delta)
  {
    alm1 = false;
  }

  delay(500);
  disp.clear();
  delay(200);
  disp.dispFloat(ds.getTempC(sensor1), 1);
  delay(1500);
  disp.clear();
  delay(200);


  disp.dispFloat(213, 0);
  // Если температура выше или равна максимальной
  if (ds1.getTempC(sensor2) >= maxtemp)
  {
    if ((alm1 == true) && (gsilence == true) && (alm2 == false))
    {
      gsilence = false;
      alm2 = true;
      alm(true);
    }
    else
    {
      alm2 = true;
      alm(true);
    }
  }
  else if (ds1.getTempC(sensor2) <= maxtemp - delta)
  {
    alm2 = false;
  }
  delay(500);
  disp.clear();
  delay(200);
  disp.dispFloat(ds1.getTempC(sensor2), 1);
  delay(1500);
  disp.clear();
  delay(200);

  if ((alm1 == false) && (alm2 == false)) //если оба остыли, то сбросить пищалку
  {
    gsilence = false;
  }
}


void timerIsr()
{
  disp.timerIsr();
}

void alm(boolean act)
{
  if (gsilence == false)
  {
    if (act == false)
    {
      digitalWrite(buzz, LOW);

    }
    if (act == true)
    {
      digitalWrite(buzz, HIGH);
      delay(500);
      digitalWrite(buzz, LOW);
      delay(500);
      digitalWrite(buzz, HIGH);
      delay(500);
      digitalWrite(buzz, LOW);
    }
  }


  else
  {
    digitalWrite(buzz, LOW);
  }
}
void mute()
{
  gsilence = true;
  alm(false);
  Serial.println("Interrupt");
}
