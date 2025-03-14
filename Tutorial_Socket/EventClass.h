#ifndef _EVENT_CLASS_
#define _EVENT_CLASS_

#define TO_INT(value) (static_cast<int>(value)) //enum long 타입을 int로 변환해주기 위해 긴 문장을 간결하게 보이도록 만듦 //enum struct라 []대괄호에 생으로 바로 못씀

#include <vector>
#include <functional>

enum struct UI_EVENT
{
	DEFAULT,
	TOGGLE_DEBUG_MODE,
	EVENT_COUNT,//이벤트 개수 //[]배열 크기 할당하기 위함
};

enum struct SCENE_EVENT
{
	DEFAULT,
	ACTIVE_MAIN_SCENE,
	ACTIVE_LOADING_SCENE,
	ACTIVE_CHAT_SCENE,
	EVENT_COUNT,//이벤트 개수
};

class EventClass
{
public:
	static EventClass& GetInstance() 
	{
		static EventClass instance;

		return instance;
	}

	//복사 생성자랑 대입 연산 제거
	EventClass(const EventClass&) = delete;
	void operator=(const EventClass&) = delete;

private:
	//생성자를 private에 두어 객체 생성 방지
	EventClass()
	{
	}

	~EventClass()
	{
	}

public:
	////////////////////////////UI_EVENT/////////////////////////////
	//이벤트 발생 시, 실행할 함수를 함수 포인터 배열에 등록하는 함수
	void Subscribe(UI_EVENT event, std::function<void()> func)
	{
		m_uiEvents[TO_INT(event)].push_back(func);
	}

	//이벤트 발생 시,  해당 이벤트의 함수들을 실행
	void Publish(UI_EVENT event)
	{
		for (UINT i = 0; i < m_uiEvents[TO_INT(event)].size(); i++)
		{
			m_uiEvents[TO_INT(event)][i]();
		}
	}
	////////////////////////////SCENE_EVENT/////////////////////////////
	//이벤트 발생 시, 실행할 함수를 함수 포인터 배열에 등록하는 함수
	void Subscribe(SCENE_EVENT event, std::function<void()> func)
	{
		m_sceneEvents[TO_INT(event)].push_back(func);
	}

	//이벤트 발생 시,  해당 이벤트의 함수들을 실행
	void Publish(SCENE_EVENT event)
	{
		for (UINT i = 0; i < m_sceneEvents[TO_INT(event)].size(); i++)
		{
			m_sceneEvents[TO_INT(event)][i]();
		}
	}

	////////////////////////////소켓 전용/////////////////////////////
	//소켓 연결
	void ConnectSocket(const wchar_t* ip, unsigned short* port)
	{
		m_connectFunc(ip, port);
	}
	
	//소켓 해제
	void DisconnectSocket()
	{
		m_disconnectFunc();
	}

	//연결 체크
	void CheckConnection(bool* result)
	{
		m_checkFunc(result);
	}

	void SendMsg(const wchar_t* msg)
	{
		m_sendFunc(msg);
	}

	void RecvMsg(wchar_t* msg)
	{
		m_recvFunc(msg);
	}

	//함수 등록
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

	void SubscribeSend(std::function<void(const wchar_t*)> func)
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
	std::function<void(const wchar_t*)> m_sendFunc;
	std::function<void(wchar_t*)> m_recvFunc;
};

#endif