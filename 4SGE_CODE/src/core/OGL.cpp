#include<windows.h>
#include<stdio.h>
#include<stdlib.h>
#include "..\..\include\core\OGL.h"

//*** OpenGL Headers ***//
//#include<gl/glew.h>
#include<C:/glew-2.1.0/include/GL/glew.h>
#include<gl/GL.h>
#include"..\..\include\math\vmath.h"
//*** Name space ***
using namespace vmath;

//*** link With OpenGL Library//
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "OpenGL32.lib")

///PP Variables
GLuint shaderProgramObject  = 0;
GLuint vao                  = 0;
GLuint vbo_position         = 0;
GLuint vbo_color            = 0;
GLuint mvpMatrixUniform     = 0;
mat4 perspectiveProjectionMatrix;//in vmath.h



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//*** Function Declarations ***
	int initialize(void);
	void uninitialize(void);
	void display(void);
	void update(void);

	//*** Local Variable Declarations ***
	WNDCLASSEX wndclass;
	HWND hwnd;
	TCHAR szAppName[] = TEXT("Tushar_Wagdare_Chi_Window");
	MSG msg;
	int iResult       = 0;
	BOOL bDone        = FALSE;
	//For resolution
	int iWinWidth, iWinHight;
	iWinWidth         = GetSystemMetrics(SM_CXSCREEN);
	iWinHight         = GetSystemMetrics(SM_CYSCREEN);

	
	//*** Code ***
	
	if (fopen_s(&gpFILE, "..\\log\\Log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Log Create Hou Shakat Nahi"), TEXT("Error"), MB_OK || MB_ICONERROR);
		exit(0);
	}
	fprintf(gpFILE, "Program Started Successfully\n\n");
	fprintf(gpFILE, "********************************************************\n");


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
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName  = NULL;
	wndclass.hIconSm       = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));


	//*** REGISTER WNDCLASSEX ***
	RegisterClassEx(&wndclass);


	//*** CREATE WINDOW ***
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szAppName,
		TEXT("Tushar Tulshiram Wagdare"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,  
		(iWinWidth / 2) - (800 / 2),
		(iWinHight / 2) - (600 / 2),
		WIN_WIDTH,
		WIN_HIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);


	ghwnd = hwnd;


	//*** Initialization ***
	iResult = initialize();
	if (iResult != 0)
	{
		MessageBox(hwnd, TEXT("initialize() Failed !"), TEXT("Error"), MB_OK || MB_ICONERROR);
		DestroyWindow(hwnd);
	}


	//*** SHOW THE WINDOW ***
	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);


	//*** Game LOOP ***
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
			if (gbActive == TRUE)
			{
				//*** Render ***
				display();

				//*** Update ***
				update();
			}
		}
	}


	//*** Uninitialization ***
	uninitialize();


	return((int)msg.wParam);
}



LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//*** Function Declaration ***
	void ToggleFullscreen(void);
	void resize(int, int);


	//*** Code ***
	switch (iMsg)
	{
	case WM_SETFOCUS:
		gbActive = TRUE;
		break;

	case WM_KILLFOCUS:
			gbActive = FALSE;
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
			if (gbFullscreen == FALSE)
			{
				ToggleFullscreen();
				gbFullscreen = TRUE;
			}
			else
			{
				ToggleFullscreen();
				gbFullscreen = FALSE;
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



void ToggleFullscreen(void)
{
	//*** Local Variable Declaration ****
	MONITORINFO mi = { sizeof(MONITORINFO) };


	//*** Code ***
	if (gbFullscreen == FALSE)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			fprintf(gpFILE, "Window Contains WS_OVERLAPPEDWINDOW\n");

			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
	}
	else
	{
		fprintf(gpFILE, "Window Is Now Already In Fullscreen Mode\n");
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
		fprintf(gpFILE, "Now Window Is Normal\n");
	}
}



int initialize(void)
{
	//*** Function Declarations ***
	void printGLInfo(void);
	void uninitialize(void);
	void resize(int, int);

	//01 -  Initialization Of PFD 
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	
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
	ghdc = GetDC(ghwnd);
		if (ghdc == NULL)
		{
			fprintf(gpFILE, "GetDC Failed !!!\n\n");
			return(-1);
		}


	//03 - Tell OS Hya PFD shi match honara PFD de
	iPexelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPexelFormatIndex == 0)
	{
		fprintf(gpFILE, "ChoosepixelFormat() Failed\n\n");
		return(-2);
	}
	

	//04 - Set Obtained Pixel Format
	if (SetPixelFormat(ghdc, iPexelFormatIndex, &pfd) == FALSE)
	{
		fprintf(gpFILE, "SetPixelFormat() Failed\n\n");
		return(-3);
	}


	//05 - Tell WGL Bridging library To Give OpenGL Copatible DC From 'ghdc'
	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		fprintf(gpFILE, "wglCreateContex() Failed\n\n");
		return(-4);
	}


	//06 - Now 'ghdc' End Its Roll And Give Controll To 'ghrc'
	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		fprintf(gpFILE, "wglMakeCurrent() failed\n\n");
		return(-5);
	}


	//Initialize GLEW
	if (glewInit() != GLEW_OK)
	{
		fprintf(gpFILE, "glewInit() Failed !\n\n");
		return(-6);
	}
	//Print GLINFO
	printGLInfo();
	

	//----------------------------------------------------------------------------------------//
	//Vertex Shader
	const GLchar* vertexShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"in vec4 aPosition;" \
		"in vec4 aColor;" \
		"out vec4 oColor;" \
		"uniform mat4 uMVPMatrix;" \
		"void main(void)" \
		"{" \
		"gl_Position = uMVPMatrix * aPosition;" \
		"oColor = aColor;" \
		"}";

	GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);///Creates Empty Shader Object
	glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);///

	glCompileShader(vertexShaderObject);///Return NoneZero Value on Successfull Compiation
	GLint status        = 0;
	GLint infoLogLength = 0;
	GLchar* szInfoLog   = NULL;

	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);//EC step 1
	if (status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLength);
			if (szInfoLog != NULL)
			{
				glGetShaderInfoLog(vertexShaderObject, infoLogLength, NULL, szInfoLog);
				fprintf(gpFILE, "Vertex Shader Compilation Error Log:%s\n", szInfoLog);
				fprintf(gpFILE, "********************************************************\n");
				free(szInfoLog);
				szInfoLog = NULL;
			}
		}
		uninitialize();
	}
	 

	//Fragment Shader
	const GLchar* fragmentShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"in vec4 oColor;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor = oColor;" \
		"}";

	GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);

	glCompileShader(fragmentShaderObject);
	status = 0;
	infoLogLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLength);
			if (szInfoLog != NULL)
			{
				glGetShaderInfoLog(fragmentShaderObject, infoLogLength, NULL, szInfoLog);
				fprintf(gpFILE, "Fragment Shader Compilation Error Log:%s\n", szInfoLog);
				fprintf(gpFILE, "********************************************************\n");
				free(szInfoLog);
				szInfoLog = NULL;
			}
		}
		uninitialize();
	}

	//Shader Program
	shaderProgramObject = glCreateProgram();

	glAttachShader(shaderProgramObject, vertexShaderObject);
	glAttachShader(shaderProgramObject, fragmentShaderObject);

	glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_POSITION, "aPosition");
	glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_COLOR, "aColor");

	glLinkProgram(shaderProgramObject);
	status = 0;
	infoLogLength = 0;
	szInfoLog = NULL;
	glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLength);
			if (szInfoLog != NULL)
			{
				glGetProgramInfoLog(shaderProgramObject, infoLogLength, NULL, szInfoLog);
				fprintf(gpFILE, "Shader Program Linking Error Log:%s\n", szInfoLog);
				fprintf(gpFILE, "********************************************************\n");
				free(szInfoLog);
				szInfoLog = NULL;
			}
		}
		uninitialize();
	}

	//Get Shader Uniform Location
	mvpMatrixUniform = glGetUniformLocation(shaderProgramObject, "uMVPMatrix");

	//Declare Position And Color Arrays
	const GLfloat triangle_position[] = 
	{
		0.0f,1.0f,0.0f,
		-1.0f,-1.0f,0.0f,
		1.0f,-1.0f,0.0f 
	};

	const GLfloat triangle_color[] = 
	{ 
		1.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,0.0f,1.0f 
	};

	//VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//VBO for Position 
	glGenBuffers(1, &vbo_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_position), triangle_position, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//VBO for color
	glGenBuffers(1, &vbo_color);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_color), triangle_color, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//
	glBindVertexArray(0);
	//-------------------------------------------------------------------------------------//

	//07 - 2nd step Enabling Depth
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0f);


	//08 - Set the Clear Color of Window To Blue
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);


	//initialize orthographicProjectionMatrix
	perspectiveProjectionMatrix = vmath::mat4::identity();


	//Warmup call
	resize(WIN_WIDTH, WIN_HIGHT);


	return(0);
}



