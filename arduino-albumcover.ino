#include <HTTPClient.h>
#include <WiFi.h>
#include <espMqttClient.h>
#include <base64.hpp>

#include <M5Unified.h>

#include <arduino_secrets.h>

const char* wifi_ssid = SECRET_WIFI_SSID;
const char* wifi_password = SECRET_WIFI_PASSWORD;

const char* mqtt_user = SECRET_MQTT_USER;
const char* mqtt_password = SECRET_MQTT_PASSWORD;

#define MQTT_HOST IPAddress(192, 168, 2, 34)
#define MQTT_PORT 1883

const char* home_assistant_prefix = "http://192.168.2.34:8123";

const char* mqtt_media_player_picture_topic = "hass/media_player/plex_plexamp_dev5/entity_picture";

//const char * imgproxy_prefix = "http://192.168.2.30:8088/_/rs:fill:240:240:0/background:0:0:0/padding:0:0:0:40/"; // m5stack core2
const char* imgproxy_prefix = "http://192.168.2.30:8088/_/rs:fill:180:180:0/background:0:0:0/padding:30/"; // m5stack dial

const char* imgproxy_postfix = ".jpg";

espMqttClient mqttClient;
bool reconnectMqtt = false;
uint32_t lastReconnect = 0;

void WiFiEvent(WiFiEvent_t event) {
    Serial.printf("[WiFi-event] event: %d\n", event);
    switch (event) {
    case SYSTEM_EVENT_STA_GOT_IP:
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
        connectToMqtt();
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        Serial.println("WiFi lost connection");
        break;
    default:
        break;
    }
}

void connectToMqtt() {
    Serial.println("Connecting to MQTT...");
    if (!mqttClient.connect()) {
        reconnectMqtt = true;
        lastReconnect = millis();
        Serial.println("Connecting failed.");
    }
    else {
        reconnectMqtt = false;
    }
}

void onMqttConnect(bool sessionPresent) {
    Serial.println("Connected to MQTT.");
    Serial.print("Session present: ");
    Serial.println(sessionPresent);
    uint16_t packetIdSub = mqttClient.subscribe(mqtt_media_player_picture_topic, 2);
    Serial.print("Subscribing at QoS 2, packetId: ");
    Serial.println(packetIdSub);
}

void onMqttDisconnect(espMqttClientTypes::DisconnectReason reason) {
    Serial.printf("Disconnected from MQTT: %u.\n", static_cast<uint8_t>(reason));

    if (WiFi.isConnected()) {
        reconnectMqtt = true;
        lastReconnect = millis();
    }
}

void onMqttMessage(const espMqttClientTypes::MessageProperties& properties, const char* topic, const uint8_t* payload, size_t len, size_t index, size_t total) {
    Serial.println("Publish received:");
    Serial.printf("  topic: %s\n  payload:", topic);

    if (strcmp (topic, mqtt_media_player_picture_topic)== 0)
    {
        int home_assistant_prefix_len = strlen(home_assistant_prefix);
        char* strval = new char[home_assistant_prefix_len + len - 1];
        memcpy(strval, home_assistant_prefix, home_assistant_prefix_len);
        memcpy(strval + home_assistant_prefix_len, payload + 1, len - 2);
        strval[home_assistant_prefix_len + len - 2] = '\0';

        Serial.println(strval);

        int imgproxy_prefix_len = strlen(imgproxy_prefix);
        int imgproxy_postfix_len = strlen(imgproxy_postfix);

        unsigned int base64_len = encode_base64_length(home_assistant_prefix_len + len);

        char* base64 = new char[imgproxy_prefix_len + base64_len + imgproxy_postfix_len + 1];

        memcpy(base64, imgproxy_prefix, imgproxy_prefix_len);

        encode_base64((unsigned char*)strval, strlen((char*)strval), (unsigned char*)base64 + imgproxy_prefix_len);

        delete[] strval;

        memcpy(base64 + imgproxy_prefix_len + base64_len, imgproxy_postfix, imgproxy_postfix_len);

        base64[imgproxy_prefix_len + base64_len + imgproxy_postfix_len] = '\0';


        Serial.println((char*)base64);

        int retries = 3;

        while (retries--)
        {
            bool status = M5.Display.drawJpgUrl((char*)base64, 0, 0);

            Serial.println(status);

            if (status)
                break;

            Serial.println("RETRY drawJpgUrl");

        }

        delete[] base64;
    }

}

void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println();
    delay(1000);

    auto cfg = M5.config();
    M5.begin(cfg);

    mqttClient.setCredentials(mqtt_user, mqtt_password);
    mqttClient.setServer(MQTT_HOST, MQTT_PORT);

    mqttClient.onConnect(onMqttConnect);
    mqttClient.onDisconnect(onMqttDisconnect);
    mqttClient.onMessage(onMqttMessage);

    WiFi.setAutoConnect(false);
    WiFi.setAutoReconnect(true);
    WiFi.onEvent(WiFiEvent);

    //WiFi.mode(WIFI_STA);
    //WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
    //WiFi.setHostname("abcd");

    Serial.println("Connecting to Wi-Fi...");
    WiFi.begin(wifi_ssid, wifi_password);
}

void loop() {

    M5.update();

    static uint32_t currentMillis = millis();

    if (reconnectMqtt && currentMillis - lastReconnect > 5000) {
        connectToMqtt();
    }

}