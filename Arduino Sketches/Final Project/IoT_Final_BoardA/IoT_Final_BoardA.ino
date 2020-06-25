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

int statnum, chgnum, selnum;
int foodselnum;
int st;
int sc;
int sl;
int j = 0;
char tmp[5];

int cntind = 0;
int step = MAIN;

typedef struct
{
    char ing[30];
} ingredients;

typedef struct
{
    char food[20];
    char type[5];
    char timer[5];
    char ing[20][20];
} foods;

typedef struct treeNode
{
    foods foodlist;
    struct treeNode *left;
    struct treeNode *right;
} treeNode;

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

char nowCooking[4][20];

char listoffoods[100][20];

int listcnt = 0;

int nullcnt = 0;

int leftcnt = 0;

int rightcnt = 0;

foods foodlist[17];

treeNode *rt = NULL;
treeNode *temp2 = NULL;

void initFoods()
{
    int i = 0;
    int k = 0;
    int a = 0;

    while (k < 16)
    {
        if (k == 3)
        {
            strcpy(foodlist[k].type, "no");
        }
        else
        {
            strcpy(foodlist[k].type, "yes");
        }
        k++;
    }
    strcpy(foodlist[0].food, "tteokboki");
    strcpy(foodlist[1].food, "ramen");
    strcpy(foodlist[2].food, "Jeyuk bokkeum");
    strcpy(foodlist[3].food, "gimbab");
    strcpy(foodlist[4].food, "dumpling");
    strcpy(foodlist[5].food, "bibimbab");
    strcpy(foodlist[6].food, "kimchi stew");
    strcpy(foodlist[7].food, "omelet rice");
    strcpy(foodlist[8].food, "doenjang stew");
    strcpy(foodlist[9].food, "soft tofu stew");
    strcpy(foodlist[10].food, "Pork cutlet");
    strcpy(foodlist[11].food, "naengmyeon");
    strcpy(foodlist[12].food, "udon");
    strcpy(foodlist[13].food, "sujebi");
    strcpy(foodlist[14].food, "kalguksu");
    strcpy(foodlist[15].food, "tteokguk");
    strcpy(foodlist[16].food, "kimchi pilau");

    strcpy(foodlist[0].timer, "1200");
    strcpy(foodlist[1].timer, "300");
    strcpy(foodlist[2].timer, "600");
    strcpy(foodlist[3].timer, "300");
    strcpy(foodlist[4].timer, "900");
    strcpy(foodlist[5].timer, "300");
    strcpy(foodlist[6].timer, "900");
    strcpy(foodlist[7].timer, "600");
    strcpy(foodlist[8].timer, "900");
    strcpy(foodlist[9].timer, "900");
    strcpy(foodlist[10].timer, "600");
    strcpy(foodlist[11].timer, "420");
    strcpy(foodlist[12].timer, "600");
    strcpy(foodlist[13].timer, "900");
    strcpy(foodlist[14].timer, "900");
    strcpy(foodlist[15].timer, "900");
    strcpy(foodlist[16].timer, "600");

    /*---------------init ingredients start---------------*/
    //tteokboki
    strcpy(foodlist[0].ing[0], "tteok");
    strcpy(foodlist[0].ing[1], "gochujang");
    strcpy(foodlist[0].ing[2], "vegitable");
    strcpy(foodlist[0].ing[3], "water");
    //ramen
    strcpy(foodlist[1].ing[0], "noodle");
    strcpy(foodlist[1].ing[1], "ramen soup");
    strcpy(foodlist[1].ing[2], "water");
    strcpy(foodlist[1].ing[3], "vegetables");
    strcpy(foodlist[1].ing[4], "welsh onion");
    //Jeyuk bokkeum
    strcpy(foodlist[2].ing[0], "spicy pork");
    strcpy(foodlist[2].ing[1], "carrot");
    strcpy(foodlist[2].ing[2], "cabbage");
    strcpy(foodlist[2].ing[3], "rice");
    strcpy(foodlist[2].ing[4], "lettuce");
    strcpy(foodlist[2].ing[5], "welsh onion");
    //gimbab
    strcpy(foodlist[3].ing[0], "dried laver");
    strcpy(foodlist[3].ing[1], "rice");
    strcpy(foodlist[3].ing[2], "ham");
    strcpy(foodlist[3].ing[3], "pickled radish");
    strcpy(foodlist[3].ing[4], "cucmber");
    strcpy(foodlist[3].ing[5], "carrot");
    //dumpling
    strcpy(foodlist[4].ing[0], "chopped meat");
    strcpy(foodlist[4].ing[1], "flour");
    strcpy(foodlist[4].ing[2], "cellophane noodles");
    strcpy(foodlist[4].ing[3], "tofu");
    strcpy(foodlist[4].ing[4], "salt");
    strcpy(foodlist[4].ing[5], "chives");
    strcpy(foodlist[4].ing[6], "black pepper");
    //bibimbab
    strcpy(foodlist[5].ing[0], "rice");
    strcpy(foodlist[5].ing[1], "fried egg");
    strcpy(foodlist[5].ing[2], "gochujang");
    strcpy(foodlist[5].ing[3], "carrot");
    strcpy(foodlist[5].ing[4], "been sprouts");
    strcpy(foodlist[5].ing[5], "zucchini");
    strcpy(foodlist[5].ing[6], "sesame oil");
    //kimchi stew
    strcpy(foodlist[6].ing[0], "pork");
    strcpy(foodlist[6].ing[1], "kimchi");
    strcpy(foodlist[6].ing[2], "water");
    strcpy(foodlist[6].ing[3], "welsh onion");
    strcpy(foodlist[6].ing[4], "tofu");
    strcpy(foodlist[6].ing[5], "red pepper powder");
    //omelet rice
    strcpy(foodlist[7].ing[0], "eggs");
    strcpy(foodlist[7].ing[1], "rice");
    strcpy(foodlist[7].ing[2], "oil");
    strcpy(foodlist[7].ing[3], "zucchini");
    strcpy(foodlist[7].ing[4], "carrot");
    strcpy(foodlist[7].ing[5], "onion");
    strcpy(foodlist[7].ing[6], "salt");
    strcpy(foodlist[7].ing[7], "black pepper");
    strcpy(foodlist[7].ing[8], "ketchup");
    //doenjang stew
    strcpy(foodlist[8].ing[0], "bean paste");
    strcpy(foodlist[8].ing[1], "tofu");
    strcpy(foodlist[8].ing[2], "zucchini");
    strcpy(foodlist[8].ing[3], "onion");
    strcpy(foodlist[8].ing[4], "welsh onion");
    strcpy(foodlist[8].ing[5], "kelp");
    //soft tofu stew
    strcpy(foodlist[9].ing[0], "soft tofu");
    strcpy(foodlist[9].ing[1], "water");
    strcpy(foodlist[9].ing[2], "zucchini");
    strcpy(foodlist[9].ing[3], "gochujang");
    strcpy(foodlist[9].ing[4], "salt");
    strcpy(foodlist[9].ing[5], "eggs");
    strcpy(foodlist[9].ing[6], "welsh onion");
    strcpy(foodlist[9].ing[7], "red pepper");
    strcpy(foodlist[9].ing[8], "crushed garlic");
    strcpy(foodlist[9].ing[9], "onion");
    //Pork cutlet
    strcpy(foodlist[10].ing[0], "pork loin");
    strcpy(foodlist[10].ing[1], "bread crumb");
    strcpy(foodlist[10].ing[2], "flour");
    strcpy(foodlist[10].ing[3], "eggs");
    strcpy(foodlist[10].ing[4], "salt");
    strcpy(foodlist[10].ing[5], "black pepper");
    strcpy(foodlist[10].ing[6], "pork cutlet sauce");
    //naengmyeon
    strcpy(foodlist[11].ing[0], "cold noodle");
    strcpy(foodlist[11].ing[1], "cucumber");
    strcpy(foodlist[11].ing[2], "brisket");
    strcpy(foodlist[11].ing[3], "water");
    strcpy(foodlist[11].ing[4], "salt");
    strcpy(foodlist[11].ing[5], "cucumber");
    strcpy(foodlist[11].ing[6], "pear");
    strcpy(foodlist[11].ing[7], "onion");
    strcpy(foodlist[11].ing[8], "garlic");
    strcpy(foodlist[11].ing[9], "ginger");
    strcpy(foodlist[11].ing[10], "dongchimi");
    strcpy(foodlist[11].ing[11], "radish");
    //udon
    strcpy(foodlist[12].ing[0], "fish cake");
    strcpy(foodlist[12].ing[1], "thick noodle");
    strcpy(foodlist[12].ing[2], "katsuoh yuksu");
    strcpy(foodlist[12].ing[3], "crushed garlic");
    strcpy(foodlist[12].ing[4], "crown daisy");
    strcpy(foodlist[12].ing[5], "red pepper");
    strcpy(foodlist[12].ing[6], "black pepper");
    strcpy(foodlist[12].ing[7], "welsh onion");
    //sujebi
    strcpy(foodlist[13].ing[0], "dough of flour");
    strcpy(foodlist[13].ing[1], "zucchini");
    strcpy(foodlist[13].ing[2], "carrot");
    strcpy(foodlist[13].ing[3], "potato");
    strcpy(foodlist[13].ing[4], "anchovy");
    strcpy(foodlist[13].ing[5], "crushed garlic");
    strcpy(foodlist[13].ing[6], "onion");
    strcpy(foodlist[13].ing[7], "welsh onion");
    //kalguksu
    strcpy(foodlist[14].ing[0], "dough of flour");
    strcpy(foodlist[14].ing[1], "zucchini");
    strcpy(foodlist[14].ing[2], "carrot");
    strcpy(foodlist[14].ing[3], "potato");
    strcpy(foodlist[14].ing[4], "anchovy");
    strcpy(foodlist[14].ing[5], "crushed garlic");
    strcpy(foodlist[14].ing[6], "onion");
    strcpy(foodlist[14].ing[7], "welsh onion");
    //tteokguk
    strcpy(foodlist[15].ing[0], "bar rice cake");
    strcpy(foodlist[15].ing[1], "egg");
    strcpy(foodlist[15].ing[2], "welsh onion");
    strcpy(foodlist[15].ing[3], "crushed garlic");
    strcpy(foodlist[15].ing[4], "black pepper");
    strcpy(foodlist[15].ing[5], "salt");
    strcpy(foodlist[15].ing[6], "soy sauce");
    strcpy(foodlist[15].ing[7], "brisket");
    //kimchi fried rice
    strcpy(foodlist[16].ing[0], "rice");
    strcpy(foodlist[16].ing[1], "oil");
    strcpy(foodlist[16].ing[2], "kimchi");
    strcpy(foodlist[16].ing[3], "ham");
    strcpy(foodlist[16].ing[4], "sesame oil");
    strcpy(foodlist[16].ing[5], "egg");
    strcpy(foodlist[16].ing[6], "onion");
    strcpy(foodlist[16].ing[7], "stir-fried meat");
    /*---------------init ingredients end---------------*/

    /*---------------Insert node---------------*/
    for (int x = 0; x < 17; x++)
    {
        insert(&rt, foodlist[x]);
    }

    // for(i=0;i<17;i++){
    //     insert(&root, *foodlist[i]);
    // }
}

