#include <iostream>
#include <Socket.h>
#include <defines.h>

int main(int argc, char* argv[]) {
	WSADATA data;
	Socket::initialize(&data);

	Socket client;
	client.open();

	client.connect(PORT, "127.0.0.1");

	//...

	Socket::cleanup();
	return 0;
}