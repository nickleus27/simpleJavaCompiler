/**
 * Starter code is by David Galles and can be found at https://www.cs.usfca.edu/~galles/compilerdesign/
 * File finished by Nick Anderson
 * nickleus27@gmail.com
 * 10/18/2022
 * 
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../semantic/hash.h"
#include "label.h"


H_hashTable labelTab;

int firsttime = 1;



Label NewLabel(void) {
  int last;
  char *retval;

  if (firsttime) {
    firsttime = 0;
    labelTab = H_HashTable(199);
  }
  last = (int) H_find(labelTab,"LABEL");
  H_delete(labelTab,"LABEL");
  H_insert(labelTab,"LABEL",((void *) last + 1));
  retval = (char *) malloc(9 * sizeof(char));
  strcpy(retval,"LABEL");
  sprintf(&retval[5],"%d",last+1);
  return retval;
  
}

Label NewNamedLabel(char *name) {
  int last;
  char *retval;

  if (firsttime) {
    firsttime = 0;
    labelTab = H_HashTable(199);
  }
  last = (int) H_find(labelTab,name);
  H_delete(labelTab,name);
  H_insert(labelTab,name,((void *) last + 1));
  retval = (char *) malloc((strlen(name)+4) * sizeof(char));
  strcpy(retval,name);
  sprintf(&retval[strlen(name)],"000");
  sprintf(&retval[strlen(name)],"%d",last+1);
  return retval;
}

char *GetLabelName(Label l) {
  return (char *) l;
}


