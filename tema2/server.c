#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#define BUFLEN 1024
#define MAX_CLIENTS 100

void usage(char *file)
{
	fprintf(stderr, "Usage: %s <SERVER_PORT>\n", file);
	exit(0);
}

int main(int argc, char *argv[])
{
	int newsockfd, portno;
	char buffer[BUFLEN];
	struct sockaddr_in serv_addr;
	int n, i, ret;

	portno = atoi(argv[1]);
	if (portno < 0)
		printf("Invalid port. Try another one.\n");

	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	struct sockaddr_in cli_addr;
	socklen_t clilen = (socklen_t) sizeof (clilen);

	int sockUDP = socket(PF_INET, SOCK_DGRAM, 0);
  	if (sockUDP < 0)
    	printf("Error opening UDP socket.\n");

    ret = bind(sockUDP, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr)); 
    if (ret < 0)
    	printf("Error binding UDP socket.\n");

    int sockTCP = socket(AF_INET, SOCK_STREAM, 0);
  	if (sockTCP < 0)
    	printf("Error opening TCP socket.\n");

    ret = bind(sockTCP, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr)); 
  	if (ret < 0)
    	printf("Error binding TCP socket.\n");

    ret = listen(sockTCP, MAX_CLIENTS);
	if (ret < 0)
		printf("Error listening from TCP socket.\n");

	fd_set read_fds;	// reading set used for select() method
	fd_set tmp_fds;		// temporary used set
	int fdmax;			// greatest fd from read_fds set

	// empty the read_fds set and the tmp_fds set
	FD_ZERO(&read_fds);
	FD_ZERO(&tmp_fds);
	
	// add the new file descriptor in read_fds set
	FD_SET(STDIN_FILENO, &read_fds);	// data from the user
  	FD_SET(sockUDP, &read_fds);			// data from UDP client
	FD_SET(sockTCP, &read_fds);			// data from TCP client
	
	fdmax = sockTCP;

	int clientid = 0;
	while (1) {
		tmp_fds = read_fds; 
		ret = select(fdmax + 1, &tmp_fds, NULL, NULL, NULL);
		if (ret < 0)
			printf("Error selecting.\n");

		for (i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &tmp_fds)) {

				// reading from stdin
				if(i == STDIN_FILENO){
						memset(buffer, 0, BUFLEN);
						scanf("%s", &buffer);
						if (strstr(buffer, "exit") != NULL) {
							printf("Server disconnected.\n");
							return 0;
						}
						printf("%s\n", &buffer);
				}

				else if (i == sockTCP) { //subscribe, unsubscribe, exit
					clientid++;
					newsockfd = accept(sockTCP, (struct sockaddr *) &cli_addr, &clilen);
					if (newsockfd < 0) {
						printf("Error in newsockfd\n");
					}

					// add the fd which was returned by accept() to the set.
					FD_SET(newsockfd, &read_fds); 
					if (newsockfd > fdmax) { 
						fdmax = newsockfd; // update fdmax
					}

					// print the new connection
					printf("New client %d connected from %s:%d\n",
							clientid , "127.0.0.1", portno);

					recvfrom(sockTCP, buffer, BUFLEN, 0, (struct sockaddr *) &cli_addr, &clilen);
            		
            		// parse the stdin command
            		char *cmd[2];
            		char *cmds = strtok(buffer, " \n");
            		int j = 0;
            		while (cmds != NULL) {
              			cmd[j] = cmds;
              			cmds = strtok(NULL, " \n");
              			j++;
					}
					
					// check for each command
					if (strcmp(cmd[0], "subscribe") == 0) {
						printf("Subscribe command received.\n");
						/**
						SUBSCRIBE COMMAND CODE
						**/
					}

					if (strcmp(cmd[0], "unsubscribe") == 0) {
						printf("Unsubscribe command received.\n");
						/**
						UNSUBSCRIBE COMMAND CODE
						**/
					}

				} else {
				
					memset(buffer, 0, BUFLEN);
					n = recv(i, buffer, sizeof(buffer), 0);
					if (n < 0) 
						printf("Error receving message.\n");
					
					n = send(i, buffer, sizeof(buffer), 0);
					if (n < 0)
						printf("Error sending message.\n");

					if (n == 0) {
						// close connection
						printf("Subscriber disconnected.");
						//close(i);
						
						// se scoate din multimea de citire socketul inchis 
						FD_CLR(i, &read_fds);
					} else {

						printf ("S-a primit de la clientul de pe socketul %d mesajul: %s\n", i, buffer);
					}
				}
			}
		}
	}


	close(sockTCP);
	close(sockUDP);

	return 0;
}