#include "UIManager.h"

UIManager::UIManager()
{
}

UIManager::~UIManager()
{
}

UIManager::UIManager(const UIManager& other)
{
}

bool UIManager::Initialize(D3DClass* pD3Dclass, TextClass* pTextClass)
{
	bool result;

	//각종 디버깅 정보들을 화면에 렌더링하는 UI 클래스
	result = m_debugUI.Initialize(pD3Dclass);
	if (!result)
	{
		return false;
	}

	EventClass::GetInstance().Subscribe(UI_EVENT::TOGGLE_DEBUG_MODE, [&]() {m_debugUI.ToggleActive(); });

	result = m_mainScene.Initialize(pD3Dclass->GetDevice(), pTextClass);
	if (!result)
	{
		return false;
	}

	result = m_loadingScene.Initialize(pD3Dclass->GetDevice(), pTextClass);
	if (!result)
	{
		return false;
	}

	result = m_chatScene.Initialize(pD3Dclass->GetDevice(), pTextClass);
	if (!result)
	{
		return false;
	}

	EventClass::GetInstance().Subscribe(UI_EVENT::ACTIVE_LOADING_SCENE, [&]() {
		m_mainScene.SetActive(false); 
		m_chatScene.SetActive(false);
		m_loadingScene.SetActive(true);
		});

	EventClass::GetInstance().Subscribe(UI_EVENT::ACTIVE_MAIN_SCENE, [&]() {
		m_mainScene.SetActive(true);
		m_chatScene.SetActive(false);
		m_loadingScene.SetActive(false);
		});

	EventClass::GetInstance().Subscribe(UI_EVENT::ACTIVE_CHAT_SCENE, [&]() {
		m_chatScene.SetActive(true);
		m_mainScene.SetActive(false);
		m_loadingScene.SetActive(false);
		});

	return true;
}


bool UIManager::Frame(D3DClass* pD3DClass, HWND hwnd, ShaderManager* pShaderManager, TextClass* pTextClass, CameraClass* pCameraClass, FrameTimer* pFrameTimer)
{
	bool result, state;

	pD3DClass->TurnZBufferOff();

	if (m_debugUI.GetActive())
	{
		m_debugUI.Frame(pTextClass, pCameraClass);
	}

	if (m_mainScene.GetActive())
	{
		m_mainScene.Frame(pD3DClass, hwnd, pShaderManager, pTextClass, pCameraClass);
	}

	if (m_loadingScene.GetActive())
	{
		m_loadingScene.Frame(pD3DClass, hwnd, pShaderManager, pTextClass, pCameraClass, pFrameTimer);
	}

	if (m_chatScene.GetActive())
	{
		m_chatScene.Frame(pD3DClass, hwnd, pShaderManager, pTextClass, pCameraClass);
	}

	pD3DClass->TurnZBufferOn();

	return true;
}
