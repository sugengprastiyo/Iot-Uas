#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SimpleDHT.h>
#define relay 4

const char *ssid = "Arya kiki";//silakan disesuaikan sendiri
const char *password = "mbayarle";//silakan disesuaikan sendiri

const char *mqtt_server = "broker.emqx.io";

WiFiClient espClient;
PubSubClient client(espClient);

SimpleDHT11 dht11(D1);


long now = millis();
long lastMeasure = 0;

void setup_wifi()
{
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client"))
    {
      Serial.println("connected");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Mqtt Node-RED");
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  pinMode(relay, OUTPUT);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  if (!client.loop())
  {
    client.connect("ESP8266Client");
  }
  now = millis();
  if (now - lastMeasure > 5000)
  {
    lastMeasure = now;
    int err = SimpleDHTErrSuccess;

    byte temperature = 0;
    byte humidity = 0;
    if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess)
    {
      Serial.print("Pembacaan DHT11 gagal, err=");
      Serial.println(err);
      delay(1000);
      return;
    }

    static char temperatureHumi[7];
    dtostrf(humidity, 4, 2, temperatureHumi);
    Serial.print("Kelembapan = ");
    Serial.println(temperatureHumi);
    if(humidity <= 40) {
      digitalWrite(relay, LOW);
      Serial.println("Suhu <40");
    } else if( humidity >= 50) {
      digitalWrite(relay, HIGH);
      Serial.println("Suhu >50");
    }
    client.publish("room/humiditySusalan", temperatureHumi);
  }
}