#ifndef _CHAT_SCENE_
#define _CHAT_SCENE_

#include "D3DClass.h"
#include "TextClass.h"
#include "InputClass.h"
#include "ShaderManager.h"
#include "CameraClass.h"
#include "EventClass.h"

//Elements
#include "Button.h"
#include "TextInput.h"
#include "TextBox.h"

class ChatScene
{
public:
	ChatScene();
	~ChatScene();

	bool Initialize(ID3D11Device*, TextClass*);
	void Frame(D3DClass*, HWND, ShaderManager*, TextClass*, CameraClass*);
	bool Render(D3DClass*, TextClass*, ShaderManager*, const XMMATRIX&, const XMMATRIX&);
	void ToggleActive();
	void SetActive(bool state);
	bool GetActive();

private:
	Button* m_exitBtn;//퇴장 버튼
	TextInput* m_msgSend;//입력용 텍스트 박스
	TextBox* m_chatLogs;//채팅방 로그 출력용

	bool m_active;
};

#endif