#include <iostream>
#include <string>
#include <Socket.h>
#include <defines.h>

int main(int argc, char* argv[]) {
	Socket sock;
	sock.open();
	sock.serve(PORT);
	
	while (true) {
		Socket client = sock.accept();

		// ...
		// client.send()...
	}

	return 0;
}