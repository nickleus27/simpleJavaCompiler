#include "offset_ref.h"

offset_ref
new_offset_ref(int offset) {
    offset_ref ref = (offset_ref)malloc(sizeof(struct offset_ref_));
    ref->count = 1;
    ref->offset = offset;
    return ref;
}

static inline void
offset_ref_inc(offset_ref ref)
{
    ref->count++;
}

static inline void
ref_dec(offset_ref ref)
{
    if (--ref->count == 0)
    {
        ref->free(ref);
    }
}