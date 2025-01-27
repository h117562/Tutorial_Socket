#ifndef _CHAT_SCENE_
#define _CHAT_SCENE_

#include <vector>

#include "D3DClass.h"
#include "TextClass.h"
#include "InputClass.h"
#include "ShaderManager.h"
#include "CameraClass.h"
#include "EventClass.h"
#include "SocketClass.h"

//Elements
#include "Button.h"
#include "TextInput.h"
#include "ChatBox.h"

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
	Button* m_exitBtn;//���� ��ư
	TextInput* m_msgSend;//�Է¿� �ؽ�Ʈ �ڽ�
	ChatBox* m_chatBox;//ä�ù� �α� ��¿�
	bool m_active;
};

#endif