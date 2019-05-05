#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define BUFLEN 1024
#define MAX_CLIENTS 100

int main (int argc, char* argv[]) {

	if (argc != 4) {
		printf("\n Usage: %s <ID_CLIENT> <ID_SERVER> <PORT_SERVER>\n", argv[0]);
		return 1;
	}

	char buffer[BUFLEN];
	char command[BUFLEN];
	int n;

	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	int portno = atoi(argv[3]);
	if (portno < 0) {
		printf("Port invalid!\n");
	}
	serv_addr.sin_port = htons(portno);
	int ret = inet_aton(argv[2], &serv_addr.sin_addr);
	if (ret<0) {
		printf("Eroare la inet aton!\n");
	}

	// opening sockets
	int sockUDP = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockUDP < 0) {
		printf("Eroare la socket UDP!\n");
	}

	int sockTCP = socket(AF_INET, SOCK_STREAM, 0);
	if (sockTCP < 0) {
		printf("Eroare la socket TCP!\n");
	}

	ret = connect(sockTCP, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	if (ret < 0){
		printf("Eroare la conectare! %d\n",ret);
	}

	fd_set read_fds;	// multimea de citire folosita in select()
	fd_set tmp_fds;		// multime folosita temporar
	int fdmax;			// valoare maxima fd din multimea read_fds

	
	// se goleste multimea de descriptori de citire (read_fds) si multimea temporara (tmp_fds)
	FD_ZERO(&read_fds);
	FD_ZERO(&tmp_fds);
	
	// se adauga noul file descriptor (socketul pe care se asculta conexiuni) in multimea read_fds
	FD_SET(STDIN_FILENO, &read_fds);
	FD_SET(sockTCP, &read_fds);

	while(1) {
		tmp_fds = read_fds;
		//citirea de la tastatura
		// memset(buffer, 0, BUFLEN);
		// fgets(buffer, BUFLEN-1, stdin);

		// if (strncmp(buffer, "exit", 4) == 0){
		// 	break;
		// }

		// n = send(sockTCP, buffer, strlen(buffer), 0);
		// if (n<0) {
		// 	printf("Eroare la trimiterea mesajului catre server!\n");
		// }

		if (FD_ISSET(sockTCP, &tmp_fds)) {
			memset(buffer, 0, BUFLEN);
			ssize_t n = recv(sockTCP, buffer, BUFLEN, 0);
			if (strstr(buffer, "exiy") != NULL) {
				printf("CLIENT DISCONNECTED.\n");
				close(sockTCP);
				close(sockUDP);
				return 0;
			}
		} else if (FD_ISSET(STDIN_FILENO, &tmp_fds)) {
			// read from keyboard
			memset(buffer, 0, BUFLEN);
			fgets(buffer, BUFLEN-1, stdin);
			strcpy(command, buffer);
			char* cmd[2];
			char* pch = strtok(buffer, " \n");
			int i=0;
			while(pch != NULL) {
				cmd[i] = pch;
				pch = strtok(NULL, " \n");
				i++;
			}
			if (strcmp(cmd[1], "subscribe") == 0){
				printf("SUBSCRIBE!\n");
			}
		}
	}
	close(sockTCP);
	close(sockUDP);
	return 0;
}