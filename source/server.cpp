#include "MBServer.hpp"
#include <iostream>

int main()
{
	MBServer server(1502);

	std::vector<float> v;
	v.emplace_back(4.f);
	v.emplace_back(6.3f);
	server.writeHoldingRegister(0, v);
	for (;;)
	{
		v = server.readHoldingRegisterFloat(0, 2);
		for (auto f : v)
		{
			std::cout << f << std::endl;
		}
		server.update();
	}

	return 0;
}
