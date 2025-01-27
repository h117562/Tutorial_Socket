#include "ChatScene.h"

ChatScene::ChatScene()
{
	m_exitBtn = 0;
	m_msgSend = 0;
	m_chatBox = 0;
	m_active = false;
}

ChatScene::~ChatScene()
{
	if (m_exitBtn)
	{
		delete m_exitBtn;
		m_exitBtn = nullptr;
	}

	if (m_msgSend)
	{
		delete m_msgSend;
		m_msgSend = nullptr;
	}

	if (m_chatBox)
	{
		delete m_chatBox;
		m_chatBox = nullptr;
	}
}

bool ChatScene::Initialize(ID3D11Device* pDevice, TextClass* pTextClass)
{
	HRESULT result;

	m_exitBtn = new Button;
	if (!m_exitBtn)
	{
		return false;
	}

	result = m_exitBtn->Initialize(pDevice, L"..//data//assets//home1.png", L"..//data//assets//home2.png", L"..//data//assets//home3.png", XMFLOAT3(-20.0f, -20.0f, 1.0f), XMFLOAT3(70.0f, 70.0f, 1.0f), ALIGNMENT_RIGHT_TOP);
	if (FAILED(result))
	{
		return false;
	}

	m_msgSend = new TextInput;
	if (!m_msgSend)
	{
		return false;
	}

	result = m_msgSend->Initialize(pDevice, L"..//data//assets//send1.png", L"..//data//assets//send2.png", XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(800, 200.0f, 1.0f), ALIGNMENT_BOTTOM);
	if (FAILED(result))
	{
		return false;
	}

	m_msgSend->SetTextMargin(10.0f, 10.0f);

	result = m_msgSend->SetTextFormat(
		pTextClass,
		L"바탕",
		DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_BOLD,
		DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_NORMAL,
		20.0f);
	if (FAILED(result))
	{
		return false;
	}

	m_msgSend->SetHorizontalAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	m_msgSend->SetVerticalAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

	result = m_msgSend->SetTextBrush(pTextClass, 1.0f, 0.9f, 0.9f, 1.0f);
	if (FAILED(result))
	{
		return false;
	}


	m_chatBox = new ChatBox;
	if (!m_chatBox)
	{
		return false;
	}

	result = m_chatBox->Initialize(pDevice, L"..//data//assets//logbg.png", XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(800, 600.0f, 1.0f), ALIGNMENT_TOP);
	if (FAILED(result))
	{
		return false;
	}

	m_chatBox->SetTextMargin(10.0f, 10.0f);

	result = m_chatBox->SetTextFormat(
		pTextClass,
		L"바탕",
		DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_BOLD,
		DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_NORMAL,
		20.0f);
	if (FAILED(result))
	{
		return false;
	}

	m_chatBox->SetHorizontalAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	m_chatBox->SetVerticalAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	result = m_chatBox->SetTextBrush(pTextClass, 1.0f, 0.9f, 0.9f, 1.0f);
	if (FAILED(result))
	{
		return false;
	}

	//새로운 채팅 내역 밀어 넣기
	EventClass::GetInstance().Subscribe(SOCKET_EVENT::NEW_CHAT, [&](wchar_t* data){
		m_chatBox->AddText(data);
		});

	return true;
}

void ChatScene::Frame(D3DClass* pD3DClass, HWND hwnd, ShaderManager* pShaderManager, TextClass* pTextClass, CameraClass* pCameraClass)
{
	bool state;

	XMMATRIX view, proj;
	float mouseX, mouseY;//정규화 마우스 좌표

	InputClass::GetInstance().GetNormalizedMousePosition(mouseX, mouseY);

	pCameraClass->GetBaseViewMatrix(view);
	pD3DClass->GetOrthoMatrix(proj);

	m_exitBtn->Frame(view, proj, mouseX, mouseY);

	m_msgSend->Frame(view, proj, mouseX, mouseY);

	Render(pD3DClass, pTextClass, pShaderManager, view, proj);

	if (m_exitBtn->IsPressed())
	{
		SocketClass::GetInstance().Disconnect();//연결 종료
		EventClass::GetInstance().Publish(SCENE_EVENT::ACTIVE_MAIN_SCENE);//처음 씬으로 돌아감
	}

	return;
}

bool ChatScene::Render(D3DClass* pD3DClass, TextClass* pTextClass, ShaderManager* pShaderManager, const XMMATRIX& view, const XMMATRIX& proj)
{
	bool result;

	result = m_chatBox->Render(pD3DClass->GetDeviceContext(), pTextClass, pShaderManager->GetUIShader(), m_chatBox->GetWorldMatrix(), view, proj);
	if (!result)
	{
		return false;
	}

	result = m_msgSend->Render(pD3DClass->GetDeviceContext(), pTextClass, pShaderManager->GetUIShader(), m_msgSend->GetWorldMatrix(), view, proj);
	if (!result)
	{
		return false;
	}

	result = m_exitBtn->Render(pD3DClass->GetDeviceContext(), pShaderManager->GetUIShader(), m_exitBtn->GetWorldMatrix(), view, proj);
	if (!result)
	{
		return false;
	}

	return true;
}

void ChatScene::ToggleActive()
{
	m_active ^= true;
}

void ChatScene::SetActive(bool state)
{
	if (!state)
	{
		m_chatBox->ClearText();
	}

	m_active = state;
}

bool ChatScene::GetActive()
{
	return m_active;
}
