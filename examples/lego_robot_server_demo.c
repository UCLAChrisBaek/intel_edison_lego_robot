/*
 * lego_robot_server_demo.c
 *
 * Author: In Hwan "Chris" Baek
 *         chris.inhwan.baek@gmail.com
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <mraa.h>
#include "lego_robot.h"

void android_GUI_control(float, float);

int main()
{
	int server_sockfd, client_sockfd;
	int server_len, client_len;
	char buffer[256], x_buffer[256], y_buffer[256], command;
	float x_value, y_value, x_scaled, y_scaled;
	struct sockaddr_in server_addr, client_addr;
     
	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (server_sockfd < 0) {
		fprintf(stderr, "Failed to open socket\n");
		exit(1);
	} 

	bzero((char *) &server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("192.168.42.1");
	server_addr.sin_port = htons(4444);

	int optval = 1;
	setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));
	
	server_len = sizeof(server_addr);
	if (bind(server_sockfd, (struct sockaddr *) &server_addr, server_len) < 0) {
        	fprintf(stderr, "Failed to bind.\n");
		exit(1);      
	}
	
	listen(server_sockfd,5);

	client_len = sizeof(client_addr);

	client_sockfd = accept(server_sockfd, (struct sockaddr *) &client_addr, &client_len); 
    if (client_sockfd < 0) {
		fprintf(stderr, "Failed to accept.\n");
		exit(1);
	} 
	printf("Client connected.\n");

	initialize();
		
	while(1) {
		bzero(buffer,256);
     
		if (read(client_sockfd, buffer, 255) < 0) {
			fprintf(stderr, "Failed to read.\n");
			exit(1);
		}

		/*
		The incoming data is formatted as follows:
			x value | y value
		x value corresponds to the x coordinate of the point on the Android device screen being touched.
		y value corresponds to the y coordinate of the point.
		If the command is "rotate", the x value is "rotate" and the y value is "cw", "ccw", or "stop".
		*/
		int i, j;
		// get x value and store it in x_buffer
		for (i=0; i<255; i++) {
			if (buffer[i] != '|') {
				x_buffer[i] = buffer[i];
			}
			else {
				x_buffer[i] = '\0';
				break;
			}
		}
		i++;
		j = 0;
		// get y value and store it in y_buffer
		while (buffer[i] != '\0') {
			y_buffer[j] = buffer[i];
			i++;
			j++;
		}
		y_buffer[j] = '\0';

		//handle "rotate" command
		if (!strcmp(x_buffer, "rotate")) {
			if (!strcmp(y_buffer, "cw\n")) {
				mraa_pwm_write(pwm1, 1.0f);
				mraa_pwm_write(pwm2, 1.0f);
				rotate_CW();
			}
			else if (!strcmp(y_buffer, "ccw\n")) {
				mraa_pwm_write(pwm1, 1.0f);
				mraa_pwm_write(pwm2, 1.0f);
				rotate_CCW();
			}
			else if (!strcmp(y_buffer, "stop\n")) {
				mraa_pwm_write(pwm1, 0.0f);
				mraa_pwm_write(pwm2, 0.0f);
			}

			usleep(10000);
		}
		
		else {	
			x_value = atof(x_buffer);
			y_value = atof(y_buffer);

			if (x_value > 150)
				x_scaled = 1.0f;
			else if (x_value < -150)
				x_scaled = -1.0f;
			else
				x_scaled = x_value / 150;
		
			if (y_value > 150)
				y_scaled = 1.0f;
			else if (y_value < -150)
				y_scaled = -1.0f;
			else
				y_scaled = y_value / 150;
			
			android_GUI_control(x_scaled, y_scaled);
			usleep(10000);
		}
    }
    
	close(server_sockfd);
	close(client_sockfd);
     
	return 0; 
}

void android_GUI_control(float x_value, float y_value)
{
	float faster_speed, slower_speed;
	//go forward
	if (y_value >= 0) {		
		//right turn
		if (x_value >= 0) {
			faster_speed = y_value + x_value/2.5;
			slower_speed = y_value - x_value/2.5;
			if (faster_speed > 1.0f)
				faster_speed = 1.0f;
			if (slower_speed < 0)
				slower_speed = 0.0f;
				
			mraa_pwm_write(pwm1, faster_speed);
			mraa_pwm_write(pwm2, slower_speed);
		}
		//left turn
		else {
			faster_speed = y_value - x_value/2.5;
			slower_speed = y_value + x_value/2.5;
			if (faster_speed > 1.0f)
				faster_speed = 1.0f;
			if (slower_speed < 0)
				slower_speed = 0.0f;
			
			mraa_pwm_write(pwm1, slower_speed);
			mraa_pwm_write(pwm2, faster_speed);
		}
		
		forward_A();
		forward_B();
	}
	else {
		//right turn
		if (x_value >= 0) {
			faster_speed = y_value - x_value/2.5;
			slower_speed = y_value + x_value/2.5;
			if (faster_speed < -1.0f)
				faster_speed = -1.0f;
			if (slower_speed > 0)
				slower_speed = 0.0f;
	
			mraa_pwm_write(pwm1, -faster_speed);
			mraa_pwm_write(pwm2, -slower_speed);
		}
		//left turn
		else {
			faster_speed = y_value + x_value/2.5;
			slower_speed = y_value - x_value/2.5;
			if (faster_speed < -1.0f)
				faster_speed = -1.0f;
			if (slower_speed > 0)
				slower_speed = 0.0f;
			
			mraa_pwm_write(pwm1, -slower_speed);
			mraa_pwm_write(pwm2, -faster_speed);
		}
		reverse_A();
		reverse_B();
	}
		
}
