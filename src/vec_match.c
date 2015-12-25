#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>

#include "vec_match.h"

#define SQUARE(a) ((a)*(a))

#define F2ZEROES(f) roundf(-(f))
#define ZEROES2F(z) (0.0f - z)


bool vec_match_do(const float *data, const float *ref,
				  const vec_match_cfg_t *cfg,
				  float *fuzzy_match_error, float *abs_match_error,
				  bool packed)
{
	int a, b;

	int err_cnt = 0;
	float env_err = 0;
	float abs_err = 0;

	for (int i = 0; i < (int)cfg->length; i++) {
		float peak = FLT_MIN;
		float base = FLT_MAX;

		// find highest value in the surrounding drift_x points
		a = i - cfg->drift_x;
		b = i + cfg->drift_x;
		if (a < 0) a = 0;
		if (b >= (int)cfg->length) b = cfg->length - 1;

		for (int j = a; j <= b; j++) {
			if (peak < ref[j]) peak = ref[j];
			if (base > ref[j]) base = ref[j];
		}

		// apply drift_y
		peak += cfg->offset_y; // add abs threshold on top
		base -= cfg->offset_y;

		// ignore abs threshold difference (float precision error)
		if (fabs(ref[i] - data[i]) > cfg->abs_threshold) {
			abs_err += SQUARE(ref[i] - data[i]);
		}


		if (data[i] >= (base - cfg->abs_threshold) && data[i] <= (peak + cfg->abs_threshold)) {
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
	if (fuzzy_match_error != NULL) *fuzzy_match_error = env_err;
	if (abs_match_error != NULL) *abs_match_error = abs_err;

	return err_cnt == 0;
}



bool vec_match(const float *data, const float *ref, const vec_match_cfg_t *cfg,
			   float *fuzzy_match_error, float *abs_match_error)
{
	return vec_match_do(data, ref, cfg, fuzzy_match_error, abs_match_error, false);
}



bool vec_match_packed(const float *data, const float *ref, const vec_match_cfg_t *cfg,
					  float *fuzzy_match_error, float *abs_match_error)
{
	return vec_match_do(data, ref, cfg, fuzzy_match_error, abs_match_error, true);
}



uint32_t vec_pack(float *result, uint32_t result_capacity,
				  const float *data, uint32_t data_length, float threshold)
{
	uint32_t result_len = 0;
	uint32_t zeroes = 0;

	for (uint32_t i = 0; i < data_length; i++) {
		if (data[i] < threshold) {
			zeroes++;
		} else {
			// write zero marker to result
			if (zeroes) {
				if (result_len < result_capacity) {
					result[result_len] = ZEROES2F(zeroes); // float and negative
				}

				zeroes = 0;
				result_len++; //length is increased even if buffer full
			}

			if (result_len < result_capacity) {
				result[result_len] = data[i];
			}

			result_len++;
		}
	}

	// handle trailing zeroes
	if (zeroes) {
		if (result_len < result_capacity) {
			result[result_len] = 0.0f - zeroes;
		}

		result_len++;
	}

	return result_len;
}




uint32_t vec_unpack(float *result, uint32_t result_capacity,
					const float *compr_data, uint32_t compr_length)
{
	uint32_t idx = 0;

	for (uint32_t i = 0; i < compr_length; i++) {
		if (compr_data[i] < 0) {
			uint32_t zeroes = F2ZEROES(compr_data[i]);
			for (uint32_t j = 0; j < zeroes; j++) {
				if (idx < result_capacity) {
					result[idx] = 0;
				}
				idx++;
			}
		} else {
			if (idx < result_capacity) {
				result[idx] = compr_data[i];
			}
			idx++;
		}
	}

	return idx;
}

