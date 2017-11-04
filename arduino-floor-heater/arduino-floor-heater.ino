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
 * D3 - LED sygnalizujący załączenie przekaźnika
 * D4 - 
 * D5 - 
 * D6 - 
 * D7 - 
 * D8 - sterowanie zewnętrznym przekaźnikiem, załączenie poprzez stan niski
 * D9 - 
 * D10 - 
 * D11 - 
 * D12 - 
 * D13 - 
 */

const int PinCzujnikaNTC = 1; // pin analogowy A1
const int LEDPrzekaznika = 3; // pin cyfrowy D3
const int PinPrzekaznika = 8; // pin cyfrowy D8

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

void PrzekaznikInicjalizuj()
/*
 * funkcja inicjalizuje obsługę zewnętrznego przekaźnika oraz diody sygnalizującej jego pracę poprzez zdefiniowanie parametrów wyjść cyfrowych oraz ustawienie odpowiednich stanów na tych wyjściach
 * przekaźnik jest załączany stanem niskim
 */
{
  pinMode(PinPrzekaznika, OUTPUT);
  pinMode(LEDPrzekaznika, OUTPUT);

  digitalWrite(PinPrzekaznika, HIGH); // przekaźnik wyłączony
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
  PrzekaznikInicjalizuj();
}

void loop() {
  Serial.println(temperaturaNTC(PinCzujnikaNTC));
  delay(1000);  // opóźnienie pętli głównej na potrzeby diagnostyczne
}
