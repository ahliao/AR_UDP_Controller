// File: drone_init.cpp
// Author: Pillar Technologies MDP Team
// Date Created: 3/9/2014
// Function: Contains the functions for connecting the
//			 computer to the AR Drone 2 through UDP/TCP

#include "drone_init.h"

// The sockets connecting to the drone
int at_socket = -1,			// sendto
	navdata_socket = -1;	// recvfrom

// structs to hold the config of the socket addresses
struct sockaddr_in 	pc_addr, drone_at, drone_nav, from;

// Converts the IEEE 754 floating-point format to the respective integer form
int IEEE754toInt(const float &a)
{
	union {		// bad practice, but it works
		float f;
		int i;
	} u;
	u.f = a;
	return u.i;
}

// Initialize the ports for the AT commands and the Navdata
int init_ports() 
{
	if ((at_socket = socket (AF_INET, SOCK_DGRAM, 0)) < 0) {
		std::cerr << "Error creating at_socket\n";
		return 1;
	}

	if ((navdata_socket = socket (AF_INET, SOCK_DGRAM, 0)) < 0) {
		std::cerr << "Error creating navdata_socket\n";
		return 1;
	}

	// for recvfrom
	pc_addr.sin_family = AF_INET;
	pc_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	pc_addr.sin_port = htons(0);

	// for sendto AT
	drone_at.sin_family = AF_INET;
	drone_at.sin_addr.s_addr = inet_addr(WIFI_MYKONOS_IP);
	drone_at.sin_port = htons(AT_PORT);

	// for sendto navdata init
	drone_nav.sin_family = AF_INET;
	drone_nav.sin_addr.s_addr = inet_addr(WIFI_MYKONOS_IP);
	drone_nav.sin_port = htons(NAVDATA_PORT);

	return 0;	// return zero if nothing went wrong
}
