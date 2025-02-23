#include "SocketClass.h"

SocketClass::SocketClass()
{
	m_sck = {};
	m_address = {};
	m_recvThread = 0;
	m_online = false;
}

SocketClass::~SocketClass()
{
	Disconnect();
	WSACleanup();//WSAStartup 이후에만 호출
}

//초기화
bool SocketClass::Initialize()
{
	int result;
	WSADATA wsaData;

	//Winsock DLL 사용을 시작
	//정상적으로 실행됐을 경우 0을 반환
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		return false;
	}

	//EventClass에 함수를 저장해놓고 사용
	//winsock2 헤더가 포함된 클래스를 포함하면 재정의 오류가 뜨므로 함수포인터로 해결하였음
	//모든 클래스에 #define 해주는 것보단 효율적이라 판단
	EventClass::GetInstance().SubscribeConnect([&](const wchar_t* ip, unsigned short* port) {Connect(ip, port); });
	EventClass::GetInstance().SubscribeDisconnect([&]() {Disconnect();});
	EventClass::GetInstance().SubscribeCheck([&](bool* result) { *result = CheckOnline(); });
	EventClass::GetInstance().SubscribeSend([&](const wchar_t* msg) {MessageSend(msg); });

	return true;
}

//연결 시도 함수
bool SocketClass::Connect(const wchar_t* ip, unsigned short* port)
{
	int result;
	char ipstr[16];	
	
	//와이드 문자로 변환
	WideCharToMultiByte(CP_ACP, 0, ip, -1, ipstr, 16, nullptr, nullptr);
		
	//소켓 생성(IPv4, 신뢰할 수 있는 양방향 연결 기반 바이트 스트림, TCP)
	m_sck = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_sck == INVALID_SOCKET)
	{
		return false;
	}

	//IPv4 주소 및 포트를 설정하는 구조체
	m_address.sin_family = AF_INET;//항상 AF_INET 값을 가져야 함, IPv4를 의미
	m_address.sin_port = htons(*port);//(host to net short) little Endian 에서 big으로 변환하는 함수

	//텍스트 형식의 IPv4, IPv6 주소를 이진형으로 변환
	//정상적으로 실행됐을 경우 1을 반환
	result = inet_pton(AF_INET, ipstr, &m_address.sin_addr);
	if (result != 1)
	{
		return false;
	}
	
	//connect 함수는 blocking 되므로 별도 스레드로 호출
	std::thread connectionTh([&]()
		{
			int errorCode;

			//설정한 주소로 연결
			//정상적으로 실행됐을 경우 0을 반환
			errorCode = connect(m_sck, (SOCKADDR*)&m_address, sizeof(m_address));
			if (errorCode == 0)
			{
				//메시지 수신은 별도의 스레드에서 처리
				m_recvThread = new std::thread(&SocketClass::MessageReceive, this);
			}
		});

	//connect 되기전에 함수가 끝날 수 있으므로 detach로 분리
	connectionTh.detach();

	return true;
}

//데이터 수신 함수
void SocketClass::MessageReceive()
{
	int result;
	char msgType;

	m_online = true;

	while (m_online)
	{
		//메시지 타입 수신
		result = recv(m_sck, &msgType, sizeof(char), 0);
		if (result <= 0)
		{
			m_online = false;
			Disconnect();
			return;
		}
		
		//메시지 타입 구분
		switch (msgType)
		{
		case TYPE_CHAT:
		{
			wchar_t chatBuffer[BUFFER_SIZE] = {};//최대 입력 가능 글자 256

			//채팅 수신
			result = recv(m_sck, (char*)chatBuffer, BUFFER_SIZE * sizeof(wchar_t), 0);
			if (result <= 0)
			{
				m_online = false;
				Disconnect();
				return;
			}

			//채팅 수신으로 ChatScene의 채팅 이력을 업데이트
			EventClass::GetInstance().RecvMsg(chatBuffer);
			
			break;
		}
		default:
		{
			break;
		}
		}
	}

	return;
}

//데이터 전송 함수
bool SocketClass::MessageSend(const wchar_t* msg)
{
	int result;

	const char msgType = TYPE_CHAT;

	//메시지 타입 전송
	result = send(m_sck, &msgType, sizeof(char), 0);
	if (result == SOCKET_ERROR)
	{
		return false;
	}

	//채팅 전송
	result = send(m_sck, (char*)msg, BUFFER_SIZE * sizeof(wchar_t), 0);
	if (result == SOCKET_ERROR)
	{
		return false;
	}

	return true;
}

//연결 종료 함수
void SocketClass::Disconnect()
{
	closesocket(m_sck);

	if(m_online)
	{ 
		if (m_recvThread != nullptr && m_recvThread->joinable())
		{
			m_recvThread->join();
		}

		delete m_recvThread;
		m_recvThread = 0;
	}
}

//연결 확인 함수
bool SocketClass::CheckOnline()
{
	return m_online;
}
