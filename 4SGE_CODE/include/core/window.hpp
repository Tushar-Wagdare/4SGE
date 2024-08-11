#ifndef _WINDOW_HPP
#define _WINDOW_HPP

#ifndef UNICODE
#define UNICODE
#endif

//*** Standard Windows Headers ***
#include<windows.h>


//*** MACRO'S ***  
#define WIN_WIDTH 800
#define WIN_HIGHT 600  


//*** Globle Function Declarations ***
//*** Globle Variable Declaration ***
FILE* gpFILE                = NULL;
//OpenGL Related Globle Variables
PIXELFORMATDESCRIPTOR pfd;
int iPexelFormatIndex  = 0;
HDC ghdc               = NULL;
HGLRC ghrc             = NULL;


//*** Class ****
class Window
{
    private:
        WNDCLASSEX wndclass;
        HWND hwnd;
        MSG msg;
        HINSTANCE hInstance         = GetModuleHandle(NULL);
        BOOL isWindoActive          = FALSE;
        BOOL isWindowFullscreen     = FALSE;
        DWORD dwStyle;
        WINDOWPLACEMENT wpPrev;
        
    public:
        Window(
            /* Mandatory arguments */
            WNDPROC lpfnWndProc,
            /* Default arguments */
            int iBrushColor         = BLACK_BRUSH,
            LPCWSTR lpCursorName    = IDC_ARROW,
            LPCWSTR lpIconName      = IDI_APPLICATION,
            LPCWSTR lpSmallIconName = IDI_APPLICATION,
            LPCWSTR lpszMenuName    = NULL,
            UINT style              = CS_HREDRAW | CS_VREDRAW | CS_OWNDC
        )
        {
            hwnd = NULL;

            ZeroMemory(&wndclass, sizeof(WNDCLASSEX));
            ZeroMemory(&msg, sizeof(MSG));

            //*** WNDCLASSEX INITIALIZATION ***
            wndclass.cbSize        = sizeof(WNDCLASSEX);
            wndclass.style         = style;
            wndclass.cbClsExtra    = 0;
            wndclass.cbWndExtra    = 0;
            wndclass.lpfnWndProc   = lpfnWndProc;
            wndclass.hInstance     = hInstance;
            wndclass.hbrBackground = (HBRUSH)GetStockObject(iBrushColor);
            wndclass.hIcon         = LoadIcon((HINSTANCE)NULL, lpIconName);;
            wndclass.hCursor       = LoadCursor(NULL, lpCursorName);
            wndclass.lpszClassName = TEXT("4SGE");
            wndclass.lpszMenuName  = NULL;
            wndclass.hIconSm       = LoadIcon((HINSTANCE)NULL, lpSmallIconName);

            //*** REGISTER WNDCLASSEX ***
            if (!RegisterClassEx(&wndclass))
            {
                MessageBox((HWND)NULL, TEXT("Failed to register a window class"), TEXT("RegisterClassEx"), MB_ICONERROR | MB_TOPMOST);
                ExitProcess(EXIT_FAILURE);
            }
        }


        void createWindow(
            /* Mandatory arguments */
            LPCWSTR lpszClassName, 
            LPCWSTR lpszAppName,
            /* Default arguments */
            DWORD dwExStyle         = WS_EX_APPWINDOW,
            DWORD dwWindowStyle     = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
            int X                   = (GetSystemMetrics(SM_CXSCREEN) / 2) - (800 / 2),
            int Y                   = (GetSystemMetrics(SM_CYSCREEN) / 2) - (600 / 2),
            int nWidth              = WIN_WIDTH,
            int nHeight             = WIN_HIGHT,
            HWND hParentWindow      = (HWND)NULL,
            HMENU hMenu             = (HMENU)NULL,
            LPVOID lpParam          = (LPVOID)NULL,
            int nShowCmd            = SW_SHOW){

            //*** CREATE WINDOW ***
            hwnd = CreateWindowEx(dwExStyle,
                lpszClassName,
                lpszAppName,
                dwWindowStyle,  
                X,
                Y,
                nWidth,
                nHeight,
                hParentWindow,
                hMenu,
                hInstance,
                lpParam);

            if (NULL == hwnd)
            {
                MessageBox((HWND)NULL, TEXT("Failed to create an application window"), TEXT("CreateWindowEx"), MB_ICONERROR | MB_TOPMOST);
                ExitProcess(EXIT_FAILURE);
            }


            //*** SHOW THE WINDOW ***
            ShowWindow(hwnd, nShowCmd);
            SetForegroundWindow(hwnd);
            SetFocus(hwnd);
        }

        
        HWND getWindowHandle()
        {
            return(hwnd);
        }


        void setWindowStatus(BOOL status)
        {
            isWindoActive = status;
        }


        BOOL getWindowStatus(void)
        {
            return(isWindoActive);
        }


        void killWindow()
        {
            DestroyWindow(hwnd);
        }


        int initGameLoop(void (*render[2])(void))
        {
            BOOL bDone = FALSE;

            while(bDone == FALSE)
            {
                if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
                {
                    if (msg.message == WM_QUIT)
                        bDone = TRUE;
                    else
                    {
                        TranslateMessage(&msg);
                        DispatchMessage(&msg);
                    }
                }
                else
                {
                    if (getWindowStatus() == TRUE)
                    {
                        //*** Render ***
                        render[0]();

                        //*** Update ***
                        render[1]();
                    }
                }
            }

            return((int)msg.wParam);
        }


        int initMessageLoop(void)
        {
            BOOL bDone = FALSE;

            if (PeekMessage(&msg, (HWND)NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
					bDone = TRUE;
				else
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}

            return((int)msg.wParam);
        }


        void setWindowFullscreenStatus(BOOL status)
        {
            isWindowFullscreen = status;
        }


        BOOL getWindowFullscreenStatus(void)
        {
            return(isWindowFullscreen);
        }


        void ToggleFullscreen(void)
        {
            //*** Local Variable Declaration ****
            MONITORINFO mi = { sizeof(MONITORINFO) };


            //*** Code ***
            if (isWindowFullscreen == FALSE)
            { 
                dwStyle          = GetWindowLong(hwnd, GWL_STYLE);
                if (dwStyle & WS_OVERLAPPEDWINDOW)
                {
                    if (GetWindowPlacement(hwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(hwnd, MONITORINFOF_PRIMARY), &mi))
                    {
                        SetWindowLong(hwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
                        SetWindowPos(hwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
                    }
                }
                ShowCursor(FALSE);
            }
            else
            {
                SetWindowPlacement(hwnd, &wpPrev);
                SetWindowLong(hwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
                SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
                ShowCursor(TRUE);
            }
        }

    
        ~Window()
        {
            //*** Code ***
            if (getWindowFullscreenStatus() == TRUE)
            {
                ToggleFullscreen();
                setWindowFullscreenStatus(FALSE);
            }

            //*** Destroy Window ***
	        killWindow();
        }
};

#endif
	