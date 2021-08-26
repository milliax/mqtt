#include <PubSubClient.h>
#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <WiFiEspUdp.h>

#include "SoftwareSerial.h"

#define relay_1 6 //繼電器控制腳1
#define relay_2 5 //繼電器控制腳1

#define WIFI_AP "home"
#define WIFI_PASSWORD "_LetMeIn"

const int MaxLength = 3300;
const char username[] = "test";
const char password[] = "test";
const IPAddress server(192, 168, 0, 23);
const char clientID[] = "arduino";

WiFiEspClient espClient;
PubSubClient client(espClient);
SoftwareSerial soft(10, 9); /* RX:D9, TX:D10 */

int status = WL_IDLE_STATUS;
unsigned long next;
unsigned long nextCurtain;

void publish();
void callback();
void reconnect();
void StepForward();
void StepBackward();

char stat = 'S';  //現在在做的事，F：向前轉 B：向後轉 S：停止
int pos;          //窗簾位置
bool embarrass = false;
long int last = 0;

void setup() {
    Serial.begin(9600);
    soft.begin(9600);
    WiFi.init(&soft);
    if (WiFi.status() == WL_NO_SHIELD) {
        Serial.println("wifi shield not present");
        while (true)
            ;
    }
    // connect to router
    Serial.println("Connecting...");
    while (status != WL_CONNECTED) {
        Serial.print("SSID: ");
        Serial.println(WIFI_AP);
        status = WiFi.begin(WIFI_AP, WIFI_PASSWORD);
    }
    Serial.println("Connected");
    pinMode(relay_1,OUTPUT);
    pinMode(relay_2,OUTPUT);
    digitalWrite(relay_1,HIGH);
    digitalWrite(relay_2,HIGH);
    // set server configuration
    client.setServer(server, 8883);
    client.setCallback(callback);
    client.setSocketTimeout(20);
    client.setKeepAlive(20);
    Serial.println(WiFi.localIP());
    next = millis() + 100;
    nextCurtain = millis() + 10;
}

char buff[33];

void wifiPart(){
  // make sure wifi is alway online
    status = WiFi.status();
    if (status != WL_CONNECTED) {
        while (status != WL_CONNECTED) {
            Serial.print("Attmpting to connect to WPA SSID: ");
            Serial.println(WIFI_AP);
            status = WiFi.begin(WIFI_AP, WIFI_PASSWORD);
            delay(100);
        }
        Serial.println("Wifi connected");
    }
    // make sure broker is always online
    if (!client.connected()) {
        reconnect();
    }
}

void curtainMove(){
  if ((long)(millis() - nextCurtain) >= 0) {
        switch (stat) {
            case 'S':
                break;
            case 'B':
                if (pos > 0) {
                    if (embarrass) {
                        digitalWrite(relay_1, LOW);
                        digitalWrite(relay_2, HIGH);
                        --pos;
                        embarrass = false;
                    } else {
                        digitalWrite(relay_1, HIGH);
                        digitalWrite(relay_2, HIGH);
                        embarrass = true;
                    }
                } else {
                  client.publish("curtain/notification", "Stopping");
                    stat = 'S';
                }
                break;
            case 'F':
                if (pos < MaxLength) {
                    if (embarrass) {
                        digitalWrite(relay_1, HIGH);
                        digitalWrite(relay_2, LOW);
                        embarrass = false;
                        ++pos;
                    } else {
                        digitalWrite(relay_1, HIGH);
                        digitalWrite(relay_2, HIGH);
                        embarrass = true;
                    }
                } else {
                  client.publish("curtain/notification", "Stopping");
                    stat = 'S';
                }
                break;
        }
        nextCurtain = millis() + 10;
    }
}

void loop() {
    if ((long)(millis() - next) >= 0) {
        wifiPart();
        client.loop();
        next = millis() + 5000;
    }
    curtainMove();
    client.loop();
    //Serial.println(millis()-last);
    //last = millis();
    Serial.println(pos);
}
// a client loop takes 0.08 seconds
// detect wifi signal takes 0.16 seconds

void reconnect() {
    // if not connected try until it works
    while (!client.connected()) {
        if (client.connect(clientID, username, password)) {
            Serial.println("Connected!");
            client.publish("curtain/notification", "aonline");
            client.subscribe("curtain/status");
        } else {
            Serial.print("failed: ");
            Serial.print(client.state());
            Serial.println("retrying");
            delay(100);
        }
    }
}

bool select = false;

void callback(char* topic, byte* payload, unsigned int leng) {
    switch((char)payload[0]){
      case 'a':
        break;
      case 'o':
        stat = 'F';
        client.publish("curtain/notification", "Opening");
        break;
      case 'c':
        stat = 'B';
        client.publish("curtain/notification", "Closing");
        break;
    }
    Serial.println();
}
