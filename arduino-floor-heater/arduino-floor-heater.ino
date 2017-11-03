/*
 * sterownik elektrycznego ogrzewania podłogowego oparty o arduino UNO
 * https://github.com/plochu/arduino-floor-heater
 *
 * inwentaryzacja wejść / wyjść Arduino UNO R3
 * A0 - 
 * A1 - pomiar rezystancji czujnika NTC temperatury podłogi
 * A2 - 
 * A3 - 
 * A4 - 
 * A5 - 
 * D0 - 
 * D1 - 
 * D2 - 
 * D3 - 
 * D4 - 
 * D5 - 
 * D6 - 
 * D7 - 
 * D8 - 
 * D9 - 
 * D10 - 
 * D11 - 
 * D12 - 
 * D13 - 
 */

float temperaturaNTC(int pinNTC)
/*
 * funkcja zwraca wartość temperatury w stopniach celcjusza wyliczonej na podstawie pomiaru rezystancji czujnika NTC
 * pomiar rezystancji odbywa się za pomocą prostego układu dzialnika napięcia wzorcowego z rezystorem wzorcowym o znanej oporności
 *
 * do obliczenia temperatury na podstawie rezystancji NTC wykorzystany jest wzór:
 * temperatura = ( ( ( ( 1 / ( ( log(rezystancjaNTC / 15500) / 3625) + (1 / 298.15 ) ) ) - 273.15) ) )
 */
{
  int odczytNTC = 0;
  int WejscioweU = 5; // wartość napięcia wzorcowego
  float WyjscioweU = 0;
  float RezystancjaWzorca = 4700; // wartość rezystora wzorcowego
  float RezystancjaNTC = 0;
  float pomocnicza = 0;

  odczytNTC = analogRead(pinNTC);
  if (odczytNTC)
  {
    pomocnicza = odczytNTC * WejscioweU;
    WyjscioweU = pomocnicza / 1024;
    pomocnicza = (WejscioweU / WyjscioweU) -1;
    RezystancjaNTC = RezystancjaWzorca * pomocnicza;
  }

  return ( ( ( ( 1 / ( ( log(RezystancjaNTC / 15500) / 3625) + (1 / 298.15 ) ) ) - 273.15) ) );
}

void setup() {
}

void loop() {
}
