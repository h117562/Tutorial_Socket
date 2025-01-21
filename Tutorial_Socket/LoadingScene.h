#ifndef _LOADING_SCENE_
#define _LOADING_SCENE_

#include "D3DClass.h"
#include "TextClass.h"
#include "InputClass.h"
#include "ShaderManager.h"
#include "CameraClass.h"
#include "FrameTimer.h"
#include "EventClass.h"

//Elements
#include "Plane.h"
#include "TextLabel.h"

class LoadingScene
{
public:
	LoadingScene();
	~LoadingScene();

	bool Initialize(ID3D11Device*, TextClass*);
	void Frame(D3DClass*, HWND, ShaderManager*, TextClass*, CameraClass*, FrameTimer*);
	bool Render(D3DClass*, TextClass*, ShaderManager*, const XMMATRIX&, const XMMATRIX&);
	void ToggleActive();
	void SetActive(bool state);
	bool GetActive();

private:
	Plane* m_backGround;//배경 이미지
	TextLabel* m_label;//텍스트 출력용

	float m_count;
	bool m_active;
};

#endif