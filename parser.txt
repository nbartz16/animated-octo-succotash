#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "card.h"
#include <stddef.h>
#include <ctype.h>

//Performs a linear search through CardStruct looking for the element with name CardName. When a matching element is found, the position of it in the array is then returned.
int LinearSearching(struct card** CardStruct,char* CardName,int i){
   int n = 0;
   while(n<i){
      if(strcmp(CardStruct[n]->name,CardName)==0){
         return n;
      }
      n++;
   }
   return 0;
}
//Ensures that the records are maintained in the order they were first read
void ShiftNameOffset(struct sortedcard** NameOffset,char* name,int k){
   int i = 0;
   int j = 0;
   while(strcmp(NameOffset[i]->name,name)!=0){
      i++;
   }
   for(j=i;j<k-1;j++){
      free(NameOffset[j]->name);
      NameOffset[j]->name = malloc(sizeof(char)*(strlen(NameOffset[j+1]->name)+1));
      NameOffset[j]->name = memmove(NameOffset[j]->name,NameOffset[j+1]->name,strlen(NameOffset[j+1]->name)+1);
   }

}
//Compare function for qsort. It compares the names of two cards and returns the difference.
int compare(const void* FirstItem, const void* SecondItem){
   struct card* FirstStruct = *(struct card**) FirstItem;
   struct card* SecondStruct = *(struct card**) SecondItem;
   return(strcmp(FirstStruct->name,SecondStruct->name));
}
//Second compare function for qsort.
int SecondCompare(const void* FirstItem, const void* SecondItem){
   struct sortedcard** FirstStruct = (struct sortedcard**) FirstItem;
   struct sortedcard** SecondStruct = (struct sortedcard**) SecondItem;
   return(strcmp(FirstStruct[0]->name,SecondStruct[0]->name));
}
//Shifts all the text in the current read line left one place.
void ShiftText(char* Pointer){
   while(Pointer[0]!='\0'){
      Pointer[0] = Pointer[1];
      Pointer++;
   }
}
//Parses the read card text to replace all instances of \n with a newline character, \"\" with \", and \", with a character indicating that it is the end of the record. 
void ParseText(char* Pointer){
   char* SecondPointer = Pointer;
   SecondPointer++;
   int Exit = 0;
   while(Exit == 0){
      if(Pointer[0] == '\"'){
         if(SecondPointer[0] == ','){
            Pointer[0] = '#';
            Exit = 1;
         }
         else{
            ShiftText(Pointer);
         }
      }
      else if(Pointer[0] == '\\'){
         if(SecondPointer[0] == 'n'){
            Pointer[0] = '\n';
            ShiftText(&Pointer[1]);
         }
      }
      Pointer++;
      SecondPointer++;
   }
}

