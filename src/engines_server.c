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

//#include </home/adonantueno/Proyectos/ControlMotoresIAR/include/iar_engines.h>
#include <iar_engines.h>

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
	

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

//void verificarPayload(uint8_t * comando, struct Command * comandos){

void verificarPayload(uint8_t * comando, uint8_t * typeMsg, int fd){
	int cmdValido = 0;
	uint8_t cmdIngenieria = 0;
	if (typeMsg == 0x8D)
		cmdIngenieria = 1;
	for (int i = 0; i <= 12; i++) {
       	if(comando==comandosValidos[i].comando)
		{
			cmdValido = 1;
			if (cmdIngenieria == 1){
				// HAY QUE INCORPORAR EL ING AL NOMBRE DEL COMANDO O 
				// REIMPLEMENTAR LA HASH TABLE Y LOS CODIGOS ACEPTADOS
				printf("Es de ING \n");
			}
			else
			{
				printf("No es de ing \n");
				(comandosValidos[i].cmd)(fd);
			}
		}
	}
	if (cmdValido == 0)
		printf("El comando solicitado no existe");
}	
		

//Llamados a funciones arduino

//void norteLento (char *ans)
void norteLento(int fd)
{
	printf("Entroooooo \n");
	if (write(fd,"E",sizeof(char)) == -1)
	{
		perror("arduino");
		close(fd);
		exit(0);
	}

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
	printf("ING");
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

char telemetria(int fd)
{	if (write(fd,"W",sizeof(char)) == -1)
	{
		perror("arduino");
		close(fd);
		exit(0);
	}

}

int main(void)
{
	//inicialización de hash table
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

	int fd, sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	int yes=1;
	socklen_t addr_len;
	char s[INET6_ADDRSTRLEN];
	int rv, numbytes;
	uint16_t packetid;
	void *puntero;
 	struct SAO_data_trasnport *ptr;
	uint8_t comandoRecibido[2] = {0,0};
	uint8_t tipoMensaje;
	int cmdValido = 0;
	struct SAO_data_transport sao_packet, sao_packet_net;

	//Estructura utilizada para parsear los mensajes recibidos
	union control
	{
		struct SAO_data_transport	paquete;
	} recibe = {puntero}, *recibeptr=&recibe;


	
	// APERTURA DE PUERTO COM --> a donde debería ir?
		
	fd = open_port("/dev/stdout");


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

		if ((numbytes = read(new_fd, &recibe, (sizeof sao_packet)+1 )) == -1)
		{
			perror("recv");
			exit(1);
		}
		
		printf("Bytes recibidos %d \n", numbytes);
	
	//Asignacion de variables para lookup table
		comandoRecibido[0]=recibeptr->paquete.payload.data[0];
		printf("comando recibido %hhx \n", comandoRecibido[0]);
		tipoMensaje = recibeptr->paquete.hdr.message_type;

	//Llamado a funcion que verifica el payload		
		verificarPayload(comandoRecibido[0],tipoMensaje, fd);


	//Debo enviar telemetria por multicast
		if (write(new_fd, "telemetria", sizeof("telemetria")) == -1)
		{
			perror("send");
			close(new_fd);
			exit(0);
		}

	}
	close(new_fd);

	return 0;
}

