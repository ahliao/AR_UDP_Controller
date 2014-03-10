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

// ffmpeg library for video decoding
#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>

// NCurses API for the keyboard input
#include <curses.h>

//char navmsg[NAVDATA_BUFFER_SIZE];

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

	// set unicast mode on
	sendto(navdata_socket, &one, 4, 0, 
			(struct sockaddr *) &drone_nav, sizeof(drone_nav));

	// fork so that child process will continueously send a wakeup command
	// for the navdata port
	while(1) {
		if (drone_control()) break;

		// read the navdata received
		if (get_navdata()) break;
	}
	// TODO closing program stuff (move to separate function)
	endwin();

	// close the sockets
	close(at_socket);
	close(navdata_socket);

	return 0;
}
