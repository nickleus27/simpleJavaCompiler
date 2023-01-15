/**
 * Starter code is by David Galles and can be found at https://www.cs.usfca.edu/~galles/compilerdesign/
 * File finished by Nick Anderson
 * nickleus27@gmail.com
 * 10/18/2022
 * 
*/

#include<stdio.h>
#include "hash.h"
#include <stdlib.h>
#include <string.h>

typedef struct tableElem_ *tableElem;

struct H_hashTable_ {
  tableElem *table;
  int size;
};

struct tableElem_ {
  void *data;
  char *key;
  tableElem next;
};

void freeHashTable(H_hashTable table) {
  free(table->table);
  free(table);
}

void freeTableElem(tableElem elem) {
  free(elem->data);
  free(elem);
}

int hash(char *key, int size);
tableElem TableElem(void *data, char *key, tableElem next);

void H_insert(H_hashTable h, char *key, void *data) {
  int hashval = hash(key,h->size);
  h->table[hashval] = TableElem((void *)data,key,h->table[hashval]);  
}

void *H_find(H_hashTable h, char *key) {
 
  tableElem tmp = h->table[hash(key,h->size)];
  while (tmp && strcmp(key,tmp->key) != 0) {
    tmp = tmp->next;
  }
  if (tmp == NULL)
    return NULL;
  return tmp->data;
}

void H_delete(H_hashTable h, char *key) {
  int index;
  tableElem ptr, tmp;

  index = hash(key,h->size);
  
  if (h->table[index] != NULL) {
    if (strcmp(h->table[index]->key,key) == 0) {
      ptr = h->table[index];
      h->table[index] = h->table[index]->next;
      free(ptr); /* Memory Leak. need to free tableElem */
    } else {
      for(tmp = h->table[index]; tmp->next != NULL &&
	    strcmp(tmp->next->key,key) != 0; tmp = tmp -> next);
      if (tmp->next) {
        ptr = tmp->next;
	      tmp->next = tmp->next->next;
        free(ptr);/* need to free tableElem */
      }
    }
  }

}



int hash(char *key, int size) {
  unsigned int hashval = 0;
  int retval;
  for (;key[0];key++) {
    hashval = (hashval << 5) +  key[0];
  }
  
  retval = hashval % size;
  return retval ;
}
H_hashTable H_HashTable(int size) {
  int i;
  H_hashTable retval = (H_hashTable) malloc(sizeof(*retval));
  retval->size = size;
  retval->table = (tableElem *) malloc(size * sizeof(tableElem *));
  for (i=0; i<size; i++)
    retval->table[i] = NULL;
  return retval;
}

tableElem TableElem(void *data, char *key, tableElem next) {
  tableElem retval = (tableElem) malloc(sizeof(*retval));
  retval->data = data;
  retval->key = key;
  retval->next = next;
  return retval;
}
