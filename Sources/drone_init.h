// File: drone_init.h
// Author: Pillar Technologies MDP Team
// Date Created: 3/9/2014
// Function: Header file for drone_init.cpp 

#ifndef DRONE_INIT_H
#define DRONE_INIT_H

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

// Macros for connecting to the drone
#define NAVDATA_PORT	5554
#define AT_PORT			5556
#define NAVDATA_BUFFER_SIZE		2048
#define WIFI_MYKONOS_IP			"192.168.1.1"

//int seq = 0;	// The sequence number
//char msg[NAVDATA_BUFFER_SIZE];	// navdata message

// The sockets connecting to the drone
extern int at_socket,			// sendto
		navdata_socket;	// recvfrom

// structs to hold the config of the socket addresses
extern struct sockaddr_in pc_addr, drone_at, drone_nav, from;

// REQUIRES: a is a valid float
// EFFECTS:  the 32-bit integer form of a
int IEEE754toInt(const float &a);

// REQUIRES: the global sockaddr_in structs, at_socket and navdata_socket
// MODIFIES: the required stuff
// EFFECTS:  Initializes the communication between the computer and drone
int init_ports();

#endif // DRONE_INIT_H