/*
* Test Project for Translational State estimation and sensor fusion
* Avionics14
* 2014 Raphael Taylor-Davies, Cambridge University Spaceflight
*/

#include "stdlib.h"
#include <stdio.h>
#include "translation_kalman.h"

int main(void)
{
	FILE *ifp = fopen("Input.txt", "r");
	if (ifp == NULL) {
		fprintf(stderr, "Can't open input text file\n");
		exit(1);
	}

	FILE *ofp = fopen("Output.txt", "w");
	if (ofp == NULL) {
		fprintf(stderr, "Can't open output text file\n");
		exit(1);
	}


	float accel[3];
	float altitude;
	translational_state_t state;


	while (fscanf(ifp, "%f, %f, %f, %f", 
		&accel[0], &accel[1], &accel[2], &altitude) != EOF) {
		
		translation_kalman_new_accel(accel);
		translation_kalman_new_pressure(altitude);

		translation_kalman_prediction_step(&state);

		printf("Pos: (%f,%f,%f) Vel: (%f,%f,%f) Accel: (%f,%f,%f)\n",state.pos[0],state.pos[1],state.pos[2],state.vel[0],state.vel[1],state.vel[2],state.accel[0],state.accel[1],state.accel[2]);
	}


	fclose(ifp);
	fclose(ofp);

	return 0;
}