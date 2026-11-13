#include "vec_sum.h"

float vec_sum(const float * restrict array, int32_t length) {
	//TODO implement
	 float sum = 0;
	 for (int32_t i = 0; i < length; i++) {
		sum += array[i];
	 }
	return sum;
}

