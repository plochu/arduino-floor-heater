/*
 * sterownik elektrycznego ogrzewania podłogowego oparty o arduino UNO
 * https://github.com/plochu/arduino-floor-heater
 *
 * inwentaryzacja wejść / wyjść Arduino UNO R3
 * A0 - 
 * A1 - pomiar rezystancji czujnika NTC temperatury podłogi
 * A2 - 
 * A3 - 
 * A4 - wyświetlacz OLED I2C
 * A5 - wyświetlacz OLED I2C
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

#include "U8glib.h" // zaawansowana biblioteka do obsługi wyświetlaczy, Universal 8bit Graphics Library, https://github.com/olikraus/u8glib/

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);  // inicjalizacja sterownika wyświetlacza OLED 128x64px, 0,96 cala, monochromatyczny, ze sterownikiem SSD1306, działający na magistrali I2C/TWI

const int PinCzujnikaNTC = 1;

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

void OLEDInicjalizuj()
/*
 * funkcja inicjalizująca sterownik wyświetlacza wg. zadanych parametrów
 */
{
  u8g.setRot180();  // obrót obrazu ze względu na specyfikę montażu wyświetlacza
}

void EkranWyswietl(int Tryb)
/*
 * funkcja wykonuje pętlę obrazu konieczną do prawidłowego wyświetlenia zawartości ekranu
 * w zależności od podanego przy wywołaniu funkcji argumentu wyświetlany jest różny zestaw paneli informacyjnych
 * jeżeli zostanie podany nierozpoznany argument to wyświetli się zestaw domyślny
 */
{
  u8g.firstPage();  
  do {
    switch (Tryb) {
      default: {
        EkranPanelTemperaturaNTC(temperaturaNTC(PinCzujnikaNTC));
        break;
      }
    }
  } while( u8g.nextPage() );  
}

void EkranPanelTemperaturaNTC(float Temp)
/*
 * panel z temperaturą z czujnika NTC
 * jako argument funkcji wprowadzana jest aktualna wartość temperatury do wyświetlenia
 * obsługiwany jest zakres wartości temperatury 0-99, poza tym zakresem wyświetlany jest komunikat "--"
 */
{
// ustawienie dużej czcionki (zawiera tylko cyfry)
  u8g.setFont(u8g_font_fur49n);

  if ( (Temp >=0 ) && (Temp <= 99) ) {
    // w zależności od tego czy wyświetlana wartość jest liczbą jedno czy dwucyfrową będzie wyświetlana w różnych miejscach
    if (Temp < 9.50) {
      u8g.setPrintPos(38,57); // pozycja liczby jednocyfrowej
      }
    else {
      u8g.setPrintPos(0,57);  // pozycja liczby dwucyfrowej
    }
  
    // wyświetlenie liczby zaokrąglonej do najbliższej liczby całkowitej
    u8g.print(Temp,0);
    }
   else {
    u8g.setPrintPos(28,57);
    u8g.print("--");
   }

// narysowanie symbolu stopni
    u8g.drawCircle(83, 14, 6);
    u8g.drawCircle(83, 14, 5);
    u8g.drawCircle(83, 14, 4);
}

void setup() {
  Serial.begin(9600); // inicjalizacja na potrzeby diagnostyczne
  OLEDInicjalizuj();
}

void loop() {
  Serial.println(temperaturaNTC(PinCzujnikaNTC));
  EkranWyswietl(0);
}
