#include "Servidor.h"



Servidor::Servidor()
{
}


Servidor::~Servidor()
{
	FD_CLR(m_listening, &m_master);
	closesocket(m_listening);

	std::string msg = "Server is shutting down. Goodbye\r\n";
	while (m_master.fd_count > 0)
	{
		// Get the socket number
		SOCKET sock = m_master.fd_array[0];

		// Send the goodbye message
		send(sock, msg.c_str(), msg.size() + 1, 0);

		// Remove it from the master file list and close the socket
		FD_CLR(sock, &m_master);
		closesocket(sock);
	}

	// Cleanup winsock
	WSACleanup();

}
bool Servidor::run()
{
	if (!initialize())return false;
	Bind();
	// Create the master file descriptor set and zero it
	FD_ZERO(&m_master);
	// Add our first socket that we're interested in interacting with; the listening socket!
	// It's important that this socket is added for our server or else we won't 'hear' incoming
	// connections 
	FD_SET(m_listening, &m_master);
	m_running = true;
	return true;
}
// Initialize winsock and create a socket
bool Servidor::initialize()
{
	//Initialize winsock
	m_ver = MAKEWORD(2, 2);
	if (WSAStartup(m_ver, &m_wsData) != 0) {
		std::cout << "Can't Initialize winsock! ALV" << std::endl;
		return false;
	}
	//Create a socket
	m_listening= socket(AF_INET, SOCK_STREAM, 0);
	if (m_listening == INVALID_SOCKET) {
		std::cout << "Can't create a socket!!! ALV" << std::endl;
		return false;
	}
	return true;
}
//Bind the ip address and port to a socket
void Servidor::Bind()
{
	m_hint.sin_family = AF_INET;
	m_hint.sin_port = htons(54000);
	m_hint.sin_addr.S_un.S_addr = INADDR_ANY; // Could also use inet_pton .... 
	
	bind(m_listening, (sockaddr*)&m_hint, sizeof(m_hint));

	// Tell winsock thesocket is for listening
	listen(m_listening, SOMAXCONN);

}

void Servidor::sendRecv()
{

	m_copy = m_master;
	m_socketCount = select(0, &m_copy, nullptr, nullptr, nullptr);

	for (int i = 0; i < m_socketCount; i++)
	{
		m_sock = m_copy.fd_array[i];
		if (m_sock == m_listening) {
			//Accept a new conection
			m_client = accept(m_listening, nullptr, nullptr);
			// Add the new connection to the list of connected clients
			FD_SET(m_client, &m_master);
			// Send a welcome message to the connected client
			std::string welcomeMsg = "Welcome to the Awesome Chat Server!\r\n";
			send(m_client, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);
		}
		else // It's an inbound message
		{
			char buf[4096];
			ZeroMemory(buf, 4096);

			// Receive message
			int bytesIn = recv(m_sock, buf, 4096, 0);
			if (bytesIn <= 0)
			{
				// Drop the client
				closesocket(m_sock);
				FD_CLR(m_sock, &m_master);
			}
			else
			{
				// Check to see if it's a command. \quit kills the server
				if (buf[0] == '\\')
				{
					// Is the command quit? 
					std::string cmd = std::string(buf, bytesIn);
					if (cmd == "\\quit")
					{
						m_running = false;
						break;
					}

					// Unknown command
					continue;
				}

				// Send message to other clients, and definiately NOT the listening socket

				for (int i = 0; i < m_master.fd_count; i++)
				{
					SOCKET outSock = m_master.fd_array[i];
					if (outSock != m_listening && outSock != m_sock)
					{
						std::ostringstream ss;
						ss << "SOCKET #" << m_sock << ": " << buf << "\r\n";
						std::string strOut = ss.str();

						send(outSock, strOut.c_str(), strOut.size() + 1, 0);
					}
				}
			}
		}
	}

}

bool Servidor::getRunning()
{
	return m_running;
}

std::string Servidor::toJson(std::string str_msj)
{
	
	return std::string();
}
