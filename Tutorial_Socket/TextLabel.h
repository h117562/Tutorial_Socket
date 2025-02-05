#ifndef _TEXT_LABEL_
#define _TEXT_LABEL_

#include "RectTransform.h"
#include "ShaderManager.h"
#include "InputClass.h"
#include "textclass.h"

//텍스트만 출력하는 UI요소
class TextLabel : public RectTransform
{
public:
	TextLabel()
	{
		m_text = L"";
		m_textFormat = nullptr;
		m_textBrush = nullptr;
	}

	~TextLabel()
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

	HRESULT Initialize(XMFLOAT3 position, XMFLOAT3 scale, UINT align)
	{
		SetTransform(position, scale, align);
		UpdateTextArea();

		return S_OK;
	}

	void Render(TextClass* pTextClass)
	{
		//저장된 브러쉬나 텍스트 형식이 없으면 기본 설정으로 텍스르 렌더링
		if (m_textFormat == nullptr || m_textBrush == nullptr)
		{
			pTextClass->RenderText(m_text.c_str(), m_textArea);
		}
		else
		{
			pTextClass->RenderText(m_text.c_str(), m_textArea, m_textFormat, m_textBrush);
		}

		return;
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

	void SetText(const wchar_t* text)
	{
		m_text = text;
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
			position.x * scale.x + (SCREEN_WIDTH - scale.x) / 2,
			position.y * -scale.y + (SCREEN_HEIGHT - scale.y) / 2,
			position.x * scale.x + (SCREEN_WIDTH - scale.x) / 2 + scale.x,
			position.y * -scale.y + (SCREEN_HEIGHT - scale.y) / 2 + scale.y
		);
	}

private:
	std::wstring m_text;
	D2D1_RECT_F m_textArea;
	IDWriteTextFormat* m_textFormat;
	ID2D1SolidColorBrush* m_textBrush;
};

#endif
