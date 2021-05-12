#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "card.h"

int main(int argc, char **argv){
	
	//valgrind --leak-check=full ./main
	printf("Hello, World!!\n");
	
	char *buf = NULL;
	size_t bufsize = 0;
	FILE *input_file = fopen("cards.csv", "r");
	ssize_t num;
	getline(&buf, &bufsize, input_file);
	
	int num_cards = 2000;
	struct card **cards = NULL;
	cards = realloc(cards, sizeof(struct card*) * num_cards);
	int k = 0;
	int i = 0;
	while( (num = getline(&buf, &bufsize, input_file)) > 0 ){
		k++;
		char *stringp = buf;
		if (stringp[strlen(stringp) - 1] == '\n'){
			stringp[strlen(stringp) - 1] = '\0';
		}
		cards[i] = malloc(sizeof(struct card));
		int id = atoi(strsep(&stringp, ","));
		cards[i]->id = id;
		stringp++;
		cards[i]->name = strsep(&stringp, "\"");
		stringp += 2;
		cards[i]->cost = strsep(&stringp, "\"");
		stringp++;
		unsigned int converted_cost = atoi(strsep(&stringp, ","));
		cards[i]->converted_cost = converted_cost;
		stringp++;
		cards[i]->type = strsep(&stringp, "\"");
		
		//printf("%ld\n", strlen(stringp));
		size_t first = strlen(stringp);
		stringp += strlen(stringp);
		
		int count = 0;
		int length = 0;
		while(count < 2){
			length++;
			char *find = stringp;
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
				
				if (find[1] == ','){
					cards[i]->stats = "\0";
					stringp--;
					break;
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
		strsep(&stringp, "\"");
		stringp = stringp + length - first + 1;
		
		for (int k = 0; k < strlen(stringp); k++){
			if(stringp[k] == '\\'){
				if(stringp[k+1] == 'n'){
					stringp[k] = ' ';
					stringp[k+1] = '\n';
				}
			}
		}
		cards[i]->text = stringp;
		
		printf("%d\n%s\n%s\n%d\n%s\n%s\n%s\n%u\n", cards[i]->id, cards[i]->name, cards[i]->cost, cards[i]->converted_cost, cards[i]->type, cards[i]->text, cards[i]->stats, cards[i]->rarity);
		//printf("%s\n", cards[i]->text);
		free(cards[i]);
		i++;
	}
	
	free(cards);
	fclose(input_file);
	free(buf);
	
	return 0;
}