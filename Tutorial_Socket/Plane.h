#ifndef _PLANE_
#define _PLANE_

#include "RectTransform.h"
#include "RectTexture2D.h"
#include "ShaderManager.h"

//배경 이미지만 출력하는 UI요소
class Plane : public RectTransform, public RectTexture2D
{
public:

	HRESULT Initialize(ID3D11Device* pDevice, const wchar_t* texturePath, XMFLOAT3 position, XMFLOAT3 scale, UINT align)
	{
		HRESULT result;

		result = InitializeBuffer(pDevice);
		if (FAILED(result))
		{
			return result;
		}

		const wchar_t* filepath[1] = { texturePath };

		result = SetTexture(pDevice, filepath, 1);
		if (FAILED(result))
		{
			return result;
		}

		SetTransform(position, scale, align);

		return S_OK;
	}

	bool Render(ID3D11DeviceContext* pDeviceContext, UIShaderClass* pUIShader, const DirectX::XMMATRIX& world, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj)
	{
		bool result;

		//행렬 업데이트
		result = pUIShader->Render(pDeviceContext, world, view, proj);
		if (!result)
		{
			return false;
		}

		Draw(pDeviceContext, 0);
	
		return true;
	}
};

#endif
