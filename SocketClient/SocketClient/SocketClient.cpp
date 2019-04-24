// SocketClient.cpp : 定义控制台应用程序的入口点。
//

#include <stdio.h>
#include <tchar.h>
#include "wrapper.h"

int _tmain(int argc, _TCHAR* argv[])
{
	wrapper::getInstance()->createSocket();
	char buff[1024] = { 0 };
	while (true)
	{
		if (wrapper::getInstance()->getServerRespond())
		{
			printf("please input msg:\n");
			gets(buff);
			wrapper::getInstance()->sendMsg(buff, strlen(buff));
		}
	}
	getchar();
	return 0;
}

