#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <WiFi.h>
#include <HTTPClient.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // Cambia la dirección 0x27 según la dirección de tu LCD
const byte ROWS = 4;
const byte COLS = 4;
byte rowPins[ROWS] = {19, 18, 5, 17};
byte colPins[COLS] = {16, 4, 2, 15};
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

String passwordInput = "";

const char* ssid = "Jorge";          // Cambiar por el nombre de la red WiFi
const char* password = "12345678";       // Cambiar por la contraseña de la red WiFi
String token = "Bearer Agregar_aquí_el_token ";
String servidor = "https://graph.facebook.com/v17.0/184761381381361/messages";
String payload = "{ \"messaging_product\": \"whatsapp\", \"to\": \"523333979218\", \"type\": \"template\", \"template\": { \"name\": \"teclado\", \"language\": { \"code\": \"es_MX\" } } }";

void setup() {
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
  lcd.setCursor(0, 0);
  lcd.print("Ingresa Pin:");
  lcd.setCursor(0, 1);
  Serial.println("");
  Serial.print("Se ha conectado al WiFi con la IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  char key = keypad.getKey();
  
  if (key) {
    if (key == '*') {
      // Si se presiona "*", actúa como "enter"
      if (passwordInput == "1214") {
        lcd.clear();
        lcd.print("Pin correcto");
        delay(2000);
        lcd.clear();

        HTTPClient http;
        http.begin(servidor.c_str());
        http.addHeader("Content-Type", "application/json"); 
        http.addHeader("Authorization", token);
        int httpPostCode = http.POST(payload);
        
        if (httpPostCode > 0) {
          int httpResponseCode = http.GET();
          if (httpResponseCode > 0) {
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
            String payload = http.getString();
            Serial.println(payload);
          } else {
            Serial.print("Error code: ");
            Serial.println(httpResponseCode);
          }
        }
        http.end();

        passwordInput = "";
        lcd.clear();
        lcd.print("Ingresa Pin:");
        lcd.setCursor(0, 1);
      } else {
        lcd.clear();
        lcd.print("Pin incorrecto");
        delay(2000);
        lcd.clear();
        lcd.print("Ingresa Pin:");
        lcd.setCursor(0, 1);
        passwordInput = "";
      }
    } else {
      lcd.print('*'); // Mostrar un asterisco en lugar de la tecla presionada
      passwordInput += key;
    }
  }

  delay(100);
}
