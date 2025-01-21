#include "MainScene.h"

MainScene::MainScene()
{
	m_enterBtn = 0;
	m_backGround = 0;
	m_ipBox = 0;
	m_label = 0;
	m_active = true;
}

MainScene::~MainScene()
{
	if (m_label)
	{
		delete m_label;
		m_label = nullptr;
	}

	if (m_enterBtn)
	{
		delete m_enterBtn;
		m_enterBtn = nullptr;
	}

	if (m_backGround)
	{
		delete m_backGround;
		m_backGround = nullptr;
	}

	if (m_ipBox)
	{
		delete m_ipBox;
		m_ipBox = nullptr;
	}

}

bool MainScene::Initialize(ID3D11Device* pDevice, TextClass* pTextClass)
{
	HRESULT result;

	m_enterBtn = new Button();
	if (!m_enterBtn)
	{
		return false;
	}

	result = m_enterBtn->Initialize(pDevice, L"..//data//assets//btn1.png", L"..//data//assets//btn2.png", L"..//data//assets//btn3.png", XMFLOAT3(0.0f, -200.0f, 0.0f), XMFLOAT3(100.0f, 100.0f, 1.0f), 0);
	if (FAILED(result))
	{
		return false;
	}

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

	m_ipBox = new TextInput;
	if (!m_ipBox)
	{
		return false;
	}

	result = m_ipBox->Initialize(pDevice, L"..//data//assets//enter1.png", L"..//data//assets//enter2.png", XMFLOAT3(0.0f, 200.0f, 0.0f), XMFLOAT3(500.0f, 50.0f, 1.0f), 0);
	if (FAILED(result))
	{
		return false;
	}

	result = m_ipBox->SetTextFormat(
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

	m_ipBox->SetHorizontalAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	m_ipBox->SetVerticalAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	result = m_ipBox->SetTextBrush(pTextClass, 1.0f, 0.9f, 0.9f, 1.0f);
	if (FAILED(result))
	{
		return false;
	}


	m_label = new TextLabel;
	if (!m_label)
	{
		return false;
	}

	m_label->SetText(L"IP ÀÔ·Â");
	m_label->Initialize(XMFLOAT3(0.0f, 250.0f, 0.0f), XMFLOAT3(100.0f, 100.0f, 1.0f), 0);

	result = m_label->SetTextFormat(
		pTextClass,
		L"±¼¸²",
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

	return true;
}

void MainScene::Frame(D3DClass* pD3DClass, HWND hwnd, ShaderManager* pShaderManager, TextClass* pTextClass, CameraClass* pCameraClass)
{
	bool state;

	XMMATRIX view, proj;
	float mouseX, mouseY;//Á¤±ÔÈ­ ¸¶¿ì½º ÁÂÇ¥

	InputClass::GetInstance().GetNormalizedMousePosition(mouseX, mouseY);

	pCameraClass->GetBaseViewMatrix(view);
	pD3DClass->GetOrthoMatrix(proj);

	m_enterBtn->Frame(view, proj, mouseX, mouseY);

	m_ipBox->Frame(view, proj, mouseX, mouseY);

	Render(pD3DClass, pTextClass, pShaderManager, view, proj);

	if (m_enterBtn->IsPressed())
	{
		EventClass::GetInstance().Publish(UI_EVENT::ACTIVE_CHAT_SCENE);
	}

	return;
}

bool MainScene::Render(D3DClass* pD3DClass, TextClass* pTextClass, ShaderManager* pShaderManager, const XMMATRIX& view, const XMMATRIX& proj)
{
	bool result;

	result = m_backGround->Render(pD3DClass->GetDeviceContext(), pShaderManager->GetUIShader(), m_backGround->GetWorldMatrix(), view, proj);
	if (!result)
	{
		return false;
	}

	result = m_enterBtn->Render(pD3DClass->GetDeviceContext(), pShaderManager->GetUIShader(), m_enterBtn->GetWorldMatrix(), view, proj);
	if (!result)
	{
		return false;
	}

	result = m_ipBox->Render(pD3DClass->GetDeviceContext(), pTextClass, pShaderManager->GetUIShader(), m_ipBox->GetWorldMatrix(), view, proj);
	if (!result)
	{
		return false;
	}

	result = m_label->Render(pTextClass);
	if (!result)
	{
		return false;
	}

	return true;
}

void MainScene::ToggleActive()
{
	m_active ^= true;
}

void MainScene::SetActive(bool state)
{
	m_active = state;
}

bool MainScene::GetActive()
{
	return m_active;
}