/*
 * lego_robot_basic_operation.c
 *
 * Author: In Hwan "Chris" Baek
 *         chris.inhwan.baek@gmail.com
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mraa.h>
#include "lego_robot.h"

#define MAXBUFSIZ 1024

int main()
{
	float speed;
	char speed_user_input[MAXBUFSIZ];
	char turn_user_input[MAXBUFSIZ];
	
	initialize();
	
	while (1) {
		printf("Speed value (-1.0 to 1.0): ");
		scanf("%s", speed_user_input);
		speed = atof(speed_user_input);

		set_speed(speed);
		
		printf("Turn (S, CW, CCW): ");                                      
        scanf("%s", turn_user_input);
        
        if (!strcmp(turn_user_input, "S") || !strcmp(turn_user_input, "s")) {
			straight(speed);
		}
		
		else if (!strcmp(turn_user_input, "CW") || !strcmp(turn_user_input, "cw")) {
			rotate_CW();
		}

		else if (!strcmp(turn_user_input, "CCW") || !strcmp(turn_user_input, "ccw")) {
			rotate_CCW();
		}
 
		else
			printf("Wrong input!\n");
			
		sleep(1);
		stop();
	}

	return 0;
}