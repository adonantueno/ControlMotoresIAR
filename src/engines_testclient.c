/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#include </home/adonantueno/Proyectos/ControlMotoresIAR/include/iar_engines.h>
//#include <iar_engines.h>

#define PORT "3490" // the port client will be connecting to 

#define MAXBUFLEN 100 // max number of bytes we can get at once 

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
	uint16_t comando [CONTPACKETLEN] = {0x80,0x00};
	int sockfd, numbytes;  
	char buff[MAXBUFLEN];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];
	char mens_cli[MAXBUFLEN];
	char mensaje [3] = {0xA};
    struct timeval t;

// DEFINICION DE PAQUETE PARA ENVIAR
	packetid = 0x00;
	
	struct SAO_data_transport sao_packet, sao_packet_net;
    sao_packet.syncword           = SYNCWORD;
    sao_packet.hdr.version        = VERSION;
    sao_packet.hdr.packetid       = packetid;
    sao_packet.hdr.message_type   = REPORTPACKET;
    sao_packet.hdr.packet_counter = 0;
    sao_packet.hdr.pdl            = sizeof(struct SAO_data_transport_payload);
	
	printf (gettimeofday(sao_packet.payload.timestamp, NULL));
	strcpy (sao_packet.payload.data, comando);
    
    sao_packet.end                = END;
 
	printf("comando: %hhx \n", comando);
	//printf("Sync: % " PRIu16 " \n", sao_packet.syncword, "\n" );
	printf("sao packet Sync: %x \n",sao_packet.syncword);
	printf("sao packet version: %x \n",sao_packet.hdr.version);
	printf("sao packet pkid: %x \n",sao_packet.hdr.packetid);
	printf("sao packet mess: %x \n",sao_packet.hdr.message_type);
	printf("sao packet pkt count: %x \n",sao_packet.hdr.packet_counter);
	printf("sao packet pdl: %x \n",sao_packet.hdr.pdl);
	printf("sao packet tstmp: %lld \n",sao_packet.payload.timestamp[0]);
	printf("sao packet tstmp: %lld \n",sao_packet.payload.timestamp[1]);
	printf("sao packet data: %hhd \n",sao_packet.payload.data[0]);
	printf("sao packet data: %hhd \n",sao_packet.payload.data[1]);

	printf("sao packet end: %x \n",sao_packet.end);

	printf("tamaño paquete %i", sizeof sao_packet);
	fwrite(&sao_packet,1,MAXBUFLEN,stdout);
	printf("\n");

	bzero(&buff, BUFFLEN);


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

	
	//if (send(sockfd, &sao_packet, MAXDATASIZE, 0) == -1) 
	if (write(sockfd, &sao_packet, sizeof sao_packet) == -1)
	{
	    perror("send");
	    exit(1);
	} 
	if ((numbytes = read(sockfd, buff, MAXBUFLEN)) == -1)
	{
	    perror("recv");
	    exit(1);
	} 
	fwrite(buff,1,MAXBUFLEN,stdout);


	printf("client: received \"%s\"\n",buff);

	close(sockfd);

	return 0;
}

