#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

char nowCooking[4][20];

char listoffoods[100][20];

int listcnt = 0;

int nullcnt = 0;

foods foodlist[17];

//treeNode *rt = NULL;
treeNode *temp2 = NULL;

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
    }else{
        compare = strcmp(foodlist.food, p->foodlist.food);
        if (compare < 0)
            p->left = insertKey(p->left, foodlist);
        else if (compare > 0)
            p->right = insertKey(p->right, foodlist);
        else
//            printf("\n ??? ???? ??? ??????! \n");
        return p;
    }
}

void insert(treeNode **root, foods foodlist)
{
    *root = insertKey(*root, foodlist);
}

void main()
{
    treeNode *rt = NULL;
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
    for(int x = 0; x < 17; x++){
        insert(&rt, foodlist[x]);
    }
	printf("\nNull count is %d\n", nullcnt);
    
    // for(i=0;i<17;i++){
    //     insert(&root, *foodlist[i]);
    // }
}