int main(int argc, char* argv[]){

   struct card** CardStruct = NULL;
   struct sortedcard** NameOffset = NULL;
   char* CurrentLine = NULL;
   char* Pointer = NULL;
   char* Counting = NULL;
   int SizeInt = 0;
   size_t p = 0;
   u_int32_t k = 0;
   int i = 0;
   int n = 0;
   int j = 0;
   FILE* BinOutNames = NULL;
   FILE* BinOutText = NULL;
   FILE* CurrentFile = NULL;
   if(argc<=1){
      fprintf(stderr,"There needs to be an input file to be read\n");
      return -1;
   }
   //Attempt to create or open two binary files so that the results can be written there.
   BinOutNames = fopen("index.bin","wb");
   BinOutText = fopen("cards.bin","wb");
   if(!((CurrentFile = fopen(argv[1],"r"))!=NULL)){
      fprintf(stderr,"./parser: cannot open(\"%s\"): No such file or directory\n",argv[1]);
      return -2;
   }
   //Reads in the first line of the file, which contains no needed data.
   getline(&CurrentLine,&p,CurrentFile);
   free(CurrentLine);
   p = 0;
   CardStruct = malloc(sizeof(struct card*));
   //This loop will continue until EOF or an error in reading from the file.
   while(getline(&CurrentLine,&p,CurrentFile)!=-1){
      //Increment the number of cards by 1 and update the memory locations.
      i++;
      CardStruct = realloc(CardStruct,sizeof(struct card*)*(i));
      NameOffset = realloc(NameOffset,sizeof(struct sortedcard*)*(i));
      CardStruct[i-1] = malloc(sizeof(struct card));
      NameOffset[i-1] = malloc(sizeof(struct sortedcard));
      Pointer = CurrentLine;
      CardStruct[i-1]->id = atoi(strsep(&Pointer,","));
      Pointer++;
      Counting = Pointer;
      //Searches the line for the first instance of a given character so that an appropriate amount of memory can be allocated to hold it.
      while(Counting[0]!='\"'){
         Counting++;
         SizeInt++;
      }
      CardStruct[i-1]->name = malloc(sizeof(char)*(SizeInt+1));
      NameOffset[i-1]->name = malloc(sizeof(char)*(SizeInt+1));
      strcpy(CardStruct[i-1]->name,strsep(&Pointer,"\""));
      strcpy(NameOffset[i-1]->name,CardStruct[i-1]->name);
      //Checks to see if there is more than one instance of a given name in the CardStruct. If n==i-1, then the only record with the name is the record currently beind read.
      n = LinearSearching(CardStruct, CardStruct[i-1]->name,i);
      //If there is a second instance of the given name
      if(n != i-1){
         //Checks to see if the new ID is smaller than the previous. If so, then the record is updated using the line that is currently being read.
         if(CardStruct[n]->id<CardStruct[i-1]->id){
            CardStruct[n]->id = CardStruct[i-1]->id;
            free(CardStruct[n]->name);
            ShiftNameOffset(NameOffset,CardStruct[i-1]->name,i);
            CardStruct[n]->name = malloc(sizeof(char)*(SizeInt+1));
            CardStruct[n]->name = memmove(CardStruct[n]->name,CardStruct[i-1]->name,SizeInt+1);
            SizeInt = 0;
            Pointer++;
            //Checks to see if the cost field is blank, and if so, stores a null character.
            if(Pointer[0]==','){
               free(CardStruct[n]->cost);
               CardStruct[n]->cost = malloc(sizeof(char));
               strcpy(CardStruct[n]->cost,"\0");
            }
            //Updates the cost value using the most recently read line.
            else{
              Pointer++;
              Counting = Pointer;
              while(Counting[0]!='\"'){
                 Counting++;
                 SizeInt++;
              }
              free(CardStruct[n]->cost);
              CardStruct[n]->cost = malloc(sizeof(char)*(SizeInt+1));
              strcpy(CardStruct[n]->cost,strsep(&Pointer,"\""));

            }
            Pointer++;
            CardStruct[n]->converted_cost = atoi(strsep(&Pointer,","));  
            Pointer++;
            SizeInt = 0;
            Counting = Pointer;
            while(Counting[0]!='\"'){
               Counting++;
               SizeInt++;
            }
            //Updates the type value using the most recently read line.
            free(CardStruct[n]->type);
            CardStruct[n]->type = malloc(sizeof(char)*(SizeInt+1));
            strcpy(CardStruct[n]->type,strsep(&Pointer,"\""));
            SizeInt = 0;
            Pointer++;
            //Checks to see if the text field is blank, and stores a null character if it is.
            if(Pointer[0]==','){
               free(CardStruct[n]->text);
               CardStruct[n]->text = malloc(sizeof(char));
               strcpy(CardStruct[n]->text,"\0");
            }
            //Parses the text field
            else{
               ParseText(Pointer);
               Counting = Pointer;
               while(Counting[0]!='#'){
                  Counting++;
                  SizeInt++;
               }
               free(CardStruct[n]->text);
               CardStruct[n]->text = malloc(sizeof(char)*(SizeInt+1));
               strcpy(CardStruct[n]->text,strsep(&Pointer,"#"));
            }
            Pointer++;
            //Checks to see if the stats field is blank, and stores a null character if it is.
            if(Pointer[1]==','){
               free(CardStruct[n]->stats);
               CardStruct[n]->stats = malloc(sizeof(char));
               strcpy(CardStruct[n]->stats,"\0");
            }
            //Reads in the stats field if it is not blank, replacing the previous value
            else{
               SizeInt = 0;
               Pointer++;
               Counting = Pointer;
               while(Counting[0]!='\"'){
                  Counting++;
                  SizeInt++;
               }
               free(CardStruct[n]->stats);
               CardStruct[n]->stats = malloc(sizeof(char)*(SizeInt+1));
               strcpy(CardStruct[n]->stats,strsep(&Pointer,"\""));
            }
            //Keeps incrementing the Pointer as long as it is not a letter.
            while(!(isalpha(Pointer[0]))){
               Pointer++;
            }
            //Uses a switch statement to assign a value to the rarity based on the character Pointer is pointing to.
            switch(Pointer[0]){
               case 'c':
                  CardStruct[n]->rarity = common;
                  break;
               case 'u':
                  CardStruct[n]->rarity = uncommon;
                  break;
               case 'r':
                  CardStruct[n]->rarity = rare;
                  break;
               case 'm':
                  CardStruct[n]->rarity = mythic;
                  break;
               default:
                  break;
            }
            //Frees all memory that was associated with the current read line, decrements i, and returns to the beginning of this loop to read another record.
            free(CurrentLine);
            free(CardStruct[i-1]->name);
            free(NameOffset[i-1]->name);
            free(CardStruct[i-1]);
            free(NameOffset[i-1]);
            i--;
            p=0;
            SizeInt = 0;
            continue;
         }
         //If the id is less than the id of the already stored card, free all of the memory associated with the newly read card, decrement i, and return to the beginning of this loop to read another record.
         else{
            free(CurrentLine);
            free(CardStruct[i-1]->name);
            free(NameOffset[i-1]->name);
            free(NameOffset[i-1]);
            free(CardStruct[i-1]);
            i--;
            p=0;
            SizeInt = 0;
            continue;
         }
      } 
      Pointer++;
      //Checks to see if the cost field is blank and stores a null character if it is.
      if(Pointer[0]==','){
         CardStruct[i-1]->cost = malloc(sizeof(char));
         strcpy(CardStruct[i-1]->cost,"\0");
      }
      else{
         Pointer++;
         Counting = Pointer;
         //Reads in the value of the cost field.
         while(Counting[0]!='\"'){
            Counting++;
            SizeInt++;
         }
         CardStruct[i-1]->cost = malloc(sizeof(char)*(SizeInt+1));
         strcpy(CardStruct[i-1]->cost,strsep(&Pointer,"\""));
      }
      SizeInt = 0;
      Pointer++;
      //Reads in the value of the converted_cost field.
      CardStruct[i-1]->converted_cost = atoi(strsep(&Pointer,","));  

      Pointer++;
      Counting = Pointer;
      //Reads in the value of the type field.
      while(Counting[0]!='\"'){
         Counting++;
         SizeInt++;
      }
      CardStruct[i-1]->type = malloc(sizeof(char)*(SizeInt+1));
      strcpy(CardStruct[i-1]->type,strsep(&Pointer,"\""));
      SizeInt = 0;
      Pointer++;
      //Checks to see if the text field is blank and stores a null character if it is.
      if(Pointer[0]==','){
         CardStruct[i-1]->text = malloc(sizeof(char));
         strcpy(CardStruct[i-1]->text,"\0");
      }
      //If the text field is not blank, then the value is read into the CardStruct.
      else{
         ParseText(Pointer);
         Counting = Pointer;
         while(Counting[0]!='#'){
            Counting++;
            SizeInt++;
         }
         CardStruct[i-1]->text = malloc(sizeof(char)*(SizeInt+1));
         strcpy(CardStruct[i-1]->text,strsep(&Pointer,"#"));
      }
      Pointer++;
      //Checks to see if the stats field is blank and stores a null character if it is.
      if(Pointer[0]==','){
         CardStruct[i-1]->stats = malloc(sizeof(char));
         strcpy(CardStruct[i-1]->stats,"\0");
      }
      //If the stats field is not blank, then the value is read into the CardStruct.
      else{
         Pointer++;
         Counting = Pointer;
         SizeInt = 1;
         while(Counting[0]!='\"'){
            Counting++;
            SizeInt++;
         }
         CardStruct[i-1]->stats = malloc(sizeof(char)*SizeInt);
         strcpy(CardStruct[i-1]->stats,strsep(&Pointer,"\""));
      }
      //This loop increments the Pointer while it is not pointing at a letter.
      while(!(isalpha(Pointer[0]))){
         Pointer++;
      }
      //Checks the value at the Pointer and assigns the rarity field in the CardStruct a value based on it.
      switch(Pointer[0]){
         case 'c':
            CardStruct[i-1]->rarity = common;
            break;
         case 'u':
            CardStruct[i-1]->rarity = uncommon;
            break;
         case 'r':
            CardStruct[i-1]->rarity = rare;
            break;
         case 'm':
            CardStruct[i-1]->rarity = mythic;
            break;
         default:
            break;
      }
      //Frees the read line.
      free(CurrentLine);
      p = 0;
      SizeInt = 0;
   }
   //Writes all of the needed information to cards.bin
   qsort(CardStruct,i,sizeof(struct card*),compare);
   for(n=0;n<i;n++){
      j = 0;
      while(strcmp(NameOffset[n]->name,CardStruct[j]->name)!=0){
         j++;
      }
      NameOffset[n]->offset = ftell(BinOutText);
      fwrite(&CardStruct[j]->id,sizeof(unsigned int),1,BinOutText);
      //K is used to keep track of the length of a given field, so that the correct number of characters can be read in from the file when it is read.
      k = strlen(CardStruct[j]->cost);
      fwrite(&k,sizeof(u_int32_t),1,BinOutText);
      fwrite(CardStruct[j]->cost,sizeof(char),strlen(CardStruct[j]->cost),BinOutText);
      fwrite(&CardStruct[j]->converted_cost,sizeof(unsigned int),1,BinOutText);
      k = strlen(CardStruct[j]->type);
      fwrite(&k,sizeof(u_int32_t),1,BinOutText);
      fwrite(CardStruct[j]->type,sizeof(char),strlen(CardStruct[j]->type),BinOutText);
      k = strlen(CardStruct[j]->text);
      fwrite(&k,sizeof(u_int32_t),1,BinOutText);
      fwrite(CardStruct[j]->text,sizeof(char),strlen(CardStruct[j]->text),BinOutText);
      k = strlen(CardStruct[j]->stats);
      fwrite(&k,sizeof(u_int32_t),1,BinOutText);
      fwrite(CardStruct[j]->stats,sizeof(char),strlen(CardStruct[j]->stats),BinOutText);
      fwrite(&CardStruct[j]->rarity,sizeof(enum rarity),1,BinOutText);
   }
   //Performs a quicksort on NameOffset and CardStruct
   qsort(NameOffset,i,sizeof(struct sortedcard*),SecondCompare);
   //This loop writes  
   for(n = 0;n<i;n++){
      k = strlen(CardStruct[n]->name);
      fwrite(&k,sizeof(u_int32_t),1,BinOutNames);
      fwrite(CardStruct[n]->name,sizeof(char),strlen(CardStruct[n]->name),BinOutNames);
      fwrite(&NameOffset[n]->offset,sizeof(long),1,BinOutNames);
      //Frees the memory associated with the card that was just written to the binary files.
      free(CardStruct[n]->name);
      free(NameOffset[n]->name);
      free(CardStruct[n]->cost);
      free(CardStruct[n]->type);
      free(CardStruct[n]->text);
      free(CardStruct[n]->stats);
      free(CardStruct[n]);
      free(NameOffset[n]);
   }
   //Frees the rest of the memory used by the program.
   free(CurrentLine);
   free(CardStruct);
   free(NameOffset);
   fclose(BinOutNames);
   fclose(BinOutText);
   fclose(CurrentFile);
}
