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


int main()
{
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

	bool ok = vec_match(data, reference, DATA_LEN, 1, 5, &env_e, &abs_e);
	printf("%s", ok ? "MATCH OK" : "MATCH FAILED");
	printf("\n");

	printf("Error rate: ENV %.2f, ABS %.2f\n", env_e, abs_e);
}
