#include "modbus/modbus.h"
#include <cerrno>
#include <cstdio>

int main()
{
	modbus_t *mb = modbus_new_tcp("0.0.0.0", 1502);
	if (mb == NULL)
	{
		fprintf(stderr, "Unable to allocate libmodbus context\n");
		return -1;
	}
	modbus_mapping_t *mb_mapping = modbus_mapping_new_start_address(0,
                                                  0,
                                                  0,
                                                  0,
                                                  0,
                                                  32,
                                                  0,
                                                  0);
    if (mb_mapping == NULL)
	{
        fprintf(stderr, "Failed to allocate the mapping: %s\n", modbus_strerror(errno));
        modbus_free(mb);
        return -1;
    }
	mb_mapping->tab_registers[0] = 53;

	int sock = modbus_tcp_listen(mb, 2);
	int aRet = modbus_tcp_accept(mb, &sock);
	if (aRet == -1)
	{
		printf("Tcp accept failed: %s\n", modbus_strerror(errno));
		modbus_free(mb);
		return -1;
	}


	printf("Starting\n");
	uint8_t *req;
	int rc;
	uint16_t c = 0;
	for (;;)
	{
		do 
		{ 
			rc = modbus_receive(mb, req);
		} while (rc == 0);
		if (rc == -1)
		{
			printf("Receive failed: %s\n", modbus_strerror(errno));
			break;
		}
		printf("Request received rc= %d\n",rc);
		int rRet = modbus_reply(mb, req, rc, mb_mapping);
		if (rRet == -1)
		{
			printf("Reply failed: %s\n", modbus_strerror(errno));
			modbus_free(mb);
			return -1;
		}
	}

	for (int i = 0; i < mb_mapping->nb_registers; i++)
	{
		printf("%d: %d\n", i, mb_mapping->tab_registers[i]);
	}

	modbus_mapping_free(mb_mapping);
	modbus_close(mb);
	modbus_free(mb);
	return 0;
}
