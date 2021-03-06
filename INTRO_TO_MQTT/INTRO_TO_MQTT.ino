/*Desenvolvido por Bruno Horta @ 2017
 * 
 * Este Código de Exemplo está inserido no contexto público e pode ser alterado ou distribuido
 *  
 * Bibliotecas necessárias
 * 
 * PubSubClient
 * ** Como instalar https://www.youtube.com/watch?v=yM1eljamBwM&list=PLxDLawCWayzCsoqDt7A7kBVDFVt0RSm1g&index=2
 * 
 * Montagem
 * Fotos em Anexo
 * */

//MQTT
#include <PubSubClient.h>
//ESP
#include <ESP8266WiFi.h>

#define MQTT_AUTH true
#define MQTT_USERNAME "username"
#define MQTT_PASSWORD "password"
#define LIGHT D1
#define BUTTON D2

//Variaveis e Constantes de DEBOUNCE
int buttonState;             
int lastButtonState = LOW;   
long lastDebounceTime = 0;
const int debounceDelay = 50;

//Constantes 
const String HOSTNAME  = "nome_do_device";

const char* ssid = "nome_da_rede_wifi";

const char* password = "password_da_rede_wifi";;

const String MQTT_LIGHT_TOPIC = "sala/luz/set";

const String MQTT_LIGHT_STATE_TOPIC = "sala/luz";

//MQTT BROKERS GRATUITOS PARA TESTES https://github.com/mqtt/mqtt.github.io/wiki/public_brokers
const char* MQTT_SERVER = "ip_ou_dns_do_broker";

WiFiClient wclient;
PubSubClient client(MQTT_SERVER,1883,wclient);


void setup() {
  Serial.begin(115200);
  client.setCallback(callback);
  pinMode(LIGHT,OUTPUT); 
  pinMode(BUTTON,INPUT_PULLUP); 
  WiFi.begin(ssid, password);
  WiFi.mode(WIFI_STA);
  Serial.println(".");
  // Aguarda até estar ligado ao Wi-Fi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Ligado a ");
  Serial.println(ssid);
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

//Chamada de recepção de mensagem 
void callback(char* topic, byte* payload, unsigned int length) {
  String payloadStr = "";
  for (int i=0; i<length; i++) {
    payloadStr += (char)payload[i];
  }
  Serial.println(payloadStr);
  String topicStr = String(topic);
 if(topicStr.equals(MQTT_LIGHT_TOPIC)){
  if(payloadStr.equals("ON")){
      turnOn();
    }else if(payloadStr.equals("OFF")) {
      turnOff();
    }
  } 
} 
void turnOn(){
  Serial.println("ON");
  digitalWrite(LIGHT,HIGH);
  client.publish(MQTT_LIGHT_STATE_TOPIC.c_str(),"ON");  
}

void turnOff(){
  digitalWrite(LIGHT,LOW);  
  Serial.println("OFF");
  client.publish(MQTT_LIGHT_STATE_TOPIC.c_str(),"OFF");
}

bool checkMqttConnection(){
  if (!client.connected()) {
    if (MQTT_AUTH ? client.connect(HOSTNAME.c_str(),MQTT_USERNAME, MQTT_PASSWORD) : client.connect(HOSTNAME.c_str())) {
      //SUBSCRIÇÃO DE TOPICOS
      Serial.println("CONNECTED");
      client.subscribe(MQTT_LIGHT_TOPIC.c_str());
    }
  }
  return client.connected();
}

void checkButtonState(){
 if(processDebounce(BUTTON)){
  if(digitalRead(LIGHT)){
      turnOff();
    }else{
      turnOn();
    } 
 }
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    if (checkMqttConnection()){
      client.loop();
      checkButtonState();
 
    }
  }
}


boolean processDebounce(int pin){
  // ler o estado do pino para uma variável local
  int sensorVal = digitalRead(pin);
  /*
   * verificar se o botão foi pressionado e o tempo que decorreu desde o último pressionar do botão 
   * é suficiente para ignorar qualquer tipo de ruído.
  */

  /*
   * Se a leitura registou uma alteração de estado, seja ele ruído ou o pressionar do botão
  */
  if (sensorVal != lastButtonState) {
    // reiniciar o contador de debouncing
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    /**
     * Qualquer que seja a leitura, esta aconteceu a um tempo superior ao intervalo 
     * de debouncing considerado (no exemplo 50 milisegundos).
     * Por essa razão, pode se assumir a leitura como sendo o estado atual do botão.
     */
    if (sensorVal != buttonState) {
      /*
       * o estado atual do botão é diferente ao último estado válido registado, por isso,
       * igualar o último estado válido, para o botão, como sendo a leitura atual.
       */
      buttonState = sensorVal;
      if (buttonState == HIGH) {
        /*
         * definir o último estado lido, para o botão, como sendo a leitura atual.
         */
        lastButtonState = sensorVal;
        /**
          * O botão encontra-se ativo, por isso retorna-se true
        */
        return true;
      }
    }
  }
  /*
   * definir último estado lido, para o botão, como sendo a leitura atual.
   */
  lastButtonState = sensorVal;
  return false;
}



