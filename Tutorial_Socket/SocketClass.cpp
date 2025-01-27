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
	WSACleanup();//WSAStartup ����
}

HRESULT SocketClass::Initialize()
{
	int result;
	WSADATA wsaData;

	//Winsock DLL ����� ����
	//���������� ������� ��� 0�� ��ȯ
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		return S_FALSE;
	}

	//���� ����(IPv4, �ŷ��� �� �ִ� ����� ���� ��� ����Ʈ ��Ʈ��, TCP)
	m_sck = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_sck == INVALID_SOCKET)
	{
		return S_FALSE;
	}

	if (!m_recvThread)
	{
		//�޽��� ������ ������ �����忡�� ó��
		m_recvThread = new std::thread(MessageReceive);
	}

	return S_OK;
}

HRESULT SocketClass::Connect(const wchar_t* ip, unsigned short port)
{
	int result;
	char ipstr[16];
	
	//���̵� ���� ��ȯ
	WideCharToMultiByte(CP_ACP, 0, ip, -1, ipstr, 16, nullptr, nullptr);
		
	//IPv4 �ּ� �� ��Ʈ�� �����ϴ� ����ü
	SOCKADDR_IN address = {};

	address.sin_family = AF_INET;//�׻� AF_INET ���� ������ ��, IPv4�� �ǹ�
	address.sin_port = htons(port);//(host to net short) little Endian ���� big���� ��ȯ�ϴ� �Լ�

	//�ؽ�Ʈ ������ IPv4, IPv6 �ּҸ� ���������� ��ȯ
	//���������� ������� ��� 1�� ��ȯ
	result = inet_pton(AF_INET, ipstr, &address.sin_addr);
	if (result != 1)
	{
		return S_FALSE;
	}
	
	std::thread th([&]() 
		{
			//������ �ּҷ� ����
			//���������� ������� ��� 0�� ��ȯ
			result = connect(m_sck, (SOCKADDR*)&address, sizeof(address));
			if (result == 0)
			{
				m_online = true;
				m_recvThread->join();
			}
		});

	th.join();

	return S_OK;
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
			wchar_t chatBuffer[256];//�ִ� �Է� ���� ���ڸ� 256�� �صξ���
			result = recv(m_sck, (char*)chatBuffer, 256, 0);
			if (result <= 0)
			{
				Disconnect();
				break;
			}

			EventClass::GetInstance().Publish(SOCKET_EVENT::NEW_CHAT, chatBuffer);

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
HRESULT SocketClass::MessageSend(const wchar_t* msg, int length)
{
	int result;

	const char buffer = TYPE_CHAT;

	//�޽��� Ÿ�� ����
	result = send(m_sck, &buffer, 1, 0);
	if (result == SOCKET_ERROR)
	{
		return S_FALSE;
	}

	//ä�� ����
	result = send(m_sck, (char*)msg, length * sizeof(wchar_t), 0);
	if (result == SOCKET_ERROR)
	{
		return S_FALSE;
	}

	return S_OK;
}

void SocketClass::Disconnect()
{
	m_online = false;
}

bool SocketClass::CheckOnline()
{
	return m_online;
}
