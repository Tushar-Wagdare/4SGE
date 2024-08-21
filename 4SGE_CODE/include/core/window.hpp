#ifndef _WINDOW_HPP
#define _WINDOW_HPP


#ifndef UNICODE
#define UNICODE
#endif


///*** Standard Windows Headers ***///
#include<windows.h>


///*** MACRO'S ***///  
#define SUCCESS   1
#define FAIL      0
#define WIN_WIDTH 800
#define WIN_HIGHT 600


///*** Typedefs ***///
typedef int status_t;


///*** Class ***///
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

        int iPexelFormatIndex       = 0;
        HDC ghdc                    = NULL;
        HGLRC ghrc                  = NULL;
        
    public:
        Window
        (
            /* Mandatory arguments */
            WNDPROC,
            /* Default arguments */
            int iBrushColor         = BLACK_BRUSH,
            LPCWSTR lpCursorName    = IDC_ARROW,
            LPCWSTR lpIconName      = IDI_APPLICATION,
            LPCWSTR lpSmallIconName = IDI_APPLICATION,
            LPCWSTR lpszMenuName    = NULL,
            UINT style              = CS_HREDRAW | CS_VREDRAW | CS_OWNDC
        );


        void createWindow
        (
            /* Mandatory arguments */
            LPCWSTR, 
            LPCWSTR,
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
            int nShowCmd            = SW_SHOW
        );

        
        HWND getWindowHandle();


        void setWindowStatus(BOOL);


        BOOL getWindowStatus(void);


        void killWindow();


        int initGameLoop(void (*[2])(void));


        int initMessageLoop(void);


        int switchToRenderingContext(PIXELFORMATDESCRIPTOR pfd = {NULL});


        HDC getDeviceContext(void);


        HGLRC getRenderingContext(void);


        void setWindowFullscreenStatus(BOOL);


        BOOL getWindowFullscreenStatus(void);


        void ToggleFullscreen(void);

    
        ~Window();
};

#endif
	