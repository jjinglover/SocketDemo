
#include <iostream>
#include <strstream>
#include <set>
#include <string>
#include <thread>
#include "winsock.h"

#pragma comment(lib, "WS2_32") // 链接到WS2_32.lib
using namespace std;

std::set<SOCKET> m_socketSet;
std::string intToString(int v){
	std::strstream ss;
	ss << v;
	std::string str;
	ss >> str;
	return str;
}

int main()
{
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 2);
	if (::WSAStartup(sockVersion, &wsaData) != 0)
	{
		exit(0);
	}

	SOCKET s_handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s_handle == INVALID_SOCKET)
	{
		printf("failed socket()");
		return 0;
	}
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(4567); //1024 ~ 49151：普通用户注册的端口号
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	if (::bind(s_handle, (sockaddr *)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		printf("failed bind()");
		return 0;
	}
	if (::listen(s_handle, 10) == SOCKET_ERROR)
	{
		printf("failed listen()");
		return 0;
	}

	sockaddr_in remoteAddr;
	int nAddrLen = sizeof(remoteAddr);
	char szText[] = "hello client...\n";
	void handleClient(SOCKET sClient);
	while (true)
	{
		SOCKET sClient = ::accept(s_handle, (SOCKADDR*)&remoteAddr, &nAddrLen);
		if (sClient == INVALID_SOCKET)
		{
			printf("Failed accept()");
		}
		else
		{
			m_socketSet.insert(sClient);

			printf("connect sucess：%s:%s \n", inet_ntoa(remoteAddr.sin_addr),intToString(sClient).c_str());
			::send(sClient, szText, strlen(szText), 0);

			auto pt = std::thread(std::bind(handleClient, sClient));
			pt.detach();
		}
	}

	closesocket(s_handle);
	getchar();
	return 0;
}

void handleClient(SOCKET sClient)
{
	std::string msg = "hello " + intToString(sClient);
	while (true)
	{
		char buff[256] = { 0 };
		int nRecv = ::recv(sClient, buff, 256, 0);
		if (nRecv > 0)
		{
			buff[nRecv] = '\0';
			printf("recv data：%s\n", buff);
			int code = strcmp(buff, "hello everyone ...\0");
			if (code == 0)
			{
				std::string notice = "hi everybody i am " + intToString(sClient);
				for (auto handle:m_socketSet)
				{
					::send(handle, notice.c_str(),notice.size(), 0);
				}
			}
			else{
				::send(sClient, msg.c_str(), msg.size(), 0);
			}
		}
		else{
			break;
		}
	}
	m_socketSet.erase(sClient);
	closesocket(sClient);
}
