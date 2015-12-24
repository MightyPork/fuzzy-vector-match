#pragma once

#include <stdint.h>
#include <stdbool.h>


/**
 * Match signal to reference, allowing for some offser and noise
 *
 * @param data matched data
 * @param ref reference data
 * @param length data length (data & ref length must be equal)
 * @param drift_x allowed horizontal drift (Hz drift if values are 1 Hz FFT bins)
 * @param offset_y allowed vertical offset (bin amplitude, positive or negative)
 * @param envl_match_error error metric calculated with allowed drift and offset
 * @param abs_match_error error metric calculated from raw data (can be used if envelope match passes)
 * @return envelope match status (match using drift and offset)
 */
bool vec_match(const float *data,
			   const float *ref,
			   uint32_t length,
			   uint8_t drift_x,
			   float offset_y,
			   float *envl_match_error,
			   float *abs_match_error);
