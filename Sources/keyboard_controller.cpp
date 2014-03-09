// Keyboard controller for the AR Drone 2
// with using UDP and AT commands
// for use by the Pillar Technologies MDP Team
// Created on: 3/2/2014

#include "navdata_common.h"

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

#define NAVDATA_PORT	5554
#define AT_PORT			5556
#define NAVDATA_BUFFER_SIZE		2048
#define WIFI_MYKONOS_IP			"192.168.1.1"

int seq = 0;
char msg[NAVDATA_BUFFER_SIZE];

int at_socket = -1,			// sendto
	navdata_socket = -1;	// recvfrom

char running = 1;			// set to 0 to halt all processes

struct sockaddr_in 	pc_addr, drone_at, drone_nav, from;

void awake()
{
	char command[256];
	char input = 0;
	while(1)
	{
		input = 0;
		if (seq < 2) // set the config for reduced data (twice for sureness)
			sprintf(command, "AT*CONFIG=%d,\"general:navdata_demo\",\"TRUE\"\r", seq);
		else if ((input = getch()) != 0) 
		{
			if (input == '1') 
				sprintf(command, "AT*CONFIG=%d,\"leds:leds_anim\",\"3,1073741824,2\"\r", seq);
			else if (input == '2') 
				sprintf(command, "AT*CONFIG=%d,\"leds:leds_anim\",\"2,1073741824,2\"\r", seq);
			else if (input == ' ')
				sprintf(command, "AT*REF=%d,%d\r", seq, 290718208);
			else if (input == 'z')
				sprintf(command, "AT*REF=%d,%d\r", seq, 290717696);
			else if (input == 'q') {
				running = 0;
				break;
			}
		}
		else
			sprintf(command, "AT*COMWDG=%d\r",seq); // reset comm watchdog

		sendto(at_socket, command, strlen(command), 0, 
				(struct sockaddr*)&drone_at, sizeof(drone_at));
		seq++;
		usleep(100000); // should be less than 0.5s to get all data
	}
	endwin();
	exit(0);
}

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

int main()
{
	// Init Curses
	WINDOW *wnd;	// pointer to the terminal window

	wnd = initscr();	// curses call to init window
	cbreak();			// config so no waiting for Enter
	//noecho();			// config for no echo of input
	//clear();

	navdata_t *data;	// struct that holds the navdata
	int l, size;
	int32_t one = 1, zero = 0;

	//std::cout << "AR Drone 2 Controller v0.01\n";

	if(init_ports()) 
	{
		//std::cout << "Error initializing the ports\n";
		exit(1);
	}

	// fork so that child process will continueously send a wakeup command
	// for the navdata port
	if (fork()) awake();

	if (bind(navdata_socket, (struct sockaddr *) &pc_addr, sizeof(pc_addr)) < 0) {
		//std::cout << "Error binding navdata_socket to pc_addr\n";
		exit(1);
	}

	// set unicast mode on
	sendto(navdata_socket, &one, 4, 0, (struct sockaddr *) &drone_nav, sizeof(drone_nav));

	// read the navdata received
	// TODO: Make the navdata reader the child
	while(running) {
		size = 0;
		size = recvfrom(navdata_socket, &msg[0], NAVDATA_BUFFER_SIZE, 0x0, 
				(struct sockaddr *)&from, (socklen_t *) &l);
		//std::cout << "\33[2J\nread " << size << " data " << std::endl;
		//for (int i = 0; i < size; ++i) std::cout << msg[i];

		data = (navdata_t *) msg;
		/*std::cout << "header " << data->header << 
			" battery " << ((navdata_demo_t*)((data->options)))->vbat_flying_percentage << std::endl <<
			" alt " << ((navdata_demo_t*)((data->options)))->altitude << std::endl <<
			" vx " << ((navdata_demo_t*)((data->options)))->vx << std::endl <<
			" theta " << ((navdata_demo_t*)((data->options)))->theta << std::endl;
		fflush(stdout);*/
	}

	// TODO closing program stuff (move to separate function)
	endwin();

	return 0;
}
