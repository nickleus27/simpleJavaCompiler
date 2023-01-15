/**
 * Starter code is by David Galles and can be found at https://www.cs.usfca.edu/~galles/compilerdesign/
 * File finished by Nick Anderson
 * nickleus27@gmail.com
 * 10/18/2022
 * 
*/

typedef struct H_hashTable_ *H_hashTable;

H_hashTable H_HashTable(int size);
void freeHashTable(H_hashTable table);
void H_insert(H_hashTable h, char *key, void *data);
void *H_find(H_hashTable h, char *key);
void H_delete(H_hashTable h, char *key);
