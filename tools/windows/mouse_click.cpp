#pragma once
#include <Windows.h>
#include <cstdio>

int main(int argc, char** argv)
{

	//POINT pt;
	//SetCursorPos(124, 16);
	////GetCursorPos(&pt);
	////printf("---mouse--- x = %d, y = %d---\n", pt.x, pt.y);
	//mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0); //�������
	//mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);hwnd //�ɿ����
	HWND hwnd; 
	hwnd = FindWindow(NULL, L"VLC media player");
	//hwnd = FindWindow(NULL, L"WR-X50�״�ϵͳ�Կ����");
	if (NULL == hwnd)
	{
		printf("Not found");
		return 1;
	}
	ShowWindow(hwnd, SW_NORMAL);
	
	// �̶�λ��,�����ö���ʾ
	SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 200, 200, SWP_SHOWWINDOW);

	return 0;
}