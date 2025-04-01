#include "MBServer.hpp"
#include <modbus/modbus.h>
#include <iostream>
#include <cstring>
#include <sys/socket.h>

MBServer::MBServer()
	: MBServer::MBServer("0.0.0.0", 502) {}

MBServer::MBServer(int port)
	: MBServer::MBServer("0.0.0.0", port) {}

MBServer::MBServer(const std::string &ip)
	: MBServer::MBServer(ip, 502) {}

MBServer::MBServer(const MBMapping &mapping)
	: MBServer::MBServer("0.0.0.0", 502, mapping) {}

MBServer::MBServer(const std::string &ip, int port)
	: MBServer::MBServer(ip, port, {{0,0},{0,0},{0,1024},{0,1024}}) {}

MBServer::MBServer(const std::string &ip, const MBMapping &mapping)
	: MBServer::MBServer(ip, 502, mapping) {}

MBServer::MBServer(int port, const MBMapping &mapping)
	: MBServer::MBServer("0.0.0.0", port, mapping) {}

MBServer::MBServer(const std::string &ip, int port, const MBMapping &mapping)
	: m_ip(ip), m_port(port), m_fdMax(0), m_lastRc(0)
{
	m_mb = ModbusUniquePtr(modbus_new_tcp(ip.c_str(), port));
	if (!m_mb)
	{
		std::cerr << "Failed to allocate libmodbus context" << std::endl;
		std::exit(EXIT_FAILURE);
	}
	m_mapping = MappingUniquePtr(modbus_mapping_new_start_address(
		mapping.bits.startAddress, mapping.bits.amount,
		mapping.inputBits.startAddress, mapping.inputBits.amount,
		mapping.regs.startAddress, mapping.regs.amount,
		mapping.inputRegs.startAddress, mapping.inputRegs.amount));
	if (!m_mapping)
	{
		std::cerr << "Failed to allocate mapping" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	m_serverSocket = modbus_tcp_listen(m_mb.get(), 255);
	if (m_serverSocket == -1)
	{
		std::cerr << "Failed to listen for clients: " << modbus_strerror(errno) << std::endl;
		std::exit(EXIT_FAILURE);
	}

	m_fdMax = m_serverSocket;

	FD_ZERO(&m_refset);
	FD_SET(m_serverSocket, &m_refset);
}

void MBServer::update()
{
	fd_set fdset = m_refset;

	if (select(m_fdMax+1, &fdset, nullptr, nullptr, nullptr) == -1)
	{
		std::cerr << "Error selecting socket: " << std::strerror(errno) << std::endl;
		std::exit(EXIT_FAILURE);
	}

	for (int sockfd = 0; sockfd <= m_fdMax; sockfd++)
	{
		if (FD_ISSET(sockfd, &fdset))
{
			if (sockfd == m_serverSocket)
			{
				int clientfd = accept(m_serverSocket, nullptr, nullptr);
				if (clientfd > 0)
				{
					std::cout << "Client connected fd: " << clientfd << std::endl;
					FD_SET(clientfd, &m_refset);
					// Increase max fd if clientfd is higher
					m_fdMax = std::max(clientfd, m_fdMax);
				}
				else
				{
					std::cerr << "Unable to accept client: " << std::strerror(errno) << std::endl;
				}
			}
			else
			{
				int ret = modbus_set_socket(m_mb.get(), sockfd);
				if (ret == -1)
				{
					std::cerr << "Set socket failed: " << modbus_strerror(errno) << std::endl;
					std::exit(EXIT_FAILURE);
				}
				m_lastRc = modbus_receive(m_mb.get(), m_req.data());
				if (m_lastRc > 0)
				{
					ret = modbus_reply(m_mb.get(), m_req.data(), m_lastRc, m_mapping.get());
					if (ret == -1)
					{
						std::cerr << "Reply failed: " << modbus_strerror(errno) << std::endl;
						std::exit(EXIT_FAILURE);
					}
				}
				else if (m_lastRc == -1)
				{
					std::cout << "Client disconnected fd: " << sockfd << std::endl;
					
					close(sockfd);

					FD_CLR(sockfd, &m_refset);

					if (sockfd == m_fdMax)
					{
						m_fdMax--;
					}
				}
			}
		}
	}
}

void MBServer::writeHoldingRegister(int address, uint16_t value)
{
	writeHoldingRegister(address, std::vector<uint16_t>(1, value));
}

void MBServer::writeHoldingRegister(int address, const std::vector<uint16_t> &values)
{
	modbus_write_registers(m_mb.get(), address, values.size(), values.data());
}

void MBServer::writeHoldingRegister(int address, float value)
{
	writeHoldingRegister(address, std::vector<float>(1, value));
}

void MBServer::writeHoldingRegister(int address, const std::vector<float> &values)
{
	// All sizes are double because float takes two registers
	int nb = values.size() * 2;
	for (int i = 0; i < values.size(); i++)
	{
		if (address + i * 2 + 2 > m_mapping->nb_registers)
		{
			nb = i * 2;
			std::cerr << "Exceeded max registers" << std::endl;
			break;
		}
		modbus_set_float_cdab(values[i], m_mapping->tab_registers + i * 2);
	}
	modbus_write_registers(m_mb.get(), address, nb, m_mapping->tab_registers);
}

void MBServer::writeInputRegister(int address, uint16_t value)
{
	writeInputRegister(address, std::vector<uint16_t>(1, value));
}

void MBServer::writeInputRegister(int address, const std::vector<uint16_t> &values)
{
	for (int i = 0; i < values.size(); i++)
	{
		m_mapping->tab_input_registers[i] = values[i];
	}
}

uint16_t MBServer::readHoldingRegisterInt(int address) const
{
	return readHoldingRegisterInt(address, 1).front();
}

std::vector<uint16_t> MBServer::readHoldingRegisterInt(int address, int nb) const
{
	std::vector<uint16_t> v(nb);
	if (nb <= m_mapping->nb_registers)
	{
		modbus_read_registers(m_mb.get(), address, nb, v.data());
	}
	else
	{
		std::cerr << "Trying to read from non-existant registers" << std::endl;
	}
	return v;
}

float MBServer::readHoldingRegisterFloat(int address) const
{
	return readHoldingRegisterFloat(address, 1).front();
}

std::vector<float> MBServer::readHoldingRegisterFloat(int address, int nb) const
{
	// All sizes are double because float takes two registers
	std::vector<float> v(nb);
	for (int i = 0; i < v.size(); i++)
	{
		if (address + nb * 2 > m_mapping->nb_input_registers)
		{
			std::cerr << "Trying to read from non-existant registers" << std::endl;
			break;
		}
		v[i] = modbus_get_float_cdab(m_mapping->tab_registers + address + i * 2);
	}
	return v;
}

uint16_t MBServer::readInputRegisterInt(int address) const
{
	return readInputRegisterInt(address, 1).front();
}

std::vector<uint16_t> MBServer::readInputRegisterInt(int address, int nb) const
{
	std::vector<uint16_t> v(nb);
	for (int i = 0; i < v.size(); i++)
	{
		v.push_back(m_mapping->tab_input_registers[i]);
	}
	return v;
}
