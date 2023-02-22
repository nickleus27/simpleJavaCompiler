/**
 * Starter code is by David Galles and can be found at https://www.cs.usfca.edu/~galles/compilerdesign/
 * File finished by Nick Anderson
 * nickleus27@gmail.com
 * 10/18/2022
 * 
*/
//#include "../lib/label_ref.h"

typedef char *Label;
typedef struct label_ref_ *label_ref;

struct label_ref_ {
    int count;
    Label label;
};

#define ASN_LABEL_REF(src, dst)    dst = src; src->count++;
#define LABEL_REF_DEC(ref)         if (--ref->count == 0){free(ref->label);free(ref);}
label_ref new_label_ref(Label label);

Label NewLabel(void);
label_ref NewNamedLabel(char *name);
char *GetLabelName(Label l);

