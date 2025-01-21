#include "ChatScene.h"

ChatScene::ChatScene()
{
	m_exitBtn = 0;
	m_msgSend = 0;
	m_chatLogs = 0;
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

	if (m_chatLogs)
	{
		delete m_chatLogs;
		m_chatLogs = nullptr;
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
		L"¹ÙÅÁ",
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


	m_chatLogs = new TextBox;
	if (!m_chatLogs)
	{
		return false;
	}

	result = m_chatLogs->Initialize(pDevice, L"..//data//assets//logbg.png", XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(800, 600.0f, 1.0f), ALIGNMENT_TOP);
	if (FAILED(result))
	{
		return false;
	}

	m_chatLogs->SetTextMargin(10.0f, 10.0f);

	result = m_chatLogs->SetTextFormat(
		pTextClass,
		L"¹ÙÅÁ",
		DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_BOLD,
		DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_NORMAL,
		20.0f);
	if (FAILED(result))
	{
		return false;
	}

	m_chatLogs->SetHorizontalAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	m_chatLogs->SetVerticalAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	result = m_chatLogs->SetTextBrush(pTextClass, 1.0f, 0.9f, 0.9f, 1.0f);
	if (FAILED(result))
	{
		return false;
	}



	return true;
}

void ChatScene::Frame(D3DClass* pD3DClass, HWND hwnd, ShaderManager* pShaderManager, TextClass* pTextClass, CameraClass* pCameraClass)
{
	bool state;

	XMMATRIX view, proj;
	float mouseX, mouseY;//Á¤±ÔÈ­ ¸¶¿ì½º ÁÂÇ¥

	InputClass::GetInstance().GetNormalizedMousePosition(mouseX, mouseY);

	pCameraClass->GetBaseViewMatrix(view);
	pD3DClass->GetOrthoMatrix(proj);

	m_exitBtn->Frame(view, proj, mouseX, mouseY);

	m_msgSend->Frame(view, proj, mouseX, mouseY);

	Render(pD3DClass, pTextClass, pShaderManager, view, proj);

	if (m_exitBtn->IsPressed())
	{
		EventClass::GetInstance().Publish(UI_EVENT::ACTIVE_MAIN_SCENE);
	}

	return;
}

bool ChatScene::Render(D3DClass* pD3DClass, TextClass* pTextClass, ShaderManager* pShaderManager, const XMMATRIX& view, const XMMATRIX& proj)
{
	bool result;

	result = m_chatLogs->Render(pD3DClass->GetDeviceContext(), pTextClass, pShaderManager->GetUIShader(), m_chatLogs->GetWorldMatrix(), view, proj);
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
	m_active = state;
}

bool ChatScene::GetActive()
{
	return m_active;
}