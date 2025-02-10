#include "SocketClass.h"

SocketClass::SocketClass()
{
	m_address = {};
	m_recvThread = 0;
	m_online = false;
}

SocketClass::~SocketClass()
{
	Disconnect();
	
	WSACleanup();//WSAStartup ����
}

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

	//EventClass�� �� �Լ��� �����س��� ���
	//winsock2 ����� ���Ե� Ŭ������ �����ϸ� ������ ������ �߹Ƿ� �Լ������ͷ� �ذ��Ͽ���
	//��� Ŭ������ #define ���ִ� �ͺ��� ȿ�����̶� �Ǵ�
	EventClass::GetInstance().SubscribeConnect([&](const wchar_t* ip, unsigned short* port) {Connect(ip, port); });
	EventClass::GetInstance().SubscribeDisconnect([&]() {Disconnect(); });
	EventClass::GetInstance().SubscribeCheck([&](bool* result) { *result = CheckOnline(); });

	return true;
}

bool SocketClass::Connect(const wchar_t* ip, unsigned short* port)
{
	int result;
	char ipstr[16];	
	
	//���̵� ���� ��ȯ
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
	
	std::thread th([&]()
		{
			int errorCode;

			//������ �ּҷ� ����
			//���������� ������� ��� 0�� ��ȯ
			errorCode = connect(m_sck, (SOCKADDR*)&m_address, sizeof(m_address));
			if (errorCode == 0)
			{
				m_online = true;
		
				if (!m_recvThread)
				{
					//�޽��� ������ ������ �����忡�� ó��
					m_recvThread = new std::thread(&SocketClass::MessageReceive, this);
					m_recvThread->detach();
				}

				return;
			}
		});

	th.detach();

	return true;
}

//������ ���� �Լ�
void SocketClass::MessageReceive()
{
	int result;
	char buffer;

	while (m_online)
	{
		//���� üũ
		result = recv(m_sck, &buffer, 1, 0);
		if (result <= 0)
		{
			Disconnect();
			break;
		}
		
		//�޽��� Ÿ�� ����
		switch (buffer)
		{
		case TYPE_CHAT://ä�� ����
		{
			wchar_t chatBuffer[256] = {};//�ִ� �Է� ���� ���� 256
			result = recv(m_sck, (char*)chatBuffer, 256, 0);
			if (result <= 0)
			{
				Disconnect();
				break;
			}

			EventClass::GetInstance().RecvMsg(chatBuffer);
			
			break;
		}
		default:
		{
			break;
		}
		}
	}

}

//������ ���� �Լ�
bool SocketClass::MessageSend(const wchar_t* msg, int length)
{
	int result;

	const char buffer = TYPE_CHAT;

	//�޽��� Ÿ�� ����
	result = send(m_sck, &buffer, 1, 0);
	if (result == SOCKET_ERROR)
	{
		Disconnect();
		return false;
	}

	//ä�� ����
	result = send(m_sck, (char*)msg, length * sizeof(wchar_t), 0);
	if (result == SOCKET_ERROR)
	{
		Disconnect();
		return false;
	}

	return true;
}

void SocketClass::Disconnect()
{
	closesocket(m_sck);
	m_online = false;

	if (m_recvThread)
	{
		delete m_recvThread;
		m_recvThread = 0;
	}
}

bool SocketClass::CheckOnline()
{
	return m_online;
}
