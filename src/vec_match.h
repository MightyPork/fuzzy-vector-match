#pragma once

#include <stdint.h>
#include <stdbool.h>

/* Example: drift_x 1, offset_y 10, abs_threshold 0.1 */
typedef struct {
	uint32_t length; // data length (data & ref length must be equal)
	uint32_t drift_x; // allowed horizontal drift (Hz drift if values are 1 Hz FFT bins)
	float offset_y;  // allowed vertical offset (bin amplitude, positive or negative)
	float abs_threshold; // absolute threshold (to fix preccision errors, also added to offset_y)
} vec_match_cfg_t;


typedef struct {
	const float *p_vec;
	uint32_t p_length;
	uint32_t real_idx; // public index
	uint32_t p_idx;
	uint32_t p_zero_j;
	uint32_t p_zero_n;
} pack_walker_t;


/**
 * Populate a packed vector walker (used to read values without unpacking)
 *
 * @param wlkr pack walker instance to populate
 * @param packed_vec vec to walk
 * @param packed_len packed vec len
 */
void pw_init(pack_walker_t *wlkr, const float *packed_vec, uint32_t packed_len);


/**
 * Get a value from the packed vect.
 *
 * Note: random access is slower than sequential
 *
 * @param w walker
 * @param idx unpacked vector index to read
 * @return value. Returns 0 if index is out of bounds.
 */
float pw_get(pack_walker_t *w, uint32_t idx);


/**
 * Match signal to reference, allowing for some offser and noise
 *
 * @param data matched data
 * @param ref reference data
 * @param cfg config struct
 * @param fuzzy_match_error error metric calculated with allowed drift and offset
 * @param abs_match_error error metric calculated from raw data (can be used if envelope match passes)
 * @return envelope match status (match using drift and offset)
 */
bool vec_match(const float *data, const float *ref, const vec_match_cfg_t *cfg,
			   float *fuzzy_match_error, float *abs_match_error);


/**
 * Match vector against a packed reference vector (without unpacking).
 * Params otherwise the same as vec_match()
 */
bool vec_match_packed(const float *data, const float *ref_packed, uint32_t ref_p_len,
					  const vec_match_cfg_t *cfg, float *fuzzy_match_error, float *abs_match_error);


/**
 * Compress a vector by replacing sequence of zeroes with a negative value indicating their count.
 *
 * Returned length may exceed result_capacity, but the buffer is never overrun.
 * That can be used to gradually increase the threshold until the compressed data fits in the result buffer.
 *
 * The compression is by definition lossy.
 *
 * @param result result vector (can be the same as data vector for in-place operation)
 * @param result_capacity size of result buffer.
 * @param data data vector
 * @param length data legth
 * @param threshold max value to be considered zero in the compression
 * @return length of result vector
 */
uint32_t vec_pack(float *result, uint32_t result_capacity,
				  const float *data, uint32_t length,
				  float threshold);


/**
 * Unpack a vector compressed with vec_pack().
 *
 * If returned length exceeds provided buffer capacity, it's an indication that you need
 * to enlarge your buffer, or increase threshold.
 *
 * The buffer is never overrun, though.
 *
 * @param result result buffer
 * @param result_capacity result buffer size
 * @param compr_data compressed data vector
 * @param compr_length compressed data vector length
 * @return
 */
uint32_t vec_unpack(float *result, uint32_t result_capacity,
					const float *compr_data, uint32_t compr_length);


/**
 * Pack to given length, adjusting threshold automatically.
 *
 * @param result result array
 * @param result_capacity result max capacity
 * @param data data to compress
 * @param data_length data length
 * @param threshold_p field to store the used threshold, can be NULL
 * @return real result size
 */
uint32_t vec_pack_auto(float *result, uint32_t result_capacity,
					  const float *data, uint32_t data_length, float *threshold_p);