treeNode *insertKey(treeNode *p, foods foodlist)
{
    treeNode *newNode;
    int compare;

    if (p == NULL)
    {
        newNode = (treeNode *)malloc(sizeof(treeNode));
        newNode->foodlist = foodlist;
        newNode->left = NULL;
        newNode->right = NULL;

        strcpy(listoffoods[listcnt], foodlist.food);
        listcnt++;

        nullcnt++;

        return newNode;
    }
    else
    {
        compare = strcmp(foodlist.food, p->foodlist.food);
        if (compare < 0){
            leftcnt++;
            p->left = insertKey(p->left, foodlist);
        }
        else if (compare > 0){
            rightcnt++;
            p->right = insertKey(p->right, foodlist);
        }
        else
            //            printf("\n �̹� ���� �Ǿ� �ֽ��ϴ�! \n");
            return p;
    }
}

void insert(treeNode **root, foods foodlist)
{
    *root = insertKey(*root, foodlist);
}

/*
void deleteNode(treeNode *root, foods foodlist)
{
    treeNode *parent, *p, *succ, *succ_parent;
    treeNode *child;
    parent = NULL;
    p = root;
    char confirm;
    
    while(1) {
        while (getchar() != '\n');
        printf("\n���� ����Ͻðڽ��ϱ�? [Y/N] : ");
        scanf("%c", &confirm);
        if (confirm == 'n' || confirm == 'N') {
            printf("\n���� ���� �޴��� ���ư��ϴ�.\n");
            getchar();
            return;
        }
        else if (confirm == 'y' || confirm == 'Y') {
            break;
        } else;
    }

    while ((p != NULL) && (strcmp(p->key.name, key.name) != 0))
    {
        parent = p;
        if (strcmp(key.name, p->key.name) < 0)
            p = p->left;
        else
            p = p->right;
    }

    p->key.movie->seat--;

    // ������ ��尡 ���� ���
    if (p == NULL)
    {
        printf("\n �� �̸����� �� ���� ������ �����ϴ�!!");
        return;
    }
    // ������ ��尡 �ܸ� ����� ���
    if ((p->left == NULL) && (p->right == NULL))
    {
        if (parent != NULL)
        {
            if (parent->left == p)
                parent->left = NULL;
            else
                parent->right = NULL;
        }
        else
            root = NULL;
    }
    // ������ ��尡 �ڽ� ��带 �� �� ���� ���
    else if ((p->left == NULL) || (p->right == NULL))
    {
        if (p->left != NULL)
            child = p->left;
        else
            child = p->right;
        if (parent != NULL)
        {
            if (parent->left == p)
                parent->left = child;
            else
                parent->right = child;
        }
        else
            root = child;
    }
    // ������ ��尡 �ڽ� ��带 �� �� ���� ���
    else
    {
        succ_parent = p;
        succ = p->right;
        while (succ->left != NULL)
        {
            succ_parent = succ;
            succ = succ->left;
        }
        if (succ_parent->left == succ)
            succ_parent->left = succ->right;
        else
            succ_parent->right = succ->right;
        p->key = succ->key;
        p = succ;
    }
    printf("\n����� ���� ��ȣ�� %s �Դϴ�\n", p->key.reserveNum);
    free(p);
    printf("\n���Ű� ��ҵǾ����ϴ�.\n");
}
*/

