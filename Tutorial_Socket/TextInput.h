#ifndef _TEXT_INPUT_
#define _TEXT_INPUT_

#include "RectTransform.h"
#include "RectTexture2D.h"
#include "ShaderManager.h"
#include "InputClass.h"
#include "textclass.h"

//�Է� ������ �ؽ�Ʈ UI���
class TextInput : public RectTransform , public RectTexture2D
{
public:
	TextInput()
	{
		m_text = L"";
		m_textInputState = TextInputState::IDLE;
		m_textFormat = nullptr;
		m_textBrush = nullptr;
	}

	~TextInput()
	{
		if (m_textFormat)
		{
			m_textFormat->Release();
			m_textFormat = nullptr;
		}

		if (m_textBrush)
		{
			m_textBrush->Release();
			m_textBrush = nullptr;
		}
	}

	HRESULT Initialize(ID3D11Device* pDevice, const wchar_t* idleTexturePath, const wchar_t* focusTexturePath, XMFLOAT3 position, XMFLOAT3 scale, UINT align)
	{
		HRESULT result;

		result = InitializeBuffer(pDevice);
		if (FAILED(result))
		{
			return result;
		}

		const wchar_t* filepath[2] = { idleTexturePath , focusTexturePath };

		result = SetTexture(pDevice, filepath, _countof(filepath));
		if (FAILED(result))
		{
			return result;
		}

		SetTransform(position, scale, align);

		UpdateTextArea();

		return S_OK;
	}

	void Frame(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj, const float& mouseX, const float& mouseY)
	{
		bool state;
		
		if (InputClass::GetInstance().GetLeftMouseButtonPressedAndReleased())
		{
			state = RayCast(GetWorldMatrix(), view, proj, mouseX, mouseY);

			if (state)
			{
				m_textInputState = TextInputState::FOCUS;
				InputClass::GetInstance().SetText(L"");

				return;
			}
			else
			{
				m_textInputState = TextInputState::IDLE;
				InputClass::GetInstance().SetText(L"");

				return;
			}
		}

		UpdateText();

		return;
	}

	bool Render(ID3D11DeviceContext* pDeviceContext, TextClass* pTextClass, UIShaderClass* pUIShader, const DirectX::XMMATRIX& world, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj)
	{
		bool result;

		result = pUIShader->Render(pDeviceContext, world, view, proj);
		if (!result)
		{
			return false;
		}

		if (m_textInputState != TextInputState::FOCUS)
		{
			Draw(pDeviceContext, 0);//0��° �ؽ��� �����ؼ� �׸���
		}
		else
		{
			Draw(pDeviceContext, 1);//1��° �ؽ��� �����ؼ� �׸���
		}

		if (m_textFormat == nullptr || m_textBrush == nullptr)
		{
			pTextClass->RenderText(m_text.c_str(), m_textArea);
		}
		else
		{
			pTextClass->RenderText(m_text.c_str(), m_textArea, m_textFormat, m_textBrush);
		}

		return true;
	}

	void SetLimitLength(const UINT length)
	{
	}

	const wchar_t* GetText()
	{
		return m_text.c_str();
	}

	//�ؽ�Ʈ ���� ����
	void SetHorizontalAlignment(DWRITE_TEXT_ALIGNMENT flag)
	{
		if (m_textFormat)
		{
			m_textFormat->SetTextAlignment(flag);
		}
	}

	//�ؽ�Ʈ ���� ����
	void SetVerticalAlignment(DWRITE_PARAGRAPH_ALIGNMENT flag)
	{
		if (m_textFormat)
		{
			m_textFormat->SetParagraphAlignment(flag);
		}
	}

	HRESULT SetTextFormat(TextClass* pTextClass, const wchar_t* fontName, DWRITE_FONT_WEIGHT weight, DWRITE_FONT_STYLE style, DWRITE_FONT_STRETCH stretch, float fontSize)
	{
		HRESULT result;

		result = pTextClass->CreateTextFormat(GetTextFormat(), fontName, weight, style, stretch, fontSize);
		if (FAILED(result))
		{
			return result;
		}
	
		return result;
	}

	HRESULT SetTextBrush(TextClass* pTextClass, float r, float g, float b, float a)
	{
		HRESULT result;

		result = pTextClass->CreateTextBrush(GetTextBrush(), r, g, b, a);
		if (FAILED(result))
		{
			return result;
		}

		return result;
	}

	void SetTextMargin(float x, float y)
	{
		m_margin = XMFLOAT2(x, y);

		UpdateTextArea();
	}

private:
	enum struct TextInputState
	{
		IDLE,
		FOCUS
	};

	IDWriteTextFormat** GetTextFormat()
	{
		return &m_textFormat;
	}

	ID2D1SolidColorBrush** GetTextBrush()
	{
		return &m_textBrush;
	}

	void UpdateText()
	{
		//�Է� ���� ������ �� InputClass�� �ؽ�Ʈ�� ������
		if (m_textInputState != TextInputState::IDLE)
		{
			m_text = InputClass::GetInstance().GetText();

			return;
		}
	}

	//�ؽ�Ʈ�� �׷��� ������ ������Ʈ
	void UpdateTextArea()
	{
		XMFLOAT3 position = GetWorldPosition();
		XMFLOAT3 scale = GetScale();

		m_textArea = D2D1::RectF(
			position.x * scale.x + (SCREEN_WIDTH - scale.x) / 2 + m_margin.x,
			position.y * -scale.y + (SCREEN_HEIGHT - scale.y) / 2 + m_margin.y,
			position.x * scale.x + (SCREEN_WIDTH - scale.x) / 2 + scale.x - m_margin.x,
			position.y * -scale.y + (SCREEN_HEIGHT - scale.y) / 2 + scale.y - m_margin.y
		);
	}

private:
	std::wstring m_text;
	D2D1_RECT_F m_textArea;
	XMFLOAT2 m_margin;
	TextInputState m_textInputState;
	IDWriteTextFormat* m_textFormat;
	ID2D1SolidColorBrush* m_textBrush;
};

#endif
