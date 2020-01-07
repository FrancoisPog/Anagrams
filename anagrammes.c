#include "anagrammes.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>


/* 
  Fonction de comptage du nombre d'occurrence de chaque lettre (et joker).
  N.B. : Si un mot contient des jokers, le nombre d'occurrence est stocké dans la case 26.
*/
void countLetter(const char *str1,int *res){
    size_t i = 0;
    while(str1[i] != '\0'){ // Parcours de la chaine de caractère. 
      //On incrémente la case correspondant au caractère courant.
      if(str1[i] == '*'){
        res[26]++;
      }else{
        res[(str1[i]-'a')]++;
      }
      ++i;
    }
}

/*
  Cette fonction test si deux chaines de caractères sont des anagrammes en prenant en compte des jokers.
*/
bool string_are_anagrams(const char *str1, const char *str2) {
  bool res = true;

  if(strlen(str1) != strlen(str2)){ // Si les chaines n'ont pas la même taille, elles ne sont pas des anagrammes
    return false;
  }

  int alphabet_size = 26;
  int *letters_str1 = calloc(alphabet_size,sizeof(int)); // On déclare un tableau de 26 cases pour stocker le résultat de countLetters(str1)
  int *letters_str2 = calloc(alphabet_size+1,sizeof(int)); // On déclare un tableau de 27 cases pour stocker le résultat de countLetters(str2) (26 lettres + jokers)
  
  //N.B. Ici str2 est le mot saisi par l'utilisateur et str1 est le mot avec lequel il sera comparé.

  countLetter(str1,letters_str1);
  countLetter(str2,letters_str2);
  
  int joker = letters_str2[26]; // On récupère le nombre de joker dans str2

  for(size_t i = 0 ; i < alphabet_size ; ++i){ //On parcours les deux tableaux 

    if(letters_str1[i] != letters_str2[i]){ //Si le nombre d'occurence d'une lettre n'est pas le même dans les deux tableaux, on décrémente la case joker de la différence.

      joker -= letters_str1[i] - letters_str2[i];

      if(joker < 0 || letters_str1[i] - letters_str2[i] < 0){ // Si le nombre de joker est inférieur à zero ou qu'une lettre apparait plus de fois dans str2 que str1, on retourne false.
        res = false;
        break;
      }
    }
  }

//libération de la mémoire
  free(letters_str2);
  free(letters_str1);

  return res;
}

/*
  Fonction dupliquant une chaine de caractère.
*/
char *string_duplicate(const char *str) {
  size_t size = strlen(str);
  char *copy = malloc((size+1)*sizeof(char)); //Allocation de l'espace mémoire pour stocker une copie de str et du caractère final.

  strcpy(copy,str); // Copie de la chaine str dans copy
  
  return copy;
}

/*
  Fonction d'échange de place de deux caractère d'une chaine.
*/
static void str_swap(char *str,size_t i , size_t j){
  char tmp = str[i];
  str[i] = str[j];
  str[j] = tmp;   
}

/*
  Tri par insertion d'une chaine de caractère.
*/
void string_sort_letters(char *str) {

  size_t size = strlen(str); 

  for(size_t i = 0 ; i < size ; ++i){ // Parcours de la chaine.
    size_t j = i;
      for(size_t k = i+1 ; k < size ; k++){ // Parcours de la chaine à partir de i.
        if(str[j] > str[k]){ // Recherche du minimum.
          j = k;
        }
      }
      str_swap(str,j,i); // Insertion du minimum à l'indice i;
  }
}

/*
  Fonction remplaçant le caractère '\n' par '\O'.
*/
void clean_newline(char *buf, size_t size) {
  size_t i = 0;
  while(i < size && buf[i] != '\n' ){
    i++;
  }

  buf[i] = '\0';
}

// Part 2

/*
  Fonction de création d'un tableau de mot
*/
void word_array_create(struct word_array *self) {

  self->size = 0;
  self->capacity = 10;
  self->data = calloc(self->capacity,sizeof(char *));
}

/*
  Fonction de destruction d'un tableau de mot.
*/
void word_array_destroy(struct word_array *self) {
  for(size_t i = 0 ; i < self->size ; ++i){
    free(self->data[i]);
  }
  free(self->data);
}

