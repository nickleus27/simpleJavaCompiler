/**
 * Starter code is by David Galles and can be found at https://www.cs.usfca.edu/~galles/compilerdesign/
 * File finished by Nick Anderson
 * nickleus27@gmail.com
 * 10/18/2022
 * 
*/

typedef char  *Register;

Register FP(void);
Register SP(void);
Register Result32(void);
Register Result64(void);
Register ReturnAddr(void);
Register Zero(void);
Register Acc32(void);
Register Acc64(void);
Register AccSPbytes(void);
Register AccSP32(void);
Register AccSP64(void);
Register Tmp0_32(void);
Register Tmp1_32(void);
Register Tmp2_32(void);
Register Tmp0_64(void);
Register Tmp1_64(void);

/*x6 16 byte reg, x4 4 byte reg = 112 bytes / 16 = 7*/
