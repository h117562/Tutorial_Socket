#ifndef _USER_INTERFACE_MANAGER_H_
#define _USER_INTERFACE_MANAGER_H_

#include "TextClass.h"
#include "ShaderManager.h"
#include "InputClass.h"
#include "FrameTimer.h"
#include "EventClass.h"

#include "DebugInfo.h"

//Tutorial Socket
#include "MainScene.h"
#include "LoadingScene.h"
#include "ChatScene.h"

class UIManager
{
public:
	UIManager();
	~UIManager();
	UIManager(const UIManager&);

	bool Initialize(D3DClass*, TextClass*);
	bool Frame(D3DClass*, HWND hwnd, ShaderManager*, TextClass*, CameraClass*, FrameTimer*);

private:
	DebugInfo m_debugUI;
	MainScene m_mainScene;
	LoadingScene m_loadingScene;
	ChatScene m_chatScene;
};

#endif