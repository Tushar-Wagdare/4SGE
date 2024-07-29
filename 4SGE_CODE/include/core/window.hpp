#pragma once
//#ifndef _WINDOW_HPP
//#define _WINDOW_HPP

#ifndef UNICODE
#define UNICODE
#endif

//*** Standard Windows Headers ***
#include<windows.h>
#include "resource.hpp"


//*** MACRO'S ***  
#define WIN_WIDTH 800
#define WIN_HIGHT 600


//*** Enums ***
enum
{
	AMC_ATTRIBUTE_POSITION  = 0,
	AMC_ATTRIBUTE_COLOR
};


//*** Globle Function Declarations ***
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void uninitialize(void);



//*** Globle Variable Declaration ***
FILE* gpFILE           = NULL;


 


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
        HINSTANCE hInstance = GetModuleHandle(NULL);
        BOOL isWindoActive          = FALSE;
        BOOL isWindowFullscreen     = FALSE;
    public:
        void (*render[2])(void);
        Window()
        {
            hwnd = NULL;

            ZeroMemory(&wndclass, sizeof(WNDCLASSEX));
            ZeroMemory(&msg, sizeof(MSG));

            //*** WNDCLASSEX INITIALIZATION ***
            wndclass.cbSize        = sizeof(WNDCLASSEX);
            wndclass.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
            wndclass.cbClsExtra    = 0;
            wndclass.cbWndExtra    = 0;
            wndclass.lpfnWndProc   = WndProc;
            wndclass.hInstance     = hInstance;
            wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
            wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
            wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
            wndclass.lpszClassName = TEXT("4SGE");
            wndclass.lpszMenuName  = NULL;
            wndclass.hIconSm       = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));

            //*** REGISTER WNDCLASSEX ***
            if (!RegisterClassEx(&wndclass))
            {
                MessageBox((HWND)NULL, TEXT("Failed to register a window class"), TEXT("RegisterClassEx"), MB_ICONERROR | MB_TOPMOST);
                ExitProcess(EXIT_FAILURE);
            }


            //*** CREATE WINDOW ***
            hwnd = CreateWindowEx(WS_EX_APPWINDOW,
                TEXT("4SGE"),
                TEXT("Tushar Tulshiram Wagdare"),
                WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,  
                (GetSystemMetrics(SM_CXSCREEN) / 2) - (800 / 2),
                (GetSystemMetrics(SM_CYSCREEN) / 2) - (600 / 2),
                WIN_WIDTH,
                WIN_HIGHT,
                NULL,
                NULL,
                hInstance,
                NULL);

            if (NULL == hwnd)
            {
                MessageBox((HWND)NULL, TEXT("Failed to create an application window"), TEXT("CreateWindowEx"), MB_ICONERROR | MB_TOPMOST);
                ExitProcess(EXIT_FAILURE);
            }


            //*** SHOW THE WINDOW ***
            ShowWindow(hwnd, SW_SHOW);
            SetForegroundWindow(hwnd);
            SetFocus(hwnd);

                if(initialize() < 0)
                {
                    fprintf(gpFILE, "initialize() failed..\n");
                    exit(0);
                }
        }


        Window(
            /* Mandatory arguments */
            LPCWSTR lpszClassName,
            LPCWSTR lpszAppName,
            WNDPROC lpfnWndProc,
            /* Default arguments */
            int iBrushColor         = WHITE_BRUSH,
            LPCWSTR lpCursorName    = IDC_ARROW,
            LPCWSTR lpIconName      = IDI_APPLICATION,
            LPCWSTR lpSmallIconName = IDI_APPLICATION,
            HINSTANCE hInstance     = GetModuleHandle(NULL),
            LPCWSTR lpszMenuName    = NULL,
            UINT style              = CS_HREDRAW | CS_VREDRAW,
            DWORD dwExStyle         = WS_EX_APPWINDOW,
            DWORD dwWindowStyle     = WS_OVERLAPPEDWINDOW,
            int X                   = CW_USEDEFAULT,
            int Y                   = CW_USEDEFAULT,
            int nWidth              = CW_USEDEFAULT,
            int nHeight             = CW_USEDEFAULT,
            HWND hParentWindow      = (HWND)NULL,
            HMENU hMenu             = (HMENU)NULL,
            LPVOID lpParam          = (LPVOID)NULL,
            int nShowCmd            = SW_SHOW
	        ){
                hwnd = NULL;

                ZeroMemory(&wndclass, sizeof(WNDCLASSEX));
                ZeroMemory(&msg, sizeof(MSG));

                wndclass.cbSize        = sizeof(WNDCLASSEX);
                wndclass.cbClsExtra    = 0;
                wndclass.cbWndExtra    = 0;
                wndclass.hbrBackground = (HBRUSH)GetStockObject(iBrushColor);
                wndclass.hIcon         = LoadIcon((HINSTANCE)NULL, lpIconName);
                wndclass.hIconSm       = LoadIcon((HINSTANCE)NULL, lpSmallIconName);
                wndclass.hInstance     = (HINSTANCE)GetModuleHandle(lpszClassName);
                wndclass.lpfnWndProc   = lpfnWndProc;
                wndclass.lpszClassName = lpszClassName;
                wndclass.lpszMenuName  = lpszMenuName;
                wndclass.style         = style;

                if (!RegisterClassEx(&wndclass))
                {
                    MessageBox((HWND)NULL, TEXT("Failed to register a window class"), TEXT("RegisterClassEx"), MB_ICONERROR | MB_TOPMOST);
                    ExitProcess(EXIT_FAILURE);
                }

                hwnd = CreateWindowEx(dwExStyle, lpszClassName, lpszAppName, dwWindowStyle,
                    X, Y, nWidth, nHeight, hParentWindow, hMenu, hInstance, lpParam
                );

                if (NULL == hwnd)
                {
                    MessageBox((HWND)NULL, TEXT("Failed to create an application window"), TEXT("CreateWindowEx"), MB_ICONERROR | MB_TOPMOST);
                    ExitProcess(EXIT_FAILURE);
                }

                ShowWindow(hwnd, nShowCmd);
                UpdateWindow(hwnd);

                if(initialize() < 0)
                {
                    fprintf(gpFILE, "initialize() failed..\n");
                    exit(0);
                }
            }
        

        int initialize(void);


        void display(void);


        void update(void);

        
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


        int initGameLoop()
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
                        display();

                        //*** Update ***
                        update();
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
            DWORD dwStyle          = 0;
            WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };


            //*** Code ***
            if (isWindowFullscreen == FALSE)
            {
                dwStyle = GetWindowLong(hwnd, GWL_STYLE);
                if (dwStyle & WS_OVERLAPPEDWINDOW)
                {
                    fprintf(gpFILE, "Window Contains WS_OVERLAPPEDWINDOW\n");

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
                fprintf(gpFILE, "Window Is Now Already In Fullscreen Mode\n");
                SetWindowPlacement(hwnd, &wpPrev);
                SetWindowLong(hwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
                SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
                ShowCursor(TRUE);
                fprintf(gpFILE, "Now Window Is Normal\n");
            }
        }

    
        ~Window()
        {
            uninitialize();
        }
};

//#endif
	