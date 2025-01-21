#ifndef _TEXT_BOX_
#define _TEXT_BOX_

#include "RectTransform.h"
#include "RectTexture2D.h"
#include "ShaderManager.h"
#include "InputClass.h"
#include "textclass.h"

//배경 이미지와 함께 텍스트를 렌더링하는 UI요소
class TextBox : public RectTransform, public RectTexture2D
{
public:
	TextBox()
	{
		m_text = L"";
		m_limit = 255;
		m_textFormat = nullptr;
		m_textBrush = nullptr;
		m_margin = XMFLOAT2(0.0f, 0.0f);
	}

	~TextBox()
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

	HRESULT Initialize(ID3D11Device* pDevice, const wchar_t* texturePath, XMFLOAT3 position, XMFLOAT3 scale, UINT align)
	{
		HRESULT result;

		result = InitializeBuffer(pDevice);
		if (FAILED(result))
		{
			return result;
		}

		const wchar_t* filepath[1] = { texturePath };

		result = SetTexture(pDevice, filepath, _countof(filepath));
		if (FAILED(result))
		{
			return result;
		}

		SetTransform(position, scale, align);

		UpdateTextArea();

		return S_OK;
	}

	bool Render(ID3D11DeviceContext* pDeviceContext, TextClass* pTextClass, UIShaderClass* pUIShader, const DirectX::XMMATRIX& world, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj)
	{
		bool result;

		result = pUIShader->Render(pDeviceContext, world, view, proj);
		if (!result)
		{
			return false;
		}

		Draw(pDeviceContext, 0);
		
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

	void SetText(const wchar_t* text)
	{
		unsigned int len = wcslen(text);

		if (len > m_limit)
		{
			m_text = std::wstring(text, m_limit);
		}
		else
		{
			m_text = text;
		}

	}

	void SetLimitLength(const UINT length)
	{
		m_limit = length;
		m_text.reserve(length);//배열 길이 미리 예약
	}

	//텍스트 가로 정렬
	void SetHorizontalAlignment(DWRITE_TEXT_ALIGNMENT flag)
	{
		if (m_textFormat)
		{
			m_textFormat->SetTextAlignment(flag);
		}
	}

	//텍스트 세로 정렬
	void SetVerticalAlignment(DWRITE_PARAGRAPH_ALIGNMENT flag)
	{
		if (m_textFormat)
		{
			m_textFormat->SetParagraphAlignment(flag);
		}
	}

	void SetTextMargin(float x, float y)
	{
		m_margin = XMFLOAT2(x, y);

		UpdateTextArea();
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

private:

	IDWriteTextFormat** GetTextFormat()
	{
		return &m_textFormat;
	}

	ID2D1SolidColorBrush** GetTextBrush()
	{
		return &m_textBrush;
	}

	//텍스트가 그려질 공간을 업데이트
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
	UINT m_limit;
	IDWriteTextFormat* m_textFormat;
	ID2D1SolidColorBrush* m_textBrush;
};

#endif
