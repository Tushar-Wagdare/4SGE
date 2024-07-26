#pragma once        

//*** MACRO'S ***
#define MYICON    101  
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

//*** Globle Variable Declaration ***
FILE* gpFILE           = NULL;
HWND ghwnd             = NULL;
BOOL gbActive          = FALSE; 
DWORD dwStyle          = 0;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
BOOL gbFullscreen      = FALSE;

//*** OpenGL Related Globle Variables ***
PIXELFORMATDESCRIPTOR pfd;
int iPexelFormatIndex  = 0;
HDC ghdc               = NULL;
HGLRC ghrc             = NULL;

