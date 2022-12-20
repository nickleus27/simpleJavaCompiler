#ifndef OFFSET_REF_H
#define OFFSET_REF_H
#include <stdlib.h>

typedef struct offset_ref_ *offset_ref;

struct offset_ref_ {
    void (*free)(offset_ref);
    int count;
    int offset;
};

#define ASN_OFFSET_REF(src, dst)    dst = src; src->count++;
#define OFFSET_REF_DEC(ref)         if (--ref->count == 0) {ref->free(ref);}
offset_ref new_offset_ref(int offset);
static inline void offset_ref_inc(offset_ref ref);
static inline void ref_dec(offset_ref ref);

#endif // END OF OFFSET_REF_H