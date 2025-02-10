#ifndef _SOCKET_CLASS_H_
#define _SOCKET_CLASS_H_

#ifndef WIN32_LEAN_AND_MEAN//windows.h �� ���Ե� winsock.h ���ܿ�
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <thread>

#pragma comment(lib, "Ws2_32.lib")

#include "EventClass.h"

//�޽��� ����
#define TYPE_JOIN 1
#define TYPE_EXIT 2
#define TYPE_CHAT 3

class SocketClass
{
public:
	SocketClass();
	~SocketClass();

	bool Initialize();
	bool Connect(const wchar_t*, unsigned short*);//���� ���� �Լ�
	void Disconnect();//�޽��� ���� ������ ����
	bool MessageSend(const wchar_t* msg, int length);//�޽��� �۽� �Լ�
	bool CheckOnline();

private:
	void MessageReceive();//�޽��� ���� �Լ�
	
private:
	SOCKET m_sck;
	SOCKADDR_IN m_address;
	std::thread* m_recvThread;
	bool m_online;
};

#endif