treeNode *searchBST(treeNode *root, foods foodlist)
{
    treeNode *p;
    int compare;
    p = root;
    while (p != NULL)
    {
        compare = strcmp(foodlist.food, p->foodlist.food);
        if (compare < 0)
            p = p->left;
        else if (compare > 0)
            p = p->right;
        else
        {
            Serial.println(p->foodlist.food);
            return p;
        }
    }
    return p;
}

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
    char temp;
    char ctimer[5];
    foods f;

    //    while(listoffoods[j] != NULL){
    //        j++;
    //    }

    // data["temperature"]  = yourData;
    if (clicked == 1)
    {
        Serial.print("Null count : ");
        Serial.println(nullcnt);
        Serial.print("Left count : ");
        Serial.println(leftcnt);
        Serial.print("Right count : ");
        Serial.println(rightcnt);
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
            if (cntind < 4 && foodselnum > 0)
            { //현재 요리중인 것이 4개보다 작을 때만 publish
                strcpy(f.food, listoffoods[4]);
                temp2 = searchBST(rt, f);
                if (temp2 != NULL)
                {
                    data["food"] = temp2->foodlist.food;
                    //                data["food"] = "test";
//                    data["type"] = temp2->foodlist.type;
//                    data["timer"] = temp2->foodlist.timer;
//                    for (int i = 0; i < 20; i++)
//                    {
//                        if(temp2->foodlist.ing[i][0] != '\0')
//                          data["ingredients"][i] = temp2->foodlist.ing[i];
//                    }
                }else;
                data["button"] = "clicked";
//                strcpy(nowCooking[cntind], listoffoods[(foodselnum - 1)]);
//                cntind++;
//                temp = 48 + cntind;
//                data["cntind"] = (char *)temp;
                step = MAIN;
            }
            else if (cntind >= 4)
            {
                display.print("                 ", 0, 0);
                display.print("                 ", 1, 0);
                display.print("                 ", 2, 0);
                display.print("                 ", 3, 0);
                display.print("Already fulled!");
                delay(2000);
                step = MAIN;
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
                step = CHGSTAT;
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
            else
            {
                step = STLEVEL;
            }
        }
        else if (step == SETTIMER)
        {
            data["button"] = "settimer";
            data["sel_food"] = nowCooking[statnum - 1];
            data["set_timer"] = tmp;
            step = MAIN;
        }
        else if (step == CANCLE)
        {
            if (((int)tgtT) % 2)
                step = MAIN;
            else
            {
                cntind--;
                data["button"] = "cncfood";
                data["cnc_food"] = nowCooking[statnum - 1];
                temp = 48 + cntind;
                data["cntind"] = (char *)temp;
                nowCooking[statnum - 1][0] = '\0';
                step = MAIN;
            }
        }
        else if (step == STLEVEL)
        {
            if (sl == 0)
                step = MAIN;
            else if (sl == 1)
            {
                data["button"] = "stlevel";
                data["st_level"] = "Low";
                step = MAIN;
            }
            else if (sl == 2)
            {
                data["button"] = "stlevel";
                data["st_level"] = "Midium";
                step = MAIN;
            }
            else if (sl == 3)
            {
                data["button"] = "stlevel";
                data["st_level"] = "High";
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
    tgtT = map(encoderValue, 0, 255, 0, 50);
    sprintf(dht_buffer2, "%2.1f", tgtT);
    //data["target"] = dht_buffer2;
    // display.print(dht_buffer2, 3, 10);
    display.print("                 ", 0, 0);
    display.print("                 ", 1, 0);
    display.print("                 ", 2, 0);
    display.print("                 ", 3, 0);

    if (step == SELMENU)
    {
        foodselnum = (int)tgtT % (listcnt + 1);
        display.print("*Select Menu*");
        if (foodselnum == 0)
        {
            display.print("Go Back", 2, 1);
            display.print(listoffoods[(int)foodselnum], 3, 1);
        }
        else if (foodselnum == 1)
        {
            display.print("Go Back", 1, 1);
            display.print(listoffoods[(int)foodselnum - 1], 2, 1);
            display.print(listoffoods[(int)foodselnum], 3, 1);
        }
        else if (foodselnum == (listcnt))
        {
            display.print(listoffoods[(int)foodselnum - 2], 1, 1);
            display.print(listoffoods[(int)foodselnum - 1], 2, 1);
        }
        else
        {
            display.print(listoffoods[(int)foodselnum - 2], 1, 1);
            display.print(listoffoods[(int)foodselnum - 1], 2, 1);
            display.print(listoffoods[(int)foodselnum], 3, 1);
        }
        display.print(">", 2, 0);
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
            if (nowCooking[0] != NULL)
                display.print(nowCooking[0], 3, 1);
        }
        else if (statnum == 1)
        {
            display.print("Go Back", 1, 1);
            if (nowCooking[0] != NULL)
                display.print(nowCooking[0], 2, 1);
            if (nowCooking[1] != NULL)
                display.print(nowCooking[1], 3, 1);
        }
        else if (statnum == 2)
        {
            if (nowCooking[0] != NULL)
                display.print(nowCooking[0], 1, 1);
            if (nowCooking[1] != NULL)
                display.print(nowCooking[1], 2, 1);
            if (nowCooking[2] != NULL)
                display.print(nowCooking[2], 3, 1);
        }
        else if (statnum == 3)
        {
            if (nowCooking[1] != NULL)
                display.print(nowCooking[1], 1, 1);
            if (nowCooking[2] != NULL)
                display.print(nowCooking[2], 2, 1);
            if (nowCooking[3] != NULL)
                display.print(nowCooking[3], 3, 1);
        }
        else
        {
            if (nowCooking[2] != NULL)
                display.print(nowCooking[2], 1, 1);
            if (nowCooking[3] != NULL)
                display.print(nowCooking[3], 2, 1);
        }
        display.print(">", 2, 0);
    }
    else if (step == CHGSTAT)
    {
        chgnum = (int)tgtT % 4;
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
        else if (chgnum == 2)
        {
            display.print("Change Timer", 1, 1);
            display.print("Cancle Cooking", 2, 1);
            display.print("Set Therm Level", 3, 1);
        }
        else
        {
            display.print("Cancle Cooking", 1, 1);
            display.print("Set Therm Level", 2, 1);
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
        display.print(nowCooking[statnum - 1], 1, 1);
        display.print("Yes", 2, 1);
        display.print("No", 3, 1);
        display.print(">", ((int)tgtT) % 2 + 2, 0);
    }
    else if (step == STLEVEL)
    {
        sl = (int)tgtT % 4;
        display.print("*Set Therm Level*");

        if (sl == 0)
        {
            display.print("Go Back", 2, 1);
            display.print("Low", 3, 1);
        }
        else if (sl == 1)
        {
            display.print("Go Back", 1, 1);
            display.print("Low", 2, 1);
            display.print("Midium", 3, 1);
        }
        else if (sl == 2)
        {
            display.print("Low", 1, 1);
            display.print("Midium", 2, 1);
            display.print("High", 3, 1);
        }
        else
        {
            display.print("Midium", 1, 1);
            display.print("High", 2, 1);
        }
        display.print(">", 2, 0);
    }
    // data["food"] = listoffoods[(int)tgtT-9]->food;

    root.printTo(msgBuffer, sizeof(msgBuffer));
    client.publish(publishTopic, msgBuffer);
}

void handleUserCommand(JsonObject *root)
{
    JsonObject &d = (*root)["d"];
    if (d.containsKey("target"))
    {
        tgtT = atoi(d["target"]);
        encoderValue = map(tgtT, 10, 50, 0, 255);
        lastPublishMillis = 0;
    }
    // user command
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
    initFoods();
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
