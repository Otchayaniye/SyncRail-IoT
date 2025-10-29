#include <WiFi.h>
#include <env.h>

WiFiClientSecure wificlient;
PubSubClient mqtt(wificlient);

const String brokerURL = "test.mosquitto.org";
const int brokerPort = 1883 ;

const String brokerUser = "";
const String brokerPass = "";

void setup() {
  Serial.begin(115200);    //configura a placa pra mostrar na tela
  WiFiClient.setInsecure(SSID, PASS);  //tenta conectar na rede
  connectToWifi();
  connectToBroker();
  }
  

void loop() {
  String mensagem = "";
  if (Serial.available() > 0) {
    mensagem = Serial.readStringUntil('\n');
    Serial.print("Mensagem digitada: ");
    Serial.println(mensagem);
    mqtt.publish("StrogonoffcomBatataPalha", mensagem.c_str());
  }
  mqtt.loop();
  
}

void callback(char* topic, byte* payload, unsigned long length) {
  String mensagemRecebida = "";
  for (int i = 0; i < length; i++) {
    mensagemRecebida += (char)payload[i];
  }
  Serial.println(mensagemRecebida);
  if(mensagemRecebida == "on"){
    digitalWrite(2, 1);
  } else if(mensagemRecebida == "off"){
    digitalWrite(2, 0);
  }
}
