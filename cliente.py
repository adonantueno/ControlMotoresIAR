# -*- coding: utf-8 -*-

# Programa Cliente
# www.pythondiario.com
 
import struct
import socket
import sys
import time
 
# Creando un socket TCP/IP
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#message=[]
#l=bytearray(0)
 
# Conecta el socket en el puerto cuando el servidor esté escuchando
server_address = ('163.10.43.23', 6666)
print >>sys.stderr, 'conectando a %s puerto %s' % server_address
sock.connect(server_address)
respuesta = 1
while (respuesta != 0):
	l=bytearray(0)
#	print("Que comando quiere ejecutar?")
	print("1 - NORTE LENTO \n2 - NORTE RAPIDO \n3 - SUR LENTO \n4 - SUR RAPIDO \n5 - ESTE LENTO \n6 - ESTE RAPIDO \n7 - OESTE LENTO \n8 - OESTE RAPIDO \n9 - PAQUETE MAL ARMADO \n10 - APAGAR MOTOR NORTE-SUR \n11 - APAGAR MOTOR ESTE-OESTE \n12 - APAGAR TODOS LOS MOTORES \n13 - MANDAR COMANDO INVALIDO \n0 - Salir")
	respuesta = int(input("Que comando quiere ejecutar"))
	if respuesta == 1:
	        l.append(struct.pack('>B',160))
		l.append(struct.pack('>B',142))
	        l.append(struct.pack('>B',1))
		l.append(struct.pack('>B',128))
	    	l.append(struct.pack('>B',159))
	elif respuesta == 2:
   		l.append(struct.pack('>B',160))
		l.append(struct.pack('>B',142))
		l.append(struct.pack('>B',1))
	  	l.append(struct.pack('>B',64))
		l.append(struct.pack('>B',159))
	elif respuesta == 3:
		l.append(struct.pack('>B',160))
	    	l.append(struct.pack('>B',142))
		l.append(struct.pack('>B',1))
		l.append(struct.pack('>B',32))
		l.append(struct.pack('>B',159))
	elif respuesta == 4:
		l.append(struct.pack('>B',160))
		l.append(struct.pack('>B',142))
		l.append(struct.pack('>B',1))
		l.append(struct.pack('>B',16))
		l.append(struct.pack('>B',159))
	elif respuesta == 5:
		l.append(struct.pack('>B',160))
		l.append(struct.pack('>B',142))
		l.append(struct.pack('>B',1))
		l.append(struct.pack('>B',8))
		l.append(struct.pack('>B',159))
	elif respuesta == 6:
		l.append(struct.pack('>B',160))
		l.append(struct.pack('>B',142))
		l.append(struct.pack('>B',1))
		l.append(struct.pack('>B',4))
		l.append(struct.pack('>B',159))
	elif respuesta == 7:
		l.append(struct.pack('>B',160))
		l.append(struct.pack('>B',142))
		l.append(struct.pack('>B',1))
		l.append(struct.pack('>B',2))
		l.append(struct.pack('>B',159))
	elif respuesta == 8:
		l.append(struct.pack('>B',160))
		l.append(struct.pack('>B',142))
		l.append(struct.pack('>B',1))
		l.append(struct.pack('>B',1))
		l.append(struct.pack('>B',159))
	elif respuesta == 9:
		l.append(struct.pack('>B',170))
                l.append(struct.pack('>B',142))
                l.append(struct.pack('>B',1))
                l.append(struct.pack('>B',1))
                l.append(struct.pack('>B',159))
        elif respuesta == 10:
                l.append(struct.pack('>B',160))
                l.append(struct.pack('>B',142))
                l.append(struct.pack('>B',1))
                l.append(struct.pack('>B',192))
                l.append(struct.pack('>B',159))
        elif respuesta == 11:
                l.append(struct.pack('>B',160))
                l.append(struct.pack('>B',142))
                l.append(struct.pack('>B',1))
                l.append(struct.pack('>B',193))
                l.append(struct.pack('>B',159))
        elif respuesta == 12:
                l.append(struct.pack('>B',160))
                l.append(struct.pack('>B',142))
                l.append(struct.pack('>B',1))
                l.append(struct.pack('>B',194))
                l.append(struct.pack('>B',159))
        elif respuesta == 13:
                l.append(struct.pack('>B',160))
                l.append(struct.pack('>B',142))
                l.append(struct.pack('>B',1))
                l.append(struct.pack('>B',88))
                l.append(struct.pack('>B',159))
	else:
		print("Saldremos del programa")
		break
	sock.sendall(l)
    	data =  0
	fin = "159"
	print >>sys.stderr, 'tamaño messag "%s"' % len(l)
    # Buscando respuesta
	amount_received = 0
	amount_expected = len(l)
#    amount_expected = len(message)
#    while amount_received < amount_expected:
	if respuesta != 9:
		while data != fin:
	    		data = sock.recv(3)
    #    amount_received += len(data)
    			print >>sys.stderr, 'recibiendo "%s"' % data
		print >>sys.stderr, 'Se recibió todo'
	raw_input("Presione una tecla para continuar...")

print >>sys.stderr, 'cerrando socket'
sock.close()
