#include "modbus/modbus.h"
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <sys/socket.h>
#include "MBServer.hpp"

int main()
{
	//modbus_t *mb = modbus_new_tcp("0.0.0.0", 1502);
	//if (mb == NULL)
	//{
	//	fprintf(stderr, "Unable to allocate libmodbus context\n");
	//	return -1;
	//}
	//modbus_mapping_t *mb_mapping = modbus_mapping_new_start_address(0,
    //                                              0,
    //                                              0,
    //                                              0,
    //                                              0,
    //                                              32,
    //                                              0,
    //                                              0);
    //if (mb_mapping == NULL)
	//{
    //    fprintf(stderr, "Failed to allocate the mapping: %s\n", modbus_strerror(errno));
    //    modbus_free(mb);
    //    return -1;
    //}
	//mb_mapping->tab_registers[0] = 53;

	//int serverSocket = modbus_tcp_listen(mb, 2);

	//// refset is not modified unless a new socket is added
	//// fds will copy refset at the start of every loop
	//fd_set refset, fds;

	//FD_ZERO(&refset);
	//FD_SET(serverSocket, &refset);

	//int fdMax = serverSocket;

	////int aRet = modbus_tcp_accept(mb, &serverSocket);
	////if (aRet == -1)
	////{
	////printf("Tcp accept failed: %s\n", modbus_strerror(errno));
	////modbus_free(mb);
	////return -1;
	////}

	//printf("Starting\n");
	//uint8_t req[255];
	//int rc;
	//uint16_t c = 0;
	//for (;;)
	//{
	//	// Copy our reference set
	//	fds = refset;

	//	
	//	if (select(fdMax + 1, &fds, nullptr, nullptr, 0) == -1)
	//	{
	//		printf("Selecting socket error: %s\n", std::strerror(errno));
	//		break;
	//	}
	//	for (int sockfd = 0; sockfd <= fdMax; sockfd++)
	//	{
	//		if (FD_ISSET(sockfd, &fds))
	//		{
	//			printf("Manipulating socket: %d\n", sockfd);
	//			// Server has new connection, since its socket ready
	//			if (sockfd == serverSocket)
	//			{
	//				int clientfd = accept(serverSocket, nullptr, nullptr);
	//				if (clientfd != -1)
	//				{
	//					printf("Client connected fd: %d\n", clientfd);
	//					if (clientfd > fdMax)
	//					{
	//						fdMax = clientfd;
	//					}
	//					FD_SET(clientfd, &refset);
	//				}
	//				else
	//				{
	//					printf("Unable to accept client\n");
	//				}

	//			}
	//			// Other sockets
	//			else
	//			{
	//				int sRet = modbus_set_socket(mb, sockfd);
	//				if (sRet == -1)
	//				{
	//					printf("Set socket failed: %s\n", modbus_strerror(errno));
	//					break;
	//				}
	//				rc = modbus_receive(mb, req);
	//				if (rc > 0)
	//				{
	//					int rRet = modbus_reply(mb, req, rc, mb_mapping);
	//					if (rRet == -1)
	//					{
	//						printf("Reply failed: %s\n", modbus_strerror(errno));
	//						break;
	//					}
	//					printf("Request received rc= %d\n",rc);
	//				}
	//				else if (rc == -1)
	//				{
	//					printf("Connection closed on socket %d\n", sockfd);

	//					close(sockfd);

	//					FD_CLR(sockfd, &refset);

	//					if (sockfd == fdMax)
	//					{
	//						fdMax--;
	//					}
	//				}
	//			}
	//		}
	//	}
	//}

	//for (int i = 0; i < mb_mapping->nb_registers; i++)
	//{
	//	printf("%d: %d\n", i, mb_mapping->tab_registers[i]);
	//}

	//modbus_mapping_free(mb_mapping);
	//modbus_close(mb);
	//modbus_free(mb);
	
	MBServer server(1502);

	for (;;)
	{
		server.update();
	}

	return 0;
}
