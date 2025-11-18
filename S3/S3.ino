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
  String boardID = "S4-Trem";
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


recebe infos dos tópicos (subscribe)
- iluminação > s1 - mqtt.subscribe(TOPIC_ILUM)
- presença1  > s2	"
- presença2  > s2	"
- presença3  > s3	"

envia (publish)

- presença3 > s3

loop()
 - ler o ultrassonico - tem no class o código
 - ver se o valor é < 10 -
	- publica (presenca3) - "detectado" mqtt.publish(TOPIC_PRESENCA_3, "detectado")

call-back(topic, payload , length)

 - se topic == iluminação
	- se mensagem == "Acender"
		- acende led
	- senão se mensagem == "Apagar"
		- apaga led

 - senão se topic == presenca1
	- se mensagem == "detectado"
		- servo1 para 90º

 - senão se topic == presenca2
	- se mensagem == "detectado"
		- servo2 para 90º

 - senão se topic == presenca3
	- se mensagem == "detectado"
		- servo1 para 120º
		- servo2 para 120º
