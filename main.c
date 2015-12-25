#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "src/vec_match.h"

#define DATA_LEN 11

static float reference[DATA_LEN] = {
	0, 10, 20, 30, 40, 50, 40, 30, 20, 10, 0
};

static float data[DATA_LEN] = {
	0, 10, 20, 30, 40, 50, 50, 40, 30, 30, 10
};


static float data_cprs[16] = {
	0, 15.7, 0, 0, 0.1, 0.2, 0.1, 10, 24.242, 0, 0, 0.1, 0.2, 0.4, 0.5, 0
};

static float data_cprs2[16];

int main(void)
{
	for (int i = 0; i < 16; i++) {
		printf("%.1f, ", data_cprs[i]);
	}
	printf("\n");

	int len = vec_pack(data_cprs2, 16, data_cprs, 16, 2);
	printf("compressed len = %d\n", len);

	for (int i = 0; i < len; i++) {
		printf("%.1f, ", data_cprs2[i]);
	}
	printf("\n");


	return 0;

	vec_match_cfg_t cfg = {
		.length = DATA_LEN,
		.drift_x = 1,
		.offset_y = 5,
		.abs_threshold = 0.1
	};

	// example

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


	// error metric fields
	float env_e, abs_e;

	bool ok = vec_match(data, reference, &cfg, &env_e, &abs_e);
	printf("%s", ok ? "MATCH OK" : "MATCH FAILED");
	printf("\n");

	printf("Error rate: ENV %.2f, ABS %.2f\n", env_e, abs_e);
}
