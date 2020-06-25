#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <IBMIOTDevice_Edge.h>

//Pin mapped
// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        15 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 4 // Popular NeoPixel ring size

#define Relay 12
#define Relay2 14

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

String user_html = ""
    "<p><input type='text' name='edge' placeholder='Edge Address'>"
    "<p><input type='text' name='devType' placeholder='Device Type'>"
    "<p><input type='text' name='devId' placeholder='Device Id'>"
    "<p><input type='text' name='meta.pubInterval' placeholder='Publish Interval'>";

char*               ssid_pfix = (char*)"esp8266basic";
unsigned long       pubInterval;
unsigned long       lastPublishMillis = 0;

int timer[4] = {0, 0, 0, 0};
int delaycnt = 0;
bool powerflag[4] = {false, false, false, false};

void publishData() {
    StaticJsonBuffer<512> jsonOutBuffer;
    JsonObject& root = jsonOutBuffer.createObject();
    JsonObject& data = root.createNestedObject("d");

    // data["temperature"]  = yourData;
    data["power1"] = powerflag[0] ? "on" : "off";
    data["power2"] = powerflag[1] ? "on" : "off";
    data["power3"] = powerflag[2] ? "on" : "off";
    data["power4"] = powerflag[3] ? "on" : "off";

    root.printTo(msgBuffer, sizeof(msgBuffer));
    client.publish(publishTopic, msgBuffer);
}

void handleUserCommand(JsonObject* root) {
    JsonObject& d = (*root)["d"];
    // user command

    if(d.containsKey("power1")) {
        if(!strcmp(d["power1"], "on")) {
            pixels.setPixelColor(0, pixels.Color(0, 150, 0));
            pixels.show();   // Send the updated pixel colors to the hardware.
            powerflag[0] = true;
        } else {
            timer[0] = 0;
            powerflag[0] = false;
            pixels.setPixelColor(0, pixels.Color(0, 0, 0));
            pixels.show();   // Send the updated pixel colors to the hardware.
        }
        lastPublishMillis = 0;
    }

    if(d.containsKey("timer1")) {
        timer[0] = d["timer1"];
    }

    if(d.containsKey("power2")) {
        if(!strcmp(d["power2"], "on")) {
            pixels.setPixelColor(1, pixels.Color(0, 150, 0));
            pixels.show();   // Send the updated pixel colors to the hardware.
            powerflag[1] = true;
        } else {
            timer[1] = 0;
            powerflag[1] = false;
            pixels.setPixelColor(1, pixels.Color(0, 0, 0));
            pixels.show();   // Send the updated pixel colors to the hardware.
        }
        lastPublishMillis = 0;
    }

    if(d.containsKey("timer2")) {
        timer[1] = d["timer2"];
    }

    if(d.containsKey("power3")) {
        if(!strcmp(d["power3"], "on")) {
            pixels.setPixelColor(2, pixels.Color(0, 150, 0));
            pixels.show();   // Send the updated pixel colors to the hardware.
            powerflag[2] = true;
        } else {
            pixels.setPixelColor(2, pixels.Color(0, 0, 0));
            pixels.show();   // Send the updated pixel colors to the hardware.
            powerflag[2] = false;
        }
        lastPublishMillis = 0;
    }

    if(d.containsKey("timer3")) {
        timer[2] = d["timer3"];
    }

    if(d.containsKey("power4")) {
        if(!strcmp(d["power4"], "on")) {
            pixels.setPixelColor(3, pixels.Color(0, 150, 0));
            pixels.show();   // Send the updated pixel colors to the hardware.
            powerflag[3] = true;
        } else {
            pixels.setPixelColor(3, pixels.Color(0, 0, 0));
            pixels.show();   // Send the updated pixel colors to the hardware.
            powerflag[3] = false;
        }
        lastPublishMillis = 0;
    }

    if(d.containsKey("timer4")) {
        timer[3] = d["timer4"];
    }

//   if(d.containsKey("power1")) {
//        pixels.setPixelColor(0, pixels.Color(0, 150, 0));
//        pixels.show();   // Send the updated pixel colors to the hardware.
//   }
//    lastPublishMillis = 0;
}

