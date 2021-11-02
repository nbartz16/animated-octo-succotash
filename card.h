enum rarity
{
   common,
   uncommon,
   rare,
   mythic
};

struct card{
   unsigned int id;
   char* name;
   char* cost;
   unsigned int converted_cost;
   char* type;
   char* text;
   char* stats;
   enum rarity rarity;
};

struct sortedcard{
   char* name;
   long offset;
};