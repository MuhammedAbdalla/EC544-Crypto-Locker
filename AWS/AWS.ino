#include <WiFi.h>

const char* wifi_network_ssid = "BU (802.1x)";
const char* wifi_network_password = "I3@(+)Friess";

const char* soft_ap_ssid = "ESP32_LOCKER";
const char* soft_ap_password = "EC544";


void setup() {
    Serial.begin(115200);

    WiFi.begin(wifi_network_ssid, wifi_network_password);
    WiFi.softAP(soft_ap_ssid, soft_ap_ssid);

    Serial.print("IP address: ");
    Serial.println(WiFi.softAPIP());

    while (Wifi.status() != WL_CONNECTED) {
        delay(550);
        Serial.println("Connecting to Wifi.");
        delay(25);
        Serial.println("Connecting to Wifi..");
        delay(25);
        Serial.println("Connecting to Wifi...");
    }
    Serial.println("Connected to Wifi");
    
}

void loop() {

}
