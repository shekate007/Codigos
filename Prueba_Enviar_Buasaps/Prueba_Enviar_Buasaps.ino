#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <WiFi.h>
#include <HTTPClient.h>

int relay = 13; // Activación relay
int buzz = 12; // Activación de buzzer
int huella = 14; // Activación huella
int FDC = 27; // Activacion sensor magnetico
int IntentosIncorrectos = 0;
bool tecladoBloqueado = false;

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
String token = "Bearer EAADGJZAYdRZBsBO6uvAoKqHRbZCc9yXF7x9uygnucDZAQFv55GbIsrlXVAyYs8AvGCpW8qjuRFRABZCZBWIEI0rGqGnh9E4FIEuVnVLGzMF6GC4ZAJikuT7dsnSNuXKpGmRL3aFG0dM10naTBkLjmauHZB2IWL7cbRBy3fXxJ7DDFehtTe2E2Mz7ddw9E0QjXRYKx8ZCL3ZAtsZBOf6RZBua ";
String servidor = "https://graph.facebook.com/v17.0/184761381381361/messages";
String payload = "{ \"messaging_product\": \"whatsapp\", \"to\": \"523333979218\", \"type\": \"template\", \"template\": { \"name\": \"teclado\", \"language\": { \"code\": \"es_MX\" } } }";
String payload1 = "{ \"messaging_product\": \"whatsapp\", \"to\": \"523333979218\", \"type\": \"template\", \"template\": { \"name\": \"huella\", \"language\": { \"code\": \"es_MX\" } } }";
String payload2 = "{ \"messaging_product\": \"whatsapp\", \"to\": \"523333979218\", \"type\": \"template\", \"template\": { \"name\": \"app\", \"language\": { \"code\": \"es_MX\" } } }";
String payload3 = "{ \"messaging_product\": \"whatsapp\", \"to\": \"523333979218\", \"type\": \"template\", \"template\": { \"name\": \"bloqueo\", \"language\": { \"code\": \"es_MX\" } } }";
String payload4 = "{ \"messaging_product\": \"whatsapp\", \"to\": \"523333979218\", \"type\": \"template\", \"template\": { \"name\": \"cerrado\", \"language\": { \"code\": \"es_MX\" } } }";

