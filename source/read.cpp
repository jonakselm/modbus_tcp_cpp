#include "modbus/modbus.h"
#include <cerrno>
#include <cstdio>

int main()
{
	modbus_t *mb = modbus_new_tcp("127.0.0.1", 1502);
	if (mb == NULL)
	{
		fprintf(stderr, "Unable to allocate libmodbus context\n");
		return -1;
	}
	int conRet = modbus_connect(mb);
	
	if (conRet == -1)
	{
		printf("Connection failed: %s\n", modbus_strerror(errno));
		modbus_free(mb);
		return -1;
	}

	//for (;;)
	{
		uint16_t rec;
		int recRet = modbus_read_registers(mb, 0, 1, &rec);

		if (recRet == -1)
		{
			printf("Read failed: %s\n", modbus_strerror(errno));
			modbus_free(mb);
			return -1;
		}
		printf("Received: %d\n", rec);
	}

	modbus_close(mb);
	modbus_free(mb);
	return 0;
}
