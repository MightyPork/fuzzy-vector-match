#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <float.h>

#include "vec_match.h"

#define SQUARE(a) ((a)*(a))

bool vec_match(const float *data,
					const float *ref,
					uint32_t length,
					uint8_t drift_x,
					float offset_y,
					float *envl_match_error,
					float *abs_match_error)
{
	int a, b;

	int err_cnt = 0;
	float env_err = 0;
	float abs_err = 0;

	for (int i = 0; i < (int)length; i++) {
		float peak = FLT_MIN;
		float base = FLT_MAX;

		// find highest value in the surrounding drift_x points
		a = i - drift_x;
		b = i + drift_x;
		if (a < 0) a = 0;
		if (b >= (int)length) b = length - 1;

		for (int j = a; j <= b; j++) {
			if (peak < ref[j]) peak = ref[j];
			if (base > ref[j]) base = ref[j];
		}

		// apply drift_y
		peak += offset_y;
		base -= offset_y;

		abs_err += SQUARE(ref[i] - data[i]);


		if (data[i] >= base && data[i] <= peak) {
			// within limits
			continue;
		} else {
			//printf("data[%d] out of range: %f, [%f ; %f]\n", i, data[i], base, peak);

			if (data[i] < base) env_err += SQUARE(base - data[i]);
			if (data[i] > peak) env_err += SQUARE(data[i] - peak);

			err_cnt++;
		}
	}

	// write error values to provided fields
	if (envl_match_error != NULL) *envl_match_error = env_err;
	if (abs_match_error != NULL) *abs_match_error = abs_err;

	return err_cnt == 0;
}
