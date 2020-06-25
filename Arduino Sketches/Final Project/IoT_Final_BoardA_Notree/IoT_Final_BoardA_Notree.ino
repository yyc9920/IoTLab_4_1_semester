#include <ESP8266WiFi.h>
#include <IBMIOTDevice_Edge.h>
#include <Wire.h>
#include <OLED32.h>
#include <DHT.h>
#include <stdlib.h>
#include <string.h>

//Step name
#define MAIN 0
#define SELMENU 1
#define SHOWSTAT 2
#define CHGSTAT 3
#define SETTIMER 4
#define CANCLE 5
#define STLEVEL 6
#define SELIND 7
#define COOKALERT 8
#define ALREADYCOOKALERT 9

String user_html = ""
                   "<p><input type='text' name='edge' placeholder='Edge Address'>"
                   "<p><input type='text' name='devType' placeholder='Device Type'>"
                   "<p><input type='text' name='devId' placeholder='Device Id'>"
                   "<p><input type='text' name='meta.pubInterval' placeholder='Publish Interval'>";

char *ssid_pfix = (char *)"esp8266basic";
unsigned long pubInterval;
unsigned long lastPublishMillis = 0;
unsigned long lastClicked = 0;
int clicked = 0;
const int pulseA = 12;
const int pulseB = 13;
const int pushSW = 2;
volatile int lastEncoded = 0;
volatile long encoderValue = 70;

/*****Select Option Numbers*****/
int statnum, chgnum, selnum;
int foodselnum;
int st; //Select Timer
int sc; //Select Change
int si; //Select Induction Number
/*****Select Option Numbers*****/

int j = 0; //integer for for loop

/*****Temporary Char Array for sprintf*****/
char tmp[5];
char tmp2[5];
char tmp3[5];
/*****Temporary Char Array for sprintf*****/

int step = MAIN; //Set Step to Main initialy

OLED display(4, 5);

#define DHTTYPE DHT22
// DHT22 is attached to GPIO14 on this Kit
#define DHTPIN 14
DHT dht(DHTPIN, DHTTYPE, 11);
float humidity;
float temp_f;
unsigned long lastDHTReadMillis = 0;
#define interval 2000
float tgtT;

int nowCooking[4];

int listcnt;

void handleRotary()
{
    // Never put any long instruction
    int MSB = digitalRead(pulseA); //MSB = most significant bit
    int LSB = digitalRead(pulseB); //LSB = least significant bit

    int encoded = (MSB << 1) | LSB;         //converting the 2 pin value to single number
    int sum = (lastEncoded << 2) | encoded; //adding it to the previous encoded value
    if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011)
        encoderValue++;
    if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000)
        encoderValue--;
    lastEncoded = encoded; //store this value for next time
    if (encoderValue > 255)
    {
        encoderValue = 255;
    }
    else if (encoderValue < 0)
    {
        encoderValue = 0;
    }
    lastPublishMillis = millis() - pubInterval + 200; // send and publish immediately
}

