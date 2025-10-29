#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient client;
PubSubClient mqtt(client);

const String SSID = "FIESC_IOT_EDU";
const String PASS = "8120gv08";
const String topico = "StrogonoffcomBatataPalha";

const String brokerURL = "test.mosquitto.org";
const int brokerPort = 1883;

const String brokerUser = "";
const String brokerPass = "";

void setup() {
  Serial.begin(115200);    //configura a placa pra mostrar na tela
  WiFi.begin(SSID, PASS);  //tenta conectar na rede
  Serial.println("Conectando ao Wifi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  Serial.println("Conectado com Sucesso parceiro!");
  Serial.println("Conectando no Broker");
  mqtt.setServer(brokerURL.c_str(), brokerPort);
  String boardID = "S1-";
  boardID += String(random(0xffff), HEX);

  while (!mqtt.connect(boardID.c_str())) {
    Serial.print(".");
    delay(200);
  }
  mqtt.subscribe(topico.c_str());
  mqtt.setCallback(callback);
  Serial.println("\nConectado com sucesso ao broker!");
  pinMode(2, OUTPUT);
}

void loop() {
  String mensagem = "";
  if (Serial.available() > 0) {
    mensagem = Serial.readStringUntil('\n');
    Serial.print("Mensagem digitada: ");
    Serial.println(mensagem);
    mqtt.publish("TopicoVini", mensagem.c_str());
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