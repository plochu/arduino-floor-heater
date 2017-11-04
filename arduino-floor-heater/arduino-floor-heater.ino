/*
 * sterownik elektrycznego ogrzewania podłogowego oparty o arduino UNO
 * https://github.com/plochu/arduino-floor-heater
 *
 * inwentaryzacja wejść / wyjść Arduino UNO R3
 * A0 - potencjometr nastawczy
 * A1 - pomiar rezystancji czujnika NTC temperatury podłogi
 * A2 - 
 * A3 - 
 * A4 - wyświetlacz OLED I2C
 * A5 - wyświetlacz OLED I2C
 * D0 - 
 * D1 - 
 * D2 - przycisk 1
 * D3 - LED sygnalizujący załączenie przekaźnika (LED przycisku 1)
 * D4 - LED przycisku 2
 * D5 - przycisk 2
 * D6 - przycisk 3
 * D7 - LED przycisku 3
 * D8 - sterowanie zewnętrznym przekaźnikiem, załączenie poprzez stan niski
 * D9 - 
 * D10 - 
 * D11 - 
 * D12 - 
 * D13 - 
 */

#include "U8glib.h" // zaawansowana biblioteka do obsługi wyświetlaczy, Universal 8bit Graphics Library, https://github.com/olikraus/u8glib/

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);  // inicjalizacja sterownika wyświetlacza OLED 128x64px, 0,96 cala, monochromatyczny, ze sterownikiem SSD1306, działający na magistrali I2C/TWI

const int PinPotencjometru = 0; // pin analogowy A0
const int PinCzujnikaNTC = 1; // pin analogowy A1
const int PinPrzycisku1 = 2;  // pin cyfrowy D2
const int LEDPrzekaznika = 3; // pin cyfrowy D3
const int LEDPrzycisku2 = 4;  // pin cyfrowy D4
const int PinPrzycisku2 = 5;  // pin cyfrowy D5
const int PinPrzycisku3 = 6;  // pin cyfrowy D6
const int LEDPrzycisku3 = 7;  // pin cyfrowy D7
const int PinPrzekaznika = 8; // pin cyfrowy D8

void PrzyciskiInicjalizuj()
/*
 * funkcja inicjalizuje obsługiwane przyciski wykorzystując rezystowy podciągające wbudowane w układ Arduino
 * rozpoznawanie naciśnięcia przycisku realizowane jest przez podanie stanu niskiego na odpowiadające mu wejście cyfrowe
 */

{
  pinMode(PinPrzycisku1, INPUT_PULLUP);
  pinMode(PinPrzycisku2, INPUT_PULLUP);
  pinMode(PinPrzycisku3, INPUT_PULLUP);
}

void LEDyInicjalizuj()
/*
 * funkcja inicjalizuje odpowiednie piny jako wyjścia do sterowania diodami LED oraz ustawia ich stan na niski
 */
{
  pinMode(LEDPrzycisku2, OUTPUT);
  digitalWrite(LEDPrzycisku2, LOW);
  pinMode(LEDPrzycisku3, OUTPUT);
  digitalWrite(LEDPrzycisku3, LOW);
}


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

void PrzekaznikInicjalizuj()
/*
 * funkcja inicjalizuje obsługę zewnętrznego przekaźnika oraz diody sygnalizującej jego pracę poprzez zdefiniowanie parametrów wyjść cyfrowych oraz ustawienie odpowiednich stanów na tych wyjściach
 * przekaźnik jest załączany stanem niskim
 */
{
  pinMode(PinPrzekaznika, OUTPUT);
  digitalWrite(PinPrzekaznika, HIGH); // przekaźnik wyłączony
    
  pinMode(LEDPrzekaznika, OUTPUT);
  digitalWrite(LEDPrzekaznika, LOW);  // LED sygnalizujący wyłączony
}

void PrzekaznikZalacz()
/*
 * funkcja załączająca zewnętrzny przekaźnik oraz włączająca diodę LED sygnalizującą jego stan
 */
{
  digitalWrite(PinPrzekaznika, LOW);  // przekaźnik załączony
  digitalWrite(LEDPrzekaznika, HIGH); // LED sygnalizujący włączony
}

void PrzekaznikWylacz()
/*
 * funkcja wyłączająca zewnętrzny przekaźnik oraz wyłączająca diodę LED sygnalizującą jego stan
 */
{
  digitalWrite(PinPrzekaznika, HIGH); // przekaźnik wyłączony
  digitalWrite(LEDPrzekaznika, LOW);  // LED sygnalizujący wyłączony
}

void PrzekaznikStan()
/*
 * funkcja sprawdza stan przekaźnika
 * - w przypadku gdy jest załączony funkcja zwraca wartość TRUE
 * - w innym wypadku funkcja zwraca wartość FALSE
 */
{
  if (digitalRead(PinPrzekaznika)) {
    return true;
  }
  else {
    return false;
  }
}

void PrzekaznikPrzelacz()
/*
 * funkcja przełącza zewnętrzny przekaźnik oraz sygnalizacyjną diodę LED na stan przeciwny
 */
{
  digitalWrite(PinPrzekaznika, !digitalRead(PinPrzekaznika));
  digitalWrite(LEDPrzekaznika, !digitalRead(LEDPrzekaznika));
}

void setup() {
  
// inicjalizacja na potrzeby diagnostyczne
  Serial.begin(9600);
  
// wyłączenie wbudowanej diody LED (domyślnie włączona)  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

// inicjalizacja sterownika 
  LEDyInicjalizuj();
  PrzyciskiInicjalizuj();
  PrzekaznikInicjalizuj();
  OLEDInicjalizuj();
}

void loop() {
  Serial.println(temperaturaNTC(PinCzujnikaNTC));
  EkranWyswietl(0);
}
