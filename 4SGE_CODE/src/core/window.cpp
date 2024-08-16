#include "..\..\include\core\window.hpp"
#include<windows.h>
#include<stdio.h>

extern FILE* gpFILE;



Window::Window(
            /* Mandatory arguments */
            WNDPROC lpfnWndProc,
            /* Default arguments */
            int iBrushColor,
            LPCWSTR lpCursorName,
            LPCWSTR lpIconName,
            LPCWSTR lpSmallIconName,
            LPCWSTR lpszMenuName,
            UINT style
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


        void Window::createWindow(
            /* Mandatory arguments */
            LPCWSTR lpszClassName, 
            LPCWSTR lpszAppName,
            /* Default arguments */
            DWORD dwExStyle,
            DWORD dwWindowStyle,
            int X,
            int Y,
            int nWidth,
            int nHeight,
            HWND hParentWindow,
            HMENU hMenu,
            LPVOID lpParam,
            int nShowCmd){

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

        
        HWND Window::getWindowHandle()
        {
            return(hwnd);
        }


        void Window::setWindowStatus(BOOL status)
        {
            isWindoActive = status;
        }


        BOOL Window::getWindowStatus(void)
        {
            return(isWindoActive);
        }


        void Window::killWindow()
        {
            DestroyWindow(hwnd);
        }


        int Window::initGameLoop(void (*render[2])(void))
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


        int Window::initMessageLoop(void)
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


        status_t Window::switchToRenderingContext(PIXELFORMATDESCRIPTOR pfd)
        {
            //01 -  Initialization Of PFD
            if(pfd.nSize == 0)
            {
                pfd.nSize      = sizeof(PIXELFORMATDESCRIPTOR);
                pfd.nVersion   = 1;
                pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
                pfd.iPixelType = PFD_TYPE_RGBA;
                pfd.cColorBits = 32;
                pfd.cRedBits   = 8;
                pfd.cGreenBits = 8;
                pfd.cBlueBits  = 8;
                pfd.cAlphaBits = 8;
                pfd.cDepthBits = 32;


                //02 - Get The DC
                ghdc = GetDC(hwnd);
                if (ghdc == NULL)
                {
                    fprintf(gpFILE,"GetDC Failed !!!\n\n");
                    return(-1);
                }


                //03 - Tell OS Hya PFD shi match honara PFD de
                iPexelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
                if (iPexelFormatIndex == 0)
                {
                    fprintf(gpFILE,"ChoosepixelFormat() Failed\n\n");
                    return(-2);
                }
                

                //04 - Set Obtained Pixel Format
                if (SetPixelFormat(ghdc, iPexelFormatIndex, &pfd) == FALSE)
                {
                    fprintf(gpFILE,"SetPixelFormat() Failed\n\n");
                    return(-3);
                }


                //05 - Tell WGL Bridging library To Give OpenGL Copatible DC From 'ghdc'
                ghrc = wglCreateContext(ghdc);
                if (ghrc == NULL)
                {
                    fprintf(gpFILE,"wglCreateContex() Failed\n\n");
                    return(-4);
                }


                //06 - Now 'ghdc' End Its Roll And Give Controll To 'ghrc'
                if (wglMakeCurrent(ghdc, ghrc) == FALSE)
                {
                    fprintf(gpFILE,"wglMakeCurrent() failed\n\n");
                    return(-5);
                }
            } 
            //ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
            
            return(0);
        }


        HDC Window::getDeviceContext(void)
        {
            return(ghdc);
        }


        HGLRC Window::getRenderingContext(void)
        {
            return(ghrc);
        }


        void Window::setWindowFullscreenStatus(BOOL status)
        {
            isWindowFullscreen = status;
        }


        BOOL Window::getWindowFullscreenStatus(void)
        {
            return(isWindowFullscreen);
        }


        void Window::ToggleFullscreen(void)
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

    
        Window::~Window()
        {
            //*** Code ***
            if (getWindowFullscreenStatus() == TRUE)
            {
                ToggleFullscreen();
                setWindowFullscreenStatus(FALSE);
            }

            //*** Make The hdc As Current DC ***//
            if (wglGetCurrentContext() == ghrc)
            {
                wglMakeCurrent(NULL, NULL);
            }


            //*** Destroy Rendering Contex***
            if (ghrc)
            {
                wglDeleteContext(ghrc);
                ghrc = NULL;
            }


            //*** Release HDC ***
            if (ghdc)
            {
                ReleaseDC(hwnd, ghdc);
                ghdc = NULL;
            }

            //*** Destroy Window ***
	        killWindow();
        }


        