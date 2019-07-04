#pragma once
#include<iostream>
//#include<winsock.h>
#include <WS2tcpip.h>
#include <string>
#include <sstream>
#include <vector>
#include "json.hpp"
using  json = nlohmann::json;
#pragma comment (lib, "ws2_32.lib")

class Servidor
{
	WSADATA m_wsData;
	WORD m_ver;
	SOCKET m_listening,m_sock,m_client;
	sockaddr_in m_hint;
	fd_set m_master,m_copy;
	bool m_running;
	int m_socketCount,toWho=0;
	std::vector<std::string> clientes;

	bool initialize();
	void Bind();

public:
	Servidor();
	~Servidor();
	bool run();
	void sendRecv();
	bool getRunning();
	std::string toJson(std::string str_msj);

};

