#include <stdint.h>
#include <stdbool.h>
#include "stdlib.h"
#include "circBufT.h"
#include "math.h"


#define BUF_SIZE 25
#define SAMPLE_RATE_HZ 100


extern int32_t ALTITUDE_BASE;
extern circBuf_t g_inBuffer;        // Buffer of size BUF_SIZE integers (sample values)


int32_t CalcAv(void);


int32_t CalcPerc(int32_t Average);

