all: receive send

receive: readArduino.c serial.c
	gcc -o receive readArduino.c serial.c -g

send: transmit.c
	gcc -o send transmit.c -g