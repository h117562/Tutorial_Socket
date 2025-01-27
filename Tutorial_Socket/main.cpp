
//#pragma comment(linker , "/entry:WinMainCRTStartup /subsystem:console")

#define _WINSOCKAPI_

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#pragma comment(lib, "Ws2_32.lib")

#include "Global.h"
#include "Systemclass.h"

//hInstance �ν��Ͻ� �ڵ�, �޸𸮿� �ε�Ǵ� exe �ĺ���
//hPrevInstance �Ⱦ��� ����
//pScmdline �����ڵ� argv �����
//nCmdshow ShowWindow() Ŀ�ǵ� 0~11 ���� ���� ex) SW_HIDE 0
//Window�� ���⼭ �������� �ʰ� SystemClass���� ����

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdshow)
{
	bool result;

	SystemClass* systemClass;

	//SystemClass ����
	systemClass = new SystemClass;
	if (!systemClass)
	{
		return 0;
	}

	//SystemClass �ʱ�ȭ
	result = systemClass->Initialize();
	if (result)
	{
		systemClass->Run();
	}

	//System Class�� ��� �޸� ��ȯ
	systemClass->Shutdown();
	delete systemClass;
	systemClass = 0;

	return 0;
}