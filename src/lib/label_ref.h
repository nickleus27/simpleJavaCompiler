#ifndef LABEL_REF_H
#define LABEL_REF_H
#include <stdlib.h>
#include "../assembly/label.h"

typedef struct label_ref_ *label_ref;

struct label_ref_ {
    int count;
    Label label;
};

#define ASN_LABEL_REF(src, dst)    dst = src; src->count++;
#define LABEL_REF_DEC(ref)         if (--ref->count == 0){free(ref->label);free(ref);}
label_ref new_label_ref(Label label);

#endif // END OF LABEL_REF_H