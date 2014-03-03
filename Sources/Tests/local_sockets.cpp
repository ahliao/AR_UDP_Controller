// Local Server/Client Example

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>

#ifndef SHUT_WR
#define SHUT_RD 0
#define SHUT_WR 1
#define SHUT_RDWR 2
#endif

int main(int argc, char **argv) {
	int z;			// Status return code
	int s[2];		// Pair of sockets
	//char *msgp;		// A message pointer
	int mlen;		// Message length
	char buf[80];	// Work buffer
	pid_t chpid;	// Child PID

	// create pair of local sockets
	z = socketpair(AF_LOCAL, SOCK_STREAM, 0, s);

	if (z == -1) {
		fprintf(stderr, "%s socketpair(2)\n",
				strerror(errno));
		exit(1);
	}

	// now fork into two processes
	if ((chpid = fork()) == (pid_t)-1) {
		// failed to fork into two processes
		fprintf(stderr, "%s fork(2)\n",
				strerror(errno));
		exit(1);
	} else if (chpid == 0) {
		// This is the child process (client)
		char rxbuf[80];	// receive buffer
		printf("Parent PID is %ld\n", (long)getppid());

		close(s[0]);		// Server uses s[1]
		s[0] = -1;			// Forget this unit

		// form the message and its length
		const char *msgp = "%A %d-%b-%Y %l:%M %p";
		mlen = strlen(msgp);

		printf("Child sending request '%s'\n", msgp);
		fflush(stdout);

		// Write a request to the server
		z = write(s[1], msgp, mlen);
		if (z < 0) {
			fprintf(stderr, "%s write(2)\n",
					strerror(errno));
			exit(1);
		}

		// Shutting down the write side of the socket
		if (shutdown(s[1], SHUT_WR) == -1) {
			fprintf(stderr, "%s shutdown (2)\n",
					strerror(errno));
			exit(1);
		}

		// Receive the reply from the server
		z = read(s[1], rxbuf, sizeof rxbuf);
		if (z < 0) {
			fprintf(stderr, "%s read(2)\n",
					strerror(errno));
			exit(1);
		}

		// Put a null byte at the end of what we received
		rxbuf[z] = 0;

		// report the results
		printf("Server returned '%s'\n", rxbuf);
		fflush(stdout);

		close(s[1]);
	} else {
		// parent process (server)
		int status;			// child termination status
		char txbuf[80];		// reply buffer
		time_t td;			// current date and time

		printf("Child PID is %ld\n", (long) chpid);
		fflush(stdout);

		close(s[1]);
		s[1] = -1;

		z = read(s[0], buf, sizeof buf);

		if (z < 0) {
			fprintf(stderr, "%s read(2)\n",
					strerror(errno));
			exit(1);
		}

		buf[z] = 0;

		time(&td);

		strftime(txbuf, sizeof txbuf, buf, localtime(&td));

		z = write(s[0], txbuf, strlen(txbuf));
		if (z < 0) {
			fprintf(stderr, "%s write(2)\n",
					strerror(errno));
			exit(1);
		}

		close(s[0]);

		// wait for the child process to exit
		waitpid(chpid, &status, 0);
	}

	return 0;
}
