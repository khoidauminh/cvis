#include "declare.h"

cplx quad1(cplx x) { return CMPLX(fabsf(crealf(x)), fabsf(cimagf(x))); }
