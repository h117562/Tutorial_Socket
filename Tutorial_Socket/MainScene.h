#ifndef _MAIN_SCENE_
#define _MAIN_SCENE_

#include "D3DClass.h"
#include "TextClass.h"
#include "InputClass.h"
#include "ShaderManager.h"
#include "CameraClass.h"
#include "EventClass.h"
#include "SocketClass.h"

//Elements
#include "Button.h"
#include "Plane.h"
#include "TextInput.h"
#include "TextLabel.h"

class MainScene
{
public:
	MainScene();
	~MainScene();

	bool Initialize(ID3D11Device*, TextClass*);
	void Frame(D3DClass*, HWND, ShaderManager*, TextClass*, CameraClass*);
	bool Render(D3DClass*, TextClass*, ShaderManager*, const XMMATRIX&, const XMMATRIX&);
	void ToggleActive();
	void SetActive(bool state);
	bool GetActive();

private:
	Button* m_enterBtn;//입장 버튼
	Plane* m_backGround;//배경 이미지
	TextInput* m_ipBox;//Ip 입력 요소
	TextLabel* m_label;//텍스트 출력용

	bool m_active;
};

#endif