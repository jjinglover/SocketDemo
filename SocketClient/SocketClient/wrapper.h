#ifndef __wrapper__
#define __wrapper__

#include <thread>
#include "winsock.h"

class wrapper
{
public:
	wrapper();
	~wrapper();

	static wrapper *getInstance();
	void createSocket();
	bool sendMsg(const char *msg,int len);
	bool getServerRespond();
protected:
	void init();
	void listenCheck();
private: 
	SOCKET          m_sockId;
	std::thread     m_thread;
	bool            m_serverRespond;
};

#endif // __wrapper_SCENE_H__
