#include <WiFi.h>
#include "env.h"
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

WiFiClientSecure wificlient;
PubSubClient mqtt(wificlient);

const int LedVermelho = 27;
const int LedVerde = 26;

void setup() {
  pinMode(LedVermelho, OUTPUT);
  pinMode(LedVerde, OUTPUT);
  wificlient.setInsecure();
  Serial.begin(115200);    //configura a placa pra mostrar na tela
  WiFi.begin(SSID, PASS);  //tenta conectar na rede
  Serial.println("Conectando ao Wifi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  Serial.println("Conectado com Sucesso parceiro!");
  Serial.println("Conectando no Broker");
  mqtt.setServer(BROKER_URL, BROKER_PORT);
  String boardID = "S1-";
  boardID += String(random(0xffff), HEX);

  while (!mqtt.connect(boardID.c_str(),BROKER_USR_NAME,BROKER_USR_PASS)) {
    Serial.print(".");
    delay(200);
  }
  mqtt.subscribe(TOPIC_VELOCIDADE);         // RECEBER informações do tópico
  mqtt.setCallback(callback);
  Serial.println("\nConectado com sucesso ao broker!");
  pinMode(2, OUTPUT);
}

void loop() {
  mqtt.loop();
}
void Leds(int velocidade){
  if(velocidade > 0) {
    digitalWrite(LedVerde, HIGH);
    digitalWrite(LedVermelho, LOW);
  } else if(velocidade < 0){
    digitalWrite(LedVerde, LOW);
    digitalWrite(LedVermelho, HIGH);
  } else {
    digitalWrite(LedVerde, LOW);
    digitalWrite(LedVermelho, LOW);
  }
}
void callback(char* topic, byte* payload, unsigned long length) {
  String mensagemRecebida = "";
  for (int i = 0; i < length; i++) {
    mensagemRecebida += (char)payload[i];
  }
  Serial.println(mensagemRecebida);
  int vel = mensagemRecebida.toInt();
  Leds(vel);
}
