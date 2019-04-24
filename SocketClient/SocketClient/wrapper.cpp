#include "wrapper.h"

#pragma comment(lib, "WS2_32") // 链接到WS2_32.lib

static wrapper *instance=nullptr;
wrapper *wrapper::getInstance()
{
	if (!instance)
	{
		instance = new wrapper;
	}
	return instance;
}

wrapper::wrapper()
:m_sockId(INVALID_SOCKET)
, m_serverRespond(false)
{
	this->init();
}

wrapper::~wrapper()
{

}

void wrapper::init()
{
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 2);
	if (::WSAStartup(sockVersion, &wsaData) != 0)
	{
		printf("init socket failed \n");
	}
}

void wrapper::createSocket()
{
	if (m_sockId != INVALID_SOCKET){
		return;
	}
	m_sockId = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_sockId == INVALID_SOCKET)
	{
		printf("failed socket() \n");
		return;
	}
	// 也可以在这里调用bind函数绑定一个本地地址
	// 否则系统将会自动安排
	// 填写远程地址信息
	sockaddr_in servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(4567);
	// 注意，这里要填写服务器程序（TCPServer程序）所在机器的IP地址
	// 如果你的计算机没有联网，直接使用127.0.0.1即可
	servAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (::connect(m_sockId, (sockaddr*)&servAddr, sizeof(servAddr)) == -1)
	{
		printf("failed connect() \n");
		return;
	}

	m_thread = std::thread(std::bind(&wrapper::listenCheck,this));
	m_thread.detach();
}
	
void wrapper::listenCheck()
{
	char buff[256];
	while (TRUE)
	{
		//从服务器端接收数据
		int nRecv = ::recv(m_sockId, buff, 256, 0);
		if (nRecv > 0)
		{
			buff[nRecv] = '\0';
			printf("%s\n", buff);
			m_serverRespond = true;
		}
		else{
			break;
		}
	}
	// 关闭套节字
	::closesocket(m_sockId);
	m_sockId = INVALID_SOCKET;
}

bool wrapper::sendMsg(const char *msg, int len)
{
	if (m_sockId != INVALID_SOCKET)
	{
		// send msg to server
		m_serverRespond = false;
		int size= ::send(m_sockId, msg, len, 0);
		return size;
	}
	else{
		this->createSocket();
		printf("reconnect server... \n");
	}
	return false;
}

bool wrapper::getServerRespond()
{
	return m_serverRespond;
}