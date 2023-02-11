#include "label_ref.h"

label_ref
new_label_ref(Label label) {
    label_ref ref = (label_ref)malloc(sizeof(struct label_ref_));
    ref->count = 1;
    ref->label = label;
    return ref;
}