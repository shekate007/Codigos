#include <WiFi.h>
#include <WiFiServer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
WiFiServer server(80);

const char* ssid = "Jorge";          // Cambiar por el nombre de la red WiFi
const char* password = "12345678";       // Cambiar por la contraseña de la red WiFi
const char *host_name = "locket";

const int pinEntrada = 14;
const int pinActivacion = 25;
const int pinDireccion1 = 26;
const int pinDireccion2 = 27;
const int velocidadMotor = 100;
const int tiempoGiro = 20;
const int ledPin = 13; // Debes ajustar este valor según la configuración de tu LED

int timeout = 10;  // segundos
unsigned long lastDataTime = 0;

void WiFiConnect(const char *ssid, const char *password, const char *hostName) {
  WiFi.begin(ssid, password);
  WiFi.setHostname(hostName);

  Serial.print("Connecting to WiFi ...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nConnection successful: " + String(WiFi.getHostname()));
  Serial.println(WiFi.localIP());
}

void HandleRequest(WiFiClient client, String request, String signalValue) {
  String response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + signalValue;
  client.print(response);
  Serial.println("Response sent.");

  delay(1000);

  if (signalValue.indexOf("open") != -1) {
   // Abrir("Abierto desde la App");
  }
}

void ActivarMotor(int direccion1, int direccion2, int velocidad, int tiempo) {
  digitalWrite(pinDireccion1, direccion1);
  digitalWrite(pinDireccion2, direccion2);
  digitalWrite(pinActivacion, HIGH);
  delay(velocidad);
  digitalWrite(pinActivacion, LOW);
  delay(tiempo);

  // Apaga el motor y regresa a la posición original
  digitalWrite(pinDireccion1, !direccion1);
  digitalWrite(pinDireccion2, !direccion2);
  digitalWrite(pinActivacion, HIGH);
  delay(velocidad);
  digitalWrite(pinActivacion, LOW);
}

void OnPinChange() {
  if (digitalRead(pinEntrada) == HIGH) {
    lcd.clear();
    digitalWrite(ledPin, LOW);
    lcd.print("huella correcta");
    delay(10000);
    lcd.clear();
    lcd.print("Ingresa pin....:");
    ActivarMotor(1, 0, velocidadMotor, tiempoGiro);
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, HIGH);
  }
}

void setup() {
  Serial.begin(115200);
  WiFiConnect("Jorge", "12345678", "locket");
    Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.init();
  
  lcd.print("Bienvenido!");
  delay(2000);
  lcd.clear();
  
  lcd.print("Conectando WiFi...");
  WiFi.begin(ssid, password);
  
  while(WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  
  lcd.clear();
  lcd.print("WiFi conectado!");
  lcd.clear();

  pinMode(pinEntrada, INPUT);
  pinMode(pinActivacion, OUTPUT);
  pinMode(pinDireccion1, OUTPUT);
  pinMode(pinDireccion2, OUTPUT);
  pinMode(ledPin, OUTPUT);

  lcd.begin(16, 2);
  lcd.print("Bienvenido Locker");
  delay(2000);
  lcd.clear();
  lcd.print("Ingresa pin....:");
  digitalWrite(ledPin, HIGH);

  attachInterrupt(digitalPinToInterrupt(pinEntrada), OnPinChange, CHANGE);

  server.begin();
  Serial.println("Waiting for connection...");
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    Serial.println("New client connected");
    lastDataTime = millis();

    String request = client.readString();
    Serial.println(request);

    String signalValue = "";

    int signalIndex = request.indexOf("signal=");
    if (signalIndex != -1) {
      signalValue = request.substring(signalIndex + 7, signalIndex + 13);
      Serial.println("Signal value: " + signalValue);
    }

    HandleRequest(client, request, signalValue);

    client.stop();
  }

  if (millis() - lastDataTime > timeout * 1000) {
    Serial.println("No data received in 10 seconds. Restarting the program.");
    ESP.restart();
  }
}