void setup() {
  pinMode(12, OUTPUT);  
  pinMode(13, OUTPUT);
  pinMode(14, INPUT);
  pinMode(27, INPUT);
  
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
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ingresa Pin o");
  lcd.setCursor(0, 1);
  lcd.print("Coloca tu dedo:");
  lcd.setCursor(0, 1);
  Serial.println("");
  Serial.print("Se ha conectado al WiFi con la IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Verificación del pin 14
  if (digitalRead(huella) == HIGH) {
    if (!tecladoBloqueado) {
      lcd.clear();
      lcd.print("Huella Correcta");
      digitalWrite(relay, HIGH);  // Activa el relay
      delay(2000);
      
      HTTPClient http;
      http.begin(servidor.c_str());
      http.addHeader("Content-Type", "application/json"); 
      http.addHeader("Authorization", token);
      int httpPostCode = http.POST(payload1);
      
      if (httpPostCode > 0) {
        int httpResponseCode = http.GET();
        if (httpResponseCode > 0) {
          Serial.print("HTTP Response code: ");
          Serial.println(httpResponseCode);
          String payload = http.getString();
          Serial.println(payload1);
        } else {
          Serial.print("Error code: ");
          Serial.println(httpResponseCode);
        }
      }
      http.end();
    
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Ingresa Pin o");
      lcd.setCursor(0, 1);
      lcd.print("Coloca tu dedo:");
      lcd.setCursor(0, 1);
    } else {
      lcd.clear();
      lcd.print("Locker");
      lcd.setCursor(0, 1);
      lcd.print("Desbloqueado");
      delay(2000);
      tecladoBloqueado = false;
      IntentosIncorrectos = 0;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Ingresa Pin o");
      lcd.setCursor(0, 1);
      lcd.print("Coloca tu dedo:");
      lcd.setCursor(0, 1);
    }
  } else {
    digitalWrite(relay, LOW);  // Desactiva el relay
    
    if (!tecladoBloqueado) {
      char key = keypad.getKey();
  
      if (key) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Ingresando Pin:");
        lcd.setCursor(0, 1);
        if (key == '*') {
          // Si se presiona "*", actúa como "enter"
          if (passwordInput == "CBD") {
            lcd.clear();
            lcd.print("Pin correcto");
            digitalWrite(relay, HIGH);  // Activa el relay
            delay(3000);
      
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
            digitalWrite(relay, LOW);  // Desactiva el relay
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Ingresa Pin o");
            lcd.setCursor(0, 1);
            lcd.print("Coloca tu dedo:");
            lcd.setCursor(0, 1);
          } else {
            lcd.clear();
            lcd.print("Pin incorrecto");
            digitalWrite(buzz, HIGH);  // Activa el buzzer
            delay(2000);
            digitalWrite(buzz, LOW);   // Desactiva el buzzer
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Intento ");
            lcd.print(IntentosIncorrectos + 1);
            delay(2000);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Ingresa Pin o");
            lcd.setCursor(0, 1);
            lcd.print("Coloca tu dedo:");
            lcd.setCursor(0, 1);
            passwordInput = "";
            IntentosIncorrectos++;
            if (IntentosIncorrectos >= 3){
              tecladoBloqueado = true;
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Locker");
              lcd.setCursor(0, 1);
              lcd.print("Bloqueado");
              delay(2000);
               HTTPClient http;
      http.begin(servidor.c_str());
      http.addHeader("Content-Type", "application/json"); 
      http.addHeader("Authorization", token);
      int httpPostCode = http.POST(payload3);
      
      if (httpPostCode > 0) {
        int httpResponseCode = http.GET();
        if (httpResponseCode > 0) {
          Serial.print("HTTP Response code: ");
          Serial.println(httpResponseCode);
          String payload = http.getString();
          Serial.println(payload3);
        } else {
          Serial.print("Error code: ");
          Serial.println(httpResponseCode);
        }
      }
      http.end();
               lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Ingresa");
              lcd.setCursor(0, 1);
              lcd.print("Huella");
            }
          }
        } else if (key == 'A') {
          // Verificar estado del sensor magnético (FDC)
          int estadoFDC = digitalRead(FDC);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Estado = ");
         // lcd.print((estadoFDC == HIGH) ? "HIGH" : "LOW");

          if (estadoFDC == HIGH) {
            lcd.setCursor(0, 1);
            lcd.print("Abierto");
          } else {
            lcd.setCursor(0, 1);
            lcd.print("Cerrado");
            delay(2000);
            
             HTTPClient http;
      http.begin(servidor.c_str());
      http.addHeader("Content-Type", "application/json"); 
      http.addHeader("Authorization", token);
      int httpPostCode = http.POST(payload4);
      
      if (httpPostCode > 0) {
        int httpResponseCode = http.GET();
        if (httpResponseCode > 0) {
          Serial.print("HTTP Response code: ");
          Serial.println(httpResponseCode);
          String payload = http.getString();
          Serial.println(payload4);
        } else {
          Serial.print("Error code: ");
          Serial.println(httpResponseCode);
        }
      }
      http.end();
          }

          delay(2000);  // Mostrar el estado durante 2 segundos
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Ingresa Pin o");
          lcd.setCursor(0, 1);
          lcd.print("Coloca tu dedo:");
          lcd.setCursor(0, 1);
        } else {
          lcd.print('*'); // Mostrar un asterisco en lugar de la tecla presionada
          passwordInput += key;
        }
      }
    } else { // Teclado bloqueado
      char key = keypad.getKey();
      if (key && digitalRead(huella) == HIGH) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Locker ");
        lcd.setCursor(0, 1);
        lcd.print("Desbloqueado ");
        delay(3000);
        tecladoBloqueado = false;
        IntentosIncorrectos = 0;
        ESP.restart();  // Reiniciar la ESP32
      }
    }
  }

  delay(100);
}
