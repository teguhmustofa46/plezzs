#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <NewPing.h> //Library untuk HC-SR04

#define DHTPIN D4 // digital pin dht11
#define DHTTYPE DHT11 // inisialisasi sensor dht11


#define TRIGGER_PIN  D5  //Pin Trigger HC-SR04 pada NodeMCU
#define ECHO_PIN     D6  //Pin Echo HC-SR04 pada NodeMCU
#define MAX_DISTANCE 250 //Maksimum Pembacaan Jarak (cm)

//MQTT Topic
#define MQTT_TEMP "temperature"
#define MQTT_HUMD "humidity"
#define MQTT_JARAK "ping_cm"

const char* ssid = "Plezz9"; 
const char* password = "adgjm1922"; 
const char* mqtt_server = "broker.mqtt-dashboard.com";
unsigned long startMillis=0; //variabel menyimpan milidetik terakhir dari loop

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); //Setup Pin HC-SR04 dan Jarak Pembacaan dengan Fungsi Library

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void DHT_config(){
 float tempRead = dht.readTemperature();
 float humdRead = dht.readHumidity();
 float jarak = sonar.ping_cm();
 if (isnan(tempRead) || isnan(humdRead))
 {
  client.publish(MQTT_TEMP, "0");
  client.publish(MQTT_HUMD, "0");
  client.publish(MQTT_JARAK, "0");
  return;
 }
 client.publish(MQTT_TEMP, String(tempRead).c_str());
 client.publish(MQTT_HUMD, String(humdRead).c_str());
 client.publish(MQTT_JARAK, String(jarak).c_str());
}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "belajarmqtt";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) 
    {
      Serial.println("connected");
      client.publish("temperature",  "Online");
      client.subscribe("humidity");
      client.subscribe("ping_cm");
    } 
    else 
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

void setup(){
  Serial.begin(9600);
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  //client.setCallback(callback);
}

void loop(){
  unsigned long currentMillis = millis(); // variabel mengambil waktu yang berjalan dan menyimpan di variabel milis_sekarang
  if (currentMillis - startMillis >= 3000) { //setiap milis_sekarang - hitungan_milis yang mencapai nilai lebih besar atau sama dengan 2000
  
    float tempRead = dht.readTemperature();
    float humdRead = dht.readHumidity();
  
    DHT_config();
    if (!client.connected()){
      reconnect();
    }
    client.loop();
    Serial.print(F("Humidity: "));
    Serial.print(humdRead);
    Serial.println(F("°C "));
    Serial.print(F("  Temperature: "));
    Serial.print(tempRead);
    Serial.println(F("°C "));


//sensor jarak
    float jarak = sonar.ping_cm(); //Melakukan Pembacaan Jarak dalam bentuk cm
  //Mencetak Hasil Pembacaan pada Serial Monitor          
  Serial.println("Monitoring Jarak");
  Serial.print("Jarak: ");
  Serial.print(jarak);
  Serial.println(" cm");
  delay(1000); //Delay 1 
  }
}
