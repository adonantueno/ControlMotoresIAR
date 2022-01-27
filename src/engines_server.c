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
#include <termios.h>
#include <fcntl.h>

#include <iar_engines.h>

#define PORT "3490"  // the port users will be connecting to

#define MAXBUFLEN 100

#define BACKLOG 10	 // how many pending connections queue will hold

static union {
	struct status_bits st;
	uint8_t      	   bitval;
} statusbits;

//open port copiado de enco_to_net
int open_port(char *device)
{
    int fd;
    struct termios io;

    fd = open (device, O_RDWR | O_NOCTTY | O_NDELAY);

    if (fd > 0)
    {
        memset (&io, 0, sizeof(io));

        // Baudrate
        cfsetispeed(&io, CONTROLBAUDRATE);
        cfsetospeed(&io, CONTROLBAUDRATE);

        // Enable receiver and set local mode
        io.c_cflag |= (CLOCAL | CREAD);

        // No parity (8N1)
        io.c_cflag &= ~PARENB;
        io.c_cflag &= ~CSTOPB;
        io.c_cflag &= ~CSIZE;
        io.c_cflag |= CS8;

        io.c_cflag &= ~CRTSCTS;

        // Set the new options for the port
        tcsetattr(fd, TCSANOW, &io);

        // Flush Buffer
        usleep(500);
        tcflush(fd, TCIOFLUSH);
	    }
    return fd;
}
	
// Open socket

int open_socket(char *address, int port, struct sockaddr_in *addr)
{
    int sock;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Error opening socket.");
        exit(1);
    }

    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = inet_addr(address);
    addr->sin_port = htons(port);

    return sock;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

//void verificarPayload(uint8_t * comando, struct Command * comandos){

int verificarPayload(uint8_t * comando, uint8_t * typeMsg, int fd, comandos *validos){
	int cmdValido = 0;
	uint8_t cmdIngenieria = 0;
	printf("El comando recibido es: %hhx \n", *comando);
	if (*typeMsg == 0x8D)
		cmdIngenieria = 1; 
	for (int i = 0; i <= COMANDOS; i++) {
       	if(*comando==validos[i].comando)
		{
			cmdValido = 1;
			(validos[i].cmd)(fd);
			/*
			Aqui se evalua si es un comando de ing. 
			if (cmdIngenieria == 1){
				// HAY QUE INCORPORAR EL ING AL NOMBRE DEL COMANDO O 
				// REIMPLEMENTAR LA HASH TABLE Y LOS CODIGOS ACEPTADOS
				printf("Es de ING \n");
			}
			else
			{
				printf("No es de ing \n");
				(validos[i].cmd)(fd);
			}
			*/
			break;

		}
	}
	if (cmdValido == 0)
		printf("El comando solicitado no existe");
		return -1;
}	
		

//Llamados a funciones arduino


void norteLento(int fd)
{
	int numbytes = 0;
	void *buf;
	printf("Entroooooo \n");
	
	if (write(fd,"E",sizeof(char)) == -1)
	{
		perror("arduino");
		close(fd);
		exit(0);
	}
	if ((numbytes = read(fd,&buf,1)) == -1){
		perror("lectura");
		close (fd);
		exit(0);
	}
	printf("numbytes %d \n", numbytes);
	//printf("telemetria %c \n", telemetria(fd,buf,1));

}

