#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <string.h>

#include "vec_match.h"

#define SQ(a) ((a)*(a))

#define IS_FZERO(f) ((f) < 0.0f)
#define F2ZERO(f) roundf(-(f))
#define ZERO2F(z) (0.0f - z)



bool vec_match_do(const float *data, const float *ref, uint32_t ref_p_len,
				  const vec_match_cfg_t *cfg,
				  float *fuzzy_match_error, float *abs_match_error, bool packed)
{
	uint32_t a, b;
	float f; // tmp float

	pack_walker_t w; // walker

	if (packed) {
		pw_init(&w, ref, ref_p_len);
	}

	int err_cnt = 0;
	float env_err = 0;
	float abs_err = 0;

	float ref_at;

	for (uint32_t i = 0; i < cfg->length; i++) {
		float peak = FLT_MIN;
		float base = FLT_MAX;

		// bounds for base and peak search
		if (i < cfg->drift_x) {
			a = 0;
		} else {
			a = i - cfg->drift_x;
		}

		if (i + cfg->drift_x >= cfg->length) {
			b = cfg->length - 1;
		} else {
			b = i + cfg->drift_x;
		}

		// find base and peak

		for (uint32_t j = a; j <= b; j++) {
			f = packed ? pw_get(&w, j) : ref[j];

			if (peak < f) peak = f;
			if (f < base) base = f;
		}

		ref_at = packed ? pw_get(&w, i) : ref[i];

		// apply drift_y
		peak += cfg->offset_y; // add abs threshold on top
		base -= cfg->offset_y;

		// ignore abs threshold difference (float precision error)
		if (fabs(ref_at - data[i]) > cfg->abs_threshold) {
			abs_err += SQ(ref_at - data[i]);
		}


		if (data[i] >= (base - cfg->abs_threshold) && data[i] <= (peak + cfg->abs_threshold)) {
			// within limits
			continue;
		} else {
			//printf("data[%d] out of range: %f, [%f ; %f]\n", i, data[i], base, peak);

			if (data[i] < base) env_err += SQ(base - data[i]);
			if (data[i] > peak) env_err += SQ(data[i] - peak);

			err_cnt++;
		}
	}

// write error values to provided fields
	if (fuzzy_match_error != NULL) *fuzzy_match_error = env_err;
	if (abs_match_error != NULL) *abs_match_error = abs_err;

	return err_cnt == 0;
}



bool vec_match(const float *data, const float *ref,
			   const vec_match_cfg_t *cfg,
			   float *fuzzy_match_error, float *abs_match_error)
{
	return vec_match_do(data, ref, cfg->length, cfg, fuzzy_match_error, abs_match_error, false);
}



bool vec_match_packed(const float *data, const float *ref, uint32_t ref_p_len,
					  const vec_match_cfg_t *cfg,
					  float *fuzzy_match_error, float *abs_match_error)
{
	return vec_match_do(data, ref, ref_p_len, cfg, fuzzy_match_error, abs_match_error, true);
}



// ---- PACKING UTILS ----


uint32_t vec_pack(float *result, uint32_t result_capacity,
				  const float *data, uint32_t data_length, float threshold)
{
	uint32_t result_len = 0;
	uint32_t zeroes = 0;

	for (uint32_t i = 0; i < data_length; i++) {
		if (data[i] < threshold) {
			zeroes++;
		} else {
			// not a zero
			// write zero marker to result
			if (zeroes) {
				if (result_len < result_capacity) {
					result[result_len] = ZERO2F(zeroes); // float and negative
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



uint32_t vec_pack_auto(float *result, uint32_t result_capacity,
					   const float *data, uint32_t data_length, float *threshold_p)
{
	// lossless case, all fits
	if (data_length <= result_capacity) {
		*threshold_p = 0;
		memcpy(result, data, data_length * sizeof(float));
		return data_length;
	}


	float thr = 0.1;
	uint32_t ref_pack_len;

	while (true) {
		ref_pack_len = vec_pack(result, result_capacity, data, data_length, thr);

		//printf("try %f -> %d\n", thr, ref_pack_len);

		if (ref_pack_len <= result_capacity) {
			if (threshold_p != NULL) *threshold_p = thr;
			return ref_pack_len;
		} else {
			float r = (result_capacity / (float)ref_pack_len);

			// experimental values, adjust to best fit your use case
			if (r < 0.5) {
				thr += 0.8;
			} else if (r < 0.6) {
				thr += 0.5;
			} else if (r < 0.75) {
				thr += 0.4;
			} else if (r < 0.90) {
				thr += 0.25;
			} else {
				thr += 0.1;
			}
		}
	}
}



uint32_t vec_unpack(float *result, uint32_t result_capacity,
					const float *compr_data, uint32_t compr_length)
{
	uint32_t idx = 0;

	for (uint32_t i = 0; i < compr_length; i++) {
		if (IS_FZERO(compr_data[i])) {
			uint32_t zeroes = F2ZERO(compr_data[i]);
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



// ---- PACK WALKER CODE ----

// init the pack walker struct
void pw_init(pack_walker_t *wlkr, const float *packed_vec, uint32_t packed_len)
{
	wlkr->p_vec = packed_vec;
	wlkr->p_length = packed_len;

	wlkr->real_idx = 0;
	wlkr->p_idx = 0;
	wlkr->p_zero_j = 0;
	wlkr->p_zero_n = 0;
}



// rewind the struct to first entry, handle leading zero(s)
void pw_rewind(pack_walker_t *w)
{
	w->real_idx = 0;
	w->p_idx = 0;

	if (IS_FZERO(w->p_vec[0])) {
		// multi-zero
		w->p_zero_n = F2ZERO(w->p_vec[0]);
		w->p_zero_j = 1;
	} else {
		w->p_zero_j = 0;
		w->p_zero_n = 0;
	}
}



// get value at index in packed vector
float pw_get(pack_walker_t *w, uint32_t idx)
{
	if (idx < w->real_idx && idx > w->real_idx / 2) {
		// backtrack

		while (w->real_idx > idx) {
			if (w->p_zero_n && w->p_zero_j > 1) { // multi-zero value
				w->p_zero_j--;
			} else {
				// go to previous
				w->p_idx--;
				if (IS_FZERO(w->p_vec[w->p_idx])) {
					// multi-zero
					w->p_zero_n = F2ZERO(w->p_vec[w->p_idx]);
					w->p_zero_j = w->p_zero_n;
				} else {
					w->p_zero_n = 0; // no zeros
				}
			}

			w->real_idx--;
		}

	} else {
		// forward

		if (idx < w->real_idx || idx == 0) {
			pw_rewind(w);
		}

		// add until reached
		while (w->real_idx < idx && w->p_idx < w->p_length) {
			if (w->p_zero_n && w->p_zero_j < w->p_zero_n) { // multi-zero value
				w->p_zero_j++;
			} else {
				// go to next
				w->p_idx++;
				if (IS_FZERO(w->p_vec[w->p_idx])) {
					// multi-zero
					w->p_zero_n = F2ZERO(w->p_vec[w->p_idx]);
					w->p_zero_j = 1;
				} else {
					w->p_zero_n = 0; // no zeros
				}
			}

			w->real_idx++;
		}
	}

	// handle overflow. underflow impossible <- index is unsigned
	if (w->p_idx >= w->p_length) {
		w->p_idx = w->p_length - 1;
		return 0;
	}

	return w->p_zero_n ? 0 : w->p_vec[w->p_idx];
}