/*
  Fonction ajoutant un mot à la fin du tableau de mots
*/
void word_array_add(struct word_array *self, const char *word) {
  if(self->size == self->capacity){ // Si le tableau est plein
    self->capacity *= 2; // On double la capacité
    char **data = calloc(self->capacity,sizeof(char*)); // On alloue de la mémoire pour le nouveau tableau
    memcpy(data, self->data, self->size * sizeof(char*)); // On copie l'ancien tableau dans le nouveau
    free(self->data); // On libère la mémoire de l'ancien tableau
    self->data = data; // On actualise le pointeur du tableau de mot
  }

  self->data[self->size] = string_duplicate(word); // On ajoute le mot a la fin du tableau
  self->size++; // On incrémente la taille du tableau
}

/*
  Fonction de recherche d'anagrammes dans un tableau de mot
*/
void word_array_search_anagrams(const struct word_array *self, const char *word, struct word_array *result) {
  for(size_t i = 0 ; i < self->size ; ++i){ // Parcours du tableau de mots
    if(string_are_anagrams(self->data[i],word)){ // On compare le mot courant et le mot donné
      word_array_add(result,self->data[i]); // S'ils sont anagrammes, on ajoute le mot dans le tableau résultat
    }
  }
}

/*
  Fonction d'échange de place de deux mots du tableau
*/
static void array_swap(struct word_array *self,size_t i , size_t j){
  char* tmp = self->data[i];
  self->data[i] = self->data[j];
  self->data[j] = tmp; 
}

/*
  Fonction de partition d'un tableau de mot (en vue du quickSort)
*/
static ssize_t array_partition(struct word_array *self, ssize_t i, ssize_t j){
  ssize_t pivot_index = i; 
  const char* pivot = self->data[pivot_index];
  array_swap(self,pivot_index,j);

  ssize_t l = i;

  for(ssize_t k = i ; k < j ; k++){
    if(strcmp(self->data[k],pivot) < 0){
      array_swap(self, k , l);
      l++;
    }
  
  }
  array_swap(self,l,j);
  return l;
}

/*
  Fonction récursive de tri rapide 
*/
static void array_quick_sort_partial(struct word_array *self,ssize_t i, ssize_t j){

  if(i<j){
    ssize_t p = array_partition(self,i,j);
    array_quick_sort_partial(self,i,p-1);
    array_quick_sort_partial(self,p+1,j);
  }

}

/*
  Fonction de tri rapide d'un tableau de mots
  N.B. Le tri rapide est le tri optimal le plus adapté aux tableaux
*/
void word_array_sort(struct word_array *self) {
  array_quick_sort_partial(self,0,self->size-1);
}

/*
  Fonction d'affichage d'un tableau de mots
*/
void word_array_print(const struct word_array *self) {
  for(size_t i = 0 ; i < self->size ; ++i){
    printf("| %s | \n",self->data[i]);
  }
  printf("\n");

}



/*
  Fonction de remplissage d'un tableau de mots à partir d'un fichier texte
*/
void word_array_read_file(struct word_array *self, const char *filename) {
  char word[WORD_LETTERS_MAX];

  FILE *fp = fopen(filename, "r");

  if (fp == NULL) {
    fprintf(stderr, "Error when opening file '%s'\n", filename);
    return;
  }

  while (!feof(fp)) {
  if (fgets(word, WORD_LETTERS_MAX, fp)){
    clean_newline(word, WORD_LETTERS_MAX);                                                
    word_array_add(self, word);
  }
}

  fclose(fp);
}

/*
  Fonction d'affichage d'une liste de buckets
*/
void word_dict_bucket_print(struct word_dict_bucket *self){
  struct word_dict_bucket *curr = self;

  while(curr != NULL){
    printf("| %s |",curr->word);
    curr = curr->next;
  }

}

/*
  Fonction d'affichage d'un dictionnaire
*/
void word_dict_print(struct word_dict *self){

  printf("[ size : %zd | count : %zd ] \n",self->size,self->count);
  for(size_t i = 0 ; i < self->size ; ++i){ // Parcours du tableau de buckets
    printf("[%zd] > ",i);
    word_dict_bucket_print(self->buckets[i]); // Affichage de la liste de buckets courante
    printf("\n");
  }
  printf("\n");

}