//TODO : BoardB로 잘 넘겨줄 수 있도록 구현하기
void publishData()
{
    StaticJsonBuffer<512> jsonOutBuffer;
    JsonObject &root = jsonOutBuffer.createObject();
    JsonObject &data = root.createNestedObject("d");

    int temp;
    char ctimer[5];
    int i;

    // data["temperature"]  = yourData;
    if (clicked == 1)
    {
        //메인 화면에서 메뉴를 고를지, 현재 요리 현황을 볼지 선택
        if (step == MAIN)
        {
            if (((int)tgtT) % 2)
                step = SHOWSTAT;
            else
                step = SELMENU;
        }
        else if (step == SELMENU)
        { //선택된 메뉴 정보를 json buffer로 publish
            if (foodselnum == 0)
            { //Go Back이 선택됬을 때
                step = MAIN;
            }
            else
            {
                step = SELIND;
            }
        }
        else if (step == SHOWSTAT)
        {
            if (statnum == 0)
            {
                step = MAIN;
            }
            else
            {
                 if (nowCooking[statnum - 1] != 0){
                     step = CHGSTAT;
                 }
            }
        }
        else if (step == CHGSTAT)
        {
            if (chgnum == 0)
            {
                step = SHOWSTAT;
            }
            else if (chgnum == 1)
            {
                step = SETTIMER;
            }
            else if (chgnum == 2)
            {
                step = CANCLE;
            }
        }
        else if (step == SETTIMER)
        {
            if (nowCooking[statnum - 1] != 0)
            {
                data["button"] = statnum;
                data["timer"] = st;
            }
            step = MAIN;
        }
        else if (step == CANCLE)
        {
            if (((int)tgtT) % 2)
                step = MAIN;
            else
            {
                data["button"] = statnum;
                data["cnc_food"] = "yes";
                nowCooking[statnum - 1] = 0;
                step = MAIN;
            }
        }
        else if (step == SELIND)
        {
            if (foodselnum > 0 && listcnt > foodselnum)
            { //현재 요리중인 것이 4개보다 작을 때만 publish

                if(nowCooking[si] != 0){
                    step = ALREADYCOOKALERT;
                }else{
                nowCooking[si] = foodselnum + 1;

                data["button"] = si+1;
                data["select"] = foodselnum;

                step = COOKALERT;}
            }
            else
            {
                step = MAIN;
            }
        }
        clicked = 0;
    }

    gettemperature();
    char dht_buffer[10];
    char dht_buffer2[10];
    sprintf(dht_buffer, "%2.1f", temp_f);
    // display.print(dht_buffer, 2, 10);
    //data["temperature"] = dht_buffer;
    tgtT = map(encoderValue, 0, 255, 0, 100);
    sprintf(dht_buffer2, "%2.1f", tgtT);
    //data["target"] = dht_buffer2;
    // display.print(dht_buffer2, 3, 10);
    display.print("                 ", 0, 0);
    display.print("                 ", 1, 0);
    display.print("                 ", 2, 0);
    display.print("                 ", 3, 0);

    if (step == SELMENU)
    {
        foodselnum = (int)tgtT % 50;
        display.print("*Select Menu*");
        display.print("Menu Number : ", 2, 1);
        sprintf(tmp, "%d", foodselnum+1);
        display.print(tmp, 3, 10);
    }
    else if (step == MAIN)
    {
        display.print("*Main*");
        display.print("Select Menu", 1, 1);
        display.print("Show Status", 2, 1);
        display.print(">", ((int)tgtT) % 2 + 1, 0);
    }
    else if (step == SHOWSTAT)
    {
        statnum = (int)tgtT % 5;
        display.print("*Now Cooking*");
        if (statnum == 0)
        {
            display.print("Go Back", 2, 1);
            if (nowCooking[0] != 0)
            {
                display.print("Id : ", 3, 1);
                sprintf(tmp, "%d", nowCooking[0]);
                display.print(tmp, 3, 6);
            }
        }
        else if (statnum == 1)
        {
            display.print("Go Back", 1, 1);
            if (nowCooking[0] != 0)
            {
                display.print("Id : ", 2, 1);
                sprintf(tmp, "%d", nowCooking[0]);
                display.print(tmp, 2, 6);
            }
            if (nowCooking[1] != 0)
            {
                display.print("Id : ", 3, 1);
                sprintf(tmp2, "%d", nowCooking[1]);
                display.print(tmp2, 3, 6);
            }
        }
        else if (statnum == 2)
        {
            if (nowCooking[0] != 0)
            {
                display.print("Id : ", 1, 1);
                sprintf(tmp, "%d", nowCooking[0]);
                display.print(tmp, 1, 6);
            }
            if (nowCooking[1] != 0)
            {
                display.print("Id : ", 2, 1);
                sprintf(tmp2, "%d", nowCooking[1]);
                display.print(tmp2, 2, 6);
            }
            if (nowCooking[2] != 0)
            {
                display.print("Id : ", 3, 1);
                sprintf(tmp3, "%d", nowCooking[2]);
                display.print(tmp3, 3, 6);
            }
        }
        else if (statnum == 3)
        {
            if (nowCooking[1] != 0)
            {
                display.print("Id : ", 1, 1);
                sprintf(tmp, "%d", nowCooking[1]);
                display.print(tmp, 1, 6);
            }
            if (nowCooking[2] != 0)
            {
                display.print("Id : ", 2, 1);
                sprintf(tmp2, "%d", nowCooking[2]);
                display.print(tmp2, 2, 6);
            }
            if (nowCooking[3] != 0)
            {
                display.print("Id : ", 3, 1);
                sprintf(tmp3, "%d", nowCooking[3]);
                display.print(tmp3, 3, 6);
            }
        }
        else
        {
            if (nowCooking[2] != 0)
            {
                display.print("Id : ", 1, 1);
                sprintf(tmp, "%d", nowCooking[2]);
                display.print(tmp, 2, 6);
            }
            if (nowCooking[3] != 0)
            {
                display.print("Id : ", 2, 1);
                sprintf(tmp2, "%d", nowCooking[3]);
                display.print(tmp2, 2, 6);
            }
        }
        display.print(">", 2, 0);
    }
    else if (step == CHGSTAT)
    {
        chgnum = (int)tgtT % 3;
        display.print("*Change Status*");
        if (chgnum == 0)
        {
            display.print("Go Back", 2, 1);
            display.print("Change Timer", 3, 1);
        }
        else if (chgnum == 1)
        {
            display.print("Go Back", 1, 1);
            display.print("Change Timer", 2, 1);
            display.print("Cancle Cooking", 3, 1);
        }
        else
        {
            display.print("Change Timer", 1, 1);
            display.print("Cancle Cooking", 2, 1);
        }
        display.print(">", 2, 0);
    }
    else if (step == SETTIMER)
    {
        st = tgtT * 30;
        display.print("*Change Timer*");
        display.print("Timer : ", 2, 1);
        sprintf(tmp, "%d", st);
        display.print(tmp, 2, 10);
    }
    else if (step == CANCLE)
    {
        display.print("Cancle :");
        display.print("Id : ", 1, 1);
        sprintf(tmp2, "%d", nowCooking[statnum - 1]);
        display.print(tmp2, 1, 6);
        display.print("Yes", 2, 1);
        display.print("No", 3, 1);
        display.print(">", ((int)tgtT) % 2 + 2, 0);
    }
    else if (step == SELIND)
    {
        si = (int)tgtT % 4;
        display.print("*Select Induction*");
        display.print("Select : ", 2, 1);
        sprintf(tmp, "%d", (si+1));
        display.print(tmp, 2, 10);
    }
    else if (step == COOKALERT)
    {
        display.print("Recipe id", 1, 1);
        sprintf(tmp, "%d", foodselnum + 1);
        display.print(tmp, 1, 12);
        display.print("is now cooking", 2, 1);
        
        delay(2000);
        step = MAIN;
    }
    else if (step == ALREADYCOOKALERT)
    {
        display.print("Induction", 1, 1);
        sprintf(tmp, "%d", si);
        display.print(tmp, 1, 12);
        display.print("is already cooking", 2, 1);
        
        delay(2000);
        step = MAIN;
    }

    root.printTo(msgBuffer, sizeof(msgBuffer));
    client.publish(publishTopic, msgBuffer);
}

