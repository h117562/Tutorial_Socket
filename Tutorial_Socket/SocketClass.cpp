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
	WSACleanup();//WSAStartup ���Ŀ��� ȣ��
}

//�ʱ�ȭ
bool SocketClass::Initialize()
{
	int result;
	WSADATA wsaData;

	//Winsock DLL ����� ����
	//���������� ������� ��� 0�� ��ȯ
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		return false;
	}

	//EventClass�� �Լ��� �����س��� ���
	//winsock2 ����� ���Ե� Ŭ������ �����ϸ� ������ ������ �߹Ƿ� �Լ������ͷ� �ذ��Ͽ���
	//��� Ŭ������ #define ���ִ� �ͺ��� ȿ�����̶� �Ǵ�
	EventClass::GetInstance().SubscribeConnect([&](const wchar_t* ip, unsigned short* port) {Connect(ip, port); });
	EventClass::GetInstance().SubscribeDisconnect([&]() {Disconnect();});
	EventClass::GetInstance().SubscribeCheck([&](bool* result) { *result = CheckOnline(); });
	EventClass::GetInstance().SubscribeSend([&](const wchar_t* msg) {MessageSend(msg); });

	return true;
}

//���� �õ� �Լ�
bool SocketClass::Connect(const wchar_t* ip, unsigned short* port)
{
	int result;
	char ipstr[16];	
	
	//���̵� ���ڷ� ��ȯ
	WideCharToMultiByte(CP_ACP, 0, ip, -1, ipstr, 16, nullptr, nullptr);
		
	//���� ����(IPv4, �ŷ��� �� �ִ� ����� ���� ��� ����Ʈ ��Ʈ��, TCP)
	m_sck = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_sck == INVALID_SOCKET)
	{
		return false;
	}

	//IPv4 �ּ� �� ��Ʈ�� �����ϴ� ����ü
	m_address.sin_family = AF_INET;//�׻� AF_INET ���� ������ ��, IPv4�� �ǹ�
	m_address.sin_port = htons(*port);//(host to net short) little Endian ���� big���� ��ȯ�ϴ� �Լ�

	//�ؽ�Ʈ ������ IPv4, IPv6 �ּҸ� ���������� ��ȯ
	//���������� ������� ��� 1�� ��ȯ
	result = inet_pton(AF_INET, ipstr, &m_address.sin_addr);
	if (result != 1)
	{
		return false;
	}
	
	//connect �Լ��� blocking �ǹǷ� ���� ������� ȣ��
	std::thread connectionTh([&]()
		{
			int errorCode;

			//������ �ּҷ� ����
			//���������� ������� ��� 0�� ��ȯ
			errorCode = connect(m_sck, (SOCKADDR*)&m_address, sizeof(m_address));
			if (errorCode == 0)
			{
				//�޽��� ������ ������ �����忡�� ó��
				m_recvThread = new std::thread(&SocketClass::MessageReceive, this);
			}
		});

	//connect �Ǳ����� �Լ��� ���� �� �����Ƿ� detach�� �и�
	connectionTh.detach();

	return true;
}

//������ ���� �Լ�
void SocketClass::MessageReceive()
{
	int result;
	char msgType;

	m_online = true;

	while (m_online)
	{
		//�޽��� Ÿ�� ����
		result = recv(m_sck, &msgType, sizeof(char), 0);
		if (result <= 0)
		{
			m_online = false;
			Disconnect();
			return;
		}
		
		//�޽��� Ÿ�� ����
		switch (msgType)
		{
		case TYPE_CHAT:
		{
			wchar_t chatBuffer[BUFFER_SIZE] = {};//�ִ� �Է� ���� ���� 256

			//ä�� ����
			result = recv(m_sck, (char*)chatBuffer, BUFFER_SIZE * sizeof(wchar_t), 0);
			if (result <= 0)
			{
				m_online = false;
				Disconnect();
				return;
			}

			//ä�� �������� ChatScene�� ä�� �̷��� ������Ʈ
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

//������ ���� �Լ�
bool SocketClass::MessageSend(const wchar_t* msg)
{
	int result;

	const char msgType = TYPE_CHAT;

	//�޽��� Ÿ�� ����
	result = send(m_sck, &msgType, sizeof(char), 0);
	if (result == SOCKET_ERROR)
	{
		return false;
	}

	//ä�� ����
	result = send(m_sck, (char*)msg, BUFFER_SIZE * sizeof(wchar_t), 0);
	if (result == SOCKET_ERROR)
	{
		return false;
	}

	return true;
}

//���� ���� �Լ�
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

//���� Ȯ�� �Լ�
bool SocketClass::CheckOnline()
{
	return m_online;
}
