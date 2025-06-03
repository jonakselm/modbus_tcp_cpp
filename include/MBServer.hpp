#include "modbus/modbus.h"
#include <string>
#include <memory>
#include <array>
#include <vector>

struct MappingPair
{
	int startAddress = 0;
	int amount = 0;
};

struct MBMapping
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
	MBServer(const MBMapping &mapping);
	MBServer(const std::string &ip, int port);
	MBServer(const std::string &ip, const MBMapping &mapping);
	MBServer(int port, const MBMapping &mapping);
	MBServer(const std::string &ip, int port, const MBMapping &mapping);

	MBServer(MBServer &) = delete;
	MBServer(const MBServer &) = delete;
	MBServer operator=(MBServer &) = delete;
	MBServer operator=(const MBServer &) = delete;

	MBServer(MBServer &&) = default;
	MBServer &operator=(MBServer &&) = default;

	~MBServer() = default;


	void update();


	void writeHoldingRegister(int address, uint16_t value);
	void writeHoldingRegister(int address, const std::vector<uint16_t> &values);
	void writeHoldingRegister(int address, float value);
	void writeHoldingRegister(int address, const std::vector<float> &values);

	void writeInputRegister(int address, uint16_t value);
	void writeInputRegister(int address, const std::vector<uint16_t> &values);

	void writeCoil(int address, bool value);
	void writeDiscreteInput(int address, bool value);


	uint16_t readHoldingRegisterInt(int address) const;
	std::vector<uint16_t> readHoldingRegisterInt(int address, int nb) const;
	float readHoldingRegisterFloat(int address) const;
	std::vector<float> readHoldingRegisterFloat(int address, int nb) const;

	uint16_t readInputRegisterInt(int address) const;
	std::vector<uint16_t> readInputRegisterInt(int address, int nb) const;

	bool readCoil(int address);
	bool readDiscreteInput(int address);

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
