#include "SocketClass.h"

SocketClass::SocketClass()
{
}

SocketClass::~SocketClass()
{
	Disconnect();

	if (m_recvThread)
	{
		m_recvThread->detach();
		delete m_recvThread;
		m_recvThread = 0;
	}

	shutdown(m_sck, SD_BOTH);
	closesocket(m_sck);
	WSACleanup();//WSAStartup 이후
}

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

	//소켓 생성(IPv4, 신뢰할 수 있는 양방향 연결 기반 바이트 스트림, TCP)
	m_sck = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_sck == INVALID_SOCKET)
	{
		return false;
	}

	if (!m_recvThread)
	{
		//메시지 수신은 별도의 스레드에서 처리
		m_recvThread = new std::thread(&SocketClass::MessageReceive, this);
	}

	//EventClass에 두 함수를 저장해놓고 사용
	//winsock2 헤더가 포함된 클래스를 포함하면 재정의 오류가 뜨므로 함수포인터로 해결하였음
	//모든 클래스에 #define 해주는 것보단 효율적이라 판단
	EventClass::GetInstance().SubscribeConnect([&](const wchar_t* ip, unsigned short port) {Connect(ip, port); });
	EventClass::GetInstance().SubscribeCheck([&](bool result) { result = CheckOnline(); });

	return true;
}

bool SocketClass::Connect(const wchar_t* ip, unsigned short port)
{
	int result;
	char ipstr[16];
	
	//와이드 문자 변환
	WideCharToMultiByte(CP_ACP, 0, ip, -1, ipstr, 16, nullptr, nullptr);
		
	//IPv4 주소 및 포트를 설정하는 구조체
	SOCKADDR_IN address = {};

	address.sin_family = AF_INET;//항상 AF_INET 값을 가져야 함, IPv4를 의미
	address.sin_port = htons(port);//(host to net short) little Endian 에서 big으로 변환하는 함수

	//텍스트 형식의 IPv4, IPv6 주소를 이진형으로 변환
	//정상적으로 실행됐을 경우 1을 반환
	result = inet_pton(AF_INET, ipstr, &address.sin_addr);
	if (result != 1)
	{
		return false;
	}
	
	std::thread th([&]() 
		{
			//설정한 주소로 연결
			//정상적으로 실행됐을 경우 0을 반환
			result = connect(m_sck, (SOCKADDR*)&address, sizeof(address));
			if (result == 0)
			{
				m_online = true;
				m_recvThread->join();
			}
		});

	th.join();

	return true;
}

//데이터 수신 함수
void SocketClass::MessageReceive()
{
	int result;
	char buffer;

	while (m_online)
	{
		//오류 체크
		result = recv(m_sck, &buffer, 1, 0);
		if (result <= 0)
		{
			Disconnect();
			break;
		}
		
		//메시지 타입 구분
		switch (buffer)
		{
		case TYPE_CHAT://채팅 수신
		{
			wchar_t chatBuffer[256];//최대 입력 가능 글자를 256로 해두었음
			result = recv(m_sck, (char*)chatBuffer, 256, 0);
			if (result <= 0)
			{
				Disconnect();
				break;
			}

			EventClass::GetInstance().Publish(CHAT_EVENT::NEW_CHAT, chatBuffer);

			break;
		}
		default:
		{
			break;
		}
		}
	}

}

//데이터 전송 함수
bool SocketClass::MessageSend(const wchar_t* msg, int length)
{
	int result;

	const char buffer = TYPE_CHAT;

	//메시지 타입 전송
	result = send(m_sck, &buffer, 1, 0);
	if (result == SOCKET_ERROR)
	{
		return false;
	}

	//채팅 전송
	result = send(m_sck, (char*)msg, length * sizeof(wchar_t), 0);
	if (result == SOCKET_ERROR)
	{
		return false;
	}

	return true;
}

void SocketClass::Disconnect()
{
	m_online = false;
}

bool SocketClass::CheckOnline()
{
	return m_online;
}
