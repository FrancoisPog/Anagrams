// François POGUET et Enzo COSTANTINI | TP1A

/*
	Ce programme est conçu de manière à détourner le principe d'une table de hachage, en effet on va générer des collisions entre des mots anagrammes entre eux,
	pour cela, on utilise comme clé les mots triés, de cette façon, tous les anagrammes de chaque mot seront stocké au même indice dans le tableau.
*/



#include "anagrammes.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <assert.h>

#define BUFSIZE 512


/* 
  Fonction affichant le temps écoulé entre deux structures timeval
*/
void print_time_diff(struct timeval time1, struct timeval time2){
  printf("%ld µs\n",((1000000*time2.tv_sec+time2.tv_usec ) - (1000000*time1.tv_sec+time1.tv_usec)));

}

/*
  Fonction testant la validité d'un mot donné
*/
bool word_valid(char *word){

  assert(word !=NULL && word[0]!='\0'); // On vérifie que word n'est pas NULL est que la chaine n'est pas vide.
  

  size_t i = 0;
  size_t count_joker = 0;


  while(word[i] != '\0'){ // Parcours de la chaine de caractère.

    if(i > WORD_LETTERS_MAX){ // Si la taille de la chaine dépasse la taille autorisée, on renvoie faux.
      return false;
    }

    if(word[i] >= 'A' && word[i] <= 'Z'){ // Si la lettre est majuscule, elle est "transformée" en minuscule.
      word[i] += 32;
    }

    if(word[i] == '*'){ // Si le caractere est '*', on incremente le compteur de joker
      count_joker++;
      if(count_joker > WILDCARDS_MAX){ // S'il y a plus de joker qu'autorisé, on renvoie false.
        return false;
      }
    }else{
      if((word[i] < 'a' || word[i] > 'z')){ // Si le caractere n'est pas une lettre minuscule (ni '*'), on renvoie false
        return false;
      }
    }
    ++i;
  }
  return true; // Si on arrive ici, le mot est valide donc on renvoie true.
}

/*
	Fonction affichant deux tableaux de même taille côte à côte
*/
void word_array_print_two(const struct word_array *tab1,const struct word_array *tab2){
  assert(tab1->size == tab2->size);

  for(size_t i = 0 ; i < tab1->size ; ++i){
    printf(" %s     |     %s\n",tab1->data[i],tab2->data[i]);
  }
  printf("\n");
}




// MAIN

int main(int argc, char *argv[]) {
	
	//Création et initialisation d'un dictionnaire
	struct word_dict dico;
	word_dict_create(&dico);

	//Création et initialisation d'un tableau de mots
	struct word_array tab;
	word_array_create(&tab);

	//Remplissage du tableau de mot depuis "dictionnaire.txt"
	word_array_read_file(&tab,"dictionnaire.txt");

	//Remplissage du dictionnaire depuis le tableau de mot
	word_dict_fill_with_array(&dico,&tab);

	//Verification du nombre de mot dans le tableau et le dictionnaire
	if((&dico)->count != (&tab)->size){
		printf("Dictionary filling error ! \n");
		return 1;
	}

	// Boucle de saisie
	while(1){

		//Décaration et initialisation du mot avec un valeur connue
		char word[BUFSIZE];
		word[0] = '?';

		//Tant que le mot n'est pas valide et que la chaine n'est pas vide, on recommence la saisie
		while(word[0]!='\0' && !word_valid(word)){
			printf("Letters : ");
			fgets(word, BUFSIZE, stdin);
			clean_newline(word, BUFSIZE);
		}

		//Si la chaine est vide, on quitte le programme
		if(word[0] == '\0'){
			break;
		}

		//Création et initialisation du tableau de mots qui stockera les resultats de la recherche avec le dictionnaire
		struct word_array result1;
		word_array_create(&result1);

		//Création et initialisation du tableau de mots qui stockera les résultats de la recherche avec le tableau
		struct word_array result2;
		word_array_create(&result2);

		//Déclaration des structures timeval
		struct timeval time_begin;
		struct timeval time_step;
		struct timeval time_end;

		//Initialisation de time_begin
		gettimeofday(&time_begin,NULL);

		//Recherche d'anagrammes avec les dictionnaire
		word_dict_search_anagrams_wildcard(&dico,word,&result1);

		//Initialisation de time_step
		gettimeofday(&time_step,NULL);

		//Recherche d'anagrammes avec le tableau de mots
		word_array_search_anagrams_wildcard(&tab,word,&result2);

		//Initialisation de time_end
		gettimeofday(&time_end,NULL);

		//Tri et affichage des résultats
		word_array_sort(&result1);
		printf("-- \n List of anagrams : \n--\n");
		printf("Left : found with dictionary \nRight : found with word array\n\n");
		
		word_array_print_two(&result1,&result2);

		//Affichage du nombre de résultats et du temps de recherche 
		printf("--\n Search for : \"%s\" \n--\n",word);
		printf("> %zd anagrams found with the dictionary in ",(&result1)->size);
		print_time_diff(time_begin,time_step);
		printf("> %zd anagrams found with the array in ",(&result2)->size);
		print_time_diff(time_step,time_end);
		printf("------------------------------------------------\n");

		//Libération de la mémoire des tableaux resultats
		word_array_destroy(&result1);
		word_array_destroy(&result2);

	}

	printf("> End of the program < \n");
	
	//Libération de la mémoire du tableau de mots et du dictionnaire
	word_dict_destroy(&dico);
	word_array_destroy(&tab);
	
	

  return 0;
}