void printGLInfo(void)
{
	//Variable Declaration 
	GLint numExtension;
	GLint i;

	//code
	fprintf(gpFILE, "OpenGL Vender       : %s\n", glGetString(GL_VENDOR));
	fprintf(gpFILE, "OpenGL Renderer     : %s\n", glGetString(GL_RENDERER));//Driver Version
	fprintf(gpFILE, "OpenGL Version      : %s\n", glGetString(GL_VERSION));
	fprintf(gpFILE, "OpenGL GLSL Version : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	fprintf(gpFILE, "********************************************************\n");

	//Supported Extensions List
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtension);
	for (i = 0; i < numExtension; i++)
	{
		fprintf(gpFILE, "%d : %s\n",i, glGetStringi(GL_EXTENSIONS, i));
	}
	fprintf(gpFILE, "********************************************************\n");
}



void resize(int width, int height)
{
	//*** Code ***
	if (height <= 0)
	{
		height = 1;
	}

	//Set perspective projection matrix
	perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
}



void display(void)
{
	//*** Code ***
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramObject);

	//Transformation
	mat4 modelViewMatrix = vmath::translate(0.0f, 0.0f, -3.0f);
	mat4 modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	//push above mvp int vertex shader's mvp uniform
	glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
	glUseProgram(0);

	SwapBuffers(ghdc);
}



void update(void)
{
	//*** Code ***
}



void uninitialize(void)
{
	//*** Function Declarations ***
	void ToggleFullscreen(void);


	//Free Shader Program Object
	if (shaderProgramObject)
	{
		glUseProgram(shaderProgramObject);

		GLint numShaders = 0;
		glGetProgramiv(shaderProgramObject, GL_ATTACHED_SHADERS, &numShaders);
		if (numShaders > 0)
		{
			GLuint* pShaders = (GLuint*)malloc(numShaders * sizeof(GLuint));
			if (pShaders != NULL)
			{
				glGetAttachedShaders(shaderProgramObject, numShaders, NULL, pShaders);
				for (GLint i = 0; i < numShaders; i++)
				{
					glDetachShader(shaderProgramObject, pShaders[i]);
					glDeleteShader(pShaders[i]);
					pShaders[i] = 0;
				}
				free(pShaders);
				pShaders = NULL;
			}
		}

		glUseProgram(0);
		glDeleteProgram(shaderProgramObject);
		shaderProgramObject = 0;
	}


	//Delete VBO Color
	if (vbo_color)
	{
		glDeleteBuffers(1, &vbo_color);
		vbo_color = 0;
	}


	//Delete VBO Position
	if (vbo_position)
	{
		glDeleteBuffers(1, &vbo_position);
		vbo_position  = 0;
	}


	//Delete VAO
	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}
	


	//*** Code ***
	if (gbFullscreen == TRUE)
	{
		ToggleFullscreen();
		gbFullscreen = FALSE;
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
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}


	//*** Destroy Window ***
	if (ghwnd)
	{
		DestroyWindow(ghwnd);
		ghwnd = NULL;
	}


	//*** Close Log File ***
	if (gpFILE)
	{
		fprintf(gpFILE, "\nProgram Ended Successfully\n");
		fclose(gpFILE);
		gpFILE = NULL;
	}
}


