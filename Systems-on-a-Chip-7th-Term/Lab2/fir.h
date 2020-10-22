#ifndef FIR_H_
#define FIR_H_

#define FIR_MAX_IN 10
#define FIR_MAX_OUT 8

void fir_filter(short x[FIR_MAX_IN], char p, short y[FIR_MAX_OUT]);

#endif
