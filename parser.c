#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "card.h"

//Comparison function which compares two cards on the basis of name and then on the basis of their id and put them in ascending order
int comp_func (const void *a, const void *b){
	
	struct card **first_card = (struct card **)a;
	struct card **second_card = (struct card **)b;
	
	//Use of strcmp() to compare two card names
	int ret_val = strcmp((*first_card)->name, (*second_card)->name);
	
	//Determining ascending order on the basis of card id
	if (ret_val == 0) {
		if ((*first_card)->id < (*second_card)->id){
			
			ret_val = -1;
		} else if ((*first_card)->id > (*second_card)->id) {
			
			ret_val = 1;
		} else {
			
			ret_val = 0;
		}
	}
		
	return ret_val;
}

int main(int argc, char **argv){
	
	//valgrind --leak-check=full ./parser
	//printf("Hello, World!!\n");
	
	char *buf = NULL;
	size_t bufsize = 0;
	
	//Open the file consisting all the data
	FILE *input_file = fopen(argv[1], "r");
	
	//Check if the file exists or not and display stderr output
	if (input_file == NULL) {
		fprintf(stderr, "./parser: cannot open(\"%s\"): No such file or directory\n", argv[1]);
		exit(1);
	}
	
	ssize_t num;
	getline(&buf, &bufsize, input_file);
	 
	int num_cards = 200000;
	struct card **cards = NULL;
	
	//Allocate the memory for pointers to the pointers of struct card
	cards = realloc(cards, sizeof(struct card*) * num_cards);
	
	int i = 0;
	int j = 0;
	char *address_buf[num_cards];
	while( (num = getline(&buf, &bufsize, input_file)) > 0 ){
		
		//Saves the starting address which needs to be freed later in the program
		address_buf[j] = strdup(buf);
		
		
		//Save the data line by line from file read each field needed to make cards
		char *stringp = address_buf[j];
		if (stringp[strlen(stringp) - 1] == '\n'){
			stringp[strlen(stringp) - 1] = '\0';
		}
		
		//Allocate memory for each card
		cards[i] = malloc(sizeof(struct card));
		
		unsigned int id = atoi(strsep(&stringp, ","));
		
		//Extract id field for card
		cards[i]->id = id;
		
		stringp++;
		//Extract name field for card
		cards[i]->name = strsep(&stringp, "\"");
		
		stringp++;
		
		//Extract cost field for card if it exists
		if (stringp[0] == '\"'){
			stringp++;
			cards[i]->cost = strsep(&stringp, "\"");
			
		} else {
			cards[i]->cost = strsep(&stringp, ",");
		}
		
		stringp++;
		unsigned int converted_cost = atoi(strsep(&stringp, ","));
		
		//Extract converted cost field for card
		cards[i]->converted_cost = converted_cost;
		stringp++;
		
		//Extract type field for card
		cards[i]->type = strsep(&stringp, "\"");
		stringp+=2;
		
		//Extract text field for card
		cards[i]->text = stringp;
		
		//Reach the last character for backtracking
		stringp += strlen(stringp);
		
		int count = 0;
		
		while(count < 2){
			
			char *find = stringp;
			
			//Extracting rarity field for card
			if (find[0] == ','){
				count++;
				if (count == 1) {
					stringp += 2;
					char *rr = strsep(&stringp, "\"");
					size_t rr_size = strlen(rr);
					if (strcmp(rr, "common") == 0){
						cards[i]->rarity = common;
					} else if(strcmp(rr, "uncommon") == 0){
						cards[i]->rarity = uncommon;
					} else if(strcmp(rr, "rare") == 0){
						cards[i]->rarity = rare;
					} else{
						cards[i]->rarity = mythic;
					}
					
					stringp = stringp - rr_size - 3;
				}
				
				//Extract stats field for card
				if (find[1] == ','){
					cards[i]->stats = "";
					
				} else if (count == 2) {
					stringp += 2;
					char *stats = strsep(&stringp, "\"");
					size_t stats_size = strlen(stats);
					cards[i]->stats = stats;
					stringp = stringp - stats_size -3;
				}
			}
			
			stringp--;
		}
		
		//Extracting and manipulating text field (extracting new line, double quotes and so on)
		if (stringp[0] == '\"'){
			stringp[0] = '\0';
			int s = 0;
			for (int r = 0;  r < strlen(cards[i]->text); r++){
			
				if(cards[i]->text[r] == '\\'){
					if(cards[i]->text[r+1]  == 'n'){
						cards[i]->text[s] = '\n';
						s++;
						r++;
					continue;
					}
				}
				
				if(cards[i]->text[r] == '\"'){
					if(cards[i]->text[r+1]  == '\"'){
					continue;
					}
				}
			
			cards[i]->text[s] = cards[i]->text[r];
			s++;
			}
			cards[i]->text[s] = '\0';
		} else if (stringp[0] == ',') {
			cards[i]->text = "";
		}
		
		j++;
		i++;
	}

	qsort(cards, i, sizeof(struct card*), comp_func);
	
	//52 width for printing characters
	int alignment = 52;
	
	for(int final = 0; final < i; final++){
		
		//Get the string form for rarity field od card
		char *rare_value;
		if(cards[final]->rarity == 0){
			rare_value = "common";
		}else if(cards[final]->rarity == 1){
			rare_value = "uncommon";
		}else if(cards[final]->rarity == 2){
			rare_value = "rare";
		} else {
			rare_value = "mythic";
		}
		
		//Print the information for final card of the file
		if(final == i - 1){
			
			printf("%s%*s\n", cards[final]->name, alignment - (int)strlen(cards[final]->name), cards[final]->cost);
		
			printf("%s%*s\n", cards[final]->type, alignment - (int)strlen(cards[final]->type), rare_value);
			
			for(int k = 0; k < alignment; k++){
				printf("-");
			}
			printf("\n");
			
			printf("%s\n", cards[final]->text);
					
			for(int k = 0; k < alignment; k++){
				printf("-");
			}
			printf("\n");
		
			printf("%*s\n", alignment, cards[final]->stats);
			printf("\n");
			
			break;
		}
		
		//Ignore the duplicate cards
		if(strcmp(cards[final]->name, cards[final+1]->name) == 0){
			
			continue;
		}
		
		//Printing all the valid cards from the file
		printf("%s%*s\n", cards[final]->name, alignment - (int)strlen(cards[final]->name), cards[final]->cost);
		
		printf("%s%*s\n", cards[final]->type, alignment - (int)strlen(cards[final]->type), rare_value);
		
		for(int k = 0; k < alignment; k++){
			printf("-");
		}
		printf("\n");
		
		printf("%s\n", cards[final]->text);
		
		for(int k = 0; k < alignment; k++){
			printf("-");
		}
		printf("\n");
		
		printf("%*s\n", alignment, cards[final]->stats);
		printf("\n");
		
	}
	
	//Freeing all the space allocated for all the cards
	for(int free_count = 0; free_count < i; free_count++){
		
		free(cards[free_count]);
	}
	
	//Freeing the duplicated buffer using all the starting addresses
	for(int free_buf = 0; free_buf < j; free_buf++){
		
		free(address_buf[free_buf]);
	}
	
	//Freeing the allocated space for all the pointers of cards
	free(cards);
	
	//Closing the file
	fclose(input_file);
	
	//Freeing the getline buffer
	free(buf);
	
	return 0;
}






