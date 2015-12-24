#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <float.h>

#define DATA_LEN 11

#define SQUARE(a) ((a)*(a))


static float reference[DATA_LEN] = {
	0, 10, 20, 30, 40, 50, 40, 30, 20, 10, 0
};


static float data[DATA_LEN] = {
	0, 10, 20, 30, 40, 50, 40, 30, 20, 10, 0
};



/**
 * Calculate fuzzy envelope
 *
 * @param data source data
 * @param envelope envelope (same length as source data)
 * @param length data length
 * @param drift_x horizontal offset (left/right growth)
 * @param drift_y vertical offset (vertical growth)
 */
void calc_envelope(const float *data, float *envelope, uint32_t length, uint8_t drift_x, float drift_y)
{
	int a, b, i, j;

	for (i = 0; i < (int)length; i++) {
		float peak = FLT_MIN;

		// find highest value in the surrounding drift_x points
		a = i - drift_x;
		b = i + drift_x;
		if (a < 0) a = 0;
		if (b >= (int)length) b = length - 1;

		for (j = a; j <= b; j++) {
			if (peak < data[j]) peak = data[j];
		}

		// apply drift_y
		peak += drift_y;
		envelope[i] = peak;
	}
}



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
bool match_envelope(const float *data,
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
			printf("data[%d] out of range: %f, [%f ; %f]\n", i, data[i], base, peak);

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




int main()
{
	printf("REF:  ");
	for (int i = 0; i < DATA_LEN; i++) {
		printf("%.0f, ", reference[i]);
	}
	printf("\n");

	printf("MEAS: ");
	for (int i = 0; i < DATA_LEN; i++) {
		printf("%.0f, ", data[i]);
	}
	printf("\n");

	float env_e, abs_e;

	bool ok = match_envelope(data, reference, DATA_LEN, 1, 5, &env_e, &abs_e);
	printf("%s", ok ? "MATCH OK" : "MATCH FAILED");
	printf("\n");

	printf("Error rate: ENV %.2f, ABS %.2f\n", env_e, abs_e);

	/*
	calc_envelope(data, envelope, 20, 1, 5);


	for (int i = 0; i < 20; i++) {
		printf("%.0f, ", envelope[i]);
	}
	printf("\n");
	*/
}
