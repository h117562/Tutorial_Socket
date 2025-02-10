#ifndef _EVENT_CLASS_
#define _EVENT_CLASS_

#define TO_INT(value) (static_cast<int>(value)) //enum long Ÿ���� int�� ��ȯ���ֱ� ���� �� ������ �����ϰ� ���̵��� ���� //enum struct�� []���ȣ�� ������ �ٷ� ����

#include <vector>
#include <functional>

enum struct UI_EVENT
{
	DEFAULT,
	TOGGLE_DEBUG_MODE,
	EVENT_COUNT,//�̺�Ʈ ���� //[]�迭 ũ�� �Ҵ��ϱ� ����
};

enum struct SCENE_EVENT
{
	DEFAULT,
	ACTIVE_MAIN_SCENE,
	ACTIVE_LOADING_SCENE,
	ACTIVE_CHAT_SCENE,
	EVENT_COUNT,//�̺�Ʈ ����
};

class EventClass
{
public:
	static EventClass& GetInstance() 
	{
		static EventClass instance;

		return instance;
	}

	//���� �����ڶ� ���� ���� ����
	EventClass(const EventClass&) = delete;
	void operator=(const EventClass&) = delete;

private:
	//�����ڸ� private�� �ξ� ��ü ���� ����
	EventClass()
	{
	}

	~EventClass()
	{
	}

public:
	////////////////////////////UI_EVENT/////////////////////////////
	//�̺�Ʈ �߻� ��, ������ �Լ��� �Լ� ������ �迭�� ����ϴ� �Լ�
	void Subscribe(UI_EVENT event, std::function<void()> func)
	{
		m_uiEvents[TO_INT(event)].push_back(func);
	}

	//�̺�Ʈ �߻� ��,  �ش� �̺�Ʈ�� �Լ����� ����
	void Publish(UI_EVENT event)
	{
		for (UINT i = 0; i < m_uiEvents[TO_INT(event)].size(); i++)
		{
			m_uiEvents[TO_INT(event)][i]();
		}
	}
	////////////////////////////SCENE_EVENT/////////////////////////////
	//�̺�Ʈ �߻� ��, ������ �Լ��� �Լ� ������ �迭�� ����ϴ� �Լ�
	void Subscribe(SCENE_EVENT event, std::function<void()> func)
	{
		m_sceneEvents[TO_INT(event)].push_back(func);
	}

	//�̺�Ʈ �߻� ��,  �ش� �̺�Ʈ�� �Լ����� ����
	void Publish(SCENE_EVENT event)
	{
		for (UINT i = 0; i < m_sceneEvents[TO_INT(event)].size(); i++)
		{
			m_sceneEvents[TO_INT(event)][i]();
		}
	}

	////////////////////////////���� ����/////////////////////////////
	//���� ����
	void ConnectSocket(const wchar_t* ip, unsigned short* port)
	{
		m_connectFunc(ip, port);
	}
	
	//���� ����
	void DisconnectSocket()
	{
		m_disconnectFunc();
	}

	//���� üũ
	void CheckConnection(bool* result)
	{
		m_checkFunc(result);
	}

	void SendMsg(wchar_t* msg)
	{
		m_sendFunc(msg);
	}

	void RecvMsg(wchar_t* msg)
	{
		m_recvFunc(msg);
	}

	//�Լ� ���
	void SubscribeConnect(std::function<void(const wchar_t*, unsigned short*)> func)
	{
		m_connectFunc = func;
	}

	void SubscribeCheck(std::function<void(bool*)> func)
	{
		m_checkFunc = func;
	}

	void SubscribeDisconnect(std::function<void()> func)
	{
		m_disconnectFunc = func;
	}

	void SubscribeSend(std::function<void(wchar_t*)> func)
	{
		m_sendFunc = func;
	}

	void SubscribeRecv(std::function<void(wchar_t*)> func)
	{
		m_recvFunc = func;
	}

private:
	std::vector<std::function<void()>> m_uiEvents[TO_INT(UI_EVENT::EVENT_COUNT)];
	std::vector<std::function<void()>> m_sceneEvents[TO_INT(SCENE_EVENT::EVENT_COUNT)];

	std::function<void(const wchar_t*, unsigned short*)> m_connectFunc;
	std::function<void()> m_disconnectFunc;
	std::function<void(bool*)> m_checkFunc;
	std::function<void(wchar_t*)> m_sendFunc;
	std::function<void(wchar_t*)> m_recvFunc;
};

#endif