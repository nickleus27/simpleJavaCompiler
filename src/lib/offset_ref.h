#ifndef OFFSET_REF_H
#define OFFSET_REF_H
#include <stdlib.h>

struct offset_ref {
    void (*free)(struct offset_ref *);
    int count;
    int offset;
};

offset_ref*
new_offset_ref(int offset) {
    offset_ref* ref = (offset_ref*)malloc(sizeof(offset_ref));
    return ref;
}

static inline void
offset_ref_inc(struct ref *ref)
{
    ref->count++;
}

static inline void
ref_dec(struct ref *ref)
{
    if (--ref->count == 0)
    {
        ref->free(ref);
    }
}

#endif // END OF OFFSET_REF_H