/*
  Fonction de destruction d'un bucket
*/
void word_dict_bucket_destroy(struct word_dict_bucket *bucket) {
    if(bucket == NULL){
      return;
    }
    word_dict_bucket_destroy(bucket->next);
    free(bucket);
}

/*
  Fonction d'ajout d'un mot dans une liste de buckets
*/
struct word_dict_bucket* word_dict_bucket_add(struct word_dict_bucket *bucket, const char *word) {
  struct word_dict_bucket *other = malloc(sizeof(struct word_dict_bucket)); // On alloue la mémoire pour un nouveau maillon
  other->word = word; // On affecte le mot au bucket
 
  other->next = bucket; // On affecte a next l'adresse de l'ancien premier élément de la liste

  return other; // On retourne le nouvel élément 
}

/*
  Fonction de création d'un dictionnaire
*/
void word_dict_create(struct word_dict *self) {
  self->size = 10;
  self->count = 0;
  self->buckets = calloc(self->size,sizeof(struct word_dict_bucket));
}

/*
  Fonction de destruction d'un dictionnaire
*/
void word_dict_destroy(struct word_dict *self) {
  for(size_t i = 0 ; i < self->size ; ++i){
    word_dict_bucket_destroy(self->buckets[i]);
  }
  free(self->buckets);
}

/*
  Fonction de hachage
*/
size_t fnv_hash(const char *key) {
  size_t FNV_offset_basis = 0xcbf29ce484222325;
  size_t FNV_prime = 0x100000001b3;
  size_t hash = FNV_offset_basis;
  size_t i = 0;

  while(key[i] != '\0'){ // Pour chaque octet de la clé
    hash = hash ^ key[i]; // On effectue un xor entre le hash et l'octet courant 
    hash = hash * FNV_prime; // puis on multiplie le hash par une valeur donnée (propre à ce type de hachage)
    i++;
  }

  return hash;
}

/*
  Fonction de calcul de l'indice dans un dictionnaire à partir d'un mot
*/
size_t word_to_index(size_t dict_size,const char *word){

  char *key = (string_duplicate(word)); // Duplication du mot 
  string_sort_letters(key); // Tri du mot

  size_t hash = fnv_hash(key); // Calcul du hash 
  size_t index = hash%dict_size; // Calcul de l'indice dans le dictionnaire

  free(key); // Libération de la duplication du mot
  key = NULL;

  return index;
}

/*
  Fonction de transfert du contenu d'un dictionnaire dans un tableau de pointeurs de buckets
*/
static void word_dict_transfer_to_array(struct word_dict *self,struct word_dict_bucket **dest){
  // N.B. Quand cette fonction est appelée lors d'un rehash, la taille du dictionnaire à déjà été doublée

   for(size_t i = 0 ; i < self->size/2 ; ++i){ // Pour chaque élément de l'ancien tableau

    struct word_dict_bucket *curr = self->buckets[i]; 

    while(curr != NULL){  // On parcours la liste chainées
      size_t index = word_to_index(self->size,curr->word); // On calcul d'index du mot dans le nouveau tableau
      dest[index] = word_dict_bucket_add(dest[index],curr->word); // Et on ajoute chaque mot dans le nouveau tableau

      curr = curr->next;
    }
    word_dict_bucket_destroy(self->buckets[i]); //On libère la mémoire de la liste qui vient d'être copié
  }

  
}

/*
  Fonction de rehash d'une table
*/
void word_dict_rehash(struct word_dict *self) {

  self->size *= 2; // On double la taille du tableau 

  struct word_dict_bucket **buckets = calloc(self->size,sizeof(struct word_dict_bucket)); // On crée un nouveau tableau de pointeurs de bucket

  word_dict_transfer_to_array(self,buckets);

  free(self->buckets); // On libère la mémoire de l'ancien tableau
  self->buckets = buckets; // On affecte le nouveau tableau à notre dictionnaire

}


