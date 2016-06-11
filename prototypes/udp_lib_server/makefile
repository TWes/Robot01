UDP_LIBRARIE=inet_udp
UDP_LIBRARIE_HEADDERS=../../install/libraries

all: main.cpp
	g++-4.8 main.cpp -std=gnu++11 -pthread -I$(UDP_LIBRARIE_HEADDERS) -L$(UDP_LIBRARIE_HEADDERS) -l$(UDP_LIBRARIE) -o udp_socket_server
