#ifndef _SOCKET_CLASS_H_
#define _SOCKET_CLASS_H_

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <thread>

#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#include "EventClass.h"

//메시지 유형
#define TYPE_JOIN 1
#define TYPE_EXIT 2
#define TYPE_CHAT 3

class SocketClass
{
public:
	static SocketClass& GetInstance()
	{
		static SocketClass instance;

		return instance;
	}

	//복사 생성자랑 대입 연산 제거
	SocketClass(const SocketClass&) = delete;
	void operator=(const SocketClass&) = delete;

private:
	SocketClass();
	~SocketClass();

public:
	HRESULT Initialize();
	HRESULT Connect(const wchar_t*, unsigned short);//서버 연결 함수
	void Disconnect();//메시지 수신 쓰레드 종료
	HRESULT MessageSend(const wchar_t* msg, int length);//메시지 송신 함수
	bool CheckOnline();

private:
	void MessageReceive();//메시지 수신 함수
	
private:
	SOCKET m_sck;
	std::thread* m_recvThread;
	bool m_online;
};

#endif