/*
  Fonction d'ajout d'un mot dans un dictionnaire
*/
void word_dict_add(struct word_dict *self, const char *word) {

  if((double)self->count/(double)self->size >= 0.5){ // Si le le nombre de mot dépasse la moitié de la taille du dictionnaire, on effectue un rehash.
    word_dict_rehash(self);
  }
 
  size_t index = word_to_index(self->size,word); // On calcul l'indice du mot dans la table.

  self->buckets[index] = word_dict_bucket_add(self->buckets[index],word); // On ajoute le mot dans le dictionnaire.
  self->count++;

  
}

/*
  Fonction de remplissage d'un dictionnaire à partir d'un tableau de mot
*/
void word_dict_fill_with_array(struct word_dict *self, const struct word_array *array) {
  for(size_t i = 0 ; i < array->size ; ++i){ 
    word_dict_add(self,array->data[i]); // On ajoute chaque élément du tableau dans le dictionnaire.
  }
}

/*
  Fonction de recherche d'anagrammes d'un mot donné dans un dictionnaire
*/
void word_dict_search_anagrams(const struct word_dict *self, const char *word, struct word_array *result) {

  size_t index = word_to_index(self->size,word); // On calcul l'indice du mot donné

  struct word_dict_bucket *curr = self->buckets[index]; // On affecte à curr l'adresse de la liste de bucket à l'indice du mot donné

  while(curr != NULL){ // On parcours la liste chainée 
    if(string_are_anagrams(curr->word,word)){ // On vérifie que les mots sont bien anagrammes en cas de collisions
      word_array_add(result,curr->word);// On ajoute chaque mot dans le tableau résultat
    }
    curr = curr->next;
  }

}


/*
  Fonction d'initialisation d'une structure joker
*/
void wildcard_create(struct wildcard *self) {
  self->count = 0;
  for(size_t i = 0 ; i < 4 ; ++i){
    self->index[i] = 0;
  }
}

/*
  Fonction de recherche de joker dans un mot
*/
void wildcard_search(struct wildcard *self, const char *word) {
  size_t i = 0;
  while(word[i] != '\0'){
    if(word[i] == '*'){
      self->index[self->count] = i;
      self->count++;
    }
    i++;
  }
}

/*
  Fonction de recherche d'anagrammes dans un tableau de mot en prenant en compte les jokers
*/
void word_array_search_anagrams_wildcard(const struct word_array *self, const char *word, struct word_array *result) {
  // N.B. La fonction de recherche d'anagrammes dans un tableau de mot prend déjà en compte les jokers.
  word_array_search_anagrams(self,word,result);

}

/*
  Fonction testant toutes les combinaisons possibles d'un mot contenant des jokers
*/
void replace_joker_and_search_anagrams (char* word ,int nbJoker,size_t* index,int lettre, struct word_array *resultat, const struct word_dict *dict){
  if(nbJoker == 0){
    word_dict_search_anagrams(dict,word,resultat); 
    return;
  }
  
  // Cette fonction test récurcivement toutes les lettres de l'alphabet pour chaque emplacement de joker

  for(size_t i = lettre ; i < 26 ; i++){ 

    word[index[nbJoker-1]] = i + 'a'; // On remplace un emplacement de joker par chaque lettre de l'alphabet et on cherche les anagrammes pour chacun d'eux

    replace_joker_and_search_anagrams(word,nbJoker-1,index,i,resultat,dict);
  }
}

/*
  Fonction de recherche d'anagrammes dans un dictionnaire en prenant en compte les jokers
*/
void word_dict_search_anagrams_wildcard(const struct word_dict *self, const char *word, struct word_array *result) {

  // Création, initialisation et recherche de jokers dans le mot donné
  struct wildcard joker;
  wildcard_create(&joker);
  wildcard_search(&joker,word);

  if((&joker)->count == 0){
    word_dict_search_anagrams(self,word,result); // Si le mot ne contient pas de joker, on appelle la fonction qui ne gère pas les jokers
    return;
  }

  char *copy = string_duplicate(word); // On duplique le mot donné

  replace_joker_and_search_anagrams(copy,(&joker)->count,(&joker)->index,0,result,self); // On appelle la fonction qui va remplacer les jokers par chaque lettre et chercher les anagrammes

  free(copy); // Libération de la mémoire de la dublication

}






