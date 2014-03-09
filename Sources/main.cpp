// Controller for the AR Drone 2
// using UDP and AT commands
// for use by the Pillar Technologies MDP Team
// Created on: 3/2/2014

// Pillar headers
#include "keyboard_controller.h"
#include "drone_init.h"

// Headers from the Parrot API
#include "navdata_common.h"

// Libraries for networking and communciation
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

// NCurses API for the keyboard input
#include <curses.h>

char msg[NAVDATA_BUFFER_SIZE];

int main()
{
	// Init Curses
	initscr();	// curses call to init window
	cbreak();			// config so no waiting for Enter
	noecho();			// config for no echo of input
	clear();

	// NCurses variables
	int term_row, term_col;
	getmaxyx(stdscr, term_row, term_col);

	navdata_t *data;	// struct that holds the navdata
	int l, size;
	int32_t one = 1, zero = 0;

	//std::cout << "AR Drone 2 Controller v0.01\n";
	mvprintw(0,0,"AR Drone 2 Controller v0.02\n");

	if(init_ports()) 
	{
		mvprintw(0,0, "Error initializing the ports");
		exit(1);
	}

	if (bind(navdata_socket, (struct sockaddr *) &pc_addr, sizeof(pc_addr)) < 0) {
		mvprintw(1,0,"Error binding navdata_socket to pc_addr");
		exit(1);
	}

	// fork so that child process will continueously send a wakeup command
	// for the navdata port
	while(1) {
		if (drone_control()) break;

		// set unicast mode on
		sendto(navdata_socket, &one, 4, 0, (struct sockaddr *) &drone_nav, sizeof(drone_nav));

		// read the navdata received
		// TODO: Make the navdata reader the child
		mvprintw(3,0,"Navdata Received");
		size = 0;
		size = recvfrom(navdata_socket, &msg[0], NAVDATA_BUFFER_SIZE, 0x0, 
				(struct sockaddr *)&from, (socklen_t *) &l);
		mvprintw(4,0,"read %d", size); 
		data = (navdata_t *) msg;
		mvprintw(5,0,"header %d", data->header);
		mvprintw(6,0,"Battery %d", 
				((navdata_demo_t*)((data->options)))->vbat_flying_percentage);
		mvprintw(7,0,"Alt %d", 
				((navdata_demo_t*)((data->options)))->altitude);
		mvprintw(8,0,"Vx %d",
				((navdata_demo_t*)((data->options)))->vx);
		mvprintw(9,0,"Theta %d",
				((navdata_demo_t*)((data->options)))->theta);
	}
	// TODO closing program stuff (move to separate function)
	endwin();

	// close the sockets
	close(at_socket);
	close(navdata_socket);

	return 0;
}
