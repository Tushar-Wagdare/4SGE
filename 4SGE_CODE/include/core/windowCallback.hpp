#include"window.hpp"

extern Window* g_myWindow;


LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//*** Function Declaration ***
	void resize(int, int);


	//*** Code ***
	switch (iMsg)
	{
	case WM_SETFOCUS:
		g_myWindow->setWindowStatus(TRUE);
		break;

	case WM_KILLFOCUS:
		g_myWindow->setWindowStatus(FALSE);
		break;

	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_ERASEBKGND:
		return(0);

	case WM_KEYDOWN:
		switch (LOWORD(wParam))
		{
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;
		}
		break;

	case WM_CHAR:
		switch (LOWORD(wParam))
		{
		case 'F':
		case 'f':
			if (g_myWindow->getWindowFullscreenStatus() == FALSE)
			{
				g_myWindow->ToggleFullscreen();
				g_myWindow->setWindowFullscreenStatus(TRUE);
			}
			else
			{
				g_myWindow->ToggleFullscreen();
				g_myWindow->setWindowFullscreenStatus(FALSE);
			} 
			break;
		}
		break;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		break;
	}


	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

