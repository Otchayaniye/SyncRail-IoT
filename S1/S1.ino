#include <WiFi.h>
#include "env.h"
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <DHT.h>
                                                                   //COLOCAR TÓPICO STATUSLED NO ENV.H
                                                                   //VERIFICAR CONFIG LED
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
                                                  pinMode(pino1, OUTPUT);
                                                  ledcAttach(pino1, 5000, 8);
  dht.begin();
  wificlient.setInsecure();
  Serial.begin(115200);   
  WiFi.begin(SSID, PASS);
  Serial.println("Conectando ao Wifi");
  while (WiFi.status() != WL_CONNECTED) {
    statusLED(1);
    Serial.print(".");
    delay(200);
  }
  mqtt.setServer(BROKER_URL, BROKER_PORT);
  String boardID = "SyncRail-S1-";
  boardID += String(random(0xffff), HEX);
  Serial.println("Conectando no Broker");
  while (!mqtt.connect(boardID.c_str(),BROKER_USR_NAME,BROKER_USR_PASS)) {
    statusLED(2);
    Serial.print(".");
    delay(200);
  }
  Serial.println("\nConectado com sucesso ao broker!");
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

void statusLED(byte status) {
	turnOffLEDs();
	switch (status) {
	case 254: (Vermelho)
    	setLEDColor(255, 0, 0);
    	break;
	case 1: (Amarelo)
    	setLEDColor(150, 255, 0);
    	break;
	case 2: (Rosa)
    	setLEDColor(150, 0, 255);
    	break;
	case 3:  (Verde)
    	setLEDColor(0, 255, 0);
    	break;
	case 4:  (Ciano)
    	setLEDColor(0, 255, 255);
    	break;
	default:
    	for (byte i = 0; i < 4; i++) {
        	setLEDColor(0, 0, 255);  (pisca azul)
        	delay(100);
        	turnOffLEDs();
        	delay(100);
    	}
    	break;
	}
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
  mqtt.publish(TOPIC_TEMPERATURA,String(temperatura).c_str());

  long distancia = lerDistancia();
  if (distancia < 5) {
    mqtt.publish(TOPIC_PRESENCA, "1");
  } else {
    mqtt.publish(TOPIC_PRESENCA, "0");
  }
  delay(3000);
  mqtt.loop();
}

void callback(char* topic, byte* payload, unsigned long length) {
}
