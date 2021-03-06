// by Wojciech Muła, http://0x80.pl
// $Date: 2008-07-12 13:23:51 $ $Revision: 1.3 $
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

#define SIMD_ALIGN __attribute__((__aligned__(16)))
#define packed4(x) {(x), (x), (x), (x)}
#define packed_float(x) packed4(x)
#define packed_dword(x) packed4(x)

uint32_t CONST[4]     SIMD_ALIGN = packed_float(0x4f000000); // = 2^31 = (float)((uint32_t)(1 << 31)));
uint32_t MASK_0_30[4] SIMD_ALIGN = packed_dword(0x7fffffff);
uint32_t MASK_31[4]   SIMD_ALIGN = packed_dword(0x80000000);


void convert_uint32_float(uint32_t in[4], float out[4]) {
	__asm__ volatile (
	"movdqu   (%%eax), %%xmm0  \n"
	"movdqa    %%xmm0, %%xmm1  \n"

	"pand   MASK_0_30, %%xmm0  \n" // xmm0 - mask MSB bit - never less then zero in U2
	"cvtdq2ps  %%xmm0, %%xmm0  \n" // convert this value to float

	"psrad        $32, %%xmm1  \n" // populate MSB in higher word (enough to mask CONST)
	"pand       CONST, %%xmm1  \n" // xmm1 = MSB set ? float(2^31) : float(0)

	"addps     %%xmm1, %%xmm0  \n" // add 2^31 if MSB set

	"movdqu    %%xmm0, (%%ebx) \n"

	: /* no output */
	: "a" (in),
	  "b" (out)
	);
}


//========================================================================
int main(int argc, char* argv[]) {
	uint32_t v_in[4];
	float    v_out[4];
	int i;

	//=== initialize v_in ==============================
	srand(time(NULL));
	for (i=1; i <= 4; i++) {
		if (i < argc)
			v_in[i-1] = strtoll(argv[i], NULL, 0);
		else
			v_in[i-1] = rand();
	}

	//=== convert ======================================
	convert_uint32_float(v_in, v_out);
	
    printf("uint32_t[4] = {");
    for (unsigned i=0; i < 4; i++) {
        if (i > 0) {
            printf(", ");
        }
        printf("%u", v_in[i]);
    }
    printf("}\n");


    printf("float[4] = {");
    for (unsigned i=0; i < 4; i++) {
        if (i > 0) {
            printf(", ");
        }
        printf("%0.3f", v_out[i]);
    }
    printf("}\n");

	return 0;
}

