#include "modbus/modbus.h"
#include <string>
#include <memory>
#include <array>

struct MappingPair
{
	int startAddress = 0;
	int amount = 0;
};

struct MappingData
{
	MappingPair bits;
	MappingPair inputBits;
	MappingPair regs;
	MappingPair inputRegs;
};

struct MappingDeleter
{
	void operator()(modbus_mapping_t *mapping)
	{
		modbus_mapping_free(mapping);
	}
};

struct MBDeleter
{
	void operator()(modbus_t *mb)
	{
		modbus_close(mb);
		modbus_free(mb);
	}
};

using ModbusUniquePtr = std::unique_ptr<modbus_t, MBDeleter>;
using MappingUniquePtr = std::unique_ptr<modbus_mapping_t, MappingDeleter>;

class MBServer
{
public:
	MBServer();
	MBServer(int port);
	MBServer(const std::string &ip);
	MBServer(const std::string &ip, int port);
	MBServer(const std::string &ip, int port, MappingData mapping);

	MBServer(MBServer &) = delete;
	MBServer(const MBServer &) = delete;
	MBServer operator=(MBServer &) = delete;
	MBServer operator=(const MBServer &) = delete;

	MBServer(MBServer &&) = default;
	MBServer &operator=(MBServer &&) = default;

	~MBServer() = default;

	void update();

private:
	ModbusUniquePtr m_mb;
	MappingUniquePtr m_mapping;
	std::string m_ip;
	int m_port;
	int m_serverSocket;
	int m_fdMax;
	fd_set m_refset;
	int m_lastRc;
	std::array<uint8_t, 255> m_req;
};
