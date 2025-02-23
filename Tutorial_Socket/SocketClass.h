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
#include <mutex>

#pragma comment(lib, "Ws2_32.lib")

#include "EventClass.h"

#define BUFFER_SIZE 256

//�޽��� ����
#define TYPE_JOIN 0x01//ä�ù� ���� �޽���
#define TYPE_EXIT 0x02//ä�ù� ���� �޽���
#define TYPE_CHAT 0x03//ä��

class SocketClass
{
public:
	SocketClass();
	~SocketClass();

	bool Initialize();
	bool Connect(const wchar_t*, unsigned short*);//���� ���� �Լ�
	void Disconnect();//�޽��� ���� ������ ����
	bool MessageSend(const wchar_t* msg);//�޽��� �۽� �Լ�
	bool CheckOnline();

private:
	void MessageReceive();//�޽��� ���� �Լ�
	
private:
	SOCKET m_sck;
	SOCKADDR_IN m_address;
	std::thread* m_recvThread;
	std::mutex m_sync;
	bool m_online;
};

#endif