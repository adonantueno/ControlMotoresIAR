# ControlMotoresIAR
Desarrollo para control de motores sobre Arduino
Contiene ManejoMotoresAntena-Ver0.2.ino, version desarrollada para manejar a través de una interfaz de red los actuadores para el movimiento de los RT del IAR. 
Establece una interfaz de comunicación (Ethernet y/o UART) para que se reciban paquetes con instrucciones para endencer y apagar el sistema, mover y detener los motores (N-S Y E-O) como así también sensar el retorno para conocer el estado real de los motores e informar algunos eventos de error y warning.

Contiene cliente.py, usado para interactuar con el sketch de arduino.