void norteRapido (int fd)
{
	if (write(fd,"A",sizeof(char)) == -1)
	{
		perror("arduino");
		close(fd);
		exit(0);
	}	
}
void surLento (int fd)
{	
	if (write(fd,"F",sizeof(char)) == -1)
	{
		perror("arduino");
		close(fd);
		exit(0);
	}
}
void surRapido (int fd)
{	
	if (write(fd,"B",sizeof(char)) == -1)
	{
		perror("arduino");
		close(fd);
		exit(0);
	}
}
void esteLento (int fd)
{
	if (write(fd,"G",sizeof(char)) == -1)
	{
		perror("arduino");
		close(fd);
		exit(0);
	}
}
void esteRapido (int fd)
{	
	if (write(fd,"C",sizeof(char)) == -1)
	{
		perror("arduino");
		close(fd);
		exit(0);
	}
}
void oesteLento (int fd)
{	if (write(fd,"H",sizeof(char)) == -1)
	{
		perror("arduino");
		close(fd);
		exit(0);
	}
}
void oesteRapido (int fd)
{	if (write(fd,"D",sizeof(char)) == -1)
	{
		perror("arduino");
		close(fd);
		exit(0);
	}
}
void norteLentoIng (int fd)
{	
	printf("ING \n");
	
	if (write(fd,"M",sizeof(char)) == -1)
	{
		perror("arduino");
		close(fd);
		exit(0);
	}
	
}

void norteRapidoIng (int fd)
{	if (write(fd,"I",sizeof(char)) == -1)
	{
		perror("arduino");
		close(fd);
		exit(0);
	}
}
void surLentoIng (int fd)
{	if (write(fd,"N",sizeof(char)) == -1)
	{
		perror("arduino");
		close(fd);
		exit(0);
	}
}
void surRapidoIng (int fd)
{	if (write(fd,"J",sizeof(char)) == -1)
	{
		perror("arduino");
		close(fd);
		exit(0);
	}
}
void esteLentoIng (int fd)
{	if (write(fd,"O",sizeof(char)) == -1)
	{
		perror("arduino");
		close(fd);
		exit(0);
	}
}
void esteRapidoIng (int fd)
{	if (write(fd,"K",sizeof(char)) == -1)
	{
		perror("arduino");
		close(fd);
		exit(0);
	}
}
void oesteLentoIng (int fd)
{	if (write(fd,"P",sizeof(char)) == -1)
	{
		perror("arduino");
		close(fd);
		exit(0);
	}
}
void oesteRapidoIng (int fd)
{	if (write(fd,"L",sizeof(char)) == -1)
	{
		perror("arduino");
		close(fd);
		exit(0);
	}
}

void pararNorteSur (int fd)
{	if (write(fd,"Y",sizeof(char)) == -1)
	{
		perror("arduino");
		close(fd);
		exit(0);
	}
 
}
void pararEsteOeste (int fd)
{	if (write(fd,"Z",sizeof(char)) == -1)
	{
		perror("arduino");
		close(fd);
		exit(0);
	}
}

void pararMotores (int fd)
{	if (write(fd,"X",sizeof(char)) == -1)
	{
		perror("arduino");
		close(fd);
		exit(0);
	}
}

void encender (int fd)
{	if (write(fd,"T",sizeof(char)) == -1)
	{
		perror("arduino");
		close(fd);
		exit(0);
	}
}
void apagar (int fd)
{	if (write(fd,"U",sizeof(char)) == -1)
	{
		perror("arduino");
		close(fd);
		exit(0);
	}
}

