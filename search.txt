#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "card.h"
#include <stddef.h>


//Compare function for bsearch, which compares the user's input to a name in the struct of sortedcards.
int compare(const void* FirstItem, const void* SecondItem){
   char* FirstName = (char*) FirstItem;
   struct sortedcard** SecondStruct = (struct sortedcard**) SecondItem;
   return(strcmp(FirstName,SecondStruct[0]->name));
}

int main(int argc, char* argv[]){

   u_int32_t StringLength = 0;
   int i = 0;
   int n = 0;
   size_t p = 0;
   char* UserInput = malloc(sizeof(char)*2000);
   char* SearchName = NULL;
   struct sortedcard** SortedCards = malloc(sizeof(struct sortedcard*));
   struct sortedcard** FoundCard = NULL;
   struct card* SingleCard = malloc(sizeof(struct card));
   SingleCard->name = NULL;
   SingleCard->type = NULL;
   SingleCard->cost = NULL;
   SingleCard->text = NULL;
   SingleCard->stats = NULL;
   FILE* BinCardNames = NULL;
   FILE* BinCardText = NULL;
   //Attempts to open index.bin. If it is unsuccessful, an error message is printed indicating this and the program is exited.
   if((BinCardNames = fopen("index.bin","rb"))==NULL){
      fprintf(stderr,"./search: cannot open(\"%s\"): No such file or directory\n","index.bin");
      free(SortedCards);
      free(SingleCard);
      free(UserInput);
      return -1;
   }
   //Attempts to open cards.bin. If it is unsuccessful, an error message is printed indicating this and the program is exited.
   BinCardText = fopen("cards.bin","rb");
   //This loop reads in the values from index.bin and stores them in the SortedCards array.
   while((fread(&StringLength,sizeof(u_int32_t),1,BinCardNames))>0){
      i++;
      SortedCards = realloc(SortedCards,sizeof(struct sortedcard*)*(i));
      SortedCards[i-1] = malloc(sizeof(struct sortedcard));
      SortedCards[i-1]->name = malloc(sizeof(char)*(StringLength+1));
      fread(SortedCards[i-1]->name,sizeof(char),StringLength,BinCardNames);
      SortedCards[i-1]->name[StringLength] = '\0';
      fread(&SortedCards[i-1]->offset,sizeof(long),1,BinCardNames);
   }
   //This loop takes user input, and exits if it is Q or q. Otherwise, it attempts to find the card in the SortedCards array. If successful, the data associated with the card is printed. If unsuccessful, an error message is printed.
   do{
      fgets(UserInput,2000,stdin);
      //Checks to see if the user input is from a terminal. If it is not, the input is then printed to the terminal.
      if(isatty(0)!=1){
         printf(">> %s",UserInput);
      }
      SearchName = malloc(sizeof(char*)*strlen(UserInput));
      SearchName = strcpy(SearchName,UserInput);
      SearchName[strlen(UserInput)-1] = '\0';
      //Attempts to find the input card name using bsearch.
      FoundCard = bsearch(SearchName,SortedCards,i,sizeof(struct sortedcard*),compare);
      //If a card was found, the records associated with the card are then read in and printed.
      if(FoundCard!=NULL){
         //Sets cards.bin to the appropriate position for the current card.
         fseek(BinCardText,FoundCard[0]->offset,SEEK_SET);
         //Reads in the card's id.
         fread(&SingleCard->id,sizeof(unsigned int),1,BinCardText);
         SingleCard->name = FoundCard[0]->name;
         //Reads in the size of the record to be read.
         fread(&StringLength,sizeof(u_int32_t),1,BinCardText);
         //Reads in the card's cost.
         SingleCard->cost = malloc(sizeof(char)*(StringLength+1));
         fread(SingleCard->cost,sizeof(char),StringLength,BinCardText);
         SingleCard->cost[StringLength] = '\0';
         //Reads in the card's converted cost.
         fread(&SingleCard->converted_cost,sizeof(unsigned int),1,BinCardText);
         //Reads in the size of the record to be read.
         fread(&StringLength,sizeof(u_int32_t),1,BinCardText);
         //Reads in the card's type.
         SingleCard->type = malloc(sizeof(char)*(StringLength+1));
         fread(SingleCard->type,sizeof(char),StringLength,BinCardText);
         SingleCard->type[StringLength] = '\0';
         //Reads in the size of the record to be read.
         fread(&StringLength,sizeof(u_int32_t),1,BinCardText);
         //Reads in the card's text.
         SingleCard->text = malloc(sizeof(char)*(StringLength+1));
         fread(SingleCard->text,sizeof(char),StringLength,BinCardText);
         SingleCard->text[StringLength] = '\0';
         //Reads in the size of the record to be read.
         fread(&StringLength,sizeof(u_int32_t),1,BinCardText);
         //Reads in the card's stats.
         SingleCard->stats = malloc(sizeof(char)*(StringLength+1));
         fread(SingleCard->stats,sizeof(char),StringLength,BinCardText);
         SingleCard->stats[StringLength] = '\0';
         //Reads in the card's rarity.
         fread(&SingleCard->rarity,sizeof(enum rarity),1,BinCardText);
         //Outputs the card's information
         printf("%s",SingleCard->name);
         for(p=0;p<(52-strlen(SingleCard->name)-strlen(SingleCard->cost));p++){
            printf(" ");
         }
         printf("%s\n",SingleCard->cost);
         printf("%s",SingleCard->type);
         switch(SingleCard->rarity){
            case 0:
               for(p=0;p<(46-strlen(SingleCard->type));p++){
                  printf(" ");
               }
               printf("common\n");
               break;
            case 1:
               for(p=0;p<(44-strlen(SingleCard->type));p++){
                  printf(" ");
               }
               printf("uncommon\n");      
               break;
            case 2:
               for(p=0;p<(48-strlen(SingleCard->type));p++){
                  printf(" ");
               }  
               printf("rare\n");
               break;
            case 3:
               for(p=0;p<(46-strlen(SingleCard->type));p++){
                  printf(" ");
               }
               printf("mythic\n");
               break;
            default:
               fprintf(stderr,"No valid rarity found\n");
               break;
            }
         printf("----------------------------------------------------\n");
         printf("%s\n",SingleCard->text);
         printf("----------------------------------------------------\n");
         printf("%52s\n\n",SingleCard->stats);
         //Frees all of the associated fields and sets the values back to NULL.
         free(SingleCard->cost);
         free(SingleCard->type);
         free(SingleCard->text);
         free(SingleCard->stats);
         SingleCard->name = NULL;
         SingleCard->type = NULL;
         SingleCard->cost = NULL;
         SingleCard->text = NULL;
         SingleCard->stats = NULL;
      }
      //If the input is not Q or q and the card is not found, outputs an error.
      else if(strcmp(SearchName,"q")!=0 && strcmp(SearchName,"Q")!=0){
         printf("./search: \'%s\' not found!\n",SearchName);
      }
      //Frees the memory associated with the SearchName field.
      free(SearchName);
      //Exits the loop if the user's input was Q or q.
   }while(strcmp(UserInput,"q\n")!=0 && strcmp(UserInput,"Q\n")!=0);
   //Frees all of the memory associated with the SortedCards array.
   for(n=i-1;n>=0;n--){
      free(SortedCards[n]->name);
      free(SortedCards[n]);
   }
   //Frees the rest of the memory used in this program.
   free(SortedCards);
   free(SingleCard);
   free(UserInput);
   fclose(BinCardNames);
   fclose(BinCardText);
}
