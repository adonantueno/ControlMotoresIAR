/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#include </home/adonantueno/Proyectos/ControlMotoresIAR/include/iar_engines.h>
//#include <iar_engines.h>

#define PORT "3490"  // the port users will be connecting to

#define MAXBUFLEN 100

#define BACKLOG 10	 // how many pending connections queue will hold

#define COMANDOS 13  //CANTIDAD DE COMANADOS VALIDOS

// Definiciones de funciones dinÃ¡micas
//typedef void (*command)(byte *respuesta, boolean ing);
typedef void (*command)(int ing); 

struct Command {
  uint8_t comando;
  command cmd; 
};

// DefiniciÃ³n arreglo para lookup table
struct Command comandosValidos[COMANDOS], *ptrComandosValidos;


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

//void verificarPayload(uint8_t * comando, struct Command * comandos){

void verificarPayload(uint8_t * comando){
	int cmdValido = 0;
	int cmdIngenieria = 0;
	printf("recibido: %hhx \n",comando);
	for (int i = 0; i <= 12; i++) {
		printf("Valido corresp: %hhx \n",comandosValidos[i].comando);
       	if(comando==comandosValidos[i].comando)
		{
			cmdValido = 1; // Is valid command
			printf("Comando valido: %i\n",cmdValido);
			printf("El comando recibido es: %hhx \n", comandosValidos[i].comando);
			printf("cmd %s \n",*comandosValidos[i].cmd);
			(comandosValidos[i].cmd)(cmdValido);
		}
	}
}	
		

//Llamados a funciones arduino

//void norteLento (char *ans)
void norteLento(int ing)
{
	printf("Entroooooo");
}

void norteRapido (char *ans)
{
}
void surLento (char *ans)
{
}
void surRapido (char *ans)
{
}
void esteLento (char *ans)
{
}
void esteRapido (char *ans)
{
}
void oesteLento (char *ans)
{
}
void oesteRapido (char *ans)
{
}
void norteLentoIng (char *ans)
{
}

void norteRapidoIng (char *ans)
{
}
void surLentoIng (char *ans)
{
}
void surRapidoIng (char *ans)
{
}
void esteLentoIng (char *ans)
{
}
void esteRapidoIng (char *ans)
{
}
void oesteLentoIng (char *ans)
{
}
void oesteRapidoIng (char *ans)
{
}

void pararNorteSur (char *ans)
{
 
}
void pararEsteOeste (char *ans)
{
}

void pararMotores (char *ans)
{
}

void encender (char *ans){
}
void apagar (char *ans){
}

char telemetria(){

}

int main(void)
{
	memset(comandosValidos, 0, sizeof(comandosValidos));
	comandosValidos[0].comando        = 0x80;
	comandosValidos[0].cmd            = norteLento; 
	comandosValidos[1].comando        = 0x40;
	comandosValidos[1].cmd            = norteRapido; 
	comandosValidos[2].comando        = 0x20;
	comandosValidos[2].cmd            = surLento; 
	comandosValidos[3].comando        = 0x10;
	comandosValidos[3].cmd            = surRapido; 
	comandosValidos[4].comando        = 0x08;
	comandosValidos[4].cmd            = esteLento; 
	comandosValidos[5].comando        = 0x04;
	comandosValidos[5].cmd            = esteRapido; 
	comandosValidos[6].comando        = 0x02;
	comandosValidos[6].cmd            = oesteLento; 
	comandosValidos[7].comando        = 0x01;
	comandosValidos[7].cmd            = oesteRapido;
	comandosValidos[8].comando        = 0xc0;
	comandosValidos[8].cmd            = pararNorteSur;
	comandosValidos[9].comando        = 0xc1;
	comandosValidos[9].cmd            = pararEsteOeste;
	comandosValidos[10].comando       = 0xc2;
	comandosValidos[10].cmd           = pararMotores;
	comandosValidos[11].comando       = 0xb0;
	comandosValidos[11].cmd           = encender;
	comandosValidos[12].comando       = 0xb1;
	comandosValidos[12].cmd           = apagar;

	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	int yes=1;
	socklen_t addr_len;
	//char buf[MAXBUFLEN];
	char s[INET6_ADDRSTRLEN];
	int rv, numbytes;
	uint16_t packetid;
	void *puntero;
 	struct SAO_data_trasnport *ptr;
	uint8_t comandoRecibido[2] = {0,0};
	int cmdValido = 0;

	union control
	{
		struct SAO_data_transport	paquete;
		//uint16_t                          syncword;
    	//struct SAO_data_transport_header  hdr;
    	//struct SAO_data_transport_payload payload;
    	//uint16_t                          end;
	} recibe = {puntero}, *recibeptr=&recibe;

	

   	struct SAO_data_transport sao_packet, sao_packet_net;
  

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	printf("server: waiting for connections...\n");
	
	while(1) {  // main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s);
		printf("server: got connection from %s\n", s);

//		if ((numbytes = read(new_fd, recibeptr, (sizeof sao_packet)+1 )) == -1)
		
		if ((numbytes = read(new_fd, &recibe, (sizeof sao_packet)+1 )) == -1)
		{
			perror("recv");
			exit(1);
		}
		
	//Asignacion de variabl para lookup table
		comandoRecibido[0]=recibeptr->paquete.payload.data[0];
			
		verificarPayload(comandoRecibido[0]);

		printf("Bytes %d \n", numbytes);
		/*
  		printf("Se Recibió: \n");
		printf("sao packet Sync: %x \n",recibeptr->paquete.syncword);
		printf("sao packet version: %d \n",recibeptr->paquete.hdr.version);
		printf("sao packet pkid: %d \n",recibeptr->paquete.hdr.packetid);
		printf("sao packet mess: %x \n",recibeptr->paquete.hdr.message_type);
		printf("sao packet pkt count: %d \n",recibeptr->paquete.hdr.packet_counter);
		printf("sao packet pdl: %d \n",recibeptr->paquete.hdr.pdl);
		printf("sao packet tstmp: %lld \n",recibeptr->paquete.payload.timestamp[0]);
		printf("sao packet tstmp: %lld \n",recibeptr->paquete.payload.timestamp[1]);
		printf("sao packet data: %hhx \n",recibeptr->paquete.payload.data[0]);
		printf("sao packet data: %hhx \n",recibeptr->paquete.payload.data[1]);

		printf("sao packet end: %x \n",recibeptr->paquete.end);

		printf("listener: packet is %d bytes long\n", numbytes);
		*/

		if (write(new_fd, recibeptr, MAXBUFLEN) == -1)
		{
			perror("send");
			close(new_fd);
			exit(0);
		}

	}
	close(new_fd);

	return 0;
}

