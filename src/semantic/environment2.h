/**
 * Starter code is by David Galles and can be found at https://www.cs.usfca.edu/~galles/compilerdesign/
 * File finished by Nick Anderson
 * nickleus27@gmail.com
 * 10/18/2022
 * 
*/

envEntry VarEntry(type typ, int offset);
envEntry FunctionEntry(type returntyp, typeList formals, Label startLabel, Label endLabel);
envEntry TypeEntry(type typ);
void setArgMemSize(envEntry functionEntry, int size);


struct envEntry_ {
  enum {Var_Entry, Function_Entry,Type_Entry} kind;
  union {
    struct {
      type typ;
      int scope;
      offset_ref offset; /* TODO: need to make this int * for adjusting offset form ARM64 */
    } varEntry;
    struct {
      int argMemSize;
      type returntyp;
      typeList formals;
      Label startLabel;
      Label endLabel;
    } functionEntry;
    struct {
      type typ;
    } typeEntry;
  } u;
};
