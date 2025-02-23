#ifndef _SOCKET_CLASS_H_
#define _SOCKET_CLASS_H_

#ifndef WIN32_LEAN_AND_MEAN//windows.h 에 포함된 winsock.h 제외용
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

//메시지 유형
#define TYPE_JOIN 0x01//채팅방 입장 메시지
#define TYPE_EXIT 0x02//채팅방 퇴장 메시지
#define TYPE_CHAT 0x03//채팅

class SocketClass
{
public:
	SocketClass();
	~SocketClass();

	bool Initialize();
	bool Connect(const wchar_t*, unsigned short*);//서버 연결 함수
	void Disconnect();//메시지 수신 쓰레드 종료
	bool MessageSend(const wchar_t* msg);//메시지 송신 함수
	bool CheckOnline();

private:
	void MessageReceive();//메시지 수신 함수
	
private:
	SOCKET m_sck;
	SOCKADDR_IN m_address;
	std::thread* m_recvThread;
	std::mutex m_sync;
	bool m_online;
};

#endif