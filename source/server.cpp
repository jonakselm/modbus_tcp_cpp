#include "modbus/modbus.h"
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <sys/socket.h>
#include "MBServer.hpp"

int main()
{
	MBServer server(1502);

	std::vector<float> v;
	v.emplace_back(4.f);
	v.emplace_back(6.3f);
	server.writeHoldingRegisterFloat(0,2.3);
	for (;;)
	{
		server.update();
	}

	return 0;
}