void status (int fd)
{
	uint8_t val;
	int ret;
	char buf[5], id[2];
	
	tcflush(fd,TCIOFLUSH);
	
	if (write(fd, "W",sizeof(char)) == -1)
	{
		perror("arduino");
		close(fd);
		exit(0);
	}
	else
	{
		/*
		Respuesta esperada (54bytes):
		OL:0\r\n                                                                         
		OR:0\r\n                                                                         
		EL:0\r\n                                                                         
		ER:0\r\n                                                                         
		SL:0\r\n                                                        
		SR:0\r\n
		LG:0\r\n                                                                                                             
		NL:0\r\n                                                        
		NR:0\r\n

		vamos a generar un campo de bits sobre un uint8_t:
		bit: 7  6  5  4  3  2  1  0
		     NR NL SR SL ER EL OR OL
		*/
		do
		{
			ret = read(fd, buf, 6);
			if (ret == 6)
			{
				memcpy(&id, &buf[0], 2);
				val = atoi(&buf[3]);
				printf("%c%c%c%c  0x%02X 0x%02X\n", 
						buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);

				if 		(strcmp(id, "OL"))
					statusbits.st.OL = val;
				else if (strcmp(id, "OR"))
					statusbits.st.OR = val;
				else if (strcmp(id, "EL"))
					statusbits.st.EL = val;
				else if (strcmp(id, "ER"))
					statusbits.st.ER = val;
				else if (strcmp(id, "SL"))
					statusbits.st.SL = val;
				else if (strcmp(id, "SR"))
					statusbits.st.SR = val;
				else if (strcmp(id, "NL"))
					statusbits.st.NL = val;
				else if (strcmp(id, "NR"))
				{
					statusbits.st.NR = val;
					break;
				}
				else if (strcmp(id, "LG"))
					continue;
			}
			else
				break;
		} while (1);
	}

	printf("Status: 0x%02X\n", statusbits.bitval);
}
/*
char telemetria(int fd, void *buf, size_t count)
{	
	
	if (write(fd,"W",sizeof(char)) == -1)
	{
		perror("arduino");
		close(fd);
		exit(0);
	}
	if (read(fd,&buf,count) == -1){
		perror("lectura arduino");
		close(fd);
		exit(0);
	}
	return buf;

}
*/
int main(void)
{
	
	//struct Command comandosValidos[COMANDOS];
	//inicialización de hash table
	//comandos comandosValidos[COMANDOS];
	//memset(comandosValidos, 0, sizeof(comandosValidos));
	comandos comandosValidos[COMANDOS] = {
		{0x80, norteLento},
		{0x8A, norteLentoIng},
		{0x40, norteRapido},
		{0x4A, norteRapidoIng},
		{0x20, surLento},
		{0x2A, surLentoIng},
		{0x10, surRapido},
		{0x1A, surRapidoIng},
		{0x08, esteLento},
		{0x8A, esteLentoIng},
		{0x04, esteRapido},
		{0xA4, esteRapidoIng},
		{0x02, oesteLento},
		{0xA2, oesteLentoIng},
		{0x01, oesteRapido},
		{0xA1, oesteRapidoIng},
		{0xC0, pararNorteSur},
		{0xC1, pararEsteOeste},
		{0xC2, pararMotores},
		{0xB0, encender},
		{0xB1, apagar},
		{0xB2, status}
	};


	int fd, sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	int rv, numbytes;
 	struct SAO_data_transport *ptr;
	uint8_t comandoRecibido = 0;
	uint8_t tipoMensaje;
	//int cmdValido = 0;
	struct SAO_data_transport sao_packet, sao_packet_net;

	//Estructura utilizada para parsear los mensajes recibidos
	union control
	{
		struct SAO_data_transport	paquete;
		uint8_t data[sizeof(struct SAO_data_transport)];
	} recibe;


	// APERTURA DE PUERTO COM, esto debe ser un link simbolico
	// al puerto real (por ej: ln -s /dev/ttyUSB0 /dev/ctrlmotores)
	fd = open_port(DEVICE);

	statusbits.bitval = 0x0;

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
	while (1)
	{
		printf("server: waiting for connections...\n");
		
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			exit(-1);
		}

		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s);
		printf("server: got connection from %s\n", s);

		while(1) {
			if ((numbytes = read(new_fd, &recibe.data, (sizeof (struct SAO_data_transport))) == -1))
			{
				perror("recv");
				exit(1);
			}
			
			printf("Bytes recibidos %d \n", numbytes);
		
		//Asignacion de variables para lookup table
			comandoRecibido = recibe.paquete.payload.data;
			tipoMensaje = recibe.paquete.hdr.message_type;

		//Llamado a funcion que verifica el payload		
		//manda comando recibido, tipo de msj y file descriptor para
		//comunicacion con arduino

		if (verificarPayload(&comandoRecibido, &tipoMensaje, fd, comandosValidos) < 0)
			continue;
		
		//Debo enviar telemetria por multicast
		//	if (write(new_fd, "telemetria", sizeof("telemetria")) == -1)
		//	{
		//		perror("send");
		//		close(new_fd);
		//		exit(0);
		//	}

		}
		close(new_fd);
	}

	return 0;
}

