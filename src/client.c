/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#include </home/adonantueno/Proyectos/socketC/include/socket.h>

#define PORT "3490" // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

//ACA DEBERÍA TENER LAS FUNCIONES PRIMARIAS PARA HACER LA COMUNICACION


int main(int argc, char *argv[])
{
	uint16_t packetid;
	int sockfd, numbytes;  
	char buff[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];
	char mens_cli[MAXDATASIZE];
	char mensaje [3] = {0xA};
    struct timeval t;

/* DEFINICION DE PAQUETE PARA ENVIAR
	packetid = 0x00;
    t.tv_sec  = 1;
    t.tv_usec = 0;
	struct SAO_data_transport_payload payload;

	gettimeofday(&payload.timestamp, NULL);
	strcpy(&payload.data,mensaje);

	struct SAO_data_transport sao_packet, sao_packet_net;
    sao_packet.syncword           = SYNCWORD;
    sao_packet.hdr.version        = VERSION;
    sao_packet.hdr.packetid       = packetid;
    sao_packet.hdr.message_type   = REPORTPACKET;
    sao_packet.hdr.packet_counter = 0;
    sao_packet.hdr.pdl            = sizeof(struct SAO_data_transport_payload);
	
	gettimeofday(sao_packet.payload.timestamp, NULL);
	sao_packet.payload.data[0] = 0x41;
    sao_packet.payload.data[1] = 0x52;

	//strcpy(sao_packet.payload.data, mensaje);

	//strcpy(sao_packet.payload.data, mensaje);
	//memcpy(*sao_packet.payload.data,0XA0,sizeof(struct SAO_data_transport_payload));  
    sao_packet.end                = END;


*/
	//printf("sao packet %s", sao_packet.syncword);

	bzero(&buff, BUFFLEN);

	/*sao_packet_net = sao_packet;

    sao_packet_net.syncword     = htons (sao_packet_net.syncword     );
    sao_packet_net.hdr.packetid = htons (sao_packet_net.hdr.packetid );
    sao_packet_net.hdr.pdl      = htons (sao_packet_net.hdr.pdl      );
    sao_packet_net.end          = htons (sao_packet_net.end          );

*/

	if (argc != 2) {
	    fprintf(stderr,"usage: client hostname\n");
	    exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			perror("client: connect");
			close(sockfd);
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure

	strcpy(mens_cli, "Prueba");	

	//if (send(sockfd, &sao_packet, MAXDATASIZE, 0) == -1) 
	if (write(sockfd, mens_cli, MAXDATASIZE) == -1)
	{
	    perror("send");
	    exit(1);
	} 
	if ((numbytes = read(sockfd, buff,MAXDATASIZE)) == -1)
	{//if ((numbytes = recv(sockfd, buff, MAXDATASIZE, 0)) == -1) {
	    perror("recv");
	    exit(1);
	} 
	
	//buf[numbytes] = '\0';

	printf("client: received \"%s\"\n",buff);

	close(sockfd);

	return 0;
}

