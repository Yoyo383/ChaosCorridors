#include <iostream>
#include "sockets.hpp"

int main() {
	sockets::initialize();

	// do server stuff

	sockets::terminate();
}