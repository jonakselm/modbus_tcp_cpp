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

MBServer::MBServer(const std::string &ip, int port)
	: MBServer::MBServer(ip, port, {{0,0},{0,0},{0,10},{0,0}}) {}

MBServer::MBServer(const std::string &ip, int port, MappingData mapping)
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
