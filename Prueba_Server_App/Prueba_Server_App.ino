#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const char* ssid = "Jorge";
const char* password = "12345678";
WiFiServer server(80);

// Configuración del display LCD I2C
LiquidCrystal_I2C lcd(0x27, 16, 2); // Dirección I2C 0x27, 16 columnas y 2 filas

void setup() {
  // Inicializar el display LCD I2C
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.init();
  lcd.print("Bienvenido");
  delay(2000);  // Espera 2 segundos para mostrar el mensaje de bienvenida

  lcd.clear();
  lcd.print("Conectando...");

  // Conectar a la red WiFi
  Serial.print("Conectando a ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    lcd.print(".");
    Serial.print(".");
  }

  lcd.clear();
  lcd.print("WiFi conectado");
  lcd.setCursor(0, 1);
  lcd.print("IP: " + WiFi.localIP().toString());

  // Inicializar el servidor
  server.begin(80);
  Serial.println("Servidor iniciado");

  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());

  lcd.clear();
  lcd.print("Esperando señal...");

  Serial.println("Listo para recibir señales");
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("Nuevo cliente conectado");

    while (client.connected()) {
      if (client.available()) {
        String request = client.readStringUntil('\r');
        Serial.println("Petición: " + request);

        if (request.indexOf("open") != -1) {
          lcd.clear();
          lcd.print("Abierto desde App");
          Serial.println("Acción: Abierto desde App");
          // Agrega aquí el código para la acción de abrir
        } else if (request.indexOf("close") != -1) {
          lcd.clear();
          lcd.print("Cerrado desde App");
          Serial.println("Acción: Cerrado desde App");
          // Agrega aquí el código para la acción de cerrar
        }

        client.println("HTTP/1.1 200 OK");
        client.println("Content-type:text/html");
        client.println();
        client.println("<html><body><h1>Comando recibido</h1></body></html>");
        delay(1);
        client.stop();
        Serial.println("Cliente desconectado");

        lcd.setCursor(0, 1);
        lcd.print("Esperando señal...");
      }
    }
  }
}
