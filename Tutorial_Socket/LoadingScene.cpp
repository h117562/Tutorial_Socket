#include "LoadingScene.h"

LoadingScene::LoadingScene()
{
	m_backGround = 0;
	m_label = 0;
	m_count = 0;
	m_active = false;
}

LoadingScene::~LoadingScene()
{
	if (m_label)
	{
		delete m_label;
		m_label = nullptr;
	}

	if (m_backGround)
	{
		delete m_backGround;
		m_backGround = nullptr;
	}

}

bool LoadingScene::Initialize(ID3D11Device* pDevice, TextClass* pTextClass)
{
	HRESULT result;

	m_backGround = new Plane;
	if (!m_backGround)
	{
		return false;
	}

	result = m_backGround->Initialize(pDevice, L"..//data//assets//panel-1.png", XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(800.0f, 800.0f, 1.0f), 0);
	if (FAILED(result))
	{
		return false;
	}

	m_label = new TextLabel;
	if (!m_label)
	{
		return false;
	}

	m_label->SetText(L"접속중...");
	m_label->Initialize(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(100.0f, 100.0f, 1.0f), 0);

	result = m_label->SetTextFormat(
		pTextClass,
		L"굴림",
		DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_BOLD,
		DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_NORMAL,
		20.0f);
	if (FAILED(result))
	{
		return false;
	}

	m_label->SetHorizontalAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	m_label->SetVerticalAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	result = m_label->SetTextBrush(pTextClass, 0.13f, 0.7f, 0.3f, 1.0f);
	if (FAILED(result))
	{
		return false;
	}

	EventClass::GetInstance().Subscribe(SCENE_EVENT::ACTIVE_LOADING_SCENE, [&]() {m_count = 0; });

	return true;
}

void LoadingScene::Frame(D3DClass* pD3DClass, HWND hwnd, ShaderManager* pShaderManager, TextClass* pTextClass, CameraClass* pCameraClass, FrameTimer* pFrameTimer)
{
	bool state = false;

	XMMATRIX view, proj;

	pCameraClass->GetBaseViewMatrix(view);
	pD3DClass->GetOrthoMatrix(proj);

	Render(pD3DClass, pTextClass, pShaderManager, view, proj);

	//텍스트 업데이트용
	m_count += pFrameTimer->GetTime();

	std::wstring tempText(L"접속중...");

	int repeat = (int)m_count % 3;

	for (int i = 0; i < repeat; i++)
	{
		tempText.pop_back();
	}
	
	m_label->SetText(tempText.c_str());

	EventClass::GetInstance().CheckConnection(&state);
	if (state)
	{
		m_count = 0.0f;

		//연결 성공하면 다음 씬으로 넘어감
		EventClass::GetInstance().Publish(SCENE_EVENT::ACTIVE_CHAT_SCENE);
	}

	//4초 넘게 연결중이면 다시 처음으로 돌아감
	if (m_count > 4.0f)
	{
		EventClass::GetInstance().Publish(SCENE_EVENT::ACTIVE_MAIN_SCENE);
		MessageBox(hwnd, L"연결 실패", L"Failed", MB_OK);
	}

	return;
}

bool LoadingScene::Render(D3DClass* pD3DClass, TextClass* pTextClass, ShaderManager* pShaderManager, const XMMATRIX& view, const XMMATRIX& proj)
{
	bool result;

	result = m_backGround->Render(pD3DClass->GetDeviceContext(), pShaderManager->GetUIShader(), m_backGround->GetWorldMatrix(), view, proj);
	if (!result)
	{
		return false;
	}

	m_label->Render(pTextClass);

	return true;
}

void LoadingScene::ToggleActive()
{
	m_active ^= true;
}

void LoadingScene::SetActive(bool state)
{
	m_active = state;
}

bool LoadingScene::GetActive()
{
	return m_active;
}