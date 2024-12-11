#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

#define LED_1 12
#define LED_2 13
#define LED_3 14
#define LED_4 26
#define LED_5 27
#define DHTPIN 21
#define BTN1 4
#define BTN2 5

void PrenderLeds(float temperature);
void CambiarTemp();
void handleRoot();
void ApagarLEDS();

DHT dht(DHTPIN, DHT11);

float temp_minima = 5.0;
int cant_Leds = 0;

WebServer server(80); // Puerto 80 HTTP

void setup() {
  Serial.begin(115200);

  WiFi.begin("IoTB", "inventaronelVAR");
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Conectando...");
  }
  Serial.println("\nConnected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");

  dht.begin();

  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  pinMode(LED_4, OUTPUT);
  pinMode(LED_5, OUTPUT);
  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);
}

void loop() {
  float temperature = dht.readTemperature();
  if (isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  CambiarTemp();

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print("°C, Sensitivity: ");
  Serial.print(temp_minima);
  Serial.println("°C per LED");

  ApagarLEDS();
  if (temperature < temp_minima){
    PrenderLeds(false, false, false, false, false);
    cant_Leds = 0;
  }

  if (temperature >= temp_minima){
    PrenderLeds(true, false, false, false, false);
    cant_Leds = 1;
  }
  if (temperature >= temp_minima + 5){
    PrenderLeds(true, true, false, false, false);
    cant_Leds = 2;
  }
  if (temperature >= temp_minima + 10){
    PrenderLeds(true, true, true, false, false);
    cant_Leds = 3;
  }
  if (temperature >= temp_minima + 15){
    PrenderLeds(true, true, true, true, false);
    cant_Leds = 4;
  }
  if (temperature >= temp_minima + 20){
    PrenderLeds(true, true, true, true, true);
    cant_Leds = 5;
  }

  server.handleClient();

  delay(1000);
}
void PrenderLeds(bool LED1, bool LED2, bool LED3, bool LED4, bool LED5)
{
  digitalWrite(LED_1, LED1);
  digitalWrite(LED_2, LED2);
  digitalWrite(LED_3, LED3);
  digitalWrite(LED_4, LED4);
  digitalWrite(LED_5, LED5);
}

void CambiarTemp() {
  if (digitalRead(BTN1) == LOW) {
    temp_minima += 1.0;
    delay(250);
  }
  if (digitalRead(BTN2) == LOW) {
    temp_minima -= 1.0;
    if (temp_minima < 1.0) temp_minima = 1.0;
    delay(250);
  }
}

void ApagarLEDS(){
    digitalWrite(LED_1, LOW);
    digitalWrite(LED_2, LOW);
    digitalWrite(LED_3, LOW);
    digitalWrite(LED_4, LOW);
    digitalWrite(LED_5, LOW);
}

void handleRoot() {
  float temperature = dht.readTemperature();
  if (isnan(temperature)) {
    server.send(200, "text/plain", "Failed to read from DHT sensor!");
    return;
  }

  String html = "<!DOCTYPE html><html><head><title>Temperature Sensor</title></head><body>";
  html += "<style>.led { width: 20px; height: 20px; border: 2px solid black; border-radius: 50%; display: inline-block; margin: 5px; }</style>";
  html += "<h1>Temperature Sensor Data</h1>";
  html += "<p>Temperature: " + String(temperature) + " °C</p>";
  html += "<p>Sensitivity: " + String(temp_minima) + " °C per LED</p>";

  html += "<div>";
  for (int i = 0; i < 5; i++){
    
    if (i < cant_Leds){
      html += "<div class='led' style='background-color: green;'></div>";
    }

    else {
      html += "<div class='led' style='background-color: grey;'></div>";
    }
  }
  html += "</div>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}