void message(char* topic, byte* payload, unsigned int payloadLength) {
    StaticJsonBuffer<512> jsonInBuffer;
    byte2buff(msgBuffer, payload, payloadLength);
    JsonObject& root = jsonInBuffer.parseObject((char*)msgBuffer);
    if (!root.success()) {
        Serial.println("handleCommand: payload parse FAILED");
        return;
    }

    handleIOTCommand(topic, &root);
    if (!strcmp(updateTopic, topic)) {
        pubInterval = (*cfg)["meta"]["pubInterval"];
    } else if (!strncmp(commandTopic, topic, 10)) {            // strcmp return 0 if both string matches
        handleUserCommand(&root);
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(Relay, OUTPUT);
    pinMode(Relay2, OUTPUT); 
    pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
    initCheck();
    // *** If no "config" is found or "config" is not "done", run configDevice ***
    if(!cfg->containsKey("config") || strcmp((const char*)(*cfg)["config"], "done")) {
         configDevice();
    }
    WiFi.mode(WIFI_STA);
    WiFi.begin((const char*)(*cfg)["ssid"], (const char*)(*cfg)["w_pw"]);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    // main setup
    Serial.printf("\nIP address : "); Serial.println(WiFi.localIP());
    JsonObject& meta = (*cfg)["meta"];
    pubInterval = meta.containsKey("pubInterval") ? atoi((const char*)meta["pubInterval"]) : 0;

    sprintf(iot_server, "%s", (const char*)(*cfg)["edge"]);
    if (!espClient.connect(iot_server, 1883)) {
        Serial.println("connection failed");
        return;
    }
    client.setServer(iot_server, 1883);   //IOT
    client.setCallback(message);
    iot_connect();
}

void loop() {
    if (!client.connected()) {
        iot_connect();
    }

    delay(1);
    delaycnt++;

    if(delaycnt > 700){
        if(timer[0] >0){
            timer[0]--;
            delaycnt = 0;
            Serial.println(timer[0]);
        }
        if(timer[1] >0){
            timer[1]--;
            delaycnt = 0;
            Serial.println(timer[1]);
        }
        if(timer[2] >0){
            timer[2]--;
            delaycnt = 0;
            Serial.println(timer[2]);
        }
        if(timer[3] >0){
            timer[3]--;
            delaycnt = 0;
            Serial.println(timer[3]);
        }
    }

    if (timer[0] == 0){
        powerflag[0] = false;
        pixels.setPixelColor(0, pixels.Color(0, 0, 0));
        pixels.show();   // Send the updated pixel colors to the hardware.
    }

    if (timer[1] == 0){
        powerflag[1] = false;
        pixels.setPixelColor(1, pixels.Color(0, 0, 0));
        pixels.show();   // Send the updated pixel colors to the hardware.
    }

     if (timer[2] == 0){
        powerflag[2] = false;
        pixels.setPixelColor(2, pixels.Color(0, 0, 0));
        pixels.show();   // Send the updated pixel colors to the hardware.
    }

    if (timer[3] == 0){
        powerflag[3] = false;
        pixels.setPixelColor(3, pixels.Color(0, 0, 0));
        pixels.show();   // Send the updated pixel colors to the hardware.
    }

    if(timer[0]%2){
        digitalWrite(Relay, HIGH);
    }else{
        digitalWrite(Relay, LOW);
    }

    if(timer[1]%2){
        digitalWrite(Relay2, HIGH);
    }else{
        digitalWrite(Relay2, LOW);
    }
    
    client.loop();
    if ((pubInterval != 0) && (millis() - lastPublishMillis > pubInterval)) {
        publishData();
        lastPublishMillis = millis();
    }
}
