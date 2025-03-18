#include <iostream>
#include <Socket.h>
#include <defines.h>

int main(int argc, char* argv[]) {
	Socket client;
	client.open();

	client.connect(PORT, "127.0.0.1");

	//...

	return 0;
}