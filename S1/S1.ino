#include <WiFi.h>
#include "env.h"
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <DHT.h>
#define DHTPIN 4
#define DHTTYPE DHT11

WiFiClientSecure wificlient;
PubSubClient mqtt(wificlient);

DHT dht(DHTPIN, DHTTYPE);

const int LDR = 34;
const int LEDR = 14;
const int LEDG = 26;
const int LEDB = 25;
const int LED = 19;
const int ULTRA_ECHO = 23;
const int ULTRA_TRIG = 22;

void setup() {
  pinMode(LDR, INPUT);
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(ULTRA_ECHO, INPUT);
  pinMode(ULTRA_TRIG, OUTPUT);
  ledcAttach(LEDR, 5000, 8);
  ledcAttach(LEDG, 5000, 8);
  ledcAttach(LEDB, 5000, 8);
  dht.begin();

  wificlient.setInsecure();
  Serial.begin(115200);
  WiFi.begin(SSID, PASS);
  Serial.println("Conectando ao Wifi");
  statusLED(1);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("\n.");
    delay(200);
  }
  statusLED(2);
  Serial.println("\nWifi conectado");
  mqtt.setServer(BROKER_URL, BROKER_PORT);
  String boardID = "SyncRail-S1-";
  boardID += String(random(0xffff), HEX);
  Serial.println("Conectando no Broker");
  statusLED(3);
  while (!mqtt.connect(boardID.c_str(), BROKER_USR_NAME, BROKER_USR_PASS)) {
    Serial.print("\n.");
    delay(200);
  }
  Serial.println("\nConectado com sucesso ao broker!");
  statusLED(4);
  mqtt.subscribe(TOPIC_ILUMINACAO);
  mqtt.setCallback(callback);
}

long lerDistancia() {
  digitalWrite(ULTRA_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRA_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRA_TRIG, LOW);

  long duracao = pulseIn(ULTRA_ECHO, HIGH);
  long distancia = duracao * 349.24 / 2 / 10000;

  return distancia;
}

void statusLED(byte status) {
  turnOffLEDs();
  switch (status) {
    case 1:
      //(Amarelo Piscando)
      for (byte i = 0; i < 4; i++) {
        setLEDColor(150, 255, 0);
        delay(100);
        turnOffLEDs();
        delay(100);
      }
      break;
    case 2:
      //(Amarelo)
      setLEDColor(150, 180, 0);
      break;
    case 3:
      //(Azul piscando)
      for (byte i = 0; i < 4; i++) {
        setLEDColor(0, 0, 255);
        delay(100);
        turnOffLEDs();
        delay(100);
      }
      break;
    case 4:
      //(Azul)
      setLEDColor(0, 0, 255);
      break;
    case 5:
      setLEDColor(255, 255, 255);
      break;
    default:
      for (byte i = 0; i < 4; i++) {
        setLEDColor(0, 255, 255);
        delay(100);
        turnOffLEDs();
        delay(100);
      }
      turnOffLEDs();
      break;
  }
}

void turnOffLEDs() {
  setLEDColor(0, 0, 0);
}

void setLEDColor(byte r, byte g, byte b) {
  ledcWrite(LEDR, r);
  ledcWrite(LEDG, g);
  ledcWrite(LEDB, b);
}

void loop() {
  float umidade = dht.readHumidity();
  float temperatura = dht.readTemperature();

  if (isnan(umidade) || isnan(temperatura)) {
    Serial.println("Erro na leitura do DHT11");
    return;
  }
  mqtt.publish(TOPIC_UMIDADE, String(umidade).c_str());
  mqtt.publish(TOPIC_TEMPERATURA, String(temperatura).c_str());

  long distancia = lerDistancia();
  if (distancia < 5) {
    mqtt.publish(TOPIC_PRESENCA, "1");
  } else {
    mqtt.publish(TOPIC_PRESENCA, "0");
  }
  int LeituraLDR = analogRead(LDR);
  if (LeituraLDR > 2700) {
    mqtt.publish(TOPIC_ILUMINACAO, "Acender");
  } else {
    mqtt.publish(TOPIC_ILUMINACAO, "Apagar");
  }
  delay(2800);
  statusLED(5);
  delay(200);
  mqtt.loop();
  turnOffLEDs();
}

void callback(char* topic, byte* payload, unsigned long length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  if (topic == "SyncRail/S1/Iluminacao") {
    if (message == "Acender") {
      digitalWrite(LED, HIGH);
      Serial.println(1);
      }
    else if (message == "Apagar") {
      digitalWrite(LED, LOW);
      Serial.println(0);
    }
  }
}
