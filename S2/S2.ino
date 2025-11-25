#include <WiFi.h>
#include "env.h"
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

WiFiClientSecure wificlient;
PubSubClient mqtt(wificlient);

const int ULTRA_ECHO1 = 23;
const int ULTRA_TRIG1 = 22;
const int ULTRA_ECHO2 = 26;
const int ULTRA_TRIG2 = 27;

void setup() {
  pinMode(ULTRA_ECHO1, INPUT);
  pinMode(ULTRA_TRIG1, OUTPUT);
  pinMode(ULTRA_ECHO2, INPUT);
  pinMode(ULTRA_TRIG2, OUTPUT);

  Serial.begin(115200);    //configura a placa pra mostrar na tela
  wificlient.setInsecure();
  WiFi.begin(SSID, PASS);  //tenta conectar na rede
  Serial.println("Conectando ao Wifi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  Serial.println("Conectado com Sucesso meu Labubu!");
  Serial.println("Conectando no Broker");
  mqtt.setServer(BROKER_URL, BROKER_PORT);
  String boardID = "S2-Deteccao";
  boardID += String(random(0xffff), HEX);

   while (!mqtt.connect(boardID.c_str(),BROKER_USR_NAME,BROKER_USR_PASS)) {
    Serial.print(".");
    delay(200);
  }
  mqtt.subscribe(TOPIC_VELOCIDADE);         //mudar para o tópico correto
  mqtt.setCallback(callback);
  Serial.println("\nConectado com sucesso ao broker!");
  pinMode(2, OUTPUT);

//definir os pinos do ultrassonico 1 e 2 como entrada e saida

long lerDistancia() {
  digitalWrite(ULTRA_TRIG1, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRA_TRIG1, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRA_TRIG1, LOW);
  
  long duracao = pulseIn(ULTRA_ECHO1, HIGH);
  long distancia = duracao * 349.24 / 2 / 10000;
  
  return distancia;
}

}

void loop() {

  long distancia = lerDistancia();
  
  Serial.print("Distância: ");
  Serial.print(distancia);
  Serial.println(" cm");
  
  if (distancia < 10) {
    Serial.println("Objeto próximo!");
    mqtt.publish(TOPIC_Presenca2, "objeto próximo!");
  }
  
  delay(500);
}

//repetir para o ultrassonico 2

long lerDistancia() {
  digitalWrite(ULTRA_TRIG2, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRA_TRIG2, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRA_TRIG2, LOW);
  
  long duracao = pulseIn(ULTRA_ECHO2, HIGH);
  long distancia = duracao * 349.24 / 2 / 10000;
  
  return distancia;
}

}

void loop() {

  long distancia = lerDistancia();
  
  Serial.print("Distância: ");
  Serial.print(distancia);
  Serial.println(" cm");
  
  if (distancia < 10) {
    Serial.println("Objeto próximo!");
    mqtt.publish(TOPIC_Presenca4, "objeto próximo!");
  }
  
  delay(500);
}
  mqtt.loop();
}

void callback(char* topic, byte* payload, unsigned long length) {
  String mensagemRecebida = "";
  for (int i = 0; i < length; i++) {
    mensagemRecebida += (char)payload[i];
  }

  //antes precisa verificar se o topic da mensagem é igual ao topico da iluminacao
  Serial.println(mensagemRecebida);
  int status = mensagemRecebida.toInt();
  Leds(status);
  }
}