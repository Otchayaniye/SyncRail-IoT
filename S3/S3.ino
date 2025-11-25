#include <WiFi.h>
#include "env.h"
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

WiFiClientSecure wificlient;
PubSubClient mqtt(wificlient);

const int Led = 2;
const int ULTRA_ECHO3 = 18;
const int ULTRA_TRIG3 = 19;
const int Servo1 = 22;
const int Servo2 = 23;


void setup() {
  pinMode(Led, OUTPUT);
  pinMode(ULTRA_ECHO3, OUTPUT);
  pinMode(ULTRA_TRIG3, OUTPUT);
  pinMode(Servo1, OUTPUT);
  pinMode(Servo2, OUTPUT);

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
  String boardID = "S3-";
  boardID += String(random(0xffff), HEX);

  while (!mqtt.connect(boardID.c_str(),BROKER_USR_NAME,BROKER_USR_PASS)) {
    Serial.print(".");
    delay(200);
  }
  Serial.println("\nConectado com sucesso ao broker!");
  mqtt.subscribe(TOPIC_ILUMINACAO);         // RECEBER informações do tópico
  mqtt.subscribe(TOPIC_Presenca1);
  mqtt.subscribe(TOPIC_Presenca2);
  mqtt.subscribe(TOPIC_Presenca3);
  mqtt.setCallback(callback);

  
  Servo1.write(0); 
  Servo2.write(0);

long lerDistancia() {
  digitalWrite(ULTRA_TRIG3, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRA_TRIG3, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRA_TRIG3, LOW);
  
  long duracao = pulseIn(ULTRA_ECHO3, HIGH);
  long distancia = duracao * 349.24 / 2 / 10000;
  
  return distancia;
}

void sensor() {
  long distancia = lerDistancia();
  
  Serial.print("Distância: ");
  Serial.print(distancia);
  Serial.println(" cm");
  
  if (distancia < 10) {
    Serial.println("Objeto próximo!");
    mqtt.publish(TOPIC_Presenca3, "detectado");
  }
  
  delay(500);
}

void loop() {
  mqtt.loop();
}

void callback(char* topic, byte* payload, unsigned long length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  if (topic == TOPIC_ILUMINACAO){
    if (message == "Acender"){
      digitalWrite(Led, HIGH);
    } else if (message == "Apagar"){
      digitalWrite(Led, LOW);
    }
  }  

  if (topic == TOPIC_Presenca1)
    if (message == "detectado")
}




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
