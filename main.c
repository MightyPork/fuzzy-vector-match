#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "src/vec_match.h"

#define DATA_LEN 16
#define REF_LEN 10

static float reference[DATA_LEN] = {
	//0, 10, 20, 30, 40, 50, 40, 30, 20, 10, 0
	0, 15.7, 0, 0, 0.1, 0.2, 0.1, 10, 24.242, 7, 0, 2, 0.2, 0.4, 0.5, 0
};

static float data[DATA_LEN] = {
	//0, 10, 20, 30, 40, 50, 50, 35, 15, 15, 0
	0, 15.7, 0, 0, 0.1, 0.2, 0.1, 10, 24, 6, 3, 2, 0.2, 0.4, 0.5, 0
};

static float ref_p[REF_LEN];


//static float data_cprs[16] = {
//	0, 15.7, 0, 0, 0.1, 0.2, 0.1, 10, 24.242, 0, 0, 2, 0.2, 0.4, 0.5, 0
//};

//static float data_packed[16];

int main(void)
{
/*	for (int i = 0; i < 16; i++) {
		printf("%.1f, ", data_cprs[i]);
	}
	printf("\n");

	int pklen = vec_pack(data_packed, 16, data_cprs, 16, 2);
	printf("compressed len = %d\n", pklen);

	for (int i = 0; i < pklen; i++) {
		printf("%.1f, ", data_packed[i]);
	}
	printf("\n");

	pack_walker_t pw;
	pw_init(&pw, data_packed, pklen);

	for(int i=0; i<16; i++) {
		printf("Value at %d is %.1f\n", i, pw_get(&pw, i));
	}

	int i = 8;
	printf("Value at %d is %.1f\n", i, pw_get(&pw, i));
	i = 7;
	printf("Value at %d is %.1f\n", i, pw_get(&pw, i));
	i = 1;
	printf("Value at %d is %.1f\n", i, pw_get(&pw, i));

	i = 15;
	printf("Value at %d is %.1f\n", i, pw_get(&pw, i));

	i = 11;
	printf("Value at %d is %.1f\n", i, pw_get(&pw, i));
	i = 12;
	printf("Value at %d is %.1f\n", i, pw_get(&pw, i));*/


/*	len = vec_unpack(data_cprs3, 16, data_cprs2, len);
	printf("unpacked len = %d\n", len);

	for (int i = 0; i < len; i++) {
		printf("%.1f, ", data_cprs3[i]);
	}
	printf("\n");
*/

	//return 0;

	vec_match_cfg_t cfg = {
		.length = DATA_LEN,
		.drift_x = 1,
		.offset_y = 1,
		.abs_threshold = 0.1
	};

	// example

	printf("REF:  ");
	for (int i = 0; i < DATA_LEN; i++) {
		printf("%.1f, ", reference[i]);
	}
	printf("\n");


	printf("MEAS: ");
	for (int i = 0; i < DATA_LEN; i++) {
		printf("%.1f, ", data[i]);
	}
	printf("\n");


	// error metric fields
	float env_e, abs_e;

	bool ok = vec_match(data, reference, &cfg, &env_e, &abs_e);
	printf("%s", ok ? "MATCH OK" : "MATCH FAILED");
	printf("\n");

	printf("Error rate: ENV %.2f, ABS %.2f\n", env_e, abs_e);

	float thr;
	int ref_pack_len = vec_pack_auto(ref_p, REF_LEN, reference, DATA_LEN, &thr);

	printf("Reference packed with zero threshold %.1f to %d items.\n", thr, ref_pack_len);
	printf("REF packed:  ");
	for (int i = 0; i < ref_pack_len; i++) {
		printf("%.1f, ", ref_p[i]);
	}
	printf("\n");

	pack_walker_t pw;
	pw_init(&pw, ref_p, ref_pack_len);

	printf("Value #8 = %.1f\n", pw_get(&pw, 8));
	printf("Value #7 = %.1f\n", pw_get(&pw, 7));
	printf("Value #15 = %.1f\n", pw_get(&pw, 15));

	printf("Trying packed match\n");
	ok = vec_match_packed(data, ref_p, ref_pack_len, &cfg, &env_e, &abs_e);
	printf("%s", ok ? "MATCH OK" : "MATCH FAILED");
	printf("\n");

	printf("Error rate: ENV %.2f, ABS %.2f\n", env_e, abs_e);
}
