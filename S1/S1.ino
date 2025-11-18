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
  dht.begin();
  wificlient.setInsecure();
  Serial.begin(115200);    //configura a placa pra mostrar na tela
  WiFi.begin(SSID, PASS);  //tenta conectar na rede
  Serial.println("Conectando ao Wifi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  Serial.println("Conectando no Broker");
  Serial.println("\nConectado com sucesso ao broker!");
  mqtt.setServer(BROKER_URL, BROKER_PORT);
  String boardID = "SyncRail-S1-";
  boardID += String(random(0xffff), HEX);
                                 
  while (!mqtt.connect(boardID.c_str(),BROKER_USR_NAME,BROKER_USR_PASS)) {
    Serial.print(".");
    delay(200);
  }
  mqtt.subscribe(TOPIC_ILUMINACAO);   // RECEBER informações do tópico
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

void loop() {
  String mensagem = "";
  float umidade = dht.readHumidity();
  float temperatura = dht.readTemperature();
  
  if (isnan(umidade) || isnan(temperatura)) {
    Serial.println("Erro na leitura do DHT11");
    return;
  }
  mqtt.publish(TOPIC_UMIDADE,String(umidade).c_str());

  long distancia = lerDistancia();
  if (distancia < 5) {
    mqtt.publish(TOPIC_PRESENCA, "1");
  } else {
    mqtt.publish(TOPIC_PRESENCA, "0");
  }
  delay(7000);
  mqtt.loop();
}

void callback(char* topic, byte* payload, unsigned long length) {
}