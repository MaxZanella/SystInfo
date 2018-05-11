#include "../libfractal/fractal.h"
#include "CUnit/Basic.h"
#include <string.h>

void test_init(void){
	struct fractal *fract;
	fract = fractal_new("fractal1", 800, 1000, 0.5, -0.9);

	CU_ASSERT(strcmp(fractal_get_name(fract), "fractal1")==0);
	CU_ASSERT(fractal_get_width(fract)==800);
	CU_ASSERT(fractal_get_height(fract)==1000);
	CU_ASSERT(fractal_get_a(fract)==0.5);
	CU_ASSERT(fractal_get_b(fract)==-0.9);
	CU_ASSERT(fractal_set_value(fract, 450,999,2564));
	CU_ASSERT(fractal_get_value(fract, 450,999)==2564);
	CU_ASSERT(fractal_free(fract));
}