void handleUserCommand(JsonObject *root)
{
    JsonObject &d = (*root)["d"];
    //user command
    if (d.containsKey("id"))
    {
        listcnt = d["id"];
    }

    if (d.containsKey("power1"))
    {
        if (!strcmp(d["power1"], "off"))
        {
            nowCooking[0] = 0;
        }
        lastPublishMillis = 0;
    }

    if (d.containsKey("power2"))
    {
        if (!strcmp(d["power2"], "off"))
        {
            nowCooking[1] = 0;
        }
        lastPublishMillis = 0;
    }

    if (d.containsKey("power3"))
    {
        if (!strcmp(d["power3"], "off"))
        {
            nowCooking[2] = 0;
        }
        lastPublishMillis = 0;
    }

    if (d.containsKey("power4"))
    {
        if (!strcmp(d["power4"], "off"))
        {
            nowCooking[3] = 0;
        }
        lastPublishMillis = 0;
    }
}

void message(char *topic, byte *payload, unsigned int payloadLength)
{
    StaticJsonBuffer<512> jsonInBuffer;
    byte2buff(msgBuffer, payload, payloadLength);
    JsonObject &root = jsonInBuffer.parseObject((char *)msgBuffer);
    if (!root.success())
    {
        Serial.println("handleCommand: payload parse FAILED");
        return;
    }

    handleIOTCommand(topic, &root);
    if (!strcmp(updateTopic, topic))
    {
        pubInterval = (*cfg)["meta"]["pubInterval"];
    }
    else if (!strncmp(commandTopic, topic, 10))
    { // strcmp return 0 if both string matches
        handleUserCommand(&root);
    }
}

void buttonClicked()
{
    if (millis() - lastClicked >= 300)
    {
        lastClicked = millis();
        clicked = 1;
        lastPublishMillis = 0;
    }
}

void setup()
{
    Serial.begin(115200);
    pinMode(pushSW, INPUT_PULLUP);
    pinMode(pulseA, INPUT_PULLUP);
    pinMode(pulseB, INPUT_PULLUP);
    attachInterrupt(pushSW, buttonClicked, FALLING);
    attachInterrupt(pulseA, handleRotary, CHANGE);
    attachInterrupt(pulseB, handleRotary, CHANGE);
    display.begin();
    // display.print("Currnt: ", 2, 2);
    // display.print("Target: ", 3, 2);
    initCheck();
    // *** If no "config" is found or "config" is not "done", run configDevice ***
    if (!cfg->containsKey("config") || strcmp((const char *)(*cfg)["config"], "done"))
    {
        configDevice();
    }
    WiFi.mode(WIFI_STA);
    WiFi.begin((const char *)(*cfg)["ssid"], (const char *)(*cfg)["w_pw"]);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    // main setup
    Serial.printf("\nIP address : ");
    Serial.println(WiFi.localIP());
    JsonObject &meta = (*cfg)["meta"];
    pubInterval = meta.containsKey("pubInterval") ? atoi((const char *)meta["pubInterval"]) : 0;

    sprintf(iot_server, "%s", (const char *)(*cfg)["edge"]);
    if (!espClient.connect(iot_server, 1883))
    {
        Serial.println("connection failed");
        return;
    }
    client.setServer(iot_server, 1883); //IOT
    client.setCallback(message);
    iot_connect();
}

void loop()
{
    if (!client.connected())
    {
        iot_connect();
    }
    client.loop();
    if ((pubInterval != 0) && (millis() - lastPublishMillis > pubInterval))
    {
        publishData();
        lastPublishMillis = millis();
    }
}

void gettemperature()
{
    unsigned long currentMillis = millis();
    if (currentMillis - lastDHTReadMillis >= interval)
    {
        lastDHTReadMillis = currentMillis;
        humidity = dht.readHumidity();  // Read humidity (percent)
        temp_f = dht.readTemperature(); // Read temperature as Fahrenheit
        // Check if any reads failed and exit early (to try again).
        if (isnan(humidity) || isnan(temp_f))
        {
            return;
        }
    